#include "downloadmanager.h"
#include "dictionary.h"
#include "version.h"
#include "configpath.h"
#include "zip.h" //https://github.com/kuba--/zip

#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSettings>

DownloadManager::DownloadManager(QObject* parent)
{
    m_pParent = parent;
    connect(&manager, &QNetworkAccessManager::finished,
            this, &DownloadManager::downloadFinished);
}

void DownloadManager::download(const QString fileName)
{
    const QString server = "https://github.com/Scrabble3D/";

    QUrl url(server + fileName);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    connect(reply, &QNetworkReply::downloadProgress,
            this, &DownloadManager::downloadProgress);

#if QT_CONFIG(ssl)
    connect(reply, &QNetworkReply::sslErrors,
            this, &DownloadManager::sslErrors);
#endif

}

int DownloadManager::lastChecked()
{
    QDateTime dtChecked(QDate(1970,1,1),QTime(0,0));
    QSettings settings(config::ini(), QSettings::IniFormat);

    settings.beginGroup("program");
    QString sLastUpdate = settings.value("lastupdate","").toString();
    if (!sLastUpdate.isEmpty())
        dtChecked = QDateTime::fromString(sLastUpdate, Qt::ISODate);
    settings.endGroup();

    return dtChecked.daysTo( QDateTime::currentDateTime() );
}

//write all data or use qdomdocument
void DownloadManager::checkUpdates() {

    QList<dicListData> lDictionaries;
    dicListData aData;

    bool bCanUpdate = false;

    //read remote info and add local data
    QFile rFile(config::conf());
    if ( rFile.open(QIODevice::ReadOnly) )
    {
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&rFile);

        while (!xmlReader.atEnd())
        {
            xmlReader.readNext();

            if (xmlReader.isStartElement() &&
               (xmlReader.name().toString() == "dictionary"))
            {
                aData.NativeName=xmlReader.attributes().value("Native").toString();
                aData.EnglishName=xmlReader.attributes().value("English").toString();
                aData.AvailableVersion=xmlReader.attributes().value("RemoteVersion").toString();
                aData.FileName=xmlReader.attributes().value("FileName").toString();
                dicList::getInfo(&aData);
                if (aData.InstalledVersionNumber > -1) {
                    //: Dictionary english.dic: 1.0.2 > 1.0.1
                    m_pParent->setProperty("addMsg", tr("Dictionary %1: %2 %3 %4").arg(
                                               aData.NativeName,
                                               aData.InstalledVersion,
                                               aData.InstalledVersionNumber > aData.AvailableVersionNumber
                                               ? " > "
                                               : aData.InstalledVersionNumber < aData.AvailableVersionNumber
                                                 ? " < "
                                                 : " = ",
                                               aData.AvailableVersion));
                    bCanUpdate = bCanUpdate || (aData.InstalledVersionNumber < aData.AvailableVersionNumber);
                }
                version::fromString(aData.AvailableVersion);
                lDictionaries.append(aData);
            } //Dictionaries

            if (xmlReader.isStartElement() &&
               (xmlReader.name().toString() == "version"))
            {
                QString sRemoteVersion = xmlReader.readElementText();
                int nRemoteVersion = version::fromString( sRemoteVersion );
                int nLocalVersion = version::fromString( version::current() );
                m_pParent->setProperty("addMsg", tr("Program version: %1 %2 %3").arg(
                                           sRemoteVersion,
                                           nRemoteVersion > nLocalVersion
                                           ? " > "
                                           : nRemoteVersion < nLocalVersion
                                             ? " < " : " = ",
                                           version::current() ) );
                bCanUpdate = bCanUpdate || (nRemoteVersion > nLocalVersion);
            }
        } //start
        rFile.close();
    };

    //run updates
    if (bCanUpdate &&
        //: Updates available /n Do you want to start now?
        QMessageBox::question(nullptr, tr("Updates available"),
                              //: Updates available /n Do you want to start now?
                              tr("Do you want to start now?")) == QMessageBox::Yes )
    {
        for (int i=0; i<lDictionaries.count(); i++) {
            if (lDictionaries[i].InstalledVersionNumber < lDictionaries[i].AvailableVersionNumber)
                download(lDictionaries[i].FileName+".zip");
        }
        //TODO: updater: update program
    }

    //last update
    QSettings settings(config::ini(), QSettings::IniFormat);
    settings.beginGroup("program");
    settings.setValue("lastupdate",QDateTime::currentDateTime().toString(Qt::ISODate));//"yyyyMMddhhmm"));
    settings.endGroup();
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_pParent->setProperty("computeProgress",(double(bytesReceived)/double(bytesTotal))*100);
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        //: Could not open english.dic for writing: disc full.
        m_pParent->setProperty("addMsg", tr("Could not open %1 for writing: %2").arg(
                                   qPrintable(filename),
                                   qPrintable(file.errorString())));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        m_pParent->setProperty("addMsg", tr("SSL error: %1").arg(
                                   qPrintable(error.errorString())));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{

    m_pParent->setProperty("computeProgress", 0); //zero progress bar

    QUrl url = reply->url();
    //special handling for Scrabble3D.conf, the only uncompressed file
    //not extracted and saved directly at the config path
    const bool isCompressed = url.fileName().endsWith("zip", Qt::CaseInsensitive);
    const QString tmpFile = isCompressed ? config::temp(url.fileName())
                                         : config::file(url.fileName());
    if (reply->error()) {
        m_pParent->setProperty("addMsg",reply->errorString() );
    } else
    if (saveToDisk(tmpFile, reply)) {
        m_pParent->setProperty("addMsg", tr("%1 successfully downloaded.").arg(
                                   qPrintable(url.fileName())));

        QString aFileName;
        //extract
        if ( isCompressed ) {
            int arg = 2;
            zip_extract(tmpFile.toUtf8(),
                        config::file("").toUtf8(),
                        nullptr, &arg );
            aFileName = url.fileName().left( url.fileName().length() - 4 );
        } else
            aFileName = url.fileName();

        if (aFileName.endsWith("dic"))
            emit onFinished(DlType::dmDictionary, aFileName);
        else if (aFileName.endsWith("qm"))
            emit onFinished(DlType::dmLocalization, aFileName);
        else if (aFileName.endsWith("conf"))
            emit onFinished(DlType::dmConfig, aFileName);
    }
    reply->deleteLater();
}

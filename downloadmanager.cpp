#include "downloadmanager.h"
#include "configpath.h"
#include "zip.h" //https://github.com/kuba--/zip
#include "version.h"

#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSettings>

DownloadManager::DownloadManager(QObject* parent)
{
    Q_UNUSED(parent);
    connect(&manager, &QNetworkAccessManager::finished,
            this, &DownloadManager::downloadFinished);
}

void DownloadManager::download(const QString fileName)
{

    QUrl url(github + fileName);
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
void DownloadManager::checkUpdates(QStringList canUpdate) {
    //run updates
    if (!canUpdate.isEmpty() &&
        //: Updates available /n Do you want to start now?
        QMessageBox::question(nullptr, tr("Updates available"),
                              //: Updates available /n Do you want to start now?
                              tr("Do you want to update now?")) == QMessageBox::Yes )
    {
        for (int i=0; i<canUpdate.count(); i++)
            download( canUpdate[i] );
    }

    //last update
    QSettings settings(config::ini(), QSettings::IniFormat);
    settings.beginGroup("program");
    settings.setValue("lastupdate",QDateTime::currentDateTime().toString(Qt::ISODate));//"yyyyMMddhhmm"));
    settings.endGroup();
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit onProgress( double(bytesReceived)/double(bytesTotal)*100 );
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        //: Could not open english.dic for writing: disc full.
        emit onMessage(tr("Could not open %1 for writing: %2").arg(
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
        emit onMessage(tr("SSL error: %1").arg(
                          qPrintable(error.errorString())));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    emit onProgress( 0 ); //zero progress bar

    QUrl url = reply->url();
    //special handling for Scrabble3D.conf, the only uncompressed file
    //not extracted and saved directly at the config path
    const bool isCompressed = url.fileName().endsWith("zip", Qt::CaseInsensitive);
    const QString tmpFile = isCompressed ? config::temp(url.fileName())
                                         : config::file(url.fileName());
    if (reply->error()) {
        emit onMessage( reply->errorString() );
    } else
    if (saveToDisk(tmpFile, reply)) {
        if (!url.fileName().endsWith("conf"))
            emit onMessage( tr("%1 successfully downloaded.").arg(
                               qPrintable(url.fileName())) );

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

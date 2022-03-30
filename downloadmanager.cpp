#include "downloadmanager.h"
#include <QNetworkReply>
#include <QFileInfo>
#include <QCoreApplication>
#include "defines.h"
#include "zip.h" //https://github.com/kuba--/zip

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

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_pParent->setProperty("computeProgress",(double(bytesReceived)/double(bytesTotal))*100);
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
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

    m_pParent->setProperty("computeProgress", 0);

    QUrl url = reply->url();
    const QString tmpFile = tempPath().path() + "/" + url.fileName();
    if (reply->error())
        m_pParent->setProperty("addMsg",reply->errorString() );
    else
    //save
    if (saveToDisk(tmpFile, reply)) {
        m_pParent->setProperty("addMsg", tr("%1 successfully downloaded.").arg(
                                   qPrintable(url.fileName())));

        //extract
        int arg = 2;
        zip_extract(tmpFile.toUtf8(),
                    dictionaryPath().path().toUtf8(),
                    nullptr, &arg );

        QString aFileName = url.fileName().left( url.fileName().length() - 4 );

        if (aFileName.endsWith("dic"))
            m_pParent->setProperty("dicDownloadFinsihed", aFileName);
        else if (aFileName.endsWith("conf"))
            m_pParent->setProperty("confDownloadFinsihed", ""); //just update the dic list for now

    }
    reply->deleteLater();
}

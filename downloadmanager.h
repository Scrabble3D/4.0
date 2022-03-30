#pragma once

#include <QObject>
#include <QNetworkAccessManager>
//TODO: downloadmanager: updater
class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;

public:
    DownloadManager(QObject* parent);
    void download(const QString fileName);

public slots:
    void downloadFinished(QNetworkReply *reply);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void sslErrors(const QList<QSslError> &errors);

private:
    bool saveToDisk(const QString &filename, QIODevice *data);
    static int onExtractZip(const char *filename, void *arg);
    QObject *m_pParent;
};

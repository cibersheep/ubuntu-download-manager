#ifndef UBUNTUDOWNLOADMANAGER_PLUGIN_H
#define UBUNTUDOWNLOADMANAGER_PLUGIN_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <ubuntu/download_manager/manager.h>

#include "download_error.h"
#include "single_download.h"

namespace Ubuntu {

namespace DownloadManager {

class UbuntuDownloadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart)
    Q_PROPERTY(bool cleanDownloads READ cleanDownloads WRITE setCleanDownloads)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(QVariantList downloads READ downloads NOTIFY downloadsChanged)

public:
    explicit UbuntuDownloadManager(QObject *parent = 0);
    ~UbuntuDownloadManager();

    Q_INVOKABLE void download(QString url);

    QVariantList downloads() { return m_downloads; }
    QString errorMessage() const { return m_errorMessage; }
    bool autoStart() const { return m_autoStart; }
    bool cleanDownloads() const { return m_cleanDownloads; }
    void setCleanDownloads(bool value);
    void setAutoStart(bool value) { m_autoStart = value; }

signals:
    void errorChanged();
    void downloadsChanged();

private slots:
    void registerError(DownloadError& error);
    void downloadFileCreated(Download* download);
    void downloadGroupCreated(GroupDownload* group);
    void downloadCompleted();

private:
    bool m_autoStart;
    bool m_cleanDownloads;
    QVariantList m_downloads;
    QString m_errorMessage;
    Manager* m_manager;
};

}
}

#endif // UBUNTUDOWNLOADMANAGER_PLUGIN_H
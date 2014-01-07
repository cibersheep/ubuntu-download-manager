/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef DOWNLOADER_LIB_DOWNLOAD_H
#define DOWNLOADER_LIB_DOWNLOAD_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QProcess>
#include <QSharedPointer>
#include "app-downloader-lib_global.h"
#include "system/process_factory.h"
#include "system/request_factory.h"
#include "system/system_network_info.h"


namespace Ubuntu {

namespace DownloadManager {

using namespace System;

class Download : public QObject {
    Q_OBJECT

 public:
    enum State {
        IDLE,
        START,
        PAUSE,
        RESUME,
        CANCEL,
        FINISH,
        ERROR
    };

    Download(const QString& id,
             const QString& path,
             bool isConfined,
             const QString& rootPath,
             const QVariantMap& metadata,
             const QMap<QString, QString>& headers,
             QObject* parent = 0);

    virtual ~Download();

    QString downloadId() const {
        return _id;
    }

    QString path() const {
        return _dbusPath;
    }

    bool isConfined() const {
        return _isConfined;
    }

    QString rootPath() const {
        return _rootPath;
    }

    Download::State state() const {
        return _state;
    }

    void setState(Download::State state);

    QObject* adaptor() const {
        return _adaptor;
    }
    void setAdaptor(QObject* adaptor);

    QMap<QString, QString> headers() const {
        return _headers;
    }

    virtual bool canDownload();

    virtual bool isValid() const {
        return _isValid;
    }
    virtual QString lastError() const {
        return _lastError;
    }

    // methods to be overriden by the children
    virtual void cancelDownload() = 0;
    virtual void pauseDownload() = 0;
    virtual void resumeDownload() = 0;
    virtual void startDownload() = 0;

 public slots:  // NOLINT(whitespace/indent)
    // slots that are exposed via dbus, they just change the state,
    // the downloader takes care of the actual download operations
    QVariantMap metadata() const {
        return _metadata;
    }

    virtual void setThrottle(qulonglong speed);

    virtual qulonglong throttle() {
        return _throttle;
    }

    void allowGSMDownload(bool allowed);
    bool isGSMDownloadAllowed();

    inline void cancel() {
        setState(Download::CANCEL);
    }

    inline void pause() {
        setState(Download::PAUSE);
    }

    inline void resume() {
        setState(Download::RESUME);
    }

    inline void start() {
        setState(Download::START);
    }

    // slots to be implemented by the children
    virtual qulonglong progress() = 0;
    virtual qulonglong totalSize() = 0;

 signals:
    // signals that are exposed via dbus
    void canceled(bool success);
    void error(const QString& error);
    void paused(bool success);
    void processing(const QString& file);
    void resumed(bool success);
    void started(bool success);
    void progress(qulonglong received, qulonglong total);

    // internal signals
    void stateChanged();

 protected:
    void setIsValid(bool isValid);
    void setLastError(const QString& lastError);
    virtual void emitError(const QString& error);

 private:
    bool _isValid = true;
    QString _lastError = "";
    QString _id;
    qulonglong _throttle;
    bool _allowGSMDownload;
    Download::State _state;
    QString _dbusPath;
    bool _isConfined;
    QString _rootPath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    SystemNetworkInfo* _networkInfo;
    QObject* _adaptor = NULL;
};

}  // DownloadManager

}  // Ubuntu
#endif  // DOWNLOADER_LIB_APP_DOWNLOAD_H
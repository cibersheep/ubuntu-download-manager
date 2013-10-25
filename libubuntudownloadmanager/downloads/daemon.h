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

#ifndef DOWNLOADER_LIB_DOWNLOAD_DAEMON_H
#define DOWNLOADER_LIB_DOWNLOAD_DAEMON_H

#include <QObject>
#include <QSharedPointer>
#include "app-downloader-lib_global.h"
#include "downloads/download_manager.h"
#include "system/application.h"
#include "system/dbus_connection.h"
#include "system/timer.h"

class DaemonPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT Daemon : public QObject {
    Q_DECLARE_PRIVATE(Daemon)
    Q_OBJECT

 public:
    explicit Daemon(QObject *parent = 0);
    Daemon(QSharedPointer<Application> app,
           DBusConnection* conn,
           Timer* timer,
           DownloadManager* man,
           QObject *parent = 0);

 public slots:  // NOLINT (whitespace/indent)
    void start();

 private:
    Q_PRIVATE_SLOT(d_func(), void onTimeout())
    Q_PRIVATE_SLOT(d_func(), void onDownloadManagerSizeChanged(int))  // NOLINT (readability/function)

 private:
    // use pimpl so that we can mantains ABI compatibility
    DaemonPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_DOWNLOAD_DAEMON_H
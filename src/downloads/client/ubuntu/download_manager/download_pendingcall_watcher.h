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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_PENDINGCALL_WATCHER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_PENDINGCALL_WATCHER_H

#include <QObject>
#include <ubuntu/transfers/visibility.h>
#include "download.h"
#include "pending_call_watcher.h"

namespace Ubuntu {

namespace DownloadManager {

class UBUNTU_TRANSFERS_PRIVATE DownloadPCW : public PendingCallWatcher {
    Q_OBJECT

 public:
    DownloadPCW(const QDBusConnection& conn,
                               const QString& servicePath,
                               const QDBusPendingCall& call,
                               Download* parent = 0);

 private slots:
    void onFinished(QDBusPendingCallWatcher* watcher);
};

}  // DownloadManager

}  // Ubuntu

#endif // DOWNLOAD_PENDINGCALL_WATCHER_H

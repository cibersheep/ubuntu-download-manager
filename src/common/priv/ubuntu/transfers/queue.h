/*
 * Copyright 2013-2014 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_DOWNLOAD_QUEUE_H
#define DOWNLOADER_LIB_DOWNLOAD_QUEUE_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QSharedPointer>

#include "ubuntu/transfers/system/network_session.h"
#include "transfer.h"

namespace Ubuntu {

namespace Transfers {

using namespace System;

class Queue : public QObject {
    Q_OBJECT

 public:
    explicit Queue(QObject* parent = 0);

    virtual void add(Transfer* transfer);

    // accessors for useful info
    virtual QString currentTransfer(const QString& appId);
    virtual QStringList paths();
    virtual QHash<QString, Transfer*> transfers();
    virtual int size();

 signals:
    // signals raised when things happens within the q
    void transferAdded(QString path);
    void transferRemoved(QString path);
    void currentChanged(QString appId, QString path);

 private:
    void onManagedTransferStateChanged();
    void onUnmanagedTransferStateChanged();
    void onSessionTypeChanged(QNetworkConfiguration::BearerType type);
    void remove(const QString& path);
    void updateCurrentTransfer(const QString& appIdToUpdate = "");

 private:
    QHash<QString, QString> _current;
    QHash<QString, Transfer*> _transfers;  // quick for access
    QHash<QString, QStringList*> _sortedPaths;  // keep the order
};

}  // Transfers

}  // Ubuntu

#endif  // DOWNLOADER_LIB_DOWNLOAD_QUEUE_H

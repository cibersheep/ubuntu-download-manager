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

#ifndef DOWNLOADER_LIB_DOWNLOAD_FACTORY_H
#define DOWNLOADER_LIB_DOWNLOAD_FACTORY_H

#include <QObject>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/transfers/system/apparmor.h>
#include <ubuntu/transfers/system/uuid_factory.h>
#include "download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

class Factory : public QObject {
    Q_OBJECT

 public:
    Factory(AppArmor* apparmor,
            QObject *parent = 0);
    virtual ~Factory();

    // create downloads comming from a dbus call

    virtual Download* createDownload(const QString& dbusOwner,
                                     const QUrl& url,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers);

    virtual Download* createDownload(const QString& dbusOwner,
                                     const QUrl& url,
                                     const QString& hash,
                                     const QString& algo,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers);

    virtual Download* createDownload(const QString& dbusOwner,
                                     StructList downloads,
                                     const QString& algo,
                                     bool allowed3G,
                                     const QVariantMap& metadata,
                                     StringMap headers);

    virtual Download* createMmsDownload(const QString& dbusOwner,
                                        const QUrl& url,
                                        const QString& hostname,
                                        int port);

    // create downloads from a group download

    virtual Download* createDownloadForGroup(bool isConfined,
                                         const QString& rootPath,
                                         const QUrl& url,
                                         const QVariantMap& metadata,
                                         const QMap<QString, QString>& headers);

    virtual Download* createDownloadForGroup(bool isConfined,
                                         const QString& rootPath,
                                         const QUrl& url,
                                         const QString& hash,
                                         const QString& algo,
                                         const QVariantMap& metadata,
                                         const QMap<QString, QString>& headers);

    // mainly for testing purposes

    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);

 private:

    SecurityDetails* getSecurityDetails(const QString& dbusOwner,
                                        const QVariantMap& metadata);

 private:
    AppArmor* _apparmor;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_DOWNLOAD_FACTORY_H

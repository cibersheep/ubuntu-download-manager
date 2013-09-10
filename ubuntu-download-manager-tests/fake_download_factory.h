/*
 * Copyright 2013 2013 Canonical Ltd.
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

#ifndef FAKE_DOWNLOAD_FACTORY_H
#define FAKE_DOWNLOAD_FACTORY_H

#include <QObject>
#include <QSharedPointer>
#include <download_factory.h>
#include "./fake.h"

class FakeDownloadFactory : public DownloadFactory, public Fake {
    Q_OBJECT

 public:
    FakeDownloadFactory(QSharedPointer<AppArmor> apparmor,
                        QSharedPointer<SystemNetworkInfo> networkInfo,
                        QSharedPointer<RequestFactory> nam,
                        QSharedPointer<ProcessFactory> processFactory,
                        QObject *parent = 0);

    Download* createDownload(const QString& downloadOwner,
                             const QUrl& url,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) override;

    Download* createDownload(const QString& downloadOwner,
                             const QUrl& url,
                             const QString& hash,
                             QCryptographicHash::Algorithm algo,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) override;

    Download* createDownload(const QString& downloadOwner,
                             StructList downloads,
                             QCryptographicHash::Algorithm algo,
                             bool allowed3G,
                             const QVariantMap& metadata,
                             StringMap headers) override;

    QList<Download*> downloads();

 private:
    QSharedPointer<AppArmor> _apparmor;
    QSharedPointer<SystemNetworkInfo> _networkInfo;
    QSharedPointer<RequestFactory> _nam;
    QSharedPointer<ProcessFactory> _processFactory;
    QList<Download*> _downloads;
};

#endif  // FAKE_DOWNLOAD_FACTORY_H

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

#ifndef FAKE_DOWNLOAD_FACTORY_H
#define FAKE_DOWNLOAD_FACTORY_H

#include <QObject>
#include <ubuntu/downloads/factory.h>
#include <gmock/gmock.h>

using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::DownloadManager::Daemon;

class MockDownloadFactory : public Factory {
 public:

    MockDownloadFactory(AppArmor* _apparmor, QObject *parent=0) : Factory(_apparmor, parent) {}

    MOCK_METHOD1(setAcceptedCertificates,
        void(const QList<QSslCertificate>&));
    MOCK_METHOD4(createDownload,
        Download*(const QString&, const QUrl&, const QVariantMap&,
                  const QMap<QString, QString>&));
    MOCK_METHOD6(createDownload,
        Download*(const QString&, const QUrl&, const QString&,
                  const QString&, const QVariantMap&,
                  const QMap<QString, QString>&));
    MOCK_METHOD6(createDownload,
        Download*(const QString&, StructList, const QString&, bool,
                  const QVariantMap&, StringMap));
    MOCK_METHOD5(createDownloadForGroup,
        Download*(bool, const QString&, const QUrl&,
                  const QVariantMap&, const QMap<QString, QString>&));
    MOCK_METHOD7(createDownloadForGroup,
        Download*(bool, const QString&, const QUrl&, const QString&,
                  const QString&, const QVariantMap&,
                  const QMap<QString, QString>&));
};

#endif  // FAKE_DOWNLOAD_FACTORY_H

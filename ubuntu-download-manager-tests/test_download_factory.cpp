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

#include <single_download.h>
#include "./test_download_factory.h"

TestDownloadFactory::TestDownloadFactory(QObject *parent)
    : QObject(parent) {
}

void
TestDownloadFactory::init() {
    _uuidFactory = QSharedPointer<UuidFactory>(new FakeUuidFactory());
    _apparmor = new FakeAppArmor(_uuidFactory);
    _networkInfo = QSharedPointer<SystemNetworkInfo>(
        new FakeSystemNetworkInfo());
    _nam = QSharedPointer<RequestFactory>(new FakeRequestFactory());
    _processFactory = QSharedPointer<ProcessFactory>(
        new FakeProcessFactory());
    _downFactory = new DownloadFactory(QSharedPointer<AppArmor>(_apparmor),
        _networkInfo, _nam, _processFactory);
}

void
TestDownloadFactory::cleanup() {
    if (_downFactory != NULL)
        delete _downFactory;
}

void
TestDownloadFactory::testCreateDownload() {
    _apparmor->record();

    // create a download, assert that it was
    // created and that the id and the path are correctly set
    Download* download = _downFactory->createDownload("", QUrl(),
        QVariantMap(), QMap<QString, QString>());

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    UuidWrapper* id = reinterpret_cast<UuidWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateDownloadWithHash() {
    _apparmor->record();

    QString hash = "my-hash";
    QCryptographicHash::Algorithm algo = QCryptographicHash::Md5;

    // same as above but assert hash and hash algo
    Download* download = _downFactory->createDownload("", QUrl(),
        hash, algo, QVariantMap(), QMap<QString, QString>());

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    UuidWrapper* id = reinterpret_cast<UuidWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());

    SingleDownload* single = reinterpret_cast<SingleDownload*>(download);
    QCOMPARE(hash, single->hash());
    QCOMPARE(algo, single->hashAlgorithm());
}

void
TestDownloadFactory::testCreateGroupDownload() {
    _apparmor->record();

    // create a download, assert that it was
    // created and that the id and the path are correctly set
    Download* download = _downFactory->createDownload("",
        QList<GroupDownloadStruct>(), QCryptographicHash::Md5,
        true, QVariantMap(), QMap<QString, QString>());

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    UuidWrapper* id = reinterpret_cast<UuidWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

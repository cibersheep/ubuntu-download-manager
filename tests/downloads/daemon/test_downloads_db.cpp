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

#include <QFile>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/transfers/system/hash_algorithm.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include <ubuntu/transfers/system/network_session.h>
#include <ubuntu/downloads/file_download.h>
#include "test_downloads_db.h"

using namespace Ubuntu::Transfers::System;

namespace {
    const QString TABLE_EXISTS = "SELECT count(name) FROM sqlite_master "\
        "WHERE type='table' AND name=:table_name;";

    const QString SELECT_SINGLE_DOWNLOAD = "SELECT appId, url, dbus_path, local_path, "\
        "hash, hash_algo, state, total_size, throttle, metadata, headers "\
        "FROM SingleDownload WHERE uuid=:uuid;";
}

TestDownloadsDb::TestDownloadsDb(QObject *parent)
    : BaseTestCase("TestDownloadsDb", parent) {
}

void
TestDownloadsDb::init() {
    BaseTestCase::init();
    _db = DownloadsDb::instance();
}

void
TestDownloadsDb::cleanup() {
    BaseTestCase::cleanup();
    QString dbFile = _db->filename();


    DownloadsDb::deleteInstance();
    _db = nullptr;

    QFile::remove(dbFile);
    NetworkSession::deleteInstance();
    FileManager::deleteInstance();
    FileNameMutex::deleteInstance();
}

void
TestDownloadsDb::testTableCreations_data() {
    QTest::addColumn<QString>("tableName");

    QTest::newRow("SingleDownload table present") << "SingleDownload";
    QTest::newRow("GroupDownload table present") << "GroupDownload";
    QTest::newRow("GroupDownload realtion table present")
        << "GroupDownloadDownloads";
}

void
TestDownloadsDb::testTableCreations() {
    QFETCH(QString, tableName);
    _db->init();
    QSqlDatabase db = _db->db();
    db.open();
    // open the db with the filename and assert that all tables are
    // present
    QSqlQuery query(db);
    query.prepare(TABLE_EXISTS);
    query.bindValue(":table_name", tableName);

    query.exec();
    int rows = 0;
    if (query.next())
        rows = query.value(0).toInt();
    db.close();
    QCOMPARE(1, rows);
}

void
TestDownloadsDb::testTableExists() {
    _db->init();
    QVERIFY(QFile::exists(_db->filename()));
}

void
TestDownloadsDb::testStoreSingleDownload_data() {
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("appId");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QString>("hashAlgoString");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<QMap<QString, QString> >("headers");

    QTest::newRow("First Row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "FIRST APP" << "first path" << QUrl("http://ubuntu.com") << ""
        << "md5" << QVariantMap() << QMap<QString, QString>();

    QVariantMap secondMetadata;
    secondMetadata["test"] = 1;
    secondMetadata["command"] = "cd";
    secondMetadata["hello"] = 23;

    QTest::newRow("Second Row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "SECOND APP" << "second path" << QUrl("http://ubuntu.com/phone")
        << "" << "sha512" << secondMetadata << QMap<QString, QString>();

    QVariantMap thirdMetadata;
    secondMetadata["test"] = 3;
    secondMetadata["command"] = "return";
    secondMetadata["hello"] = 500;

    QMap<QString, QString> thirdHeaders;
    thirdHeaders["my-header"] = "I do something cool";

    QTest::newRow("Third Row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "THIRD APP" << "third path" << QUrl("http://ubuntu.com/tablet")
        << "" << "sha384" << thirdMetadata << thirdHeaders;
}

void
TestDownloadsDb::testStoreSingleDownload() {
    _db->init();
    QFETCH(QString, id);
    QFETCH(QString, appId);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(QString, hashAlgoString);
    QFETCH(QVariantMap, metadata);
    QFETCH(StringMap, headers);

    QScopedPointer<FileDownload> download(new FileDownload(id, appId, path,
        false, "", url, hash, hashAlgoString, metadata, headers));

    _db->storeSingleDownload(download.data());
    // query that the download is there and that the data is correct
    QSqlDatabase db = _db->db();
    db.open();
    QSqlQuery query(db);
    query.prepare(SELECT_SINGLE_DOWNLOAD);
    query.bindValue(":uuid", id);
    query.exec();
    if (query.next()) {
        auto dbAppId = query.value(0).toString();
        QCOMPARE(appId, dbAppId);

        auto dbUrl = query.value(1).toString();
        QCOMPARE(url.toString(), dbUrl);

        auto dbDbusPath = query.value(2).toString();
        QCOMPARE(path, dbDbusPath);

        auto dbLocalPath = query.value(3).toString();
        QCOMPARE(dbLocalPath, download->filePath());

        auto dbHash = query.value(4).toString();
        QCOMPARE(hash, dbHash);

        auto dbHashAlgo = query.value(5).toString();
        QCOMPARE(hashAlgoString, dbHashAlgo);

        int stateDb = query.value(6).toInt();
        QCOMPARE(0, stateDb);

        auto dbTotalSize = query.value(7).toString();
        QCOMPARE(QString::number(download->totalSize()), dbTotalSize);

        auto dbThrottle = query.value(8).toString();
        QCOMPARE(QString::number(download->throttle()), dbThrottle);

        db.close();
    } else {
        db.close();
        QFAIL("Download was not found!");
    }
}

void
TestDownloadsDb::testStoreSingleDownloadPresent_data() {
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("appId");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QString>("hashAlgoString");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<QMap<QString, QString> >("headers");

    QTest::newRow("First Row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "TEST APP" << "first path" << QUrl("http://ubuntu.com") << ""
        << "md5" << QVariantMap() << QMap<QString, QString>();

    QVariantMap secondMetadata;
    secondMetadata["test"] = 1;
    secondMetadata["command"] = "cd";
    secondMetadata["hello"] = 23;

    QTest::newRow("Second Row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "SECOND APP" << "second path" << QUrl("http://ubuntu.com/phone")
        << "" << "sha512" << secondMetadata << QMap<QString, QString>();

    QVariantMap thirdMetadata;
    secondMetadata["test"] = 3;
    secondMetadata["command"] = "return";
    secondMetadata["hello"] = 500;

    QMap<QString, QString> thirdHeaders;
    thirdHeaders["my-header"] = "I do something cool";

    QTest::newRow("Third Row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "LAST APP" << "third path" << QUrl("http://ubuntu.com/tablet")
        << "" << "sha384"
        << thirdMetadata << thirdHeaders;
}

void
TestDownloadsDb::testStoreSingleDownloadPresent() {
    _db->init();
    QFETCH(QString, id);
    QFETCH(QString, appId);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(QString, hashAlgoString);
    QFETCH(QVariantMap, metadata);
    QFETCH(StringMap, headers);

    QScopedPointer<FileDownload> download(new FileDownload(id, appId, path, true, "", url, hash,
        hashAlgoString, metadata, headers));

    _db->storeSingleDownload(download.data());

    // create a second download with same id but a diff path to test is update
    QString newPath = path + path;
    QScopedPointer<FileDownload> secondDownload(new FileDownload(id, appId,
        newPath, true, "", url, hash, hashAlgoString, metadata, headers));

    _db->storeSingleDownload(secondDownload.data());

    // query that the download is there and that the data is correct
    QSqlDatabase db = _db->db();
    db.open();
    QSqlQuery query(db);
    query.prepare(SELECT_SINGLE_DOWNLOAD);
    query.bindValue(":uuid", id);
    query.exec();
    if (query.next()) {
        auto dbAppId = query.value(0).toString();
        QCOMPARE(appId, dbAppId);

        auto dbUrl = query.value(1).toString();
        QCOMPARE(url.toString(), dbUrl);

        auto dbDbusPath = query.value(2).toString();
        QCOMPARE(newPath, dbDbusPath);

        auto dbLocalPath = query.value(3).toString();
        QCOMPARE(dbLocalPath, secondDownload->filePath());

        auto dbHash = query.value(4).toString();
        QCOMPARE(hash, dbHash);

        auto dbHashAlgo = query.value(5).toString();
        QCOMPARE(hashAlgoString, dbHashAlgo);

        int stateDb = query.value(6).toInt();
        QCOMPARE(0, stateDb);

        auto dbTotalSize = query.value(7).toString();
        QCOMPARE(QString::number(download->totalSize()), dbTotalSize);

        auto dbThrottle = query.value(8).toString();
        QCOMPARE(QString::number(download->throttle()), dbThrottle);

        db.close();
    } else {
        db.close();
        QFAIL("Download was not found!");
    }
}

void
TestDownloadsDb::testConnectedToDownload() {
    QScopedPointer<TestingDb> testingDb(new TestingDb);
    SignalBarrier spy(testingDb.data(), SIGNAL(downloadStored(Download*)));

    auto id = UuidUtils::getDBusString(QUuid::createUuid());
    auto appId = QString("TEST");
    QString path = "first path";
    auto url =  QUrl("http://ubuntu.com");
    auto hash = QString();
    QString hashAlgoString = "md5";
    QVariantMap metadata;
    QMap<QString, QString> headers;

    QScopedPointer<FileDownload> download(new FileDownload(id, appId, path,
        true, "", url, hash, hashAlgoString, metadata, headers));

    testingDb->connectToDownload(download.data());
    // update the throttle and test that it is update
    download->setThrottle(90);
    download->setState(Download::PAUSE);
    QVERIFY(spy.ensureSignalEmitted());
    QTRY_COMPARE(2, spy.count());  // one per update
}

void
TestDownloadsDb::testDisconnectedFromDownload() {
    QScopedPointer<TestingDb> testingDb(new TestingDb);
    SignalBarrier spy(testingDb.data(), SIGNAL(downloadStored(Download*)));

    auto id = UuidUtils::getDBusString(QUuid::createUuid());
    auto appId = QString("TEST");
    QString path = "first path";
    auto url =  QUrl("http://ubuntu.com");
    auto hash = QString();
    QString hashAlgoString = "md5";
    QVariantMap metadata;
    QMap<QString, QString> headers;

    QScopedPointer<FileDownload> download(new FileDownload(id, appId, path,
        true, "", url, hash, hashAlgoString, metadata, headers));

    testingDb->connectToDownload(download.data());
    // update the throttle and test that it is update
    download->setThrottle(90);
    QVERIFY(spy.ensureSignalEmitted());
    QTRY_COMPARE(1, spy.count());
    testingDb->disconnectFromDownload(download.data());
    download->setState(Download::PAUSE);
    QTRY_COMPARE(1, spy.count());
}

void
TestDownloadsDb::testGetStateMissingDownload() {
    _db->init();
    auto result = _db->getDownloadState("random uuid");
    QVERIFY(!result.isValid());
}

void
TestDownloadsDb::testGetStateDownload_data() {
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("appId");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QString>("hashAlgoString");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<QMap<QString, QString> >("headers");

    QTest::newRow("First Row") << UuidUtils::getDBusString(QUuid::createUuid())
    << "FIRST APP" << "first path" << QUrl("http://ubuntu.com") << ""
    << "md5" << QVariantMap() << QMap<QString, QString>();

    QVariantMap secondMetadata;
    secondMetadata["test"] = 1;
    secondMetadata["command"] = "cd";
    secondMetadata["hello"] = 23;

    QTest::newRow("Second Row") << UuidUtils::getDBusString(QUuid::createUuid())
    << "SECOND APP" << "second path" << QUrl("http://ubuntu.com/phone")
    << "" << "sha512" << secondMetadata << QMap<QString, QString>();

    QVariantMap thirdMetadata;
    secondMetadata["test"] = 3;
    secondMetadata["command"] = "return";
    secondMetadata["hello"] = 500;

    QMap<QString, QString> thirdHeaders;
    thirdHeaders["my-header"] = "I do something cool";

    QTest::newRow("Third Row") << UuidUtils::getDBusString(QUuid::createUuid())
    << "THIRD APP" << "third path" << QUrl("http://ubuntu.com/tablet")
    << "" << "sha384" << thirdMetadata << thirdHeaders;
}

void
TestDownloadsDb::testGetStateDownload() {
    _db->init();
    QFETCH(QString, id);
    QFETCH(QString, appId);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(QString, hashAlgoString);
    QFETCH(QVariantMap, metadata);
    QFETCH(StringMap, headers);

    QScopedPointer<FileDownload> download(new FileDownload(id, appId, path, true, "", url, hash,
                                                           hashAlgoString, metadata, headers));

    _db->storeSingleDownload(download.data());

    // create a second download with same id but a diff path to test is update
    QString newPath = path + path;
    QScopedPointer<FileDownload> secondDownload(new FileDownload(id, appId,
                                                                 newPath, true, "", url, hash, hashAlgoString, metadata, headers));

    _db->storeSingleDownload(secondDownload.data());
    auto state = _db->getDownloadState(id);
    QVERIFY(state.isValid());
    QCOMPARE(state.getUrl(), url.toString());
    QCOMPARE(state.getHash(), hash);
    QCOMPARE(state.getMetadata(), metadata);
}

void
TestDownloadsDb::testGetUncollectedDownloads_data() {
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("appId");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QString>("hashAlgoString");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<QMap<QString, QString> >("headers");

    QTest::newRow("First Row") << UuidUtils::getDBusString(QUuid::createUuid())
    << "FIRST APP" << "first path" << QUrl("http://ubuntu.com") << ""
    << "md5" << QVariantMap() << QMap<QString, QString>();

    QVariantMap secondMetadata;
    secondMetadata["test"] = 1;
    secondMetadata["command"] = "cd";
    secondMetadata["hello"] = 23;

    QTest::newRow("Second Row") << UuidUtils::getDBusString(QUuid::createUuid())
    << "SECOND APP" << "second path" << QUrl("http://ubuntu.com/phone")
    << "" << "sha512" << secondMetadata << QMap<QString, QString>();

    QVariantMap thirdMetadata;
    secondMetadata["test"] = 3;
    secondMetadata["command"] = "return";
    secondMetadata["hello"] = 500;

    QMap<QString, QString> thirdHeaders;
    thirdHeaders["my-header"] = "I do something cool";

    QTest::newRow("Third Row") << UuidUtils::getDBusString(QUuid::createUuid())
    << "THIRD APP" << "third path" << QUrl("http://ubuntu.com/tablet")
    << "" << "sha384" << thirdMetadata << thirdHeaders;

}

void
TestDownloadsDb::testGetUncollectedDownloads() {
    _db->init();
    QFETCH(QString, id);
    QFETCH(QString, appId);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(QString, hashAlgoString);
    QFETCH(QVariantMap, metadata);
    QFETCH(StringMap, headers);

    QScopedPointer<FileDownload> fileDownload(new FileDownload(id, appId, path, true, "", url, hash,
                                                               hashAlgoString, metadata, headers));
    fileDownload->setState(Download::UNCOLLECTED);
    _db->storeSingleDownload(fileDownload.data());

    auto uncollected = _db->getUncollectedDownloads(appId);
    auto download = uncollected[0];
    QCOMPARE(download->metadata(), metadata);
}

QTEST_MAIN(TestDownloadsDb)

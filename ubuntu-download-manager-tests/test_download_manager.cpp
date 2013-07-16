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

#include <QSignalSpy>
#include "test_download_manager.h"

TestDownloadManager::TestDownloadManager(QObject *parent) :
    QObject(parent)
{
}

void TestDownloadManager::init()
{
    _conn = new FakeDBusConnection();
    _networkInfo = new FakeSystemNetworkInfo();
    _q = new FakeDownloadQueue(_networkInfo);
    _uuidFactory = new FakeUuidFactory();
    _man = new DownloadManager(_conn, _networkInfo, _q, _uuidFactory);
}

void TestDownloadManager::cleanup()
{
    if (_conn != NULL)
        delete _conn;
    if (_networkInfo)
        delete _networkInfo;
    if (_q != NULL)
        delete _q;
    if (_uuidFactory)
        delete _uuidFactory;
    if (_man)
        delete _man;
}

QCryptographicHash::Algorithm TestDownloadManager::algoFromString(const QString& data)
{
    QString algoLower = data.toLower();
    if (algoLower == "md5")
        return QCryptographicHash::Md5;
    else if (algoLower == "sha1")
        return QCryptographicHash::Sha1;
    else if (algoLower == "sha224")
        return QCryptographicHash::Sha224;
    else if (algoLower == "sha256")
        return QCryptographicHash::Sha256;
    else if (algoLower == "sha384")
        return QCryptographicHash::Sha384;
    else if (algoLower == "sha512")
        return QCryptographicHash::Sha512;
    return QCryptographicHash::Md5;
}

void TestDownloadManager::testCreateDownload_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<QVariantMap>("headers");

    QVariantMap firstMetadata, firstHeaders, secondHeaders, secondMetadata, thirdHeaders, thirdMetadata;

    firstMetadata["command"] = "cp test.com";
    firstMetadata["download-type"] = "click";

    firstHeaders["header1"] = "headerdata";
    firstHeaders["header2"] = "header2 data";

    QTest::newRow("First row") << "http://ubuntu.com" << firstMetadata << firstHeaders;

    secondMetadata["name"] = "paul";
    secondMetadata["download-type"] = "update";

    secondHeaders["Auth"] = "auth header";
    secondHeaders["test"] = "header2 data";

    QTest::newRow("Second row") << "http://ubuntu.com/phone" << secondMetadata << secondHeaders;

    thirdMetadata["command"] = "sudo update";
    thirdMetadata["download-type"] = "update";

    thirdHeaders["header1"] = "headerdata";
    thirdHeaders["header2"] = "header2 data";

    QTest::newRow("Last row") << "http://ubuntu.com/phablet" << thirdMetadata << thirdHeaders;
}

void TestDownloadManager::testCreateDownload()
{
    QFETCH(QString, url);
    QFETCH(QVariantMap, metadata);
    QFETCH(QVariantMap, headers);
    _q->record();
    _conn->record();

    // assert that the download is created with the corret info and that
    // we do connect the object to the dbus session
    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));
    _man->createDownload(url, metadata, headers);

    QCOMPARE(spy.count(), 1);

    // grab the created download and assert that it was created correctly with the data
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(1, calledMethods.count());

    Download* download = (Download*) calledMethods[0].params().inParams()[0];
    QCOMPARE(_uuidFactory->data(), download->downloadId());
    QCOMPARE(QUrl(url), download->url());

    QVariantMap downloadMetadata = download->metadata();

    foreach(const QString& key, metadata.keys())
    {
        QVERIFY(downloadMetadata.contains(key));
        QCOMPARE(metadata[key], downloadMetadata[key]);
    }

    QVariantMap downloadHeaders = download->headers();

    foreach(const QString& key, headers.keys())
    {
        QVERIFY(downloadHeaders.contains(key));
        QCOMPARE(headers[key], downloadHeaders[key]);
    }

    calledMethods = _conn->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("registerObject"), calledMethods[0].methodName());
}

void TestDownloadManager::testCreateDownloadWithHash_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("algo");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<QVariantMap>("headers");

    QVariantMap firstMetadata, firstHeaders, secondHeaders, secondMetadata, thirdHeaders, thirdMetadata;

    firstMetadata["command"] = "cp test.com";
    firstMetadata["download-type"] = "click";

    firstHeaders["header1"] = "headerdata";
    firstHeaders["header2"] = "header2 data";

    QTest::newRow("First row") << "http://ubuntu.com" << "md5" << "my-first-hash" << firstMetadata << firstHeaders;

    secondMetadata["name"] = "paul";
    secondMetadata["download-type"] = "update";

    secondHeaders["Auth"] = "auth header";
    secondHeaders["test"] = "header2 data";

    QTest::newRow("Second row") << "http://ubuntu.com/phone" << "sha1" << "my second hash" << secondMetadata << secondHeaders;

    thirdMetadata["command"] = "sudo update";
    thirdMetadata["download-type"] = "update";

    thirdHeaders["header1"] = "headerdata";
    thirdHeaders["header2"] = "header2 data";

    QTest::newRow("Last row") << "http://ubuntu.com/phablet" << "sha256" << "my-third-hash" << thirdMetadata << thirdHeaders;
}

void TestDownloadManager::testCreateDownloadWithHash()
{
    QFETCH(QString, url);
    QFETCH(QString, algo);
    QFETCH(QString, hash);
    QFETCH(QVariantMap, metadata);
    QFETCH(QVariantMap, headers);
    _q->record();
    _conn->record();

    // assert that the download is created with the corret info and that
    // we do connect the object to the dbus session
    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));
    _man->createDownloadWithHash(url, algo, hash, metadata, headers);

    QCOMPARE(spy.count(), 1);

    // grab the created download and assert that it was created correctly with the data
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(1, calledMethods.count());

    Download* download = (Download*) calledMethods[0].params().inParams()[0];
    QCOMPARE(_uuidFactory->data(), download->downloadId());
    QCOMPARE(QUrl(url), download->url());

    QVariantMap downloadMetadata = download->metadata();

    QCOMPARE(hash, download->hash());
    QCOMPARE(algoFromString(algo), download->hashAlgorithm());

    foreach(const QString& key, metadata.keys())
    {
        QVERIFY(downloadMetadata.contains(key));
        QCOMPARE(metadata[key], downloadMetadata[key]);
    }

    QVariantMap downloadHeaders = download->headers();

    foreach(const QString& key, headers.keys())
    {
        QVERIFY(downloadHeaders.contains(key));
        QCOMPARE(headers[key], downloadHeaders[key]);
    }

    calledMethods = _conn->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("registerObject"), calledMethods[0].methodName());
}

void TestDownloadManager::testGetAllDownloads()
{
    // add a number of downloads and assert that all the paths are returned
    _q->record();
    _conn->record();

    if (_man)
        delete _man;

    // do not use the fake uuid factory, else we only get one object path
    _man = new DownloadManager(_conn, _networkInfo, _q, new UuidFactory());

    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));

    QString firstUrl("http://www.ubuntu.com"), secondUrl("http://www.ubuntu.com/phone"), thirdUrl("http://www");
    QVariantMap firstMetadata, firstHeaders, secondMetadata, secondHeaders, thirdMetadata, thirdHeaders;

    _man->createDownload(firstUrl, firstMetadata, firstHeaders);
    _man->createDownload(secondUrl, secondMetadata, secondHeaders);
    _man->createDownload(thirdUrl, thirdMetadata, thirdHeaders);

    QCOMPARE(spy.count(), 3);

    // get the diff create downloads and theri paths so that we can assert that they are returned
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QList<QString> paths;
    for(int index=0; index < calledMethods.count(); index++)
    {
        Download* download = (Download*) calledMethods[index].params().inParams()[0];
        paths << download->path();
    }

    QList<QDBusObjectPath> allDownloads = _man->getAllDownloads();
    QCOMPARE(paths.count(), allDownloads.count());

    foreach(const QDBusObjectPath& path, allDownloads)
    {
        QVERIFY(paths.contains(path.path()));
    }
}

void TestDownloadManager::testAllDownloadsWithMetadata()
{
    // add a number of downloads and assert that all the paths are returned
    _q->record();
    _conn->record();

    if (_man)
        delete _man;

    // do not use the fake uuid factory, else we only get one object path
    _man = new DownloadManager(_conn, _networkInfo, _q, new UuidFactory());

    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));

    QString firstUrl("http://www.ubuntu.com"), secondUrl("http://www.ubuntu.com/phone"), thirdUrl("http://www");
    QVariantMap firstMetadata, firstHeaders, secondMetadata, secondHeaders, thirdMetadata, thirdHeaders;

    firstMetadata["type"] = "first";
    secondMetadata["type"] = "second";
    thirdMetadata["type"] = "first";

    _man->createDownload(firstUrl, firstMetadata, firstHeaders);
    _man->createDownload(secondUrl, secondMetadata, secondHeaders);
    _man->createDownload(thirdUrl, thirdMetadata, thirdHeaders);

    QCOMPARE(spy.count(), 3);

    // get the diff create downloads and theri paths so that we can assert that they are returned
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QList<Download*> downloads;
    for(int index=0; index < calledMethods.count(); index++)
    {
        Download* download = (Download*) calledMethods[index].params().inParams()[0];
        downloads << download;
    }

    QList<QDBusObjectPath> filtered = _man->getAllDownloadsWithMetadata("type", "first");

    QCOMPARE(2, filtered.count());
    QCOMPARE(filtered[0].path(), downloads[0]->path());
    QCOMPARE(filtered[1].path(), downloads[2]->path());
}

void TestDownloadManager::testSetThrottleNotDownloads_data()
{
    QTest::addColumn<qlonglong>("speed");

    QTest::newRow("First row") << 200LL;
    QTest::newRow("Second row") << 1212LL;
    QTest::newRow("Third row") << 998LL;
    QTest::newRow("Last row") << 60LL;
}

void TestDownloadManager::testSetThrottleNotDownloads()
{
    QFETCH(qlonglong, speed);
    _man->setDefaultThrottle(speed);
    QCOMPARE(_man->defaultThrottle(), speed);
}

void TestDownloadManager::testSetThrottleWithDownloads_data()
{
    QTest::addColumn<qlonglong>("speed");

    QTest::newRow("First row") << 200LL;
    QTest::newRow("Second row") << 1212LL;
    QTest::newRow("Third row") << 998LL;
    QTest::newRow("Last row") << 60LL;
}

void TestDownloadManager::testSetThrottleWithDownloads()
{
    QFETCH(qlonglong, speed);

    // add a number of downloads and assert that we do set their throttle limit
    if (_man)
        delete _man;

    // do not use the fake uuid factory, else we only get one object path
    _man = new DownloadManager(_conn, _networkInfo, _q, new UuidFactory());

    QString firstUrl("http://www.ubuntu.com"), secondUrl("http://www.ubuntu.com/phone"), thirdUrl("http://www");
    QVariantMap firstMetadata, firstHeaders, secondMetadata, secondHeaders, thirdMetadata, thirdHeaders;

    firstMetadata["type"] = "first";
    secondMetadata["type"] = "second";
    thirdMetadata["type"] = "first";

    _man->createDownload(firstUrl, firstMetadata, firstHeaders);
    _man->createDownload(secondUrl, secondMetadata, secondHeaders);
    _man->createDownload(thirdUrl, thirdMetadata, thirdHeaders);

    _man->setDefaultThrottle(speed);

    QList<MethodData> calledMethods = _q->calledMethods();
    for(int index=0; index < calledMethods.count(); index++)
    {
        Download* download = (Download*) calledMethods[index].params().inParams()[0];
        QCOMPARE(download->throttle(), speed);
    }

}
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

#ifndef TEST_APP_DOWNLOAD_H
#define TEST_APP_DOWNLOAD_H

#include <QDir>
#include <QObject>
#include <download.h>
#include <metatypes.h>
#include "fake_system_network_info.h"
#include "fake_request_factory.h"
#include "test_runner.h"

class TestDownload: public QObject
{
    Q_OBJECT
public:
    explicit TestDownload(QObject *parent = 0);

private slots:

    void init();
    void cleanup();

    // constructors tests
    void testNoHashConstructor();
    void testHashConstructor();

    // data function to be used for the accessor tests
    void testNoHashConstructor_data();
    void testHashConstructor_data();
    void testPath_data();
    void testUrl_data();
    void testProgress_data();
    void testOnSuccessHash_data();
    void testSetRawHeadersStart_data();
    void testSetRawHeadersResume_data();
    void testSetRawHeadersWithRangeStart_data();
    void testSetRawHeadersWithRangeResume_data();
    void testSetThrottleNoReply_data();
    void testSetThrottle_data();
    void testSetGSMDownloadSame_data();
    void testSetGSMDownloadDiff_data();
    void testCanDownloadGSM_data();
    void testCanDownloadNoGSM_data();

    // accessor methods
    void testPath();
    void testUrl();
    void testProgress();
    void testTotalSize();
    void testTotalSizeNoProgress();
    void testSetThrottleNoReply();
    void testSetThrottle();
    void testSetGSMDownloadSame();
    void testSetGSMDownloadDiff();
    void testCanDownloadGSM();
    void testCanDownloadNoGSM();

    // dbus method tests
    void testCancel();
    void testPause();
    void testResume();
    void testStart();

    // network related tests
    void testCancelDownload();
    void testCancelDownloadNotStarted();
    void testPauseDownload();
    void testPauseDownloadNotStarted();
    void testResumeRunning();
    void testResumeDownload();
    void testStartDownload();
    void testStartDownloadAlreadyStarted();
    void testOnSuccessNoHash();
    void testOnSuccessHashError();
    void testOnSuccessHash();
    void testOnHttpError();
    void testSetRawHeadersStart();
    void testSetRawHeadersWithRangeStart();
    void testSetRawHeadersResume();
    void testSetRawHeadersWithRangeResume();

private:
    bool removeDir(const QString& dirName);

private:
    QDir _testDir;
    QUuid _id;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    QString _path;
    QUrl _url;
    QCryptographicHash::Algorithm _algo;
    FakeSystemNetworkInfo* _networkInfo;
    FakeRequestFactory* _reqFactory;

};

Q_DECLARE_METATYPE(QNetworkInfo::NetworkMode)
DECLARE_TEST(TestDownload)

#endif // TEST_APP_DOWNLOAD_H

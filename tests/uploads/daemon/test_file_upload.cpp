/*
 * Copyright 2014 Canonical Ltd.
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

#include <matchers.h>
#include <network_reply.h>

#include "test_file_upload.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestFileUpload::init() {
    BaseTestCase::init();
    _id = "fake_id";
    _appId = "My APP";
    _path = "/dbus/path/upload";
    _isConfined = true;
    _rootPath = testDirectory();
    _filePath = dataDirectory() + "/otasigned.zip";

    _reqFactory = new MockRequestFactory();
    RequestFactory::setInstance(_reqFactory);
    _fileManager = new MockFileManager();
    FileManager::setInstance(_fileManager);
}

void
TestFileUpload::verifyMocks() {
    QVERIFY(Mock::VerifyAndClearExpectations(_reqFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(_fileManager));
}

void
TestFileUpload::cleanup() {
    BaseTestCase::cleanup();
    RequestFactory::deleteInstance();
    FileManager::deleteInstance();
}

void
TestFileUpload::testIsErrorWhenPathNotAbs() {
    // create an upload that uses a relative path
    QString relativePath = "../data/otasigned.zip";
    auto error = QString("Path is not absolute: '%1'").arg(relativePath);
    QScopedPointer<FileUpload> upload(
        new FileUpload(_id, _appId, _path, _isConfined, _rootPath, _url, relativePath,
            _metadata, _headers));
    QVERIFY(!upload->isValid());
    QCOMPARE(error, upload->lastError());
}

void
TestFileUpload::testIsErrorWhenPathMissing() {
    auto missingPath = dataDirectory() + "/missing.tar.xz";
    auto error = QString("Path does not exist: '%1'").arg(missingPath);
    QScopedPointer<FileUpload> upload(
        new FileUpload(_id, _appId, _path, _isConfined, _rootPath, _url, missingPath,
            _metadata, _headers));
    QVERIFY(!upload->isValid());
    QCOMPARE(error, upload->lastError());
}

void
TestFileUpload::testIsPausable() {
    // mock expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(nullptr));

    QScopedPointer<FileUpload> upload(
        new FileUpload(_id, _appId, _path, _isConfined, _rootPath, _url, _filePath,
            _metadata, _headers));
    QVERIFY(!upload->pausable());
    verifyMocks();
}

void
TestFileUpload::testStartCannotOpenFile() {
    // use a fake file so that we can fail the open and test that error is
    // returned
    auto file = new MockFile("test");

    // mocks expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(_))
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto upload = new FileUpload(_id, _appId, _path, _isConfined, _rootPath,
        _url, _filePath, _metadata, _headers);

    SignalBarrier spy(upload, SIGNAL(started(bool)));

    upload->start();  // change state
    upload->startTransfer();

    QVERIFY(spy.ensureSignalEmitted());
    QTRY_COMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());

    delete upload;

    verifyMocks();
}

void
TestFileUpload::testStartCorrectHeaders() {
    // use a fake file so that we can fail the open and test that error is
    // returned
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // mocks expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*_reqFactory, post(RequestDoesNotHaveHeader("Content-Length"), _))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    auto upload = new FileUpload(_id, _appId, _path, _isConfined, _rootPath,
        _url, _filePath, _metadata, _headers);

    SignalBarrier spy(upload, SIGNAL(started(bool)));

    upload->start();  // change state
    upload->startTransfer();

    QVERIFY(spy.ensureSignalEmitted());
    QTRY_COMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete upload;

    verifyMocks();
}

void
TestFileUpload::testSetThrottlePresentReply() {
    // use a fake file so that we can fail the open and test that error is
    // returned
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // mocks expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*_reqFactory, post(_, _))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(2);

    auto upload = new FileUpload(_id, _appId, _path, _isConfined, _rootPath,
        _url, _filePath, _metadata, _headers);

    SignalBarrier spy(upload, SIGNAL(started(bool)));

    upload->start();  // change state
    upload->startTransfer();

    upload->setThrottle(2);

    QVERIFY(spy.ensureSignalEmitted());
    QTRY_COMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete upload;

    verifyMocks();
}

void
TestFileUpload::testFinishedEmitted() {
    // fake the finish of the upload and ensure that we do get the signal
    auto file = new MockFile("test");
    auto responseFile = new MockFile("response");
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());
    QByteArray responseData(5000, 'f');

    // mocks expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(2)
        .WillOnce(Return(file))
        .WillOnce(Return(responseFile));

    EXPECT_CALL(*file, open(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*responseFile, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*responseFile, write(responseData))
        .Times(1);

    EXPECT_CALL(*responseFile, close())
        .Times(1);

    EXPECT_CALL(*_reqFactory, post(RequestDoesNotHaveHeader("Content-Length"), _))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), readAll())
        .Times(1)
        .WillOnce(Return(responseData));

    auto upload = new FileUpload(_id, _appId, _path, _isConfined, _rootPath,
        _url, _filePath, _metadata, _headers);

    SignalBarrier startSpy(upload, SIGNAL(started(bool)));

    upload->start();  // change state
    upload->startTransfer();

    // emit the finished signal from the reply

    QVERIFY(startSpy.ensureSignalEmitted());
    QTRY_COMPARE(startSpy.count(), 1);

    QList<QVariant> arguments = startSpy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    SignalBarrier finishSpy(upload, SIGNAL(finished(const QString&)));

    reply->finished();

    QVERIFY(finishSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishSpy.count(), 1);

    delete upload;

    verifyMocks();
}

void
TestFileUpload::testUploadProgressEmitted() {
    // emit the process signal an test that it does work
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // mocks expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*_reqFactory, post(_, _))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    auto upload = new FileUpload(_id, _appId, _path, _isConfined, _rootPath,
        _url, _filePath, _metadata, _headers);

    SignalBarrier startSpy(upload, SIGNAL(started(bool)));

    upload->start();  // change state
    upload->startTransfer();

    QVERIFY(startSpy.ensureSignalEmitted());
    QTRY_COMPARE(startSpy.count(), 1);

    QList<QVariant> arguments = startSpy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    SignalBarrier progressSpy(upload, SIGNAL(progress(qulonglong, qulonglong)));

    reply->uploadProgress(12, 90);

    QVERIFY(progressSpy.ensureSignalEmitted());
    QTRY_COMPARE(progressSpy.count(), 1);

    delete upload;

    verifyMocks();
}

QTEST_MAIN(TestFileUpload)

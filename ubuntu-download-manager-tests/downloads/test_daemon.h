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

#ifndef TEST_DOWNLOAD_DAEMON_H
#define TEST_DOWNLOAD_DAEMON_H

#include <QObject>
#include <downloads/daemon.h>
#include "fakes/fake_application.h"
#include "fakes/fake_dbus_connection.h"
#include "fakes/fake_download_manager.h"
#include "fakes/fake_timer.h"
#include "base_testcase.h"
#include "test_runner.h"

class TestDaemon : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDaemon(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testStart();
    void testStartPath();
    void testStartFailServiceRegister();
    void testStartFailObjectRegister();
    void testTimerStop();
    void testTimerStart();
    void testTimeoutExit();
    void testDisableTimeout();
    void testSelfSignedCerts();
    void testSelfSignedCertsMissingPath();
    void testStoppable_data();
    void testStoppable();
    void testSetTimeout_data();
    void testSetTimeout();
    void testSetSelfSignedSslCerts();

 private:
    FakeTimer* _timer;
    FakeApplication* _app;
    QSharedPointer<Application> _appPointer;
    FakeDownloadManager* _man;
    QSharedPointer<FakeDBusConnection> _conn;
    Daemon* _daemon;
};

DECLARE_TEST(TestDaemon)

#endif  // TEST_DOWNLOAD_DAEMON_H
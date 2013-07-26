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

#ifndef DOWNLOADER_LIB_PROCESS_H
#define DOWNLOADER_LIB_PROCESS_H

#include <QObject>
#include <QProcess>

class ProcessPrivate;
class Process : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Process)

 public:
    explicit Process(QObject *parent = 0);

    virtual void start(const QString& program,
                       const QStringList& arguments,
                       QProcess::OpenMode mode = QProcess::ReadWrite);

 signals:
    void error(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

 private:
    Q_PRIVATE_SLOT(d_func(), void onReadyReadStandardError())
    Q_PRIVATE_SLOT(d_func(), void onReadyReadStandardOutput())

 private:
    // use pimpl so that we can mantains ABI compatibility
    ProcessPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_PROCESS_H

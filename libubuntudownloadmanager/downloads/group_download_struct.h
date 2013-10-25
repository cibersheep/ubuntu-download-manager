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

#ifndef DOWNLOADER_LIB_GROUP_DOWNLOAD_STRUCT_H
#define DOWNLOADER_LIB_GROUP_DOWNLOAD_STRUCT_H

#include <QDBusArgument>

class GroupDownloadStruct {
    Q_PROPERTY(QString url READ getUrl)
    Q_PROPERTY(QString hash READ getHash)
    Q_PROPERTY(QString localFile READ getLocalFile)

 public:
    GroupDownloadStruct();
    GroupDownloadStruct(const QString& url, const QString& localFile,
        const QString& hash);
    GroupDownloadStruct(const GroupDownloadStruct& other);
    GroupDownloadStruct& operator=(const GroupDownloadStruct& other);
    ~GroupDownloadStruct();

    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const GroupDownloadStruct& group);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        GroupDownloadStruct& group);

    // register Secret with the Qt type system
    static void registerMetaType();

    // property getters
    QString getUrl();
    QString getHash();
    QString getLocalFile();

 private:
    QString _url;
    QString _localFile;
    QString _hash;
};

Q_DECLARE_METATYPE(GroupDownloadStruct)

#endif  // DOWNLOADER_LIB_GROUP_DOWNLOAD_STRUCT_H
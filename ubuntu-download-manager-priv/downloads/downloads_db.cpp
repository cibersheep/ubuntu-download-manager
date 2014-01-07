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

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <ubuntu/download_manager/system/hash_algorithm.h>
#include "downloads/downloads_db.h"
#include "system/logger.h"

#define SINGLE_DOWNLOAD_TABLE "CREATE TABLE SingleDownload("\
    "uuid VARCHAR(40) PRIMARY KEY, "\
    "url TEXT NOT NULL, "\
    "dbus_path TEXT NOT NULL UNIQUE, "\
    "local_path TEXT, "\
    "hash TEXT, "\
    "hash_algo TEXT, "\
    "state VARCHAR(6) NOT NULL, "\
    "total_size TEXT, "\
    "throttle TEXT, "\
    "metadata TEXT, "\
    "headers TEXT)"

#define GROUP_DOWNLOAD_TABLE "CREATE TABLE GroupDownload("\
    "uuid VARCHAR(40) PRIMARY KEY, "\
    "dbus_path TEXT NOT NULL UNIQUE, "\
    "state INTEGER NOT NULL, "\
    "total_size TEXT, "\
    "throttle TEXT, "\
    "metadata TEXT, "\
    "headers TEXT)"

#define GROUP_DOWNLOAD_RELATION "CREATE TABLE GroupDownloadDownloads("\
    "group_id VARCHAR(36), "\
    "download_id VARCHAR(36), "\
    "FOREIGN KEY(group_id) REFERENCES GroupDownload(uuid), "\
    "FOREIGN KEY(download_id) REFERENCES SingleDownload(uuid))"

#define PRESENT_SINGLE_DOWNLOAD "SELECT count(uuid) FROM SingleDownload "\
    "WHERE uuid=:uuid;"

#define INSERT_SINGLE_DOWNLOAD "INSERT INTO SingleDownload("\
    "uuid, url, dbus_path, local_path, hash, hash_algo, state, total_size, "\
    "throttle, metadata, headers) VALUES (:uuid, :url, :dbus_path, "\
    ":local_path, :hash, :hash_algo, :state, :total_size, :throttle, "\
    ":metadata, :headers)"

#define UPDATE_SINGLE_DOWNLOAD "UPDATE SingleDownload SET "\
    "url=:url, dbus_path=:dbus_path, local_path=:local_path, "\
    "hash=:hash, hash_algo=:hash_algo, state=:state, total_size=:total_size, "\
    "throttle=:throttle, metadata=:metadata, headers=:headers "\
    "WHERE uuid=:uuid"

#define IDLE_STRING "idle"
#define START_STRING "start"
#define PAUSE_STRING "pause"
#define RESUME_STRING "resume"
#define CANCEL_STRING "cancel"
#define FINISH_STRING "finish"
#define ERROR_STRING "error"

namespace Ubuntu {

namespace DownloadManager {

DownloadsDb::DownloadsDb(QObject* parent)
    : QObject(parent) {
    _fileManager = FileManager::instance();
    internalInit();
}

QSqlDatabase
DownloadsDb::db() {
    return _db;
}

QString
DownloadsDb::filename() {
    return _dbName;
}

bool
DownloadsDb::dbExists() {
    return _fileManager->exists(_dbName);
}

void
DownloadsDb::internalInit() {
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::DataLocation);
    QString path = dataPath + QDir::separator() + "ubuntu-download-manager";

    bool wasCreated = QDir().mkpath(path);
    if (!wasCreated) {
        qCritical() << "Could not create the data path" << path;
    }
    _dbName = path + QDir::separator() + "downloads.db";
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(_dbName);
    qDebug() << "Db file is" << _dbName;
}

bool
DownloadsDb::init() {
    TRACE;
    // create the required tables
    bool opened = _db.open();
    if (!opened) {
        qCritical() << _db.lastError();
        return false;
    }

    _db.transaction();

    // create the required tables and indexes
    bool success = true;
    QSqlQuery query;
    success &= query.exec(SINGLE_DOWNLOAD_TABLE);
    success &= query.exec(GROUP_DOWNLOAD_TABLE);
    success &= query.exec(GROUP_DOWNLOAD_RELATION);

    if (success)
        _db.commit();
    else
        _db.rollback();
    _db.close();
    return success;
}

QString
DownloadsDb::stateToString(Download::State state) {
    switch (state) {
        case Download::IDLE:
            return IDLE_STRING;
        case Download::START:
            return START_STRING;
        case Download::PAUSE:
            return PAUSE_STRING;
        case Download::RESUME:
            return RESUME_STRING;
        case Download::CANCEL:
            return CANCEL_STRING;
        case Download::FINISH:
            return FINISH_STRING;
        case Download::ERROR:
            return ERROR_STRING;
        default:
            return IDLE_STRING;
    }
}

Download::State
DownloadsDb::stringToState(QString state) {
    QString lowerState = state.toLower();
    if (lowerState == IDLE_STRING)
        return Download::IDLE;
    if (lowerState == START_STRING)
        return Download::START;
    if (lowerState == PAUSE_STRING)
        return Download::PAUSE;
    if (lowerState == RESUME_STRING)
        return Download::RESUME;
    if (lowerState == CANCEL_STRING)
        return Download::CANCEL;
    if (lowerState == FINISH_STRING)
        return Download::FINISH;
    if (lowerState == ERROR_STRING)
        return Download::ERROR;

    // default case
    return Download::IDLE;
}

QString
DownloadsDb::metadataToString(const QVariantMap& metadata) {
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(QVariant(metadata));
    return QString(jsonDoc.toJson());
}

QString
DownloadsDb::headersToString(const QMap<QString, QString>& headers) {
    QVariantMap headersVariant;
    foreach(const QString& key, headers.keys()) {
        headersVariant[key] = headers[key];
    }
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(
        QVariant(headersVariant));
    return QString(jsonDoc.toJson());
}

bool
DownloadsDb::storeSingleDownload(FileDownload* download) {
    // decide if we store it as a new download or update an existing one
    bool opened = _db.open();

    if (!opened) {
        qCritical() << _db.lastError();
        return false;
    }

    QSqlQuery query;
    query.prepare(PRESENT_SINGLE_DOWNLOAD);
    query.bindValue(":uuid", download->downloadId());

    query.exec();
    int rows = 0;
    if (query.next())
        rows = query.value(0).toInt();

    if (rows > 0) {
        qDebug() << "Update download";
        query.prepare(UPDATE_SINGLE_DOWNLOAD);
    } else {
        qDebug() << "Insert download";
        query.prepare(INSERT_SINGLE_DOWNLOAD);
    }

    query.bindValue(":uuid", download->downloadId());
    query.bindValue(":url", download->url().toString());
    query.bindValue(":dbus_path", download->path());
    query.bindValue(":local_path", download->filePath());
    query.bindValue(":hash", download->hash());
    query.bindValue(":hash_algo",
        HashAlgorithm::getHashAlgo(download->hashAlgorithm()));
    query.bindValue(":state", stateToString(download->state()));
    query.bindValue(":total_size",
        QString::number(download->totalSize()));
    query.bindValue(":throttle",
        QString::number(download->throttle()));
    query.bindValue(":metadata",
        metadataToString(download->metadata()));
    query.bindValue(":headers",
        headersToString(download->headers()));

    bool success = query.exec();
    if (!success)
        qDebug() << query.lastError();

    _db.close();

    return success;
}


}  // DownloadManager

}  // Ubuntu
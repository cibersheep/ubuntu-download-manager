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

#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QSslError>
#include <ubuntu/download_manager/metadata.h>
#include <ubuntu/download_manager/system/hash_algorithm.h>
#include "downloads/file_download.h"
#include "system/filename_mutex.h"
#include "system/logger.h"
#include "system/network_reply.h"

#define DOWN_LOG(LEVEL) LOG(LEVEL) << ((parent() != nullptr)?"GroupDownload {" + parent()->objectName() + " } ":"") << "Download ID{" << objectName() << " } "


namespace {

    const QString DATA_FILE_NAME = "data.download";
    const QString NETWORK_ERROR = "NETWORK ERROR";
    const QString HASH_ERROR = "HASH ERROR";
    const QString COMMAND_ERROR = "COMMAND ERROR";
    const QString SSL_ERROR = "SSL ERROR";
    const QString FILE_SYSTEM_ERROR = "FILE SYSTEM ERROR: %1";
    const QString TEMP_EXTENSION = ".tmp";
    const QString AUTH_ERROR = "AUTHENTICATION ERROR";
    const QString PROXY_AUTH_ERROR = "PROXY_AUTHENTICATION ERROR";
    const QString UNEXPECTED_ERROR = "UNEXPECTED_ERROR";
}

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

FileDownload::FileDownload(const QString& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers, parent),
      _totalSize(0),
      _url(url),
      _hash(""),
      _algo(QCryptographicHash::Md5) {
    init();
}

FileDownload::FileDownload(const QString& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QString& hash,
                   const QString& algo,
                   const QVariantMap& metadata,
                   const QMap<QString, QString> &headers,
                   QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers, parent),
      _totalSize(0),
      _url(url),
      _hash(hash) {
    init();
    _algo = HashAlgorithm::getHashAlgo(algo);
    // check that the algorithm is correct if the hash is not empty
    if (!_hash.isEmpty() && !HashAlgorithm::isValidAlgo(algo)) {
        setIsValid(false);
        setLastError(QString("Invalid hash algorithm: '%1'").arg(algo));
    }
}

FileDownload::~FileDownload() {
    if (_currentData != nullptr) {
        _currentData->close();
    }
    delete _currentData;
    delete _reply;
}

void
FileDownload::cancelDownload() {
    TRACE << _url;

    if (_reply != nullptr) {
        // disconnect so that we do not get useless signals
        // and remove the reply
        disconnectFromReplySignals();
        _reply->abort();
        _reply->deleteLater();
        _reply = nullptr;
    }

    // remove current data and metadata
    cleanUpCurrentData();
    // unlock the file name so that other downloads can use it it if is
    // no used in the file system
    unlockFilePath();
    _downloading = false;
    emit canceled(true);
}

void
FileDownload::pauseDownload() {
    TRACE << _url;

    if (_reply == nullptr) {
        // cannot pause because is not running
        DOWN_LOG(INFO) << "Cannot pause download because reply is NULL";
        DOWN_LOG(INFO) << "EMIT paused(false)";
        emit paused(false);
        return;
    }

    DOWN_LOG(INFO) << "Pausing download" << _url;
    // we need to disconnect the signals to ensure that they are not
    // emitted due to the operation we are going to perform. We read
    // the data in the reply and store it in a file
    disconnectFromReplySignals();

    // do abort before reading
    _reply->abort();
    _currentData->write(_reply->readAll());
    if (!flushFile()) {
        emit paused(false);
    } else {
        _reply->deleteLater();
        _reply = nullptr;
        DOWN_LOG(INFO) << "EMIT paused(true)";
        _downloading = false;
        emit paused(true);
    }
}

void
FileDownload::resumeDownload() {
    DOWN_LOG(INFO) << __PRETTY_FUNCTION__ << _url;

    if (_reply != nullptr) {
        // cannot resume because it is already running
        DOWN_LOG(INFO) << "Cannot resume download because reply != NULL";
        DOWN_LOG(INFO) << "EMIT resumed(false)";
        emit resumed(false);
        return;
    }

    DOWN_LOG(INFO) << "Resuming download.";
    QNetworkRequest request = buildRequest();

    // overrides the range header, we do not let clients set the range!!!
    qint64 currentDataSize = _currentData->size();
    QByteArray rangeHeaderValue = "bytes=" +
        QByteArray::number(currentDataSize) + "-";
    request.setRawHeader("Range", rangeHeaderValue);

    _reply = _requestFactory->get(request);
    _reply->setReadBufferSize(throttle());

    connectToReplySignals();

    DOWN_LOG(INFO) << "EMIT resumed(true)";
    _downloading = true;
    emit resumed(true);
}

void
FileDownload::startDownload() {
    TRACE << _url;

    if (_reply != nullptr) {
        // the download was already started, lets say that we did it
        DOWN_LOG(INFO) << "Cannot start download because reply != NULL";
        DOWN_LOG(INFO) << "EMIT started(false)";
        emit started(true);
        return;
    }

    // create file that will be used to maintain the state of the
    // download when resumed.
    _currentData = FileManager::instance()->createFile(_tempFilePath);
    bool canWrite = _currentData->open(QIODevice::ReadWrite | QFile::Append);

    if (!canWrite) {
        emit started(false);
    }

    DOWN_LOG(INFO) << "Network is accessible, performing download request";
    // signals should take care of calling deleteLater on the
    // NetworkReply object
    _reply = _requestFactory->get(buildRequest());
    _reply->setReadBufferSize(throttle());

    connectToReplySignals();
    DOWN_LOG(INFO) << "EMIT started(true)";
    _downloading = true;
    emit started(true);
}

qulonglong
FileDownload::progress() {
    return (_currentData == nullptr) ? 0 : _currentData->size();
}

qulonglong
FileDownload::totalSize() {
    return _totalSize;
}

void
FileDownload::setThrottle(qulonglong speed) {
    TRACE << _url;
    Download::setThrottle(speed);
    if (_reply != nullptr)
        _reply->setReadBufferSize(speed);
}

void
FileDownload::onDownloadProgress(qint64 currentProgress, qint64 bytesTotal) {
    TRACE << _url << currentProgress << bytesTotal;

    // write the current info we have, just in case we are killed in the
    // middle of the download
    _currentData->write(_reply->readAll());
    if (!flushFile()) {
        return;
    }
    qulonglong received = _currentData->size();

    if (bytesTotal == -1) {
        // we do not know the size of the download, simply return
        // the same for received and for total
        emit Download::progress(received, received);
        return;
    } else {
        if (_totalSize == 0) {
            qlonglong uBytestTotal = bytesTotal;
            // bytesTotal is different when we have resumed because we
            // are not counting the size that  we already downloaded,
            // therefore we only do this once
            // update the metadata
            _totalSize = uBytestTotal;
        }
        emit Download::progress(received, _totalSize);
        return;
    }
}

void
FileDownload::onError(QNetworkReply::NetworkError code) {
    DOWN_LOG(ERROR) << _url << " ERROR:" << ":" << code;
    _downloading = false;
    QString msg;
    QString errStr;

    // decide if we are talking about an http error or no
    auto statusCode = _reply->attribute(
        QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid()) {
        auto status = statusCode.toInt();
        if (status >= 300) {
            auto reasonVar = _reply->attribute(
                QNetworkRequest::HttpReasonPhraseAttribute);
            if (reasonVar.isValid()) {
                msg = reasonVar.toString();
            } else {
                msg = "";
            }
            HttpErrorStruct err(status, msg);
            emit httpError(err);
            errStr = NETWORK_ERROR;
        }
    } else {
        if (code == QNetworkReply::AuthenticationRequiredError) {
            AuthErrorStruct err(AuthErrorStruct::Server, _reply->errorString());
            emit authError(err);
            errStr = AUTH_ERROR;
        } else if (code == QNetworkReply::ProxyAuthenticationRequiredError) {
            AuthErrorStruct err(AuthErrorStruct::Proxy, _reply->errorString());
            emit authError(err);
            errStr = PROXY_AUTH_ERROR;
        } else {
            NetworkErrorStruct err(code, _reply->errorString());
            emit networkError(err);
        }
    }
    emitError(errStr);
}

void
FileDownload::onRedirect(QUrl redirect) {
    DOWN_LOG(INFO) << "Following redirect to" << redirect;
    // update the _url value and perform a second request to try and get the data
    if (_visitedUrls.contains(redirect)) {
        // we are in a loop!!! we have to raise an error about this.
        DOWN_LOG(WARNING) << "Redirect loop found";
        NetworkErrorStruct err(QNetworkReply::ContentNotFoundError);
        emit networkError(err);
        emitError(NETWORK_ERROR);
        return;
    }
    _visitedUrls.append(_url);
    _url = redirect;

    // clean the reply
    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = nullptr;

    // clean the local file without unlocking the file the reason for
    // this is that the file pat is going to be reused to store the
    // data from the redirect
    cleanUpCurrentData();

    // perform again the request but do not emit started signal
    _currentData = FileManager::instance()->createFile(_tempFilePath);
    bool canWrite = _currentData->open(QIODevice::ReadWrite | QFile::Append);

    if (!canWrite) {
        emitError(FILE_SYSTEM_ERROR);
        return;
    }

    _reply = _requestFactory->get(buildRequest());
    _reply->setReadBufferSize(throttle());

    connectToReplySignals();
}

void
FileDownload::onDownloadCompleted() {
    TRACE << _url;

    // if the hash is present we check it
    if (!_hash.isEmpty()) {
        emit processing(filePath());
        _currentData->reset();
        QCryptographicHash hash(_algo);
        // addData is smart enough to not load the entire file in memory
        hash.addData(_currentData->device());
        QString fileSig = QString(hash.result().toHex());

        if (fileSig != _hash) {
            DOWN_LOG(ERROR) << HASH_ERROR << fileSig << "!=" << _hash;
            emitError(HASH_ERROR);
            return;
        }
    }

    // there are two possible cases, the first, we do not have the metadata
    // info to execute a command once the download was finished and that
    // means we are done here else we execute the command AND raise the
    // finish signals once the command was done (or an error occurred in
    // the command execution.
    if (metadata().contains(Metadata::COMMAND_KEY)) {
        // just emit processing if we DO NOT have a hash because else we
        // already emitted it.
        if (_hash.isEmpty()) {
            emit processing(filePath());
        }
        // toStringList will return an empty list if it cannot be converted
        QStringList commandData =
            metadata()[Metadata::COMMAND_KEY].toStringList();
        if (commandData.count() == 0) {
            DOWN_LOG(ERROR) << "COMMAND DATA MISSING";
            emitError(COMMAND_ERROR);
            return;
        } else {
            // first item of the string list is the command
            // the rest is the arguments
            QString command = commandData.at(0);
            commandData.removeAt(0);
            QStringList args;

            foreach(const QString& arg, commandData) {
                if (arg == Metadata::COMMAND_FILE_KEY)
                    args << filePath();
                else
                    args << arg;
            }

            Process* postDownloadProcess =
                ProcessFactory::instance()->createProcess();

            // connect to signals so that we can tell the clients that
            // the operation succeed

            connect(postDownloadProcess, &Process::finished,
                this, &FileDownload::onProcessFinished);
            connect(postDownloadProcess, &Process::error,
                this, &FileDownload::onProcessError);

            DOWN_LOG(INFO) << "Executing" << command << args;
            postDownloadProcess->start(command, args);
            return;
        }
    } else {
        emitFinished();
    }

    // clean the reply
    _reply->deleteLater();
    _reply = nullptr;
}

void
FileDownload::onFinished() {
    TRACE << _url;
    // the reply has finished but the resource might have been moved
    // and we must do a new request
    auto redirect = _reply->attribute(
        QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if(!redirect.isEmpty() && redirect != _url) {
        onRedirect(redirect);
    } else {
        onDownloadCompleted();
    }
}

void
FileDownload::onSslErrors(const QList<QSslError>& errors) {
    TRACE << errors;
    if (!_reply->canIgnoreSslErrors(errors)) {
        _downloading = false;
        emitError(SSL_ERROR);
    }
}

void
FileDownload::onProcessError(QProcess::ProcessError error) {
    auto p = qobject_cast<Process*>(sender());
    auto standardOut = p->readAllStandardOutput();
    auto standardErr = p->readAllStandardError();
    DOWN_LOG(ERROR) << "Error " << error << "executing"
        << p->program() << "with args" << p->arguments()
        << "Stdout:" << standardOut << "Stderr:" << standardErr;
    p->deleteLater();
    ProcessErrorStruct err(error, 0, standardOut, standardErr);
    emit processError(err);
    emitError(COMMAND_ERROR);
}

void
FileDownload::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    TRACE << exitCode << exitStatus;
    auto p = qobject_cast<Process*>(sender());
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        // remove the file since we are done with it
        cleanUpCurrentData();
        emitFinished();
        // remove the file because that is the contract that we have with
        // the clients
        auto fileMan = FileManager::instance();

        if (fileMan->exists(_tempFilePath)) {
            LOG(INFO) << "Removing '" << _tempFilePath << "'";
            fileMan->remove(_tempFilePath);
        }

        if (fileMan->exists(_filePath)) {
            LOG(INFO) << "Removing '" << _filePath << "'";
            fileMan->remove(_filePath);
        }
    } else {
        auto standardOut = p->readAllStandardOutput();
        auto standardErr = p->readAllStandardError();
        ProcessErrorStruct err(exitStatus, "ErrorInProcess", exitCode,
            standardOut, standardErr);
        emit processError(err);
        emitError(COMMAND_ERROR);
    }
    p->deleteLater();
}

void
FileDownload::onOnlineStateChanged(bool online) {
    TRACE << online;
    _connected = online;
    // if we are downloading and the status is correct let's call
    // the method again, else do nothing
    if (_connected && _downloading) {
        Download::State currentState = state();
        if (currentState == Download::START || currentState == Download::RESUME) {
            resumeDownload();
        }
    }

    // if no longer online yet we have a reply (that is, we are trying
    // to get data from the missing connection) we pause
    if (!_connected && _reply != nullptr) {
        pauseDownload();
        // set it to be downloading even when pause download sets it
        // to false
        _downloading = true;
    }
}

void
FileDownload::init() {
    _requestFactory = RequestFactory::instance();
    _fileNameMutex = System::FileNameMutex::instance();
    SystemNetworkInfo* networkInfo = SystemNetworkInfo::instance();
    _connected = networkInfo->isOnline();
    _downloading = false;

    // connect to the network changed signals
    connect(networkInfo, &SystemNetworkInfo::onlineStateChanged,
        this, &FileDownload::onOnlineStateChanged);

    initFileNames();

    // ensure that the download is valid
    if (!_url.isValid()) {
        setIsValid(false);
        setLastError(QString("Invalid URL: '%1'").arg(_url.toString()));
    }
}

void
FileDownload::connectToReplySignals() {
    if (_reply != nullptr) {
        connect(_reply, &NetworkReply::downloadProgress,
            this, &FileDownload::onDownloadProgress);
        connect(_reply, &NetworkReply::error,
            this, &FileDownload::onError);
        connect(_reply, &NetworkReply::finished,
            this, &FileDownload::onFinished);
        connect(_reply, &NetworkReply::sslErrors,
            this, &FileDownload::onSslErrors);
    }
}

void
FileDownload::disconnectFromReplySignals() {
    if (_reply != nullptr) {
        disconnect(_reply, &NetworkReply::downloadProgress,
            this, &FileDownload::onDownloadProgress);
        disconnect(_reply, &NetworkReply::error,
            this, &FileDownload::onError);
        disconnect(_reply, &NetworkReply::finished,
            this, &FileDownload::onFinished);
        disconnect(_reply, &NetworkReply::sslErrors,
            this, &FileDownload::onSslErrors);
    }
}

bool
FileDownload::flushFile() {
    auto flushed  = _currentData->flush();
    if (!flushed) {
        auto err = _currentData->error();
        DOWN_LOG(ERROR) << "Could not write that in the file system" << err;
        emitError(QString(FILE_SYSTEM_ERROR).arg(err));
    }
    return flushed;
}

void
FileDownload::initFileNames() {
    // the mutex will ensure that we do not have race conditions about
    // the file names in the download manager
    QString path = _url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = DATA_FILE_NAME;

    auto metadataMap = metadata();

    if (!isConfined() && metadataMap.contains(Metadata::LOCAL_PATH_KEY)) {
        _filePath = metadataMap[Metadata::LOCAL_PATH_KEY].toString();
        _tempFilePath = _fileNameMutex->lockFileName(
            _filePath + TEMP_EXTENSION);

        // in this case and because the app is not confined we are
        // going to check if the file exists, if it does we will
        // raise an error
        if (QFile::exists(_filePath)) {
            setIsValid(false);
            setLastError(QString("File already exists at: '%1'").arg(
                _filePath));
        }
    } else {
        auto desiredPath = rootPath() + QDir::separator() + basename;
        _filePath = _fileNameMutex->lockFileName(desiredPath);
        _tempFilePath = _filePath + TEMP_EXTENSION;
    }
}

void
FileDownload::emitFinished() {
    auto fileMan = FileManager::instance();

    DOWN_LOG(INFO) << "EMIT finished" << filePath();
    setState(Download::FINISH);

    if (fileMan->exists(_tempFilePath)) {
        DOWN_LOG(INFO) << "Rename '" << _tempFilePath << "' to '"
            << _filePath << "'";
        fileMan->rename(_tempFilePath, _filePath);
    }
    unlockFilePath();
    emit finished(_filePath);
}

void
FileDownload::unlockFilePath() {
    if (!isConfined() && metadata().contains(Metadata::LOCAL_PATH_KEY)) {
        _fileNameMutex->unlockFileName(_tempFilePath);
	} else { 
        _fileNameMutex->unlockFileName(_filePath);
    }
}

void
FileDownload::cleanUpCurrentData() {
    bool success = true;
    QFile::FileError error = QFile::NoError;
    if (_currentData != nullptr) {
        success = _currentData->remove();

        if (!success)
            error = _currentData->error();

        _currentData->deleteLater();
        _currentData = nullptr;
    } else {
        QScopedPointer<QFile> tempFile(new QFile(_tempFilePath));
        success = tempFile->remove();
        if (!success)
            error = tempFile->error();
    }

    if (!success)
        qWarning() << "Error " << error <<
            "removing file with path" << _filePath;
}

QNetworkRequest
FileDownload::buildRequest() {
    QNetworkRequest request = QNetworkRequest(_url);
    QMap<QString, QString> headersMap = headers();
    foreach(const QString& header, headersMap.keys()) {
        QString data = headersMap[header];
        if (header.toLower() == "range"
                || header.toLower() == "accept-encoding")
            // do not add the header
            continue;
        request.setRawHeader(header.toUtf8(), data.toUtf8());
    }
    // very important we must ensure that we do not decompress any download
    // else we will have an error in the checksum for example #1224678
    request.setRawHeader("Accept-Encoding", "identity");
    return request;
}

void
FileDownload::emitError(const QString& error) {
    TRACE << error;
    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = nullptr;
    cleanUpCurrentData();
    // let other downloads use the same file name
    unlockFilePath();
    Download::emitError(error);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu

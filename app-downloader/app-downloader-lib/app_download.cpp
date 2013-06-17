#include <QBuffer>
#include <QDebug>
#include <QNetworkReply>
#include <QSignalMapper>
#include <QTemporaryFile>
#include "app_download.h"



/**
 * PRIVATE IMPLEMENATION
 */

class AppDownloadPrivate
{
    Q_DECLARE_PUBLIC(AppDownload)
public:
    explicit AppDownloadPrivate(QString path, QUrl url, QNetworkAccessManager* nam, AppDownload* parent);
    explicit AppDownloadPrivate(QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
        QNetworkAccessManager* nam, AppDownload* parent);

    // public methods
    QString path();
    QUrl url();
    AppDownload::State state();

    // methods that do really perform the actions
    void cancelDownload();
    void pauseDownload();
    void resumeDownload();
    void startDownload();

    // plublic slots used by public implementation
    void cancel();
    void pause();
    void resume();
    void start();

    // slots executed to keep track of the newtork reply
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError code);
    void onFinished();
    void onSslErrors(const QList<QSslError>& errors);

private:
    void connetToReplySignals();
    void disconnectFromReplySignals();

private:
    AppDownload::State _state;
    QString _path;
    QUrl _url;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    QNetworkAccessManager* _nam;
    QNetworkReply* _reply;
    QTemporaryFile* _currentData;
    AppDownload* q_ptr;

};

AppDownloadPrivate::AppDownloadPrivate(QString path, QUrl url, QNetworkAccessManager* nam, AppDownload* parent):
    _state(AppDownload::IDLE),
    _path(path),
    _url(url),
    _hash(""),
    _nam(nam),
    q_ptr(parent)
{
    _reply = NULL;
    _currentData = NULL;
}

AppDownloadPrivate::AppDownloadPrivate(QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
    QNetworkAccessManager* nam, AppDownload* parent):
        _state(AppDownload::IDLE),
        _path(path),
        _url(url),
        _hash(hash),
        _algo(algo),
        _nam(nam),
        q_ptr(parent)
{
    _reply = NULL;
    _currentData = NULL;
}

void AppDownloadPrivate::connetToReplySignals()
{
    Q_Q(AppDownload);
    if (_reply != NULL)
    {
        q->connect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
            q, SLOT(onDownloadProgress(qint64, qint64)));
        q->connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            q, SLOT(onError(QNetworkReply::NetworkError)));
        q->connect(_reply, SIGNAL(finished()),
            q, SLOT(onFinished()));
        q->connect(_reply, SIGNAL(sslErrors ( const QList<QSslError>&)),
            q, SLOT(onSslErrors(const QList<QSslError>&)));
    }
}

void AppDownloadPrivate::disconnectFromReplySignals()
{
    Q_Q(AppDownload);
    if (_reply != NULL)
    {
        q->disconnect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
            q, SLOT(onDownloadProgress(qint64, qint64)));
        q->disconnect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            q, SLOT(onError(QNetworkReply::NetworkError)));
        q->disconnect(_reply, SIGNAL(finished()),
            q, SLOT(onFinished()));
        q->disconnect(_reply, SIGNAL(sslErrors ( const QList<QSslError>&)),
            q, SLOT(onSslErrors(const QList<QSslError>&)));
    }
}

QString AppDownloadPrivate::path()
{
    return _path;
}

QUrl AppDownloadPrivate::url()
{
    return _url;
}

AppDownload::State AppDownloadPrivate::state()
{
    return _state;
}

void AppDownloadPrivate::cancelDownload()
{
    Q_Q(AppDownload);

    if (_reply == NULL)
    {
        // cannot run because it is not running
        emit q->canceled(false);
        return;
    }

    qDebug() << "Canceling download for " << _url;

    // disconnect so that we do not get useless signals and rremove the reply
    disconnectFromReplySignals();
    _reply->abort();
    _reply->deleteLater();
    _reply = NULL;

    // delete the current data, we did cancel.
    _currentData->deleteLater();
    _currentData = NULL;

    emit q->canceled(true);
}

void AppDownloadPrivate::pauseDownload()
{
    Q_Q(AppDownload);

    if (_reply == NULL)
    {
        // cannot pause because is not running
        emit q->paused(false);
        return;
    }

    qDebug() << "Pausing download for " << _url;
    // we need to disconnect the signals to ensure that they are not emitted due
    // to the operation we are going to perform. We read the data in the reply and
    // store it in a file
    disconnectFromReplySignals();

    // do abort before reading
    _reply->abort();
    _currentData->write(_reply->readAll());
    _reply->deleteLater();
    _reply = NULL;
    emit q->paused(true);
}

void AppDownloadPrivate::resumeDownload()
{
    Q_Q(AppDownload);

    if (_reply != NULL)
    {
        // cannot resume because it is already running
        emit q->resumed(false);
        return;
    }

    qint64 currentDataSize = _currentData->size();
    qDebug() << "Resuming download for " << _url << "at" << currentDataSize;

    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(currentDataSize) + "-";
    QNetworkRequest request = QNetworkRequest(_url);
    request.setRawHeader("Range",rangeHeaderValue);

    _reply = _nam->get(request);
    connetToReplySignals();

    emit q->resumed(true);
}

void AppDownloadPrivate::startDownload()
{
    Q_Q(AppDownload);

    if (_reply != NULL)
    {
        // the download was already started, lets say that we did it
        emit q->started(true);
        return;
    }

    qDebug() << "START:" << _url;
    // create temp file that will be used to mantain the state of the download when resumed.
    _currentData = new QTemporaryFile();
    _currentData->open();
    qDebug() << "Tempp IO Device created in " << _currentData->fileName();

    // signals should take care or calling deleteLater on the QNetworkReply object
    _reply = _nam->get(QNetworkRequest(_url));
    connetToReplySignals();
    emit q->started(true);
}

void AppDownloadPrivate::cancel()
{
    Q_Q(AppDownload);
    qDebug() << "CANCELED:" << _url;
    _state = AppDownload::CANCELED;
    emit q->stateChanged();
}

void AppDownloadPrivate::pause()
{
    Q_Q(AppDownload);
    qDebug() << "PAUSED:" << _url;
    _state = AppDownload::PAUSED;
    emit q->stateChanged();
}

void AppDownloadPrivate::resume()
{
    Q_Q(AppDownload);
    qDebug() << "RESUMED:" << _url;
    _state = AppDownload::RESUMED;
    emit q->stateChanged();
}

void AppDownloadPrivate::start()
{
    Q_Q(AppDownload);
    qDebug() << "STARTED:" << _url;
    _state = AppDownload::STARTED;
    emit q->stateChanged();
}

void AppDownloadPrivate::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_Q(AppDownload);

    // do write the current info we have just in case
    _currentData->write(_reply->readAll());

    // ignore the case of 0 or when we do not know yet the size
    if (!bytesTotal >= 0)
    {
        qDebug() << _url << "PROGRESS: " << bytesReceived << "/" << bytesTotal;
        emit q->progress(bytesReceived, bytesTotal);
    }
}

void AppDownloadPrivate::onError(QNetworkReply::NetworkError code)
{
    qDebug() << _url << "ERROR:" << ":" << code;
    // get the error data, disconnect and remove the reply

    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = NULL;
}

void AppDownloadPrivate::onFinished()
{
    Q_Q(AppDownload);

    qDebug() << _url << "FINIHSED";
    _currentData->reset();
    QByteArray data = _currentData->readAll();

    // if the hash is present we check it
    if (!_hash.isEmpty())
    {
        // do calculate the hash of the file so far and ensure that they are the same
        QCryptographicHash hash(_algo);
        hash.addData(data);
        QByteArray fileSig = hash.result();
        QByteArray originalSig = QByteArray::fromHex(_hash.toLatin1());
        if (fileSig != originalSig)
        {
            qDebug() << "HASH ERROR:" << fileSig << "!=" << originalSig;
            emit q->error("HASH ERROR");
            return;
        }
    }
    emit q->finished();
    _reply->deleteLater();
    _reply = NULL;

}

void AppDownloadPrivate::onSslErrors(const QList<QSslError>& errors)
{
    qDebug() << _url << "SLL ERRORS:" << errors.count();
}

/**
 * PUBLIC IMPLEMENTATION
 */

AppDownload::AppDownload(QString path, QUrl url, QNetworkAccessManager* nam, QObject* parent):
    QObject(parent),
    d_ptr(new AppDownloadPrivate(path, url, nam, this))
{
}

AppDownload::AppDownload(QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
    QNetworkAccessManager* nam, QObject* parent):
        QObject(parent),
        d_ptr(new AppDownloadPrivate(path, url, hash, algo, nam, this))
{
}

QString AppDownload::path()
{
    Q_D(AppDownload);
    return d->path();
}

QUrl AppDownload::url()
{
    Q_D(AppDownload);
    return d->url();
}

AppDownload::State AppDownload::state()
{
    Q_D(AppDownload);
    return d->state();
}

void AppDownload::cancelDownload()
{
    Q_D(AppDownload);
    d->cancelDownload();
}

void AppDownload::pauseDownload()
{
    Q_D(AppDownload);
    d->pauseDownload();
}

void AppDownload::resumeDownload()
{
    Q_D(AppDownload);
    d->resumeDownload();
}

void AppDownload::startDownload()
{
    Q_D(AppDownload);
    d->startDownload();
}

void AppDownload::cancel()
{
    Q_D(AppDownload);
    d->cancel();
}

void AppDownload::pause()
{
    Q_D(AppDownload);
    d->pause();
}

void AppDownload::resume()
{
    Q_D(AppDownload);
    d->resume();
}

void AppDownload::start()
{
    Q_D(AppDownload);
    d->start();
}


#include "moc_app_download.cpp"

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

#include <QFinalState>
#include <QState>
#include <QStateMachine>
#include <QSslError>
#include "download_sm.h"

namespace Ubuntu {

namespace DownloadManager {

namespace StateMachines {


/*
 * HEAD TRANSITION
 */
HeaderTransition::HeaderTransition(const SMFileDownload* sender,
                                   QState* sourceState,
                                   QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(headRequestCompleted()),
        sourceState, nextState) {
}

void
HeaderTransition::onTransition(QEvent * event) {
    DownloadSMTransition::onTransition(event);
}

/*
 * NETWORK ERROR TRANSITION
 */

NetworkErrorTransition::NetworkErrorTransition(const SMFileDownload* sender,
                                               QState* sourceState,
                                               QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(error(QNetworkReply::NetworkError)),
        sourceState, nextState) {
}

void
NetworkErrorTransition::onTransition(QEvent* event) {
    SMFileDownload* down = download();
    QStateMachine::SignalEvent* e =
        static_cast<QStateMachine::SignalEvent*>(event);
    QVariant v = e->arguments().at(0);
    QNetworkReply::NetworkError code = v.value<QNetworkReply::NetworkError>();

    down->emitNetworkError(code);
    DownloadSMTransition::onTransition(event);
}

/*
 * SSL ERROR TRANSITION
 */

SslErrorTransition::SslErrorTransition(const SMFileDownload* sender,
                                       QState* sourceState,
                                       QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(sslErrors(const QList<QSslError>&)),
        sourceState, nextState) {
}

void
SslErrorTransition::onTransition(QEvent * event) {
    SMFileDownload* down = download();
    QStateMachine::SignalEvent* e =
        static_cast<QStateMachine::SignalEvent*>(event);
    QVariant v = e->arguments().at(0);
    QList<QSslError> errors = v.value<QList<QSslError> > ();

    down->emitSslError(errors);
    DownloadSMTransition::onTransition(event);
}

/*
 * START TRANSITION
 */

StartDownloadTransition::StartDownloadTransition(const SMFileDownload* sender,
                                                 QState* sourceState,
                                                 QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(downloadingStarted()),
        sourceState, nextState) {
}

void
StartDownloadTransition::onTransition(QEvent * event) {
    Q_UNUSED(event);
    SMFileDownload* down = download();
    // tell the down to start and set the state
    down->requestDownload();
    down->setState(Download::START);
}

/*
 * DOWNLOADING LOST CONNECTION
 */

StopRequestTransition::StopRequestTransition(const SMFileDownload* sender,
                                             const char* signal,
                                             QState* sourceState,
                                             QAbstractState* nextState)
    : DownloadSMTransition(sender, signal, sourceState, nextState){
}

void
StopRequestTransition::onTransition(QEvent * event) {
    Q_UNUSED(event);
    SMFileDownload* down = download();
    down->stopRequestDownload();
    down->setState(Download::PAUSE);
}

/*
 * CANCEL DOWNLOAD TRANSITION
 */

CancelDownloadTransition::CancelDownloadTransition(const SMFileDownload* sender,
                                                   QState* sourceState,
                                                   QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(canceled()), sourceState, nextState) {
}

void
CancelDownloadTransition::onTransition(QEvent * event) {
    Q_UNUSED(event);
    SMFileDownload* down = download();
    down->cancelRequestDownload();
    down->setState(Download::CANCEL);
}

/*
 * RESUME DOWNLOAD TRANSITION
 */
ResumeDownloadTransition::ResumeDownloadTransition(const SMFileDownload* sender,
                                                   const char* signal,
                                                   QState* sourceState,
                                                   QAbstractState* nextState)
    : DownloadSMTransition(sender, signal, sourceState, nextState) {
}

void
ResumeDownloadTransition::onTransition(QEvent * event) {
    Q_UNUSED(event);
    SMFileDownload* down = download();
    down->requestDownload();
    down->setState(Download::RESUME);
}

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    explicit DownloadSMPrivate(DownloadSM* parent)
        : q_ptr(parent) {

        _idle = new QState();
        _init = new QState();
        _downloading = new QState();
        _downloadingNotConnected = new QState();
        _paused = new QState();
        _pausedNotConnected = new QState();
        _downloaded = new QState();
        _hashing = new QState();
        _postProcessing = new QState();
        _error = new QFinalState();
        _canceled = new QFinalState();
        _finished = new QFinalState();

        // add the idle state transitions
        _headerTransition = new HeaderTransition(_down,
            _idle, _init);
        _idleNetworkErrorTransition = new NetworkErrorTransition(
            _down, _idle, _error);
        _idleSslErrorTransition = new SslErrorTransition(_down,
            _idle, _error);

        _idle->addTransition(_headerTransition);
        _idle->addTransition(_idleNetworkErrorTransition);
        _idle->addTransition(_idleSslErrorTransition);

        // add the init state transtions
        _startDownload = new StartDownloadTransition(_down,
            _init, _downloading);
        _initNetworkErrorTransition = new NetworkErrorTransition(_down,
            _init, _error);
        _initSslErrorTransition = new SslErrorTransition(_down, _init, _error);
        _init->addTransition(_startDownload);
        _init->addTransition(_initNetworkErrorTransition);
        _init->addTransition(_initSslErrorTransition);

        // add the downloading transitions
        _downloadingLostConnectionTransition = new StopRequestTransition(_down,
            SIGNAL(connectionDisabled()), _downloading, _downloadingNotConnected);
        _downloadingPausedTransition = new StopRequestTransition(_down,
            SIGNAL(paused()), _downloading, _paused);
        _downloadingCancelTransition = new CancelDownloadTransition(_down,
            _downloading, _canceled);
        _downloadingNetworkErrorTransition = new NetworkErrorTransition(_down,
            _downloading, _error);
        _downloadingSslErrorTransition = new SslErrorTransition(_down,
            _downloading, _error);
        _downloading->addTransition(_downloadingLostConnectionTransition);
        _downloading->addTransition(_downloadingPausedTransition);
        _downloading->addTransition(_downloadingCancelTransition);
        _downloading->addTransition(_downloadingNetworkErrorTransition);
        _downloading->addTransition(_downloadingSslErrorTransition);

        // add the downloading not connected transitions
        _downloadingReconnectTransition = new ResumeDownloadTransition(_down,
            SIGNAL(connectionEnabled()), _downloadingNotConnected,
            _downloading);
        _downloadingNotConnectedCanceled = new CancelDownloadTransition(_down,
            _downloadingNotConnected, _canceled);
        _downloadingNotConnected->addTransition(_downloadingReconnectTransition);
        _downloadingNotConnected->addTransition(_downloadingNotConnectedCanceled);
        // this is a special case, we are moving from downloadingNotConnected
        // to pausedNotConnected. In downloadingNotConnected the state was
        // already set to be paused and the request was stopped, when we are
        // paused we are not changing internally b ut we are moving to a diff
        // state
        _downloadingNotConnected->addTransition(_down, SIGNAL(paused()),
            _pausedNotConnected);

        // add the pause transitions
        _pauseResumeTransition = new ResumeDownloadTransition(_down,
            SIGNAL(resumed()), _paused, _downloading);
        _pausedCancelTransition = new CancelDownloadTransition(_down,
            _paused, _canceled);
        _paused->addTransition(_pauseResumeTransition);
        _paused->addTransition(_pausedCancelTransition);
        // similar transition to the _downloadingNotConnected and
        // _pausedNotConnected
        _paused->addTransition(_down, SIGNAL(connectionDisabled()),
            _pausedNotConnected);

        // paused not connected transitions
        _pausedNotConnectedCancelTransition = new CancelDownloadTransition(
            _down, _pausedNotConnected, _canceled);
        _pausedNotConnected->addTransition(_pausedNotConnectedCancelTransition);
        _pausedNotConnected->addTransition(_down, SIGNAL(resumed()),
            _downloadingNotConnected);
        _pausedNotConnected->addTransition(_down, SIGNAL(connectionEnabled()),
            _paused);

        // downloaded transitions
        _downloadedCancelTransition = new CancelDownloadTransition(
            _down, _downloaded, _canceled);
        _downloaded->addTransition(_downloadedCancelTransition);
        _downloaded->addTransition(_down, SIGNAL(finished()),
            _finished);
        _downloaded->addTransition(_down, SIGNAL(hashingStarted()),
            _hashing);
        _downloaded->addTransition(_down, SIGNAL(postProcessingStarted()),
            _postProcessing);

        // hashing transitions
        _hashing->addTransition(_down, SIGNAL(finished()),
            _finished);
        _hashing->addTransition(_down, SIGNAL(hashingError()),
            _error);
        _hashing->addTransition(_down, SIGNAL(postProcessingStarted()),
            _postProcessing);
    }

    ~DownloadSMPrivate() {
        delete _headerTransition;
        delete _idleNetworkErrorTransition;
        delete _idleSslErrorTransition;
        delete _startDownload;
        delete _initNetworkErrorTransition;
        delete _initSslErrorTransition;
        delete _downloadingLostConnectionTransition;
        delete _downloadingPausedTransition;
        delete _downloadingCancelTransition;
        delete _downloadingNetworkErrorTransition;
        delete _downloadingSslErrorTransition;
        delete _downloadingReconnectTransition;
        delete _downloadingNotConnectedCanceled;
        delete _pauseResumeTransition;
        delete _pausedCancelTransition;
        delete _idle;
        delete _init;
        delete _downloading;
        delete _downloadingNotConnected;
        delete _paused;
        delete _pausedNotConnected;
        delete _downloaded;
        delete _hashing;
        delete _postProcessing;
        delete _error;
        delete _canceled;
        delete _finished;
    }

 private:
    QStateMachine _stateMachine;

    // intermediate steps
    QState* _idle;
    QState* _init;
    QState* _downloading;
    QState* _downloadingNotConnected;
    QState* _paused;
    QState* _pausedNotConnected;
    QState* _downloaded;
    QState* _hashing;
    QState* _postProcessing;
    // finish steps
    QFinalState* _error;
    QFinalState* _canceled;
    QFinalState* _finished;
    // idle transitions
    HeaderTransition* _headerTransition;
    NetworkErrorTransition* _idleNetworkErrorTransition;
    SslErrorTransition* _idleSslErrorTransition;
    // init transitions
    StartDownloadTransition* _startDownload;
    NetworkErrorTransition* _initNetworkErrorTransition;
    SslErrorTransition* _initSslErrorTransition;
    // downloading transtions
    StopRequestTransition* _downloadingLostConnectionTransition;
    StopRequestTransition* _downloadingPausedTransition;
    CancelDownloadTransition* _downloadingCancelTransition;
    NetworkErrorTransition* _downloadingNetworkErrorTransition;
    SslErrorTransition* _downloadingSslErrorTransition;
    // downloading not connected transitions
    ResumeDownloadTransition* _downloadingReconnectTransition;
    CancelDownloadTransition* _downloadingNotConnectedCanceled;
    // paused transitions
    ResumeDownloadTransition* _pauseResumeTransition;
    CancelDownloadTransition* _pausedCancelTransition;
    // paused not connected transitions
    CancelDownloadTransition* _pausedNotConnectedCancelTransition;
    // downloaded transitions
    CancelDownloadTransition* _downloadedCancelTransition;

    SMFileDownload* _down;
    DownloadSM* q_ptr;
};

DownloadSM::DownloadSM(QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadSMPrivate(this)){
}

DownloadSM::~DownloadSM() {
    delete d_ptr;
}

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

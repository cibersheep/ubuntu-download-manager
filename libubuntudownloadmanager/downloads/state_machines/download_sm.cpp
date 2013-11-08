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
#include "download_sm.h"
#define IDLE_STATE 0
#define INIT_STATE 1
#define DOWNLOADING_STATE 2
#define DOWNLOADING_NOT_CONNECTED_STATE  3
#define PAUSED_STATE 4
#define PAUSED_NOT_CONNECTED_STATE 5
#define DOWNLOADED_STATE 6
#define HASHING_STATE 7
#define POST_PROCESSING_STATE 8
#define ERROR_STATE 9
#define CANCELED_STATE 10
#define FINISHED_STATE 11

namespace Ubuntu {

namespace DownloadManager {

namespace StateMachines {

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    explicit DownloadSMPrivate(DownloadSM* parent)
        : _idle(),
          _init(),
          _downloading(),
          _downloadingNotConnected(),
          _paused(),
          _pausedNotConnected(),
          _downloaded(),
          _hashing(),
          _postProcessing(),
          _error(),
          _canceled(),
          _finished(),
          q_ptr(parent) {
    }

    ~DownloadSMPrivate() {
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
    // states
    QState* _idle;
    QState* _init;
    QState* _downloading;
    QState* _downloadingNotConnected;
    QState* _paused;
    QState* _pausedNotConnected;
    QState* _downloaded;
    QState* _hashing;
    QState* _postProcessing;
    // final states
    QFinalState* _error;
    QFinalState* _canceled;
    QFinalState* _finished;

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

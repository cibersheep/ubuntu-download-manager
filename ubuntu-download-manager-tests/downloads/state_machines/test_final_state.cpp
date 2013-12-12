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

#include <QSignalSpy>
#include "test_final_state.h"

TestFinalState::TestFinalState(QObject *parent)
    : BaseTestCase("TestFinalState", parent) {
}

void
TestFinalState::init() {
    BaseTestCase::init();
    _originalName = "Paco";
    _originalSurname = "Paquito";
    _obj = new TestFinalStateHelperObject(_originalName, _originalSurname);
    _s1 = new QState();
    _s2 = new FinalState();
    _transition = _s1->addTransition(_obj, SIGNAL(moveStates()), _s2);

    _stateMachine.addState(_s1);
    _stateMachine.addState(_s2);

    _stateMachine.setInitialState(_s1);
}

void
TestFinalState::cleanup() {
    BaseTestCase::cleanup();
    _stateMachine.removeState(_s1);
    _stateMachine.removeState(_s2);
    delete _obj;
    delete _transition;
    delete _s1;
    delete _s2;
}

void
TestFinalState::testSetNotProperties() {
    QSignalSpy startedSpy(&_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(&_stateMachine, SIGNAL(finished()));

    _stateMachine.start();
    // ensure that we started
    QTRY_COMPARE(startedSpy.count(), 1);

    // raise the signal and assert that the correct method was called with the
    // correct error code
    _obj->emitMoveStates();

    // ensure that we finished
    QTRY_COMPARE(finishedSpy.count(), 1);

    // assert that the values are the same
    QCOMPARE(_originalName, _obj->name());
    QCOMPARE(_originalSurname, _obj->surname());
}

void
TestFinalState::testSetSingleProperty() {
    QString newName = "testSetSingleProperty";
    _s2->assignProperty(_obj, "name", newName);

    QSignalSpy startedSpy(&_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(&_stateMachine, SIGNAL(finished()));

    _stateMachine.start();
    // ensure that we started
    QTRY_COMPARE(startedSpy.count(), 1);

    // raise the signal and assert that the correct method was called with the
    // correct error code
    _obj->emitMoveStates();

    // ensure that we finished
    QTRY_COMPARE(finishedSpy.count(), 1);

    // assert that the values are the same
    QCOMPARE(newName, _obj->name());
    QCOMPARE(_originalSurname, _obj->surname());
}

void
TestFinalState::testMultipleProperties() {
    QString newName = "testSetSingleProperty";
    QString newSurname = "surnameTestSetSingleProperty";
    _s2->assignProperty(_obj, "name", newName);
    _s2->assignProperty(_obj, "surname", newSurname);

    QSignalSpy startedSpy(&_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(&_stateMachine, SIGNAL(finished()));

    _stateMachine.start();
    // ensure that we started
    QTRY_COMPARE(startedSpy.count(), 1);

    // raise the signal and assert that the correct method was called with the
    // correct error code
    _obj->emitMoveStates();

    // ensure that we finished
    QTRY_COMPARE(finishedSpy.count(), 1);

    // assert that the values are the same
    QCOMPARE(newName, _obj->name());
    QCOMPARE(newSurname, _obj->surname());
}
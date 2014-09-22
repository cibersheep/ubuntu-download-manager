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

#ifndef TEST_METADTA_H
#define TEST_METADTA_H

#include <QObject>
#include <base_testcase.h>

class TestMetadata : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestMetadata(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)
    void testGetTittleEmpty();
    void testGetTittlePresent_data();
    void testGetTittlePresent();
    void testGetShowInIndicatorEmpty();
    void testGetShowInIndicatorPresent_data();
    void testGetShowInIndicatorPresent();
};

#endif

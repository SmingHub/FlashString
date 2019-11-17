/**
 * data.cpp
 *
 * Copyright 2019 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the FlashString Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: 18 Sep 2018 - mikee47 <mike@sillyhouse.net>
 *
 ****/

#include "data.h"
#include <WConstants.h>

/**
 * String
 */

DEFINE_FSTR(externalFSTR1, EXTERNAL_FSTR1_TEXT)

/**
 * Array
 */

DEFINE_FSTR_ARRAY(fstrArrayDouble, double, PI, 53.0, 100, 1e8, 47);
DEFINE_FSTR_ARRAY(fstrArrayint64, int64_t, 1, 2, 3, 4, 5);

// A multi-dimensional Array (table)
DEFINE_FSTR_ARRAY(fstrArrayMulti, TableRow_Float_3, {1, 2, 3}, {4, 5, 6}, {7, 8, 9});

DEFINE_FSTR_ARRAY(row1, float, 1, 2, 3);
DEFINE_FSTR_ARRAY(row2, float, 4, 5, 6, 7, 8, 9, 10);

/**
 * Vector
 */

DEFINE_FSTR_LOCAL(data1, "Test string #1");
DEFINE_FSTR_LOCAL(data2, "Test string #2");
DEFINE_FSTR_VECTOR(table, FlashString, &data1, &data2);

DEFINE_FSTR_VECTOR(arrayVector, FSTR::Array<float>, &row1, &row2);

/**
 * Map
 */

DEFINE_FSTR_MAP(arrayMap, int, FSTR::Array<float>, {1, &row1}, {2, &row2});

// Map of `FlashString => FlashString`

// Define the keys
DEFINE_FSTR_LOCAL(key1, "key1");
DEFINE_FSTR_LOCAL(key2, "key2");
// Import a couple of files as content
IMPORT_FSTR(FS_content1, COMPONENT_PATH "/files/content1.txt");
IMPORT_FSTR(FS_content2, COMPONENT_PATH "/files/content2.txt");
// Define the mapping
DEFINE_FSTR_MAP(stringMap, FlashString, FlashString, {&key1, &FS_content1}, {&key2, &FS_content2});

DEFINE_FSTR_MAP(enumMap, MapKey, FlashString, {KeyA, &FS_content1}, {KeyB, &FS_content2});
DEFINE_FSTR_MAP(tableMap, FlashString, FSTR::Vector<FlashString>, {&key1, &table});

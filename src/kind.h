/*
**  Copyright 2016 Atronix Engineering, Inc
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**  http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

#ifndef D2D_KIND_H
#define D2D_KIND_H

#include <string>
#include <unordered_map>

/***********************************************************************
**
*/  enum Reb_Kind
/*
**      Please keep in sync with R3
**
***********************************************************************/
{
    REB_TRASH = 0, // 0
    REB_UNSET = 4, // 4
    REB_NONE = 8, // 8
    REB_BAR = 12, // 12
    REB_LIT_BAR = 16, // 16
    REB_LOGIC = 20, // 20
    REB_INTEGER = 24, // 24
    REB_DECIMAL = 28, // 28
    REB_PERCENT = 32, // 32
    REB_MONEY = 36, // 36
    REB_CHAR = 40, // 40
    REB_PAIR = 44, // 44
    REB_TUPLE = 48, // 48
    REB_TIME = 52, // 52
    REB_DATE = 56, // 56
    REB_WORD = 60, // 60
    REB_SET_WORD = 64, // 64
    REB_GET_WORD = 68, // 68
    REB_LIT_WORD = 72, // 72
    REB_REFINEMENT = 76, // 76
    REB_ISSUE = 80, // 80
    REB_BINARY = 84, // 84
    REB_STRING = 88, // 88
    REB_FILE = 92, // 92
    REB_EMAIL = 96, // 96
    REB_URL = 100, // 100
    REB_TAG = 104, // 104
    REB_BITSET = 108, // 108
    REB_IMAGE = 112, // 112
    REB_VECTOR = 116, // 116
    REB_BLOCK = 120, // 120
    REB_GROUP = 124, // 124
    REB_PATH = 128, // 128
    REB_SET_PATH = 132, // 132
    REB_GET_PATH = 136, // 136
    REB_LIT_PATH = 140, // 140
    REB_MAP = 144, // 144
    REB_DATATYPE = 148, // 148
    REB_TYPESET = 152, // 152
    REB_FUNCTION = 156, // 156
    REB_VARARGS = 160, // 160
    REB_OBJECT = 164, // 164
    REB_FRAME = 168, // 168
    REB_MODULE = 172, // 172
    REB_ERROR = 176, // 176
    REB_TASK = 180, // 180
    REB_PORT = 184, // 184
    REB_GOB = 188, // 188
    REB_EVENT = 192, // 192
    REB_HANDLE = 196, // 196
    REB_STRUCT = 200, // 200
    REB_LIBRARY = 204, // 204
    REB_MAX = 208, // 208
    REB_KIND_SERIES = REB_MAX + 4,
    REB_KIND_ARRAY,
    REB_KIND_CONTEXT,
    REB_KIND_KEYLIST,
    REB_KIND_VARLIST,
    REB_KIND_FIELD,
    REB_KIND_STU,
    REB_KIND_HASH,
    REB_KIND_CHUNK,
    REB_KIND_CALL,
    REB_KIND_ROUTINE_INFO,
    REB_KIND_MAX,

    // Types of content
    GOBT_COLOR,
    GOBT_IMAGE,
    GOBT_STRING,
    GOBT_DRAW,
    GOBT_TEXT,
    GOBT_EFFECT,
};

extern std::unordered_map<int, std::string> kind2str;
#endif //D2D_KIND_H

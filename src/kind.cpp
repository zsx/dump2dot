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

#include "kind.h"
#include <string>
#include <unordered_map>

std::unordered_map<int, std::string> kind2str = {
    {REB_TRASH, "TRASH"},
    {REB_UNSET, "UNSET"},
    {REB_NONE, "NONE"},
    {REB_BAR, "BAR"},
    {REB_LIT_BAR, "LIT_BAR"},
    {REB_LOGIC, "LOGIC"},
    {REB_INTEGER, "INTEGER"},
    {REB_DECIMAL, "DECIMAL"},
    {REB_PERCENT, "PERCENT"},
    {REB_MONEY, "MONEY"},
    {REB_CHAR, "CHAR"},
    {REB_PAIR, "PAIR"},
    {REB_TUPLE, "TUPLE"},
    {REB_TIME, "TIME"},
    {REB_DATE, "DATE"},
    {REB_WORD, "WORD"},
    {REB_SET_WORD, "SET_WORD"},
    {REB_GET_WORD, "GET_WORD"},
    {REB_LIT_WORD, "LIT_WORD"},
    {REB_REFINEMENT, "REFINEMENT"},
    {REB_ISSUE, "ISSUE"},
    {REB_BINARY, "BINARY"},
    {REB_STRING, "STRING"},
    {REB_FILE, "FILE"},
    {REB_EMAIL, "EMAIL"},
    {REB_URL, "URL"},
    {REB_TAG, "TAG"},
    {REB_BITSET, "BITSET"},
    {REB_IMAGE, "IMAGE"},
    {REB_VECTOR, "VECTOR"},
    {REB_BLOCK, "BLOCK"},
    {REB_GROUP, "GROUP"},
    {REB_PATH, "PATH"},
    {REB_SET_PATH, "SET_PATH"},
    {REB_GET_PATH, "GET_PATH"},
    {REB_LIT_PATH, "LIT_PATH"},
    {REB_MAP, "MAP"},
    {REB_DATATYPE, "DATATYPE"},
    {REB_TYPESET, "TYPESET"},
    {REB_FUNCTION, "FUNCTION"},
    {REB_VARARGS, "VARARGS"},
    {REB_OBJECT, "OBJECT"},
    {REB_FRAME, "FRAME"},
    {REB_MODULE, "MODULE"},
    {REB_ERROR, "ERROR"},
    {REB_TASK, "TASK"},
    {REB_PORT, "PORT"},
    {REB_GOB, "GOB"},
    {REB_EVENT, "EVENT"},
    {REB_HANDLE, "HANDLE"},
    {REB_STRUCT, "STRUCT"},
    {REB_LIBRARY, "LIBRARY"},
    {REB_KIND_SERIES, "SERIES"},
    {REB_KIND_ARRAY, "ARRAY"},
    {REB_KIND_CONTEXT, "CONTEXT"},
    {REB_KIND_KEYLIST, "KEYLIST"},
    {REB_KIND_VARLIST, "VARLIST"},
    {REB_KIND_FIELD, "FIELD"},
    {REB_KIND_STU, "STU"},
    {REB_KIND_HASH, "HASH"},
    {REB_KIND_CHUNK, "CHUNK"},
    {REB_KIND_CALL, "CALL"},
    {REB_KIND_ROUTINE_INFO, "ROUTINE_INFO"},
    {REB_KIND_MAX, "KIND_MAX"},
    {GOBT_COLOR, "GOBT_COLOR" },
    {GOBT_IMAGE, "GOBT_IMAGE" },
    {GOBT_STRING, "GOBT_STRING" },
    {GOBT_DRAW, "ARRAY(DRAW)" },
    {GOBT_TEXT, "ARRAY(TEXT)" },
    {GOBT_EFFECT, "ARRAY(EFFECT)" }
};

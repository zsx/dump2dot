#ifndef D2D_KIND_H
#define D2D_KIND_H

#include <unordered_map>
#include <string>

/***********************************************************************
**
*/  enum Reb_Kind
/*
**      Internal datatype numbers. These change. Do not export.
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
    REB_NATIVE = 156, // 156
    REB_ACTION = 160, // 160
    REB_ROUTINE = 164, // 164
    REB_COMMAND = 168, // 168
    REB_FUNCTION = 172, // 172
    REB_VARARGS = 176, // 176
    REB_OBJECT = 180, // 180
    REB_FRAME = 184, // 184
    REB_MODULE = 188, // 188
    REB_ERROR = 192, // 192
    REB_TASK = 196, // 196
    REB_PORT = 200, // 200
    REB_GOB = 204, // 204
    REB_EVENT = 208, // 208
    REB_CALLBACK = 212, // 212
    REB_HANDLE = 216, // 216
    REB_STRUCT = 220, // 220
    REB_LIBRARY = 224, // 224
    REB_MAX = 228, // 228
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

std::unordered_map<enum Reb_Kind, std::string> kind2str = {
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
    {REB_NATIVE, "NATIVE"},
    {REB_ACTION, "ACTION"},
    {REB_ROUTINE, "ROUTINE"},
    {REB_COMMAND, "COMMAND"},
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
    {REB_CALLBACK, "CALLBACK"},
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
    {GOBT_DRAW, "GOBT_DRAW" },
    {GOBT_TEXT, "GOBT_TEXT" },
    {GOBT_EFFECT, "GOBT_EFFECT" }
};

#endif //D2D_KIND_H

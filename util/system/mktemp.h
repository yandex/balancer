#pragma once

#include <util/generic/stroka.h>

/*
 * Create temp file name in the specified directory.
 * If wrkDir is NULL or empty, P_tmpdir is used.
 * throw exception on error
 */
Stroka MakeTempName(const char* wrkDir = 0, const char* prefix = "yandex");

#pragma once

void _ErrorException(bool error, const char * operation, __int64 errorCode, unsigned __int64 line, const char * file);
void _ErrorMessageBox(bool error, const char * operation, __int64 errorCode, unsigned __int64 line, const char * file);
/*
Copyright (c) 2010, G-sTyLeZzZ 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL G-sTyLeZzZ BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#pragma warning (disable:4005 700 996)
#include "../SDK/amx/amx.h"
#include "../SDK/plugincommon.h"
#if (defined(WIN32) || defined(_WIN32) || defined(_WIN64))
	#include "windows.h"
#else
	#include "pthread.h"
#endif
#include "time.h"
#include "sstream"
#include "fstream"
#include "iostream"
#include "vector"
#include "queue"
#include "list"
#include "string.h"
#include "mysql_include/mysql.h"
#if defined(LINUX) || defined(FREEBSD) || defined(__FreeBSD__) || defined(__OpenBSD__)
	#include "stdarg.h"
#endif
#include "source/CMySQLHandler.h"
#include "source/CAmxString.h"
#include "source/CScripting.h"
#include "source/CMutex.h"

#define arguments(a) (params[0] != (a*4)) ? 0 : 1

typedef void (*logprintf_t)(char* format, ...);

//Kye's sleep macro
#ifdef WIN32
	#define SLEEP(x) { Sleep(x); }
#else
	#define SLEEP(x) { usleep(x * 1000); }
	typedef unsigned long DWORD;
#endif

#if !defined NULL
	#define NULL 0
#endif

#define VALID_CONNECTION_HANDLE(function,id) \
	if(id >= SQLHandle.size()) { \
		Debug(">> %s() - Invalid connection handle. (You set: %d, Highest connection handle ID is %d).",function,id+1,SQLHandle.size()); \
		return 0; \
	}


//using namespace std;
using std::string;
using std::stringstream;
using std::list;
using std::ofstream;
using std::basic_istringstream;
using std::queue;
using std::vector;
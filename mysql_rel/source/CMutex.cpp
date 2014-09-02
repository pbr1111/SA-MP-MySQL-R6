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

/*
	Parts taken from:
	http://en.wikipedia.org/wiki/Singleton_pattern
*/

#pragma once
#include "CMutex.h"

Mutex* Mutex::m_pInstance = NULL;

Mutex::Mutex()
{
	// constructor
#ifdef WIN32
	m_mutexHandle = CreateMutex(NULL, FALSE, LPCWSTR("MySQL_Plugin"));
#else
	pthread_mutex_init(&m_mutexHandle, NULL); 
#endif
}

Mutex::~Mutex()
{
	// deconstructor
#ifdef WIN32
	CloseHandle(m_mutexHandle);
#else
	pthread_mutex_destroy(&m_mutexHandle);
#endif
	m_pInstance = NULL;
}

Mutex* Mutex::getInstance() 
{
	// based on the singleton structure
	if(m_pInstance == NULL) {
		m_pInstance = new Mutex();
	}
	return m_pInstance;
}

void Mutex::_lockMutex()
{
#ifdef WIN32
	WaitForSingleObject(m_mutexHandle, INFINITE);
#else
	pthread_mutex_lock(&m_mutexHandle);
#endif
}

void Mutex::_unlockMutex()
{
#ifdef WIN32
	ReleaseMutex(m_mutexHandle);
#else
	pthread_mutex_unlock(&m_mutexHandle);
#endif
}
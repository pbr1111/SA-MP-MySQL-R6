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

#include "main.h"
#include "malloc.h"

list<AMX *> p_Amx;
//map<AMX *,vector<CMySQLHandler*>> ScriptPointer;
void **ppPluginData;
extern void	*pAMXFunctions;
extern logprintf_t logprintf;
#ifdef WIN32
	HANDLE threadHandle;
	DWORD __stdcall ProcessQueryThread(LPVOID lpParam);
#else
	void * ProcessQueryThread(void *lpParam);
#endif

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	if(mysql_library_init(0, NULL, NULL))
	{
		logprintf("Couldn't initialize MySQL library.");
		Natives::getInstance()->Debug("Can't start SQL-plugin due to missing library. (mysqllib)");
		exit(0);
		return 0;
	}
	logprintf( "\n\n  > MySQL plugin R6 successfully loaded.\n");
#ifdef WIN32
	DWORD dwThreadId = 0;
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessQueryThread, NULL, 0, &dwThreadId);
	CloseHandle(threadHandle);
#else
	pthread_t threadHandle;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int error = pthread_create(&threadHandle, &attr, &ProcessQueryThread, NULL);
#endif
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	for(vector<CMySQLHandler *>::iterator i = SQLHandle.begin();i != SQLHandle.end();++i) {
		delete * i;
		SQLHandle.erase(i);
	}
	SQLHandle.clear();
	p_Amx.clear();
	delete Mutex::getInstance();
	delete Natives::getInstance();
	logprintf("\n\n  > MySQL plugin unloaded.\n");
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	if(SQLHandle.size() > 0) {
		unsigned int i = 0;
		for(i;i < SQLHandle.size();i++) {
			if(!SQLHandle[i]->errorCallback.empty()) {
				CMySQLHandler::errorInfo tempData = SQLHandle[i]->errorCallback.front();
				cell amx_Address[3],amx_Ret,*phys_addr; 
				int amx_Idx;
				//OnQueryError(errorid, error[], resultid, extraid, callback[], query[], connectionHandle)
				for (list<AMX *>::iterator a = p_Amx.begin(); a != p_Amx.end(); a++) {
					if(amx_FindPublic(* a, "OnQueryError", &amx_Idx) == AMX_ERR_NONE) {
						Natives::getInstance()->Debug("OnQueryError() - Called.");
						amx_Push(* a,(cell)i+1);
						amx_PushString(* a,&amx_Address[0],&phys_addr,tempData.m_szQuery.c_str(),0,0);
						amx_PushString(* a,&amx_Address[1],&phys_addr,tempData.m_szCallback.c_str(),0,0);
						amx_Push(* a,tempData.m_uiExtra);
						amx_Push(* a,tempData.m_uiResult);
						amx_PushString(* a,&amx_Address[2],&phys_addr,tempData.m_szError.c_str(),0,0);
						amx_Push(* a,tempData.m_uiErrorID);
						amx_Exec(* a,&amx_Ret,amx_Idx);
						amx_Release(* a,amx_Address[0]);
						amx_Release(* a,amx_Address[1]);
						amx_Release(* a,amx_Address[2]);
					}
				}
				SQLHandle[i]->errorCallback.pop();
			}
			if(!SQLHandle[i]->callbackData.empty()) {
				CMySQLHandler::mainInfo tempData = SQLHandle[i]->callbackData.front();
				cell amx_Address,amx_Ret,*phys_addr; 
				int amx_Idx;
				for (list<AMX *>::iterator a = p_Amx.begin(); a != p_Amx.end(); a++) {
					if(amx_FindPublic(* a,tempData.m_szCallback.c_str(), &amx_Idx) == AMX_ERR_NONE) {
							//CustomCallback(query[], index, extraid, connectionHandle);
							Natives::getInstance()->Debug("%s(%d,\"%s\") - Called.",tempData.m_szCallback.c_str(),tempData.m_uiResult,tempData.m_szQuery.c_str());
							amx_Push(* a,(cell)i+1); //connectionHandle
							amx_Push(* a,tempData.m_uiExtra); // extraid
							amx_Push(* a,tempData.m_uiResult); // index
							amx_PushString(* a,&amx_Address,&phys_addr,tempData.m_szQuery.c_str(),0,0); //query[]
							amx_Exec(* a,&amx_Ret,amx_Idx);
							amx_Release(* a,amx_Address);
					}
				}
				SQLHandle[i]->callbackData.pop();
				SQLHandle[i]->m_bQueryProcessing = false;
			}
		}
	}
}

#ifdef WIN32
DWORD __stdcall ProcessQueryThread(LPVOID lpParam)
#else
void * ProcessQueryThread(void *lpParam)
#endif
{
	//This is the main thread which executes all threaded SQL queries
	while(true) {
		Mutex::getInstance()->_lockMutex();
		if(SQLHandle.size() > 0) {
			unsigned int i = 0;
			for(i;i < SQLHandle.size();i++) {
				if(!SQLHandle[i]->queueData.empty() && SQLHandle[i]->m_bIsConnected && !SQLHandle[i]->m_bQueryProcessing && !SQLHandle[i]->m_bNonThreadedQuery /*&& SQLHandle[i]->errorCallback.empty()*/) {
					/*if(SQLHandle[i]->m_stConnectionPtr) { 
						Natives::Debug("CMySQLHandler::ProcessQueryThread() - Pointer is valid");
					} else {
						Natives::Debug("CMySQLHandler::ProcessQueryThread() - Pointer is NULL");
					}*/
					if(!mysql_ping(SQLHandle[i]->m_stConnectionPtr)) {
						CMySQLHandler::mainInfo cQueue = SQLHandle[i]->queueData.front();
						SQLHandle[i]->m_bQueryProcessing = true;
						if(!mysql_real_query(SQLHandle[i]->m_stConnectionPtr,cQueue.m_szQuery.c_str(),cQueue.m_szQuery.length())) {
							Natives::getInstance()->Debug("CMySQLHandler::ProcessQueryThread(%s) - Data is getting passed to %s() - (Threadsafe: %s)",cQueue.m_szQuery.c_str(),cQueue.m_szCallback.c_str(),(mysql_thread_safe() ? "Yes":"No"));
							SQLHandle[i]->callbackData.push(cQueue);
							SQLHandle[i]->m_dwError = 0;
						} else {
							Natives::getInstance()->Debug("CMySQLHandler::ProcessQueryThread() - Error will be triggered to OnQueryError()");
							CMySQLHandler::errorInfo cError;
							cError.m_szQuery = cQueue.m_szQuery;
							cError.m_uiErrorID = mysql_errno(SQLHandle[i]->m_stConnectionPtr);
							SQLHandle[i]->m_dwError = cError.m_uiErrorID;
							cError.m_uiResult = cQueue.m_uiResult;
							cError.m_szCallback = cQueue.m_szCallback;
							//cError.m_szCallback.assign(cQueue.m_szCallback);
							cError.m_szError = mysql_error(SQLHandle[i]->m_stConnectionPtr);
							SQLHandle[i]->errorCallback.push(cError);
							SQLHandle[i]->m_bQueryProcessing = false;
						}
						SQLHandle[i]->queueData.pop();
					} else {
						Natives::getInstance()->Debug("CMySQLHandler::ProcessQueryThread() - Reconnection attempt in background thread.");
						SQLHandle[i]->m_bIsConnected = false;
						if((SQLHandle[i]->m_stResult = mysql_store_result(SQLHandle[i]->m_stConnectionPtr)) != NULL)  {
							mysql_free_result(SQLHandle[i]->m_stResult);
							SQLHandle[i]->m_stResult = NULL;
						}
						SQLHandle[i]->Connect();
					}
				}
			}
		}
		Mutex::getInstance()->_unlockMutex();
		//we use sleep to avoid memory leaks etc.
		SLEEP(5);
	}
}

#if defined __cplusplus
	extern "C"
#endif
const AMX_NATIVE_INFO MySQLNatives[] = 
{
	{"mysql_connect",			Natives::getInstance()->n_mysql_connect},
	{"mysql_query",				Natives::getInstance()->n_mysql_query},
	{"mysql_get_charset",		Natives::getInstance()->n_mysql_get_charset},
	{"mysql_free_result",		Natives::getInstance()->n_mysql_free_result},
	{"mysql_store_result",		Natives::getInstance()->n_mysql_store_result},
	{"mysql_real_escape_string",Natives::getInstance()->n_mysql_real_escape_string},
	{"mysql_reload",			Natives::getInstance()->n_mysql_reload},
	{"mysql_retrieve_row",		Natives::getInstance()->n_mysql_retrieve_row},
	{"mysql_field_count",		Natives::getInstance()->n_mysql_field_count},
	{"mysql_close",				Natives::getInstance()->n_mysql_close},
	{"mysql_ping",				Natives::getInstance()->n_mysql_ping},
	{"mysql_num_rows",			Natives::getInstance()->n_mysql_num_rows},
	{"mysql_num_fields",		Natives::getInstance()->n_mysql_num_fields},
	{"mysql_stat",				Natives::getInstance()->n_mysql_stat},
	{"mysql_set_charset",		Natives::getInstance()->n_mysql_set_charset},
	{"mysql_affected_rows",		Natives::getInstance()->n_mysql_affected_rows},
	{"mysql_warning_count",		Natives::getInstance()->n_mysql_warning_count},
	{"mysql_errno",				Natives::getInstance()->n_mysql_errno},
	{"mysql_fetch_int",			Natives::getInstance()->n_mysql_fetch_int},
	{"mysql_fetch_float",		Natives::getInstance()->n_mysql_fetch_float},
	{"mysql_fetch_field",		Natives::getInstance()->n_mysql_fetch_field},
	{"mysql_fetch_field_row",	Natives::getInstance()->n_mysql_fetch_field_row},
	{"mysql_fetch_row_format",	Natives::getInstance()->n_mysql_fetch_row_format},
	{"mysql_debug",				Natives::getInstance()->n_mysql_debug},
	{"mysql_insert_id",			Natives::getInstance()->n_mysql_insert_id},
	{"mysql_reconnect",			Natives::getInstance()->n_mysql_reconnect},
	{"mysql_query_callback",	Natives::getInstance()->n_mysql_query_callback},
	{NULL,NULL}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx )
{
	p_Amx.push_back(amx);
	return amx_Register(amx,MySQLNatives,-1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	for(list<AMX *>::iterator i = p_Amx.begin();i != p_Amx.end();i++) {
		if(* i == amx) {
			p_Amx.erase(i);
			break;
		}
	}
	return AMX_ERR_NONE;
}
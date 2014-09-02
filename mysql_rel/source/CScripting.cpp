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
#include "CScripting.h"

vector<CMySQLHandler*> SQLHandle;
StrAmx *AMX_H;
logprintf_t logprintf;
unsigned int ID = 0;
bool Debugging = false;
extern list<AMX *> p_Amx;
Natives* Natives::m_pInstance = NULL;

Natives::Natives() 
{
	// constructor
}

Natives::~Natives() 
{
	// deconstructor
}

Natives* Natives::getInstance() 
{
	// based on the singleton structure
	if(m_pInstance == NULL) {
		m_pInstance = new Natives();
	}
	return m_pInstance;
}

cell AMX_NATIVE_CALL Natives::n_mysql_connect( AMX* amx, cell* params )
{
	string
		host = AMX_H->GetString(amx, params[1]),
		user = AMX_H->GetString(amx, params[2]),
		db = AMX_H->GetString(amx, params[3]),
		pass = AMX_H->GetString(amx, params[4]);
	unsigned int port = params[5];

	Debug(">> mysql_connect( )",host.c_str(),user.c_str(),db.c_str(),pass.c_str());
	unsigned int i = 0;
	if(SQLHandle.size() > 0) {
		bool match = false;
		while(i != SQLHandle.size()) {
			//this code is used to avoid double instances of the same connection
			//I recommend using mysql_reconnect() to reconnect instead of mysql_connect(), because the plugin saves the data of a connection handle
			if(!SQLHandle[i]->m_Hostname.compare(host) 
				&& !SQLHandle[i]->m_Username.compare(user)
				&& !SQLHandle[i]->m_Database.compare(db) 
				&& !SQLHandle[i]->m_Password.compare(pass) 
				&& SQLHandle[i]->m_Port == port) {

				SQLHandle[i]->m_bIsConnected = false;
				SQLHandle[i]->Connect();
				match = true;
				break;
			}
			i++;
		}
		if(!match) {
			CMySQLHandler *cHandle = new CMySQLHandler(host,user,pass,db,port);
			SQLHandle.push_back(cHandle);
			ID = (unsigned int)(SQLHandle.size());
			return (cell)ID;
		}
	} else { 
		CMySQLHandler *cHandle = new CMySQLHandler(host,user,pass,db,port);
		SQLHandle.push_back(cHandle);
		ID = (unsigned int)(SQLHandle.size());
		return (cell)ID;
	}
	return (cell)(i+1);
}

cell AMX_NATIVE_CALL Natives::n_mysql_reconnect( AMX* amx,cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_reconnect( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_reconnect",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	cHandle->Disconnect();
	cHandle->m_bIsConnected = false;
	cHandle->Connect();
	return 1;
}

cell AMX_NATIVE_CALL Natives::n_mysql_query( AMX* amx, cell* params )
{
	unsigned int cID = params[4]-1;
	Debug(">> mysql_query( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_query",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->OldQuery(AMX_H->GetString(amx,params[1]),params[2],params[3]);
}

cell AMX_NATIVE_CALL Natives::n_mysql_query_callback( AMX* amx, cell* params )
{
	unsigned int cID = params[5]-1;
	Debug(">> mysql_query_callback( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_query_callback",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->Query(AMX_H->GetString(amx,params[2]),AMX_H->GetString(amx,params[3]),params[1],params[4]);
}

cell AMX_NATIVE_CALL Natives::n_mysql_set_charset( AMX* amx, cell* params )
{
	unsigned int cID = params[2]-1;
	Debug(">> mysql_set_charset( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_set_charset",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	cHandle->SetCharset(AMX_H->GetString(amx,params[1]));
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_get_charset( AMX* amx, cell* params )
{
	unsigned int cID = params[2]-1;
	Debug(">> mysql_get_charset( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_get_charset",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	AMX_H->SetString(amx,params[1],cHandle->GetCharset());
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_insert_id( AMX* amx,cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_insert_id( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_insert_id",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->InsertId();
}

cell AMX_NATIVE_CALL Natives::n_mysql_free_result( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_free_result( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_free_result",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->FreeResult();
}

cell AMX_NATIVE_CALL Natives::n_mysql_store_result( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_store_result( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_store_result",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->StoreResult();
}

cell AMX_NATIVE_CALL Natives::n_mysql_real_escape_string( AMX* amx, cell* params )
{
	unsigned int cID = params[3]-1;
	Debug(">> mysql_real_escape_string( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_real_escape_string",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	char tmp_buffer[8192+1]; // allocate a return string
	memset(tmp_buffer,0,8192+1); // init the return string to 0
	int ret_len = cHandle->EscapeStr(AMX_H->GetString(amx,params[1]),tmp_buffer);
	AMX_H->SetString(amx,params[2],tmp_buffer);
	return ret_len;
}

cell AMX_NATIVE_CALL Natives::n_mysql_field_count( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_field_count( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_field_count",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->FieldCount();
}

cell AMX_NATIVE_CALL Natives::n_mysql_reload( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_reload( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_reload",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->Reload();
}

cell AMX_NATIVE_CALL Natives::n_mysql_close( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1, iCount = 0;
	Debug(">> mysql_close( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_close",cID);
	Mutex::getInstance()->_lockMutex();
	if((iCount = SQLHandle[cID]->queueData.size()) > 0) {
		Debug(">> There is still at least one query in the thread which has not been executed (Total: %d). Closing connection..", iCount);
		while(!SQLHandle[cID]->queueData.empty()) SQLHandle[cID]->queueData.pop();
	}
	delete SQLHandle[cID]; //delete the class instance and call the deconstructor
	SQLHandle.erase(SQLHandle.begin()+cID);
	Mutex::getInstance()->_unlockMutex();
	return 1;
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_row_format( AMX* amx, cell* params )
{
	unsigned int cID = params[3]-1;
	Debug(">> mysql_fetch_row_format( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_fetch_row_format",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	cHandle->Delimiter = AMX_H->GetString(amx,params[2]);
	string fRow = cHandle->FetchRow();
	if(fRow.compare("NULL") != 0) {
		AMX_H->SetString(amx,params[1],fRow);
		cHandle->m_szResult.clear();
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_field_row( AMX* amx, cell* params )
{
	unsigned int cID = params[3]-1;
	Debug(">> mysql_fetch_field_row( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_fetch_field_row",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	string f_Field = AMX_H->GetString(amx,params[2]);
	cHandle->FetchField(f_Field);
	AMX_H->SetString(amx,params[1],cHandle->m_szResult);
	cHandle->m_szResult.clear();
	return 1;
}

cell AMX_NATIVE_CALL Natives::n_mysql_retrieve_row( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_retrieve_row( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_retrieve_row",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->RetrieveRow();
}

cell AMX_NATIVE_CALL Natives::n_mysql_ping (AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_ping( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_ping",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	Mutex::getInstance()->_lockMutex();
	if(!cHandle->queueData.empty()) {
		//the ping check is already done in the thread
		Mutex::getInstance()->_unlockMutex();
		return 1;
	} else {
		Mutex::getInstance()->_unlockMutex();
		//#define JernejL
		#if defined JernejL
			return cHandle->Ping();
		#else
			return (cell)((cHandle->Ping() == 0) ? 1 : (-1));
		#endif
	}
}

cell AMX_NATIVE_CALL Natives::n_mysql_num_rows( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_num_rows( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_num_rows",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->NumRows();
}

cell AMX_NATIVE_CALL Natives::n_mysql_num_fields( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_num_fields( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_num_fields",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->NumFields();
}

cell AMX_NATIVE_CALL Natives::n_mysql_affected_rows( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_affected_rows( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_stat",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->AffectedRows();
}

cell AMX_NATIVE_CALL Natives::n_mysql_stat( AMX* amx, cell* params )
{
	unsigned int cID = params[2]-1;
	Debug(">> mysql_stat( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_stat",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	AMX_H->SetString(amx,params[1],cHandle->Statistics());
	return 1;
}

cell AMX_NATIVE_CALL Natives::n_mysql_warning_count( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_warning_count( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_warning_count",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->WarningCount();
}

cell AMX_NATIVE_CALL Natives::n_mysql_errno( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_errno( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_errno",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	return (cell)cHandle->m_dwError;
}

int IsNumeric(const char *numstring)
{
	while(*numstring)  {
		if(*numstring < '0' || *numstring > '9')  {
			return 0;
		}
		numstring++;
	}
	return 1;
}

template<typename ConvertToType, typename Type> 
ConvertToType ctot(const std::basic_string<Type>& cTargetString)
{
    basic_istringstream<Type> cTemp(cTargetString);
    ConvertToType cInt;
    cTemp >> cInt;
    return cInt;
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_int( AMX* amx, cell* params )
{
	unsigned int cID = params[1]-1;
	Debug(">> mysql_fetch_int( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_fetch_int",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	cHandle->Delimiter = " ";
	string fRow = cHandle->FetchRow();
	if(IsNumeric(cHandle->m_szResult.c_str()) && !cHandle->m_szResult.empty()) {
		int fRet = ctot<cell>(cHandle->m_szResult);
		cHandle->m_szResult.clear();
		return fRet;
	}
	return (-1);
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_float( AMX* amx, cell* params )
{
	unsigned int cID = params[2]-1;
	cell * fPointer; float fFloat;
	amx_GetAddr(amx, params[1],&fPointer);
	Debug(">> mysql_fetch_float( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_fetch_float",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	cHandle->Delimiter = " ";
	cHandle->FetchRow();
	if(!cHandle->m_szResult.empty()) {
		fFloat = ctot<float>(cHandle->m_szResult);
		cHandle->m_szResult.clear();
		* fPointer = (*((cell*)&fFloat));
		return 1;
	}
	fFloat = 0.0f;
	* fPointer = (*((cell*)&fFloat));
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_field( AMX* amx, cell* params )
{
	unsigned int cID = params[3]-1;
	Debug(">> mysql_fetch_field( Connection handle: %d )",cID+1);
	VALID_CONNECTION_HANDLE("mysql_fetch_field",cID);
	CMySQLHandler *cHandle = SQLHandle[cID];
	AMX_H->SetString(amx,params[2],cHandle->FetchFieldName(params[1]));
	return 1;
}

cell AMX_NATIVE_CALL Natives::n_mysql_debug( AMX* amx, cell* params )
{
	if(params[1]) {
		time_t rawtime;
		struct tm *timeinfo;
		char timeform[10];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(timeform,10,"%x",timeinfo);
		Debugging = true;
		Debug(" ");
		Debug("---------------------------");
		Debug("MySQL Debugging activated (%s)",timeform);
		Debug("---------------------------");
		Debug(" ");
	}
	if(!params[1]) {
		Debug("---------------------------");
		Debug("MySQL Debugging de-activated");
		Debug("---------------------------");
		Debugging = false;
	}
	return 1;
}

string stringvprintf(const char *format, va_list args)
{
	int length = vsnprintf(NULL,0,format,args);
	char *chars = new char[length + 1];
	length = vsnprintf(chars, length + 1, format, args);
	string result(chars);
	delete chars;
	return result;
}

void Natives::Debug(char *text,...)
{
	if(Debugging)
	{
		time_t rawtime;
		struct tm *timeinfo;
		char timeform[10];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(timeform,10,"%X",timeinfo);
		va_list args;
		va_start(args, text);
		string buffer = stringvprintf(text, args);
		va_end(args);
		ofstream logfile;
		logfile.open("Debug.txt",std::ios_base::app);
		if (logfile.is_open())
		{
			logfile << "[" << timeform << "] " << buffer << "\r\n";
			buffer.clear();
			logfile.flush();
			logfile.close();
		}
	}
}
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
#include "CMySQLHandler.h"

CMySQLHandler::CMySQLHandler(std::string host, std::string user, std::string passw, std::string db,unsigned int port = 3306)
{
	m_Hostname.assign(host);
	m_Username.assign(user);
	m_Password.assign(passw);
	m_Database.assign(db);
	m_Port = port;
	m_bIsConnected = false;
	m_bQueryProcessing = false;
	m_bNonThreadedQuery = false;
	Natives::getInstance()->Debug("CMySQLHandler::CMySQLHandler() - constructor called.");
	Natives::getInstance()->Debug("CMySQLHandler::CMySQLHandler() - Connecting to \"%s\" | DB: \"%s\" | Username: \"%s\" | Port: \"%d\" ...",m_Hostname.c_str(),m_Database.c_str(),m_Username.c_str(), m_Port);
	this->Connect();
}

CMySQLHandler::~CMySQLHandler()
{
	Natives::getInstance()->Debug("CMySQLHandler::~CMySQLHandler() - deconstructor called.");
	FreeResult();
	Disconnect();
}

bool CMySQLHandler::FetchField(string column)
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::FetchField(%s) - You cannot call this function now. (Reason: Connection is dead)",column.c_str());
		return 0;
	}
	if(m_stField == NULL || m_stRow == NULL) {
		Natives::getInstance()->Debug("CMySQLHandler::FetchField(%s) - You cannot call this function now. (Reason: Fields/Rows are empty.)",column.c_str());
		return 0;
	}
	for(string::size_type i = 0;i < m_dwFields;i++) {
		if(column.compare(m_stField[i].name) == 0) {
			m_cSStream << (m_stRow[i] ? m_stRow[i] : "NULL");
			break;
		}
	}
	getline(m_cSStream,m_szResult);
	m_cSStream >> m_szResult;
	Natives::getInstance()->Debug("CMySQLHandler::FetchField(\"%s\") - %s",column.c_str(),m_szResult.c_str());
	m_cSStream.clear();
	return 1;
}

string CMySQLHandler::FetchFieldName(int number)
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::FetchFieldName() - You cannot call this function now. (Reason: Connection is dead)");
		return 0;
	}
	if(m_stResult == NULL) {
		Natives::getInstance()->Debug("CMySQLHandler::FetchFieldName() - You cannot call this function now. (Reason: No results.)");
		return 0;
	}
	m_stField = (MYSQL_FIELD *)malloc(sizeof(char *));
	m_stField = mysql_fetch_field_direct(m_stResult,number);
	string szFieldname(m_stField->name);
	free(m_stField);
	Natives::getInstance()->Debug("CMySQLHandler::FetchFieldName(%d) - Returned: %s",number,szFieldname.c_str());
	return szFieldname;
}

string CMySQLHandler::FetchRow()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::FetchRow() - You cannot call this function now. (Reason: Connection is dead)");
		return string("NULL");
	}
	if(m_stResult == NULL) {
		Natives::getInstance()->Debug("CMySQLHandler::FetchRow() - You cannot call this function now. (Reason: Empty Result)");
		return string("NULL");
	}
	m_dwFields = mysql_num_fields(m_stResult);
	m_stField = mysql_fetch_fields(m_stResult);
	if((m_stRow = mysql_fetch_row(m_stResult))) {
		for(string::size_type i = 0;i < m_dwFields;i++) {
			m_cSStream << (m_stRow[i] ? m_stRow[i] : "NULL") << Delimiter;
		}
		getline(m_cSStream,m_szResult);
		m_cSStream >> m_szResult;
		if(m_szResult.empty()) {
			Natives::getInstance()->Debug("CMySQLHandler::FetchRow() - Result is empty.");
			return string("NULL");
		} else {
			m_cSStream.clear();
			m_szResult.erase(m_szResult.length()-1,m_szResult.length());
			Natives::getInstance()->Debug("CMySQLHandler::FetchRow() - Return: %s",m_szResult.c_str());
			return m_szResult;
		}
	} else {
		m_dwError = mysql_errno(m_stConnectionPtr);
		if(m_dwError > 0) {
			Natives::getInstance()->Debug("CMySQLHandler::FetchRow() - An error has occured. (Error ID: %d, %s)",m_dwError,mysql_error(m_stConnectionPtr));
		}
		return string("NULL");
	}
}

int CMySQLHandler::RetrieveRow()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::RetrieveRow() - You cannot call this function now. (Reason: Connection is dead)");
		return 0;
	}
	if(m_stResult == NULL) {
		Natives::getInstance()->Debug("CMySQLHandler::RetrieveRow() - You cannot call this function now. (Reason: Empty Result)");
		return 0;
	}
	m_dwFields = mysql_num_fields(m_stResult);
	m_stField = mysql_fetch_fields(m_stResult);
	if((m_stRow = mysql_fetch_row(m_stResult))) {
		return 1;
	}
	return 0;
}

int CMySQLHandler::Query(string query,string callback,int indexID,int extraID)
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::Query() - You cannot call this function now. (Reason: Connection is dead)");
		return 0;
	} else {
		mainInfo passData;
		passData.m_szQuery = query;
		passData.m_uiResult = indexID;
		passData.m_uiExtra = extraID;
		passData.m_szCallback = callback;
		Mutex::getInstance()->_lockMutex();
		queueData.push(passData);
		Mutex::getInstance()->_unlockMutex();
		Natives::getInstance()->Debug("CMySQLHandler::Query(\"%s\") - Custom callback query with index %d started (ExtraID: %d)",callback.c_str(),indexID,extraID);
		return indexID;
	}
}

int CMySQLHandler::OldQuery(string query,int resultID,int extraID)
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::Query() - You cannot call this function now. (Reason: Connection is dead)");
		return 0;
	} else {
		if(resultID != (-1)) {
			//while(m_uiResult_ID != (-1)) SLEEP(5);
			mainInfo passData;
			passData.m_szQuery = query;
			passData.m_uiResult = resultID;
			passData.m_uiExtra = extraID;
			passData.m_szCallback = string("OnQueryFinish"); //I added this to avoid breaking existing scripts
			Mutex::getInstance()->_lockMutex();
			queueData.push(passData);
			Mutex::getInstance()->_unlockMutex();
			Natives::getInstance()->Debug("CMySQLHandler::Query(resultID) - Threaded query with id %d started. (Extra ID: %d)",resultID,extraID);
			return resultID;
		} else {
			m_bNonThreadedQuery = true;
			if(mysql_real_query(m_stConnectionPtr,query.c_str(),query.length()) != 0) {
				m_dwError = mysql_errno(m_stConnectionPtr);
				Natives::getInstance()->Debug("CMySQLHandler::Query(%s) - An error has occured. (Error ID: %d, %s)",query.c_str(),m_dwError,mysql_error(m_stConnectionPtr));
				m_bNonThreadedQuery = false;
				errorInfo st_Error;
				st_Error.m_szQuery = query;
				st_Error.m_uiErrorID = mysql_errno(this->m_stConnectionPtr);
				st_Error.m_uiResult = (-1);
				st_Error.m_szCallback = "NULL";
				st_Error.m_uiExtra = (-1);
				st_Error.m_szError = mysql_error(this->m_stConnectionPtr);
				this->errorCallback.push(st_Error);
				return 0;
			}
			m_dwError = 0;
			m_bNonThreadedQuery = false;
			Natives::getInstance()->Debug("CMySQLHandler::Query(%s) - Successfully executed.",query.c_str());
		}
	}
	return 1;
}

bool CMySQLHandler::Connect()
{
	if(m_bIsConnected) {
		return 0;
	}
	m_stConnectionPtr = mysql_init(NULL);
	if(m_stConnectionPtr == NULL) Natives::getInstance()->Debug("CMySQLHandler::Connect() - MySQL init failed.");
	if(!mysql_real_connect(m_stConnectionPtr,m_Hostname.c_str(),m_Username.c_str(),m_Password.c_str(),m_Database.c_str(),m_Port,NULL,CLIENT_COMPRESS)) {
		m_dwError = mysql_errno(m_stConnectionPtr);
		m_bIsConnected = false;
		Natives::getInstance()->Debug("CMySQLHandler::Connect() - %s (Error ID: %d)",mysql_error(m_stConnectionPtr),m_dwError);
		return 0;
	} else {
		m_bIsConnected = true;
		Natives::getInstance()->Debug("CMySQLHandler::Connect() - Connection was successful.");
		my_bool reconnect;
		mysql_options(m_stConnectionPtr, MYSQL_OPT_RECONNECT, &reconnect);
		Natives::getInstance()->Debug("CMySQLHandler::Connect() - Auto-Reconnect has been enabled.");
		return 1;
	}
	return 1;
}

void CMySQLHandler::Disconnect()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::Disconnect() - You cannot call this function now. (Reason: Connection is dead)");
	} else {
		if(m_stConnectionPtr == NULL) { 
			Natives::getInstance()->Debug("CMySQLHandler::Disconnect() - Connection is already gone."); 
		} else { 
			Mutex::getInstance()->_lockMutex();
			int iCount = 0;
			if((iCount = queueData.size()) > 0) {
				Natives::getInstance()->Debug(">> There is still at least one query in the thread which has not been executed (Total: %d). Closing connection..", iCount);
				while(!queueData.empty()) queueData.pop();
			}
			Mutex::getInstance()->_unlockMutex();
			mysql_close(m_stConnectionPtr);
			m_stConnectionPtr = NULL;
		}
		Natives::getInstance()->Debug("CMySQLHandler::Disconnect() - Connection was closed.");
		m_bIsConnected = false;
	}
}

int CMySQLHandler::SetCharset(string charsetname)
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::SetCharset() - You cannot call this function now. (Reason: Dead Connection)");
		return (-1);
	}
	return mysql_set_character_set(m_stConnectionPtr, charsetname.c_str());
}

string CMySQLHandler::GetCharset()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::GetCharset() - You cannot call this function now. (Reason: Dead Connection)");
		return string("NULL");
	}
	return mysql_character_set_name(m_stConnectionPtr);
}

bool CMySQLHandler::FreeResult()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::FreeResult() - There is nothing to free (Reason: Connection is dead)");
		return 0;
	}
	if(m_stResult == NULL) {
		Natives::getInstance()->Debug("CMySQLHandler::FreeResult() - The result is already empty.");
	} else {
		mysql_free_result(m_stResult);
		m_stResult = NULL;
		m_stRow = NULL;
		Natives::getInstance()->Debug("CMySQLHandler::FreeResult() - Result was successfully free'd.");
	}
	return 1;
}

bool CMySQLHandler::StoreResult()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::StoreResult() - There is nothing to store (Reason: Connection is dead)");
		return 0;
	}
	if(!(m_stResult = mysql_store_result(m_stConnectionPtr))) //prevent it from crashing
	{
		Natives::getInstance()->Debug("CMySQLHandler::StoreResult() - No data to store.");
		return 0;
	}
	Natives::getInstance()->Debug("CMySQLHandler::StoreResult() - Result was stored.");
	return 1;
}

my_ulonglong CMySQLHandler::InsertId()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::InsertId() - You cannot call this function now(Reason: Connection is dead).");
		return 0;
	}
	return mysql_insert_id(m_stConnectionPtr);
}

int CMySQLHandler::Reload()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::InsertId() - You cannot call this function now(Reason: Connection is dead).");
		return 0;
	}
	return mysql_reload(m_stConnectionPtr);;
}

my_ulonglong CMySQLHandler::AffectedRows()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::AffectedRows() - You cannot call this function now(Reason: Connection is dead).");
		return 0;
	}
	my_ulonglong ullAffected = mysql_affected_rows(m_stConnectionPtr);
	Natives::getInstance()->Debug("CMySQLHandler::NumRows() - Returned %d affected rows(s)",ullAffected);
	return ullAffected;
}

int CMySQLHandler::Ping()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::Ping() - You cannot call this function now. (Reason: Dead Connection)");
		return 1;
	} else if(mysql_ping(m_stConnectionPtr) != 0) {
		m_dwError = mysql_errno(m_stConnectionPtr);
		Natives::getInstance()->Debug("CMySQLHandler::Ping(Thread ID %d) - An error has occured (%s, Error ID: %d)",mysql_error(m_stConnectionPtr),m_dwError);
		return 1;
	} else {
		Natives::getInstance()->Debug("CMySQLHandler::Ping() - Connection is still alive.");
		return 0;
	}
}

my_ulonglong CMySQLHandler::NumRows()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::NumRows() - You cannot call this function now. (Reason: Dead Connection)");
		return (-1);
	}
	if(m_stResult == NULL) {
		Natives::getInstance()->Debug("CMySQLHandler::NumRows() - You cannot call this function now. (Reason: Dead Connection)");
		return (-1);
	}
	my_ulonglong ullRows = mysql_num_rows(m_stResult);
	Natives::getInstance()->Debug("CMySQLHandler::NumRows() - Returned %d row(s)",ullRows);
	return ullRows;
}

unsigned int CMySQLHandler::NumFields()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::NumFields() - You cannot call this function now. (Reason: Dead Connection)");
		return (-1);
	}
	if(m_stResult == NULL) {
		Natives::getInstance()->Debug("CMySQLHandler::NumFields() - You cannot call this function now. (Reason: Dead Connection)");
		return (-1);
	}
	unsigned int uiNumFields = mysql_num_fields(m_stResult);
	Natives::getInstance()->Debug("CMySQLHandler::NumFields() - Returned %d field(s)",uiNumFields);
	return uiNumFields;
}

unsigned int CMySQLHandler::FieldCount()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::NumFields() - You cannot call this function now. (Reason: Dead Connection)");
		return (-1);
	}
	return mysql_field_count(m_stConnectionPtr);
}

unsigned int CMySQLHandler::WarningCount()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::WarningCount() - You cannot call this function now. (Reason: Dead Connection)");
		return (-1);
	}
	unsigned int uiWarnings = mysql_warning_count(m_stConnectionPtr);
	Natives::getInstance()->Debug("CMySQLHandler::WarningCount() - Returned %d warning(s)",uiWarnings);
	return uiWarnings;
}

string CMySQLHandler::Statistics()
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::Statistics() - You cannot call this function now. (Reason: Dead Connection)");
		return 0;
	}
	return string(mysql_stat(m_stConnectionPtr));
}

int CMySQLHandler::EscapeStr(string source, char *to)
{
	if(!m_bIsConnected) {
		Natives::getInstance()->Debug("CMySQLHandler::EscapeString(%s) - You cannot call this function now. (Reason: Dead Connection)");
		return 0;
	}
	int length = mysql_real_escape_string(m_stConnectionPtr,to,source.c_str(),source.length());
	Natives::getInstance()->Debug("CMySQLHandler::EscapeString(%s); - Escaped %u characters to %s.",source.c_str(),length,to);
	return length;
}
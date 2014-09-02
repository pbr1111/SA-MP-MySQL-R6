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

#include "../main.h"

class CMySQLHandler
{
public:
	CMySQLHandler(std::string host,std::string user,std::string passw,std::string db,unsigned int port);
	~CMySQLHandler();
	int	Query(std::string query,std::string callback,int indexID,int extraID);
	int	OldQuery(std::string query,int resultID,int extraID);
	int	Ping();
	int RetrieveRow();
	int SetCharset(std::string charsetname);
	int EscapeStr(std::string source, char *to);
	int Reload();
	bool Connect();
	bool FreeResult();
	bool StoreResult();
	bool FetchField(std::string column);
	bool m_bIsConnected;
	bool m_bNonThreadedQuery;
	bool m_bQueryProcessing;
	my_ulonglong InsertId();
	my_ulonglong NumRows();
	my_ulonglong AffectedRows();
	void Disconnect();

	unsigned int NumFields();
	unsigned int WarningCount();
	unsigned int FieldCount();

	unsigned int m_dwError,m_dwFields;

	struct mainInfo {
		std::string m_szQuery;
		int m_uiResult;
		int m_uiExtra;
		std::string m_szCallback;
	};

	struct errorInfo {
		std::string m_szQuery;
		std::string m_szError;
		int m_uiErrorID;
		int m_uiResult;
		int m_uiExtra;
		std::string m_szCallback;
	};

	std::queue<mainInfo> queueData;
	std::queue<mainInfo> callbackData;
	std::queue<errorInfo> errorCallback;

	std::string FetchRow();
	std::string Statistics();
	std::string GetCharset();
	std::string FetchFieldName(int number);
	std::string
			m_Hostname,m_Username,m_Password,m_Database,Delimiter,m_szResult;
	unsigned int m_Port;

	std::stringstream m_cSStream;
	MYSQL * m_stConnectionPtr;
	MYSQL_RES * m_stResult;
protected:
	MYSQL_ROW m_stRow;
	MYSQL_FIELD * m_stField;
};

extern std::vector<CMySQLHandler *> SQLHandle;

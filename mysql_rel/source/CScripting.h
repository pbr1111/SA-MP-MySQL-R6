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

class Natives
{
public:
	static cell AMX_NATIVE_CALL n_mysql_connect( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_query( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_free_result( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_store_result( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_real_escape_string( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_close( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_fetch_int( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_fetch_float( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_field_count( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_num_rows( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_fetch_row_format( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_ping( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_debug( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_fetch_field_row( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_fetch_field (AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_stat( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_errno( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_warning_count( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_reload( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_num_fields( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_affected_rows( AMX* amx, cell* params );
	static cell AMX_NATIVE_CALL n_mysql_insert_id( AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_mysql_reconnect( AMX* amx,cell* params );
	static cell AMX_NATIVE_CALL n_mysql_set_charset( AMX* amx,cell* params );
	static cell AMX_NATIVE_CALL n_mysql_get_charset( AMX* amx,cell* params );
	static cell AMX_NATIVE_CALL n_mysql_retrieve_row( AMX* amx,cell* params );
	static cell AMX_NATIVE_CALL n_mysql_query_callback( AMX* amx,cell* params );
	static void Debug(char* text,...);
	static Natives* getInstance();
	~Natives();
private:
	static Natives* m_pInstance;
	Natives();
};
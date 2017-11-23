
#include "DB_CommonAPI.h"



SP_DB_CommonAPI::SP_DB_CommonAPI()
{
}

SP_DB_CommonAPI::~SP_DB_CommonAPI()
{
}

LP_DB_INST SP_DB_CommonAPI::Open(string &host, unsigned int port, string &user, string &pwd, string &db_name)
{
    LP_DB_INST client = mysql_init((LP_DB_INST)NULL);
	
	if (NULL != client) 
	{
		char enable = 1;
		mysql_options(client, MYSQL_OPT_RECONNECT, &enable);
	    
		if (NULL != mysql_real_connect(client, host.c_str(), user.c_str(), pwd.c_str(), db_name.c_str(), 3306, NULL, 0)) {

			printf("connect to mysql host: %s\n", host.c_str());

			return client;
		}
		else
		{
			printf("mysql_real_connect failed, errno = %d.\n", mysql_errno(client));
		}
	}
	else
	{
		printf("mysql_init failed.\n");
	}

    return NULL;
}

void SP_DB_CommonAPI::Close(LP_DB_INST inst)
{
	if (NULL != inst)
	{
		mysql_close(inst);
	}
}

int SP_DB_CommonAPI::Detect(LP_DB_INST inst)
{
	return mysql_ping(inst);
}

int SP_DB_CommonAPI::Query(LP_DB_INST inst, const char *sql)
{
	if(NULL != inst)
	{
		if (mysql_query(inst, sql))
		{
			printf("query failed\n");

			return -1;
		}
	}
	
	return 0;
}

int SP_DB_CommonAPI::UseDB(LP_DB_INST inst, string &db_name)
{
	if(NULL != inst)
	{
		if(mysql_select_db(inst , db_name.c_str()))  
		{
			printf("change database failed\n");

			return -1;  
		}
	}

	return 0;
}

LP_DB_RES SP_DB_CommonAPI::GetQuery(LP_DB_INST inst, const char *sql)
{
	if(NULL != inst)
	{
		if (0 == mysql_query(inst , sql)) 
		{
			return mysql_store_result(inst);
		}
	}

	printf("GetQuery failed\n");

	return NULL;
}

void SP_DB_CommonAPI::DeleteRes(LP_DB_RES res)
{
	if (NULL != res)
	{
		mysql_free_result(res);
	}
}

LP_DB_ROWS SP_DB_CommonAPI::FatchRows(LP_DB_RES res)
{
	if (NULL != res)
	{
		return mysql_fetch_row(res);
	}

	return NULL;
}


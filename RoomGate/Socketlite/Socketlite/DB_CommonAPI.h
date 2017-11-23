
#ifndef __SP_DB_COMMON_API_H__
#define __SP_DB_COMMON_API_H__

#include "SL_Config.h"


#include <list>
using namespace std;

class SP_DB_CommonAPI
{
public:
    static LP_DB_INST Open(string &host, unsigned int port, string &user, string &pwd, string &db_name);
    static void Close(LP_DB_INST inst);
	static int UseDB(LP_DB_INST inst, string &db_name);
    static int Query(LP_DB_INST inst, const char *sql);
	static LP_DB_RES GetQuery(LP_DB_INST inst, const char *sql);
	static void DeleteRes(LP_DB_RES res);
	static LP_DB_ROWS FatchRows(LP_DB_RES res);
	static int Detect(LP_DB_INST inst);


protected:
    SP_DB_CommonAPI();
    ~SP_DB_CommonAPI();

private:
    //void *Get();
    //int Release(void*);

private:
    //list<void*> pool_;
    //SL_Sync_Semaphore semaphore_;
    //SL_Sync_SpinMutex mutex_;
    //static CDBConnectPool *Instance_;
};
#endif


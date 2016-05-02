#pragma once
#include "type_define.h"
#include "entry.h"
#include "timer.h"

#include <mysql.h>

class MysqlPool;


enum HandleState
{
	HandleState_Invalid  = 1,   //无效
	HandleState_Valid    = 2,   //有效
	HandleState_Used     = 3,   //正在被使用
};


struct MysqlUrl : public Entry
{
	const std::string url;
	std::string host;
	std::string user;
	std::string passwd;
	unsigned int port;
	std::string dbname;
};


class MysqlHandle : public Entry
{
public:
	MysqlHandle(const MysqlUrl *url, MysqlPool *pool);
	~MysqlHandle();

	MYSQL *GetMysql() {return m_mysql;}
	bool InitHandle();
	void FinalHandle();
	bool SetHandle();
	void FreeHandle();
	void CheckUseTime();
	//int ExecSql(const char *sql, unsigned int sqllen, bool need_errlog = true);
	//bool *exeSelect(const char *sql, unsigned int sqllen);

	char *escapeString(const char *src, char *dest, unsigned int size);
	string& escapeString(const std::string& src, string& dest);
private:
	bool InitMysql();

public:
	MysqlPool *m_mysql_pool;
	HandleState m_state;
private:
	MYSQL *m_mysql;
	std::string m_last_sql;
	const MysqlUrl *m_url;
	unsigned int m_count;
	Time m_life_time;
	Time m_use_time;
	Time m_select_time;
	Time m_update_time;
	Time m_insert_time;
	Time m_delete_time;
	uint32 m_timet_out;
};

class MysqlHandleManager
{

};

class MysqlPool
{
public:
	MysqlPool(int mMaxHandle = 64);
	~MysqlPool();
	bool ParseUrl(const unsigned int id, const char *szUrl);
	MysqlHandle *GetHandle();
	void PutHandle(MysqlHandle *handle);
private:
	MysqlHandleManager m_mum;
	uint32 m_max_handle;
};
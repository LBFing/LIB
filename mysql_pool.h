#ifndef __MYSQL_POOL_H__
#define __MYSQL_POOL_H__

#include "type_define.h"
#include "timer.h"
#include "entry_manager.h"
#include "var_type.h"
#include "regex_parse.h"

#include <mysql.h>
static VarType VAR_NULL;


class MysqlRow : public Entry
{
public:
	explicit MysqlRow(uint32 nField);
	~MysqlRow();
	void SetField(uint32 nField);
	void SetValue(uint32 nField, const char* value, uint32 nLen);
	VarType& GetValue(uint32 nField);

private:
	vector<VarType> m_value;
};


class DataSet
{
public:
	explicit DataSet(uint32 nRow, uint32 nField);
	~DataSet();
	bool PutField(uint32 nField, const char* szName);
	void PutValue(uint32 nRow, uint32 nField, const char* value , uint32 nLen);
	VarType& GetValue(uint32 nRow, string strName);
	uint32 Size();
public:
	map<string , uint32> m_field_map;
	EntryManager<MysqlRow, true> m_record;

};



enum HandleState
{
	HandleState_Invalid  = 1,   //无效
	HandleState_Valid    = 2,   //有效
	HandleState_Used     = 3,   //正在被使用
};


class MysqlUrl : public Entry
{
public:
	explicit MysqlUrl(const char* szUrl)
	{
		m_url = szUrl;
		ParseUrl();
	}
	~MysqlUrl() {}
	bool ParseUrl()
	{
		Regex re;
		if(re.Compile("mysql://(.+):(.+)@(.+):(.+)/(.+)") && re.Match(m_url.c_str()))
		{
			std::string port_str;
			re.GetSub(m_user, 1);
			re.GetSub(m_passwd, 2);
			re.GetSub(m_host, 3);
			re.GetSub(port_str, 4);
			m_port = atoi(port_str.c_str());
			re.GetSub(m_dbname, 5);
			return true;
		}
		else
		{
			return false;
		}
	}
public:
	std::string m_url;
	std::string m_host;
	std::string m_user;
	std::string m_passwd;
	std::string m_dbname;
	unsigned int m_port;
};

class MysqlPool;
class MysqlHandle : public Entry
{
public:
	MysqlHandle(const MysqlUrl* url, MysqlPool* pool, uint32 id);
	~MysqlHandle();
	inline MYSQL* GetMysql() {return m_mysql;}
	bool InitMysql();
	void FinalHandle();
	bool SetHandle();
	void FreeHandle();
	void CheckUseTime();
	int ExecSql(const char* szSql, uint32 nLen, bool need_errlog = true);
	DataSet* ExeSelect(const char* szSql, unsigned int nLen);
	char* escapeString(const char* szSrc, char* szDest, unsigned int size);
	string& escapeString(const std::string& src, string& dest);
private:
	bool InitHandle();
public:
	MysqlPool* m_mysql_pool;
	HandleState m_state;
private:
	MYSQL* m_mysql;
	std::string m_last_sql;
	const MysqlUrl* m_url;
	unsigned int m_count;
	Time m_life_time;
	Time m_use_time;
	Time m_select_time;
	//Time m_update_time;
	//Time m_insert_time;
	//Time m_delete_time;
	uint32 m_timet_out;
};

class MysqlPool
{
public:
	MysqlPool(int mMaxHandle = 64);
	~MysqlPool();

	bool PutUrl(const char* szUrl, const unsigned int id = 0);
	MysqlHandle* GetHandle(uint32 id = 0);
	void PutHandle(MysqlHandle* handle);
private:
	EntryManager<MysqlHandle, true> m_mum;
	EntryManager<MysqlUrl, true> m_murl;
	uint32 m_max_handle;
};

class AutoHandle
{
public:
	AutoHandle(MysqlPool* pool, uint32 id = 0) : m_pool(pool)
	{
		m_handle = m_pool->GetHandle(id);
	}
	~AutoHandle()
	{
		m_pool->PutHandle(m_handle);
	}
	bool isValid();
	operator MysqlHandle* ()
	{
		return m_handle;
	}
	MysqlHandle* operator()() const
	{
		return m_handle;
	}
	MysqlHandle* GetHandle()
	{
		return m_handle;
	}

private:
	MysqlPool*	m_pool;
	MysqlHandle* m_handle;
};


#endif
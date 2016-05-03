#pragma once
#include "type_define.h"
#include "entry.h"
#include "timer.h"
#include "entry_manager.h"
#include "var_type.h"
#include "regex_parse.h"

#include <mysql.h>
static VarType VAR_NULL;

struct MysqlField
{
	char szName[128];
	uint32 nIndex;
	MysqlField()
	{
		bzero(szName, 0);
		nIndex = 0;
	}
	bool operator< (const MysqlField& field) const
	{
		return strcmp(szName, field.szName);
	}
};




class MysqlRow : public Entry
{
public:
	explicit MysqlRow(uint32 nField);
	~MysqlRow();
	void SetField(uint32 nField);
	void SetValue(uint32 nField, const char *value, uint32 nLen);
	VarType& GetValue(uint32 nField);

private:
	vector<VarType> m_value;
};


class DataSet
{
public:
	explicit DataSet(uint32 nRow, uint32 nField);
	~DataSet();
	bool PutField(uint32 nField, const char *szName);
	void PutValue(uint32 nRow, uint32 nField, const char *value , uint32 nLen);
	VarType& GetValue(uint32 nRow, const char *szName);

private:
	typedef set<MysqlField> SetField;
	typedef SetField::iterator IterFeild;

	SetField m_field_set;
	EntryManager<MysqlRow, true> m_record;

};



enum HandleState
{
	HandleState_Invalid  = 1,   //无效
	HandleState_Valid    = 2,   //有效
	HandleState_Used     = 3,   //正在被使用
};


class MysqlUrl
{
public:
	explicit MysqlUrl(const char* *szUrl)
	{
		m_url = szUrl;
		ParseUrl();
	}
	~MysqlUrl(){}
	bool ParseUrl()
	{
		Regex re;
		if(re.Compile("mysql://(.+):(.+)@(.+):(.+)/(.+)") && re.Match(m_url.c_str()))
		{
			std::string port_str;
			re.GetSub(m_player, 1);
			re.GetSub(m_passwd, 2);
			re.GetSub(m_host, 3);
			re.GetSub(port_str, 4);
			m_port = atoi(port_str.c_str());
			re.GetSub(m_dbname, 5);
			//printf("%s, %s, %s, %s, %s\n", player.c_str(), passwd.c_str(), host.c_str(), port_str.c_str(), dbname.c_str());
			return true;			
		}
		else
		{
			return false;
		}
	}

private:
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
	MysqlHandle(const MysqlUrl *url, MysqlPool *pool,uint32 id)
	{
		m_id = id;
		m_url = url;
		m_state = HandleState_Invalid;
		m_count = 0;
		m_mysql = NULL;
		m_timet_out = 10000L;
		m_mysql_pool = pool;
	}
	~MysqlHandle() {FinalHandle();}

	inline MYSQL *GetMysql() {return m_mysql;}
	bool InitMysql() 
	{
		FinalHandle();
		if(InitHandle() == false)
		{
			FinalHandle();
			return false;
		}
		else
		{
			return true;
		}
	}
	void FinalHandle()
	{
		if(m_mysql)
		{
			printf("InitHandle():The mysql connect will been closed...\n");
			mysql_close(m_mysql);
			m_mysql = NULL;
		}
		m_state = HandleState_Invalid;
		m_count = 0;
		m_last_sql = "";
	}
	bool SetHandle()
	{
		if(m_state == HandleState_Used)
		{
			return false;
		}
	}
	void FreeHandle()
	{
		m_state = HandleState_Valid;
	}
	void CheckUseTime()
	{

	}
	//int ExecSql(const char *sql, unsigned int sqllen, bool need_errlog = true);
	//bool *exeSelect(const char *sql, unsigned int sqllen);

	char *escapeString(const char *src, char *dest, unsigned int size);
	string& escapeString(const std::string& src, string& dest);
private:
	bool InitHandle()
	{
		if(m_mysql)
		{
			printf("InitHandle():The mysql connect will been closed...\n");
			mysql_close(m_mysql);
			m_mysql = NULL;
		}

		m_mysql = mysql_init(NULL);
		if(m_mysql == NULL)
		{
			printf("InitHandle():Initiate mysql MERROR...\n");
			return false;
		}

		if(mysql_real_connect(m_mysql, m_url->m_host.c_str(), m_url->m_user.c_str(), m_url->m_passwd.c_str(), m_url->m_dbname.c_str(), m_url->m_port, NULL, CLIENT_COMPRESS | CLIENT_INTERACTIVE) == NULL)
		{	
			printf("InitHandle():connect mysql://%s:%u/%s failed...\n", m_url->m_host.c_str(), m_url->m_port, m_url->m_dbname.c_str());
			return false;
		}
		printf("initMysql():connect mysql://%s:%u/%s successful...\n", m_url->m_host.c_str(), m_url->m_port, m_url->m_dbname.c_str());
		m_state = HandleState_Valid;
		m_life_time.now();
		m_count = 0;
		return true;
	}

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


class MysqlPool
{
public:
	MysqlPool(int mMaxHandle = 64);
	~MysqlPool();
	bool PutUrl(const unsigned int id, const char *szUrl);
	MysqlHandle *GetHandle();
	void PutHandle(MysqlHandle *handle);
private:
	EntryManager<MysqlHandle,true> m_mum;
	uint32 m_max_handle;
};
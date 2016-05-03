#pragma once
#include "type_define.h"
#include "entry.h"
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
	explicit MysqlUrl(const char *szUrl)
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
	MysqlHandle(const MysqlUrl *url, MysqlPool *pool, uint32 id)
	{
		SetId(id);
		m_url = url;
		m_state = HandleState_Invalid;
		m_count = 0;
		m_mysql = NULL;
		m_timet_out = 10000L;
		m_mysql_pool = pool;
	}
	~MysqlHandle()
	{
		FinalHandle();
		m_url = NULL;
	}

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
		m_use_time.Now();
		if(m_count > 10000 || mysql_ping(m_mysql) != 0)
		{
			if(InitMysql() == false)
			{
				return false;
			}
		}
		m_state = HandleState_Used;
		m_count++;
		return true;
	}

	void FreeHandle()
	{
		m_state = HandleState_Valid;
		m_count--;
	}
	void CheckUseTime()
	{
		if(m_use_time.Elapse(Time()) > m_timet_out)
		{
			printf("sql语句超时:%ums,描述:%s \n", m_timet_out, m_last_sql.c_str());
			m_timet_out += 10000L;
		}
	}
	int ExecSql(const char *szSql, uint32 nLen, bool need_errlog = true)
	{
		if(szSql == NULL || nLen == 0 || m_mysql == NULL)
		{
			return -1;
		}
		m_last_sql = szSql;
		int nRet = mysql_real_query(m_mysql, szSql, nLen);
		if(nRet && need_errlog)
		{
			printf("SQL:%s  Error:%s\n", szSql, mysql_error(m_mysql));
		}
		return nRet;
	}

	DataSet *ExeSelect(const char *szSql, unsigned int nLen)
	{
		if(m_mysql == NULL)
		{
			printf("NULL m_mysql Error. ---- %s\n", szSql);
			return NULL;
		}
		m_select_time.Now();
		if(ExecSql(szSql, nLen))
		{
			printf("ExeSelect Error. ---- %s\n", szSql);
			return NULL;
		}

		MYSQL_RES *result = mysql_store_result(m_mysql);
		if(result == NULL)
		{
			printf("Result Get Error:%s\n", mysql_error(m_mysql));
			return NULL;
		}
		uint32 nRow =  mysql_num_rows(result);
		if(nRow == 0)
		{
			mysql_free_result(result);
			return NULL;
		}
		uint32 nField = mysql_num_fields(result);
		if(nField == 0)
		{
			mysql_free_result(result);
			return NULL;
		}

		DataSet *ret_set = new DataSet(nRow, nField);
		MYSQL_FIELD *mysql_fields = mysql_fetch_fields(result);
		for(uint32 i = 0 ; i < nField; i++)
		{
			if(ret_set->PutField(i, mysql_fields[i].name) == false)
			{
				printf("error PutField\n");
				mysql_free_result(result);
				delete ret_set;
				ret_set = NULL;
				return NULL;
			}
		}

		MYSQL_ROW row;
		uint32 i = 0;
		while((row = mysql_fetch_row(result)))
		{
			unsigned long *lengths = mysql_fetch_lengths(result);
			for(uint32 j = 0 ; j < nField; j++)
			{
				//cout << "ROW[j]" << row[j] << endl;
				ret_set->PutValue(i, j, row[j], lengths[j]);
			}
			i++;
		}
		return ret_set;
	}

	char *escapeString(const char *szSrc, char *szDest, unsigned int size)
	{
		if(szSrc == NULL || szDest == NULL || m_mysql == NULL) { return NULL; }
		char *end = szDest;
		mysql_real_escape_string(m_mysql, end, szSrc, size == 0 ? strlen(szSrc) : size);
		return szDest;
	}
	string& escapeString(const std::string& src, string& dest)
	{
		if(m_mysql == NULL) { return dest; }
		char buff[2 * src.length() + 1];
		bzero(buff, sizeof(buff));
		mysql_real_escape_string(m_mysql, buff, src.c_str(), src.length());
		dest = buff;
		return dest;
	}
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
		m_life_time.Now();
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

	bool PutUrl(const char *szUrl, const unsigned int id = 0);
	MysqlHandle *GetHandle(uint32 id = 0);
	void PutHandle(MysqlHandle *handle);
private:
	EntryManager<MysqlHandle, true> m_mum;
	EntryManager<MysqlUrl, true> m_murl;
	uint32 m_max_handle;
};

class AutoHandle
{
public:
	AutoHandle(MysqlPool *pool, uint32 id = 0) : m_pool(pool)
	{
		m_handle = m_pool->GetHandle(id);
	}
	~AutoHandle()
	{
		m_pool->PutHandle(m_handle);
	}
	bool isValid();
	operator MysqlHandle *()
	{
		return m_handle;
	}
	MysqlHandle *operator()() const
	{
		return m_handle;
	}
	MysqlHandle *GetHandle()
	{
		return m_handle;
	}

private:
	MysqlPool	*m_pool;
	MysqlHandle *m_handle;
};
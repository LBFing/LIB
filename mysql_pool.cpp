#include "mysql_pool.h"
#include "logger.h"

MysqlRow::MysqlRow(uint32 nField)
{
	SetField(nField);
}
MysqlRow::~MysqlRow()
{

}

void MysqlRow::SetField(uint32 nField)
{
	m_value.resize(nField);
}
void MysqlRow::SetValue(uint32 nField, const char *value, uint32 nLen)
{
	if(nField < m_value.size())
	{
		m_value[nField].put(value, nLen);
	}
}

VarType& MysqlRow::GetValue(uint32 nField)
{
	if(nField < m_value.size())
	{
		return m_value[nField];
	}
	else
	{
		return VAR_NULL;
	}
}

//==========================================================
DataSet::DataSet(uint32 nRow, uint32 nField)
{
	for(uint32 i = 0 ; i < nRow; i++)
	{
		MysqlRow *row = new MysqlRow(nField);
		if(row)
		{
			row->SetId(i);
			if(m_record.AddEntry(row) == false)
			{
				delete row;
				row = NULL;
			}
		}
	}
}
DataSet::~DataSet()
{
	m_record.RemoveAllEntry();
	m_field_map.clear();
}

bool DataSet::PutField(uint32 nField, const char *szName)
{
	return m_field_map.insert(make_pair(szName, nField)).second;
}

void DataSet::PutValue(uint32 nRow, uint32 nField, const char *value , uint32 nLen)
{
	MysqlRow *row = m_record.GetEntryById(nRow);
	if(row)
	{
		row->SetValue(nField, value, nLen);
	}
}

VarType& DataSet::GetValue(uint32 nRow, string strName)
{
	map<string, uint32>::iterator iter = m_field_map.find(strName);
	if(iter == m_field_map.end())
	{
		return VAR_NULL;
	}

	MysqlRow *row =  m_record.GetEntryById(nRow);
	if(row)
	{
		return row->GetValue(iter->second);
	}
	else
	{
		return VAR_NULL;
	}

}

uint32 DataSet::Size()
{
	return m_record.Size();
}

//============================================

MysqlHandle::MysqlHandle(const MysqlUrl *url, MysqlPool *pool, uint32 id)
{
	SetId(id);
	m_url = url;
	m_state = HandleState_Invalid;
	m_count = 0;
	m_mysql = NULL;
	m_timet_out = 10000L;
	m_mysql_pool = pool;
}
MysqlHandle::~MysqlHandle()
{
	FinalHandle();
	m_url = NULL;
}


bool MysqlHandle::InitMysql()
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

void MysqlHandle::FinalHandle()
{
	if(m_mysql)
	{
		DEBUG("InitHandle():The mysql connect will been closed...");
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
	m_state = HandleState_Invalid;
	m_count = 0;
	m_last_sql = "";
}

bool MysqlHandle::SetHandle()
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

void MysqlHandle::FreeHandle()
{
	m_state = HandleState_Valid;
	m_count--;
}

void MysqlHandle::CheckUseTime()
{
	if(m_use_time.Elapse(Time()) > m_timet_out)
	{
		WARN("sql语句超时:%ums,描述:%s", m_timet_out, m_last_sql.c_str());
		m_timet_out += 10000L;
	}
}

int MysqlHandle::ExecSql(const char *szSql, uint32 nLen, bool need_errlog)
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

DataSet *MysqlHandle::ExeSelect(const char *szSql, unsigned int nLen)
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

char *MysqlHandle::escapeString(const char *szSrc, char *szDest, unsigned int size)
{
	if(szSrc == NULL || szDest == NULL || m_mysql == NULL) { return NULL; }
	char *end = szDest;
	mysql_real_escape_string(m_mysql, end, szSrc, size == 0 ? strlen(szSrc) : size);
	return szDest;
}

string& MysqlHandle::escapeString(const std::string& src, string& dest)
{
	if(m_mysql == NULL) { return dest; }
	char buff[2 * src.length() + 1];
	bzero(buff, sizeof(buff));
	mysql_real_escape_string(m_mysql, buff, src.c_str(), src.length());
	dest = buff;
	return dest;
}

bool MysqlHandle::InitHandle()
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

//============================================
MysqlPool::MysqlPool(int mMaxHandle)
{
	m_max_handle = mMaxHandle;
	printf("Version of the mysql libs is %s\n" , mysql_get_client_info());
	if(!mysql_thread_safe())
	{
		WARN("The mysql libs is not thread safe...");
	}
}

MysqlPool::~MysqlPool()
{
	m_mum.RemoveAllEntry();
	m_murl.RemoveAllEntry();
}


bool MysqlPool::PutUrl(const char *szUrl, const unsigned int id)
{
	MysqlUrl *mysql_url = new MysqlUrl(szUrl);
	if(mysql_url == NULL)
	{
		return false;
	}
	mysql_url->SetId(id);
	m_murl.AddEntry(mysql_url);

	MysqlHandle *handle = new MysqlHandle(mysql_url, this, id);
	if(handle == NULL)
	{
		m_murl.RemoveEntry(mysql_url);
		return false;
	}

	if(handle->InitMysql() == false)
	{
		m_murl.RemoveEntry(mysql_url);
		return false;
	}

	if(m_mum.AddEntry(handle) == false)
	{
		m_murl.RemoveEntry(mysql_url);
		handle->FinalHandle();
		delete handle;
		handle = NULL;
		return false;
	}
	return true;
}


MysqlHandle *MysqlPool::GetHandle(uint32 id)
{

	struct GetHandleExec : public Callback<MysqlHandle>
	{
		MysqlHandle *_handle;
		GetHandleExec(): _handle(NULL) {}
		bool exec(MysqlHandle *entry)
		{
			switch(entry->m_state)
			{
				case HandleState_Valid:
				case HandleState_Invalid:
					{
						if(entry->SetHandle())
						{
							_handle = entry;
							return false;
						}
					}
					break;
				case HandleState_Used:
					{
						entry->CheckUseTime();
					}
					break;
				default:
					break;
			}
			return true;
		}
	};
	GetHandleExec Exec;
	while(true)
	{
		m_mum.execEveryEntry<>(Exec);
		if(Exec._handle)
		{
			return Exec._handle;
		}
		if(m_mum.Size() < m_max_handle)
		{
			MysqlUrl *url = m_murl.GetEntryById(id);
			if(url == NULL)
			{
				return NULL;
			}

			MysqlHandle *handle = new MysqlHandle(url, this, id);
			if(handle == NULL)
			{
				return NULL;
			}

			if(handle->InitMysql() == false)
			{
				delete handle;
				handle = NULL;
				return NULL;
			}

			if(m_mum.AddEntry(handle) == false)
			{
				handle->FinalHandle();
				delete handle;
				handle = NULL;
				return NULL;
			}
			return handle;
		}
		usleep(1000 * 50);

	}
	return NULL;
}

void MysqlPool::PutHandle(MysqlHandle *handle)
{
	if(handle)
	{
		handle->FreeHandle();
	}
}

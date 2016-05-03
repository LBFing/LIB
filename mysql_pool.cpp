#include "mysql_pool.h"

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


MysqlPool::MysqlPool(int mMaxHandle)
{
	m_max_handle = mMaxHandle;
	printf("Version of the mysql libs is %s\n" , mysql_get_client_info());
	if(!mysql_thread_safe())
	{
		printf("The mysql libs is not thread safe...\n");
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

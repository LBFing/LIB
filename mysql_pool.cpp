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
	m_field_set.clear();
}

bool DataSet::PutField(uint32 nField, const char *szName)
{
	MysqlField field;
	strncpy(field.szName, szName, 127);
	field.nIndex = nField;
	return m_field_set.insert(field).second;
}

void DataSet::PutValue(uint32 nRow, uint32 nField, const char *value , uint32 nLen)
{
	MysqlRow *row = m_record.GetEntryById(nRow);
	if(row)
	{
		row->SetValue(nField, value, nLen);
	}
}

VarType& DataSet::GetValue(uint32 nRow, const char *szName)
{
	MysqlField field;
	strncpy(field.szName, szName, 127);
	IterFeild iter_field = m_field_set.find(field);
	if(iter_field == m_field_set.end())
	{
		return VAR_NULL;
	}

	MysqlRow *row =  m_record.GetEntryById(nRow);
	if(row)
	{
		return row->GetValue(iter_field->nIndex);
	}
	else
	{
		return VAR_NULL;
	}
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

}

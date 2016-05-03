#pragma once
#include "type_define.h"

//T 必须为Entry 类型，才能放入Entry管理器中，支持调用
//Callback 回调累，用于遍历EntryManager中的对象
template <typename T>
struct Callback
{
	virtual bool exec(T *e) = 0;
	virtual ~Callback() {};
};

//EntryManager 管理器
//T必须为Entry类型
//bInc 为false时 Entry id在Add之后自动分配; 为true时，需要在Add之前设置
template<typename T, bool bInc>
class EntryManager
{
public:
	EntryManager();
	virtual ~EntryManager();
	T *GetEntryById(uint32 id);
	bool AddEntry(T *entry);
	void RemoveEntry(T *entry);
	void RemoveEntryById(uint32 id);
	void RemoveAllEntry();
	uint32 Size() {return m_mapEntry.size();}

	template <typename T1>
	bool execEveryEntry(Callback<T1>& callback)
	{
		for(EntryMapIter iter = m_mapEntry.begin(); iter != m_mapEntry.end(); iter++)
		{
			if(!callback.exec((T1 *)iter->second))
			{
				return false;
			}
		}
		return true;
	}
private:
	void ClearAll();
private:
	typedef map<uint32, T *> EntryMap;
	typedef typename EntryMap::iterator EntryMapIter;

	EntryMap m_mapEntry;
	static uint32 m_increase;
};

template<typename T, bool bInc>
uint32 EntryManager<T, bInc>::m_increase = 100000;

template<typename T, bool bInc>
EntryManager<T, bInc>::EntryManager()
{
	m_mapEntry.clear();
}

template<typename T, bool bInc>
EntryManager<T, bInc>::~EntryManager()
{
	ClearAll();
}

template<typename T, bool bInc>
T *EntryManager<T, bInc>::GetEntryById(uint32 id)
{
	EntryMapIter iter = m_mapEntry.find(id);
	if(iter == m_mapEntry.end())
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}

template<typename T, bool bInc>
bool EntryManager<T, bInc>::AddEntry(T *entry)
{
	if(bInc)
	{
		uint32 id = entry->GetId();
		EntryMapIter iter = m_mapEntry.find(id);
		if(iter != m_mapEntry.end())
		{
			return false;
		}
		else
		{
			m_mapEntry.insert(make_pair<uint32, T *>(id, entry));
			return true;
		}
	}
	else
	{
		m_increase++;
		entry->SetId(m_increase);
		m_mapEntry.insert(make_pair<uint32, T *>(entry->GetId(), entry));
		return true;
	}

}

template<typename T, bool bInc>
void EntryManager<T, bInc>::RemoveEntry(T *entry)
{
	m_mapEntry.erase(entry->GetId());
	if(entry)
	{
		delete entry;
		entry = NULL;
	}
}

template<typename T, bool bInc>
void EntryManager<T, bInc>::RemoveEntryById(uint32 id)
{
	T *entry = GetEntryById(id);
	if(entry)
	{
		RemoveEntry(entry);
	}
}

template<typename T, bool bInc>
void EntryManager<T, bInc>::RemoveAllEntry()
{
	ClearAll();
}

template<typename T, bool bInc>
void EntryManager<T, bInc>::ClearAll()
{
	for(EntryMapIter iter = m_mapEntry.begin(); iter != m_mapEntry.end(); iter++)
	{
		if(iter->second != NULL)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
	m_mapEntry.clear();
}
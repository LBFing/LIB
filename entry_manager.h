#pragma once
#include "type_define.h"

template <typename T>
struct Callback
{
	virtual bool exec(T *e) = 0;
	virtual ~Callback() {};
};


template<typename T, bool bInc>
class EntryManager
{
public:
	EntryManager();
	virtual ~EntryManager();
	T *GetEntryById(uint32 id);
	bool AddEntry(T *entry);
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
	cout << "bInc :" << bInc << endl;
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
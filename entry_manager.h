#pragma once

template<typename T>
class EntryManager
{
public:
	EntryManager();
	virtual ~EntryManager();
	T* GetEntryById(uint32 id);
	bool AddEntry(T* entry);
private:
	typedef map<uint32,T*> EntryMap;
	typedef EntryMap::iterator EntryMapIter;
	
	EntryMap m_mapEntry;
};

template<typename T>
EntryManager::EntryManager()
{
	m_mapEntry.clear();
}

template<typename T>
EntryManager::~EntryManager()
{
	for(EntryMapIter iter = m_mapEntry.begin(); iter != m_mapEntry.end(); iter++)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
	m_mapEntry.clear();
}

template<typename T>
T* EntryManager::GetEntryById(uint32 id)
{
	EntryMapIter iter = m_mapEntry.find(id);
	if (iter == m_mapEntry.end())
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}

template<typename T>
bool EntryManager::AddEntry(T* entry)
{
	uint32 id = entry->GetId();
	EntryMapIter iter = m_mapEntry.find(id);
	if (iter != m_mapEntry.end())
	{
		return false	
	}
	else
	{
		m_mapEntry.insert(id,entry);
		return true;
	}
}
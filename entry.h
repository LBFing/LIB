#pragma once
#include "type_define.h"

class Entry
{
public:
	uint32 GetId()const
	{
		return m_id;
	}

	void SetId(uint32 id)
	{
		m_id = id;
	}
private:
	uint32 m_id;
};


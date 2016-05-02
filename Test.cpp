#include "singleton.h"
#include "entry.h"
#include "entry_manager.h"
#include "buffer.h"
#include "timer.h"
#include "tinyxml2.h"
#include "parse_json.h"
#include "message_queue.h"
#include "regex_parse.h"
#include "mysql_pool.h"
#include "var_type.h"

using namespace tinyxml2;

class CTestSingle : public SingletonBase<CTestSingle>
{
public:
	friend class SingletonBase<CTestSingle>;
	void Print() {cout << "Print" << endl;}
	CTestSingle() {cout << "CTestSingle" << endl;}
	~CTestSingle() {cout << "~CTestSingle" << endl;}
};


class Item : public Entry
{
public:
	Item()
	{
		m_name = "Item";
	}
	~Item() {cout << "~Item" << endl;}

	void Print() {cout << "Item :" << GetId() << endl;}
private:
	string m_name;
};


void TestSingleTone()
{
	CTestSingle::newInstance();
	CTestSingle::getInstance().Print();
	CTestSingle::delInstance();
}

void TestEntryManager()
{
	EntryManager<Item, false> ItemManager;
	Item *item1 = new Item;
	Item *item2 = new Item;
	Item *item3 = new Item;
	item1->SetId(1);
	item2->SetId(2);
	item3->SetId(3);


	ItemManager.AddEntry(item1);
	ItemManager.AddEntry(item2);
	ItemManager.AddEntry(item3);

	cout << ItemManager.Size() << endl;

	struct CallTest : public Callback<Item>
	{
		CallTest() {}
		bool exec(Item *item)
		{
			if(item->GetId() == 1)
			{
				item->Print();
				return false;
			}
			item->Print();
			return true;
		}
	} Exec;

	ItemManager.execEveryEntry<>(Exec);

}

void TestBuffer()
{

	typedef struct CmdType
	{
		uint8 first;
		uint8 second;
	} Cmd;

	Cmd cmd1 = {1, 2};
	Cmd cmd2 = {3, 4};



	BufferCmdQueue cmd_buffer;
	cout << cmd_buffer.maxSize() << endl;

	cmd_buffer.put((uint8 *)&cmd1, sizeof(Cmd));
	cout << cmd_buffer.rd_size() << "  " << cmd_buffer.wr_size() << endl;
	cmd_buffer.put((uint8 *)&cmd2, sizeof(Cmd));
	cout << cmd_buffer.rd_size() << "  " << cmd_buffer.wr_size() << endl;
}

void TestTimer()
{
	Time t2;
	Time t1;
	char Buffer[20] = {};
	t1.Format(Buffer, 20);
	cout << "Time:" << Buffer << endl;

	t1.AddDelay(1000L * 10);
	t1.Format(Buffer, 20);
	cout << "Time:" << Buffer << endl;

	cout << t2.Elapse(t1) << endl;;

	//Timer t3(1000L);
	//uint64 count = 0;
	// while(1)
	// {
	// 	Time t4;
	// 	if(t3(t4))
	// 	{
	// 		t4.Format(Buffer, 20);
	// 		cout << "Time:" << Buffer << endl;
	// 		cout << "count:" << count << endl;
	// 		count++;
	// 	}

	// 	// if(count == 3)
	// 	// {
	// 	// 	t3.Stop();
	// 	// 	t3.Next(t1, 5000L);
	// 	// 	count++;
	// 	// }

	// 	if(count == 3)
	// 	{
	// 		break;
	// 	}
	// }
	Clocker clocker(16 * 3600 + 19 * 60, 24 * 3600);
	uint64 escape = clocker.LeftSec(Time());
	cout << "escape:" << escape / 3600 << ":" << (escape % 3600) / 60 << endl;
}

void TestXMLParse()
{
	XMLDocument doc;
	doc.LoadFile("test.xml");
	XMLElement *scene = doc.RootElement();
	XMLElement *surface = scene->FirstChildElement("node");
	while(surface)
	{
		XMLElement *surfaceChild = surface->FirstChildElement();
		const char *content;
		const char *name ;
		const XMLAttribute *attributeOfSurface = surface->FirstAttribute();
		cout << attributeOfSurface->Name() << ":" << attributeOfSurface->Value() << endl;
		while(surfaceChild)
		{
			name = surfaceChild->Name();
			content = surfaceChild->GetText();
			surfaceChild = surfaceChild->NextSiblingElement();
			cout << name << ":" << content << endl;
		}
		surface = surface->NextSiblingElement();
	}
}

int TestJson()
{
	JSonParse dc;
	char szFileName[] = "test.json";
	dc.LoadFile(szFileName);
	dc.Parse();
	cJSON *pSub = dc.GetObjectItem("hello");
	printf("obj_1 : %s\n", pSub->valuestring);
	//parseJson(makeJson());
	return 1;
}


int TestRegex()
{

	Regex re;
	//int port = 3306;
	string player;
	string passwd;
	string host;
	string port_str;
	string dbname;
	string url = "mysql://zqgame:zqgame@183.61.85.89:3306/zqgame_001";
	if(re.Compile("mysql://(.+):(.+)@(.+):(.+)/(.+)") && re.Match(url.c_str()))
	{
		std::string port_str;
		re.GetSub(player, 1);
		re.GetSub(passwd, 2);
		re.GetSub(host, 3);
		re.GetSub(port_str, 4);
		//port = atoi(port_str.c_str());
		re.GetSub(dbname, 5);
		printf("%s, %s, %s, %s, %s\n", player.c_str(), passwd.c_str(), host.c_str(), port_str.c_str(), dbname.c_str());
	}
	return 0;
}

void TestMessage()
{
	struct Cmd
	{
		int first;
		int second;
	};

	MsgQueue obj;
	Cmd cmd;

	for(int i = 0 ; i < 2048; i++)
	{
		cmd.first = i;
		cmd.second = i + 1;
		obj.Put(sizeof(cmd), (unsigned char *)(&cmd));
	}

	for(int i = 0 ; i < 20480; i++)
	{
		CmdPair *cmd_pair =  obj.Get();
		if(cmd_pair == NULL) { continue; }
		Cmd *cmd = (Cmd *)cmd_pair->second;
		cout << cmd->first << "  " << cmd->second << endl;
		obj.Erase();
	}

	cout << obj.m_cmd_write << endl;
	cout << obj.m_cmd_read << endl;
	cout << obj.m_queue.size() << endl;

	cout << "==============" << endl;

	cmd.first = 0;
	cmd.second = 0 + 1;
	obj.Put(sizeof(cmd), (unsigned char *)(&cmd));
	cout << "==============" << endl;

	cout << obj.m_cmd_write << endl;
	cout << obj.m_cmd_read << endl;
	cout << obj.m_queue.size() << endl;

}

void TestMysqlPool()
{
	MysqlPool obj(111);
}


void TestVarType()
{
	uint32 nRow = 7;
	uint32 nField = 5;
	char szName[10] = {0};

	DataSet *ret_set = new DataSet(nRow, nField);
	if(ret_set == NULL)
	{
		printf("error dataset new\n");
	}

	for(uint32 i = 0 ; i < nField; i++)
	{
		sprintf(szName, "Field%d", i);
		if(ret_set->PutField(i, szName) == false)
		{
			printf("error PutField\n");
			return;
		}
	}

	// IterFeild iter1 = ret_set->m_field_set.begin();
	// IterFeild iter2 = ret_set->m_field_set.end();
	// while(iter1  != iter2)
	// {
	// 	cout << (*iter1).nIndex << ":" << (*iter1).szName << endl;
	// 	iter1++;
	//}
	//cout << ret_set->m_field_set.size() << " " << ret_set->m_record.Size() << endl;

	for(uint32 i = 0 ; i < nRow; i++)
	{
		for(uint32 j = 0 ; j < nField; j++)
		{
			sprintf(szName, "value_%d_%d", i, j);
			ret_set->PutValue(i, j, szName, strlen(szName) + 1);
		}
	}

	for(uint32 i = 0 ; i < nRow; i++)
	{
		const char *value = ret_set->GetValue(i, "Field1");
		cout << "Field1:" << value << endl;
	}
}


int main(int argc, char const *argv[])
{
	//TestXMLParse();
	//TestJson();
	//TestMessage();
	//TestRegex();
	TestMysqlPool();
	TestVarType();


	return 0;
}

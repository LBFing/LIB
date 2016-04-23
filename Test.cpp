#include "singleton.h"
#include "entry.h"
#include "entry_manager.h"
#include "buffer.h"
#include "timer.h"

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


void Test()
{
	CTestSingle::newInstance();
	CTestSingle::getInstance().Print();
	CTestSingle::delInstance();
}

void Test1()
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

void Test2()
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

void Test3()
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

	Timer t3(1000L);
	uint64 count = 0;
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

int main(int argc, char const *argv[])
{
	Test();
	Test1();
	Test2();
	Test3();
	return 0;
}

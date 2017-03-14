#include "mysql_pool.h"

void TestMysqlPool()
{
	MysqlPool obj(111);
	string str = "mysql://root:123456@127.0.0.1:3306/LBF";
	bool bRet = obj.PutUrl(str.c_str(), 1);
	if (bRet)
	{
		cout << "mysql init success" << endl;
	}

	AutoHandle handle(&obj, 1);
	if (!handle())
	{
		printf("不能从数据库连接池获取连接句柄\n");
		return ;
	}

	string strSql = "select * from SERVERLIST";
	DataSet* ret_set = handle()->ExeSelect(strSql.c_str(), strSql.length());

	if (!ret_set || ret_set->Size() == 0)
	{
		if (ret_set != NULL)
		{
			delete ret_set;
			ret_set = NULL;
		}
		printf("数据库中的记录为空\n");
		return;
	}

	for (uint32 i = 0 ; i < ret_set->Size() ; i++)
	{
		int a1 = ret_set->GetValue(i, "ID");
		int a2 = ret_set->GetValue(i, "TYPE");
		int a3 = ret_set->GetValue(i, "DYNAMIC");
		const char* b1 = ret_set->GetValue(i, "NAME");
		const char* b2 = ret_set->GetValue(i, "IP");
		int a4 = ret_set->GetValue(i, "PORT");
		const char* b3 = ret_set->GetValue(i, "EXTIP");
		int a5 = ret_set->GetValue(i, "EXTPORT");
		int a6 = ret_set->GetValue(i, "NETTYPE");
		printf("%d\t%d\t%d\t%s\t%s\t%d\t%s\t%d\t%d\n", a1, a2, a3, b1, b2, a4, b3, a5, a6);
	}

}

int main(int argc, char const* argv[])
{
	TestMysqlPool();
	return 0;
}
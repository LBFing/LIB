#include "singleton.h"



class CTestSingle : public SingletonBase<CTestSingle>
{
public:
	friend SingletonBase<CTestSingle>;
	void Print() {cout<<"Print"<<endl;}
	CTestSingle(){cout<<"CTestSingle"<<endl;}
	~CTestSingle(){cout<<"~CTestSingle"<<endl;}
};

int main(int argc, char const *argv[])
{
	CTestSingle::newInstance();
	CTestSingle::getInstance().Print();
	CTestSingle::delInstance();
	return 0;
}
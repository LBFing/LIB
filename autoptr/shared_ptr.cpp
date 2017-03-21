#include <iostream>
#include <boost/smart_ptr.hpp>

using namespace std;

class Simple
{
public:
	Simple(int param = 0)
	{
		number = param;
		std::cout << "Simple: " << number << std::endl;
	}

	~Simple()
	{
		std::cout << "~Simple: " << number << std::endl;
	}

	void PrintSomething()
	{
		std::cout << "PrintSomething: " << info_extend.c_str() << std::endl;
	}

	std::string info_extend;
	int number;
};

void TestSharedPtr(boost::shared_ptr<Simple> memory)    // 注意：无需使用 reference (或 const reference)
{
	memory->PrintSomething();
	std::cout << "TestSharedPtr UseCount: " << memory.use_count() << std::endl;
}

void TestSharedPtr2()
{
	boost::shared_ptr<Simple> my_memory(new Simple(1));
	if (my_memory.get())
	{
		my_memory->PrintSomething();
		my_memory.get()->info_extend = "Addition";
		my_memory->PrintSomething();
		(*my_memory).info_extend += " other";
		my_memory->PrintSomething();
	}

	std::cout << "TestSharedPtr2 UseCount: " << my_memory.use_count() << std::endl;
	TestSharedPtr(my_memory);
	std::cout << "TestSharedPtr2 UseCount: " << my_memory.use_count() << std::endl;

	//my_memory.release();// 编译 error: 同样，shared_ptr 也没有 release 函数
	//}
}

int main()
{
	TestSharedPtr2();
	return 0;
}

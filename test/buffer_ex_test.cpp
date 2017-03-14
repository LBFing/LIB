#include "buffer_ex.h"


#define BOOST_CHECK_EQUAL(a,b) \
	if(a != b) \
	{\
		std::cout << "Test Failed Line:" <<__LINE__ <<endl; \
		exit(0); \
	}

void TestBuff1()
{
	BufferEx buf;
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 0);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);


	const string str(200, 'x');
	buf.Append(str);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), str.size());
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - str.size());
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);

	const string str2 =  buf.RetrieveAsString(50);
	BOOST_CHECK_EQUAL(str2.size(), 50);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), str.size() - str2.size());
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - str.size());
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend + str2.size());
	BOOST_CHECK_EQUAL(str2, string(50, 'x'));

	cout << __FUNCTION__ << " success!!!" << endl;
}

int main()
{
	TestBuff1();
	return 0;
}

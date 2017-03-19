#include "log_stream.h"

#define BOOST_CHECK_EQUAL(a,b) \
	if(a != b) \
	{\
		std::cout << "Test Failed Line:" <<__LINE__ <<endl; \
		exit(0); \
	}


void TestBuff1()
{
	LogStream os;
	const LogStream::Buffer& buf = os.GetBuffer();
	BOOST_CHECK_EQUAL(buf.ToString(), string(""));
	os << true;
	BOOST_CHECK_EQUAL(buf.ToString(), string("1"));
	os << '\n';
	BOOST_CHECK_EQUAL(buf.ToString(), string("1\n"));
	os << false;
	BOOST_CHECK_EQUAL(buf.ToString(), string("1\n0"));

	os.ResetBuffer();
	BOOST_CHECK_EQUAL(buf.ToString(), string(""));
	os << 1;
	BOOST_CHECK_EQUAL(buf.ToString(), string("1"));
	os << 0;
	BOOST_CHECK_EQUAL(buf.ToString(), string("10"));
	os << -1;
	BOOST_CHECK_EQUAL(buf.ToString(), string("10-1"));

	os.ResetBuffer();

	os << 0 << " " << 123 << 'x' << 0x64;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0 123x100"));

	os.ResetBuffer();
	os << -2147483647;
	BOOST_CHECK_EQUAL(buf.ToString(), string("-2147483647"));
	os << static_cast<int>(-2147483647 - 1);
	BOOST_CHECK_EQUAL(buf.ToString(), string("-2147483647-2147483648"));
	os << ' ';
	os << 2147483647;
	BOOST_CHECK_EQUAL(buf.ToString(), string("-2147483647-2147483648 2147483647"));
	os.ResetBuffer();


	os << std::numeric_limits<int16_t>::min();
	BOOST_CHECK_EQUAL(buf.ToString(), string("-32768"));
	os.ResetBuffer();

	os << std::numeric_limits<uint16_t>::min();
	BOOST_CHECK_EQUAL(buf.ToString(), string("0"));
	os.ResetBuffer();

	os << std::numeric_limits<uint16_t>::max();
	BOOST_CHECK_EQUAL(buf.ToString(), string("65535"));
	os.ResetBuffer();

	os << std::numeric_limits<int32_t>::min();
	BOOST_CHECK_EQUAL(buf.ToString(), string("-2147483648"));
	os.ResetBuffer();

	os << std::numeric_limits<int32_t>::max();
	BOOST_CHECK_EQUAL(buf.ToString(), string("2147483647"));
	os.ResetBuffer();

	os << std::numeric_limits<uint32_t>::min();
	BOOST_CHECK_EQUAL(buf.ToString(), string("0"));
	os.ResetBuffer();

	os << std::numeric_limits<uint32_t>::max();
	BOOST_CHECK_EQUAL(buf.ToString(), string("4294967295"));
	os.ResetBuffer();


	os << std::numeric_limits<int64_t>::min();
	BOOST_CHECK_EQUAL(buf.ToString(), string("-9223372036854775808"));
	os.ResetBuffer();

	os << std::numeric_limits<int64_t>::max();
	BOOST_CHECK_EQUAL(buf.ToString(), string("9223372036854775807"));
	os.ResetBuffer();

	os << std::numeric_limits<uint64_t>::min();
	BOOST_CHECK_EQUAL(buf.ToString(), string("0"));
	os.ResetBuffer();

	os << std::numeric_limits<uint64_t>::max();
	BOOST_CHECK_EQUAL(buf.ToString(), string("18446744073709551615"));
	os.ResetBuffer();


	int16_t a1 = 1;
	int32_t b1 = 2;
	int64_t c1 = 3;
	os << a1;
	os << b1;
	os << c1;
	BOOST_CHECK_EQUAL(buf.ToString(), string("123"));

	os.ResetBuffer();

	os << 0.0;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0"));
	os.ResetBuffer();

	os << 1.0;
	BOOST_CHECK_EQUAL(buf.ToString(), string("1"));
	os.ResetBuffer();

	os << 0.1;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0.1"));
	os.ResetBuffer();

	os << 0.05;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0.05"));
	os.ResetBuffer();

	os << 0.15;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0.15"));
	os.ResetBuffer();

	double a = 0.1;
	os << a;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0.1"));
	os.ResetBuffer();

	double b = 0.05;
	os << b;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0.05"));
	os.ResetBuffer();

	double c = 0.15;
	os << c;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0.15"));
	os.ResetBuffer();

	os << a + b;
	BOOST_CHECK_EQUAL(buf.ToString(), string("0.15"));
	os.ResetBuffer();

	os << 1.23456789;
	BOOST_CHECK_EQUAL(buf.ToString(), string("1.23456789"));
	os.ResetBuffer();

	os << 1.234567;
	BOOST_CHECK_EQUAL(buf.ToString(), string("1.234567"));
	os.ResetBuffer();

	os << -123.456;
	BOOST_CHECK_EQUAL(buf.ToString(), string("-123.456"));
	os.ResetBuffer();


	os << static_cast<void*>(0);
	BOOST_CHECK_EQUAL(buf.ToString(), string("0x0"));
	os.ResetBuffer();

	os << reinterpret_cast<void*>(8888);
	BOOST_CHECK_EQUAL(buf.ToString(), string("0x22B8"));
	os.ResetBuffer();


	os << "Hello ";
	BOOST_CHECK_EQUAL(buf.ToString(), string("Hello "));

	string chenshuo = "Shuo Chen";
	os << chenshuo;
	BOOST_CHECK_EQUAL(buf.ToString(), string("Hello Shuo Chen"));

	os.ResetBuffer();

	os << Fmt("%4d", 1);
	BOOST_CHECK_EQUAL(buf.ToString(), string("   1"));
	os.ResetBuffer();

	os << Fmt("%4.2f", 1.2);
	BOOST_CHECK_EQUAL(buf.ToString(), string("1.20"));
	os.ResetBuffer();

	os << Fmt("%4.2f", 1.2) << Fmt("%4d", 43);
	BOOST_CHECK_EQUAL(buf.ToString(), string("1.20  43"));
	os.ResetBuffer();

	for (int i = 0; i < 399; ++i)
	{
		os << "123456789 ";
		BOOST_CHECK_EQUAL(buf.Length(), 10 * (i + 1));
		BOOST_CHECK_EQUAL(buf.Avail(), 4000 - 10 * (i + 1));
	}

	os << "abcdefghi ";
	BOOST_CHECK_EQUAL(buf.Length(), 3990);
	BOOST_CHECK_EQUAL(buf.Avail(), 10);

	os << "abcdefghi";
	BOOST_CHECK_EQUAL(buf.Length(), 3999);
	BOOST_CHECK_EQUAL(buf.Avail(), 1);

	os << "123";
	cout << "Length :" << buf.Length() << endl;
	cout << "Avail :" << buf.Avail() << endl;
	cout << __FUNCTION__ << " success!!!" << endl;

}

int main()
{
	TestBuff1();
	return 0;
}

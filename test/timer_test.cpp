#include "timer.h"


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

	Timer t3(1000L);
	uint64 count = 0;
	while (1)
	{
		Time t4;
		if (t3(t4))
		{
			t4.Format(Buffer, 20);
			cout << "Time:" << Buffer << endl;
			cout << "count:" << count << endl;
			count++;
		}

		if (count == 3)
		{
			break;
		}
	}
	Clocker clocker(16 * 3600 + 19 * 60, 24 * 3600);
	uint64 escape = clocker.LeftSec(Time());
	cout << "escape:" << escape / 3600 << ":" << (escape % 3600) / 60 << endl;
}

int main(int argc, char const* argv[])
{
	TestTimer();
}
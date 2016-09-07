#pragma once
#include <iostream>
using namespace std;

class Noncopyable
{
protected:
	Noncopyable() {};
	~Noncopyable() {};
private:
	Noncopyable(const Noncopyable&);
	const Noncopyable& operator = (const Noncopyable&);
};
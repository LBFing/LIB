#ifndef __TYPE_DEFINE_H__
#define __TYPE_DEFINE_H__

#include <iostream>
#include <assert.h>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <vector>
#include <set>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <errno.h>
#include <algorithm>
#include <sys/uio.h>
#include <sys/timerfd.h>
#include <limits>
#include <memory>
#include <poll.h>

using namespace std;

typedef float f32;
typedef double f64;
typedef char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef unsigned long uLong;

#define  SECOND 1000000

template <bool x> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> { enum {value = 1};};
template <int x> struct static_assert_test{};

#define JOIN_TEST(a,b) a##b

#define STATIC_ASSERT_CHECK(B) \
	typedef static_assert_test<sizeof(STATIC_ASSERTION_FAILURE<(bool)(B)>)> \
	JOIN_TEST(st_assert_test,__LINE__) __attribute__((unused))


#endif

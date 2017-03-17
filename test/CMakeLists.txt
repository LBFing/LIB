CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(CMAKE_BUILE_TYPE DEBUG)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wall -Werror -g")

#SET(EXECUTE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test/bin)

INCLUDE_DIRECTORIES("/root/study/LIB/")

FIND_LIBRARY(MYLIB LBFing /root/study/LIB/build/lib)
IF(NOT MYLIB)
	MESSAGE(FATAL_ERROR "libLBFing.a not found")
ENDIF(NOT MYLIB)

LINK_LIBRARIES(mysqlclient pthread z m rt dl hiredis)

add_executable(buffer_ex_test buffer_ex_test.cpp)

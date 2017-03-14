CC      =   g++
FLAGS   =   -Wall -Werror -g
SRCFILE =   $(wildcard *.cpp)
OBJS    =   $(patsubst %.cpp, %.o, $(SRCFILE))
LIBS    =   -L../ -lLBFing -lpthread  -lrt
INC     =   -I../

LIBS_MYSQL= ${shell mysql_config --libs_r}
CPPFLAGS_MYSQL=${shell mysql_config --cflags}

LIBS_REDIS= ${shell pkg-config --libs hiredis}
CPPFLAGS_REDIS=${shell pkg-config --cflags hiredis}

#mysql库
LIBS += $(LIBS_MYSQL)
INC += $(CPPFLAGS_MYSQL)

#redis库
LIBS += $(LIBS_REDIS)
INC += $(CPPFLAGS_REDIS)

PROGRAM = 	buffer_ex_test \
		  	singleton_test \
		  	entry_manager_test \
		  	buffer_test \
		  	timer_test \
		  	xml_test \
		  	json_test \
		  	regex_test \
		  	message_queueu_test \
		  	mysql_test 


defalut:	$(PROGRAM)

%.o:%.cpp
	$(CC) $(FLAGS) $(INC) -c $< -o $@

buffer_ex_test:buffer_ex_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

singleton_test:singleton_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

entry_manager_test:entry_manager_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

buffer_test:buffer_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

timer_test:timer_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

xml_test:xml_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

json_test:json_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

regex_test:regex_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

message_queueu_test:message_queueu_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

mysql_test:mysql_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)
	$(RM) *.o

CC		=	g++
FLAGS	= 	-Wall -g
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
PROGRAM =	Main
LIBS 	=  -lpthread  -lrt
INC 	=

LIBS_MYSQL= ${shell mysql_config --libs_r}
CPPFLAGS_MYSQL=${shell mysql_config --cflags}

LIBS += $(LIBS_MYSQL)
INC += $(CPPFLAGS_MYSQL)

defalut:	$(OBJS) $(PROGRAM)

%.o:%.cpp
	$(CC) $(FLAGS) $(INC) -c $< -o $@

$(PROGRAM):$(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)

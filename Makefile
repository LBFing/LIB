CC	=	g++
FLAGS	= 	-Wall
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
PROGRAM =	Main

OSTYPE = `uname`
ifneq ($OSTYPE,Linux)
	FLAGS += -D WIN32
endif


defalut:	$(OBJS) $(PROGRAM)

%.o:%.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(PROGRAM):$(OBJS)
	$(CC) $(FLAGS) -o $@ $^
.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)
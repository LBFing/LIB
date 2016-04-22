CC		=	g++
FLAGS	= 	-Wall
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
PROGRAM =	Main
LIBS 	=  

OSTYPE = `uname`
ifneq ($OSTYPE,Linux)
	FLAGS += -D WIN32
	LIBS  += -lpthreadGC2
else
	LIBS  += -lpthread
endif


defalut:	$(OBJS) $(PROGRAM)

%.o:%.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(PROGRAM):$(OBJS)
	$(CC) $(FLAGS) $(LIBS) -o $@ $^
.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)


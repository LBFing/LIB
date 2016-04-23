CC		=	g++
FLAGS	= 	-Wall -g
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
PROGRAM =	Main
LIBS 	=  -lpthread  -lrt

defalut:	$(OBJS) $(PROGRAM)

%.o:%.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(PROGRAM):$(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)

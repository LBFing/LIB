CC=g++
CPPFLAGS=-Wall -Werror -g
BIN=svr_poll

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@
$(BIN):poll_server.o
	$(CC) -o $@ $^

clean:
	rm -f *.o $(BIN)

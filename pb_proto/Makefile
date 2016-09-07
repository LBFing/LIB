all: proto_msg write reader

clean:
	rm -f write reader msg.*.cc msg.*.h *.o  log

proto_msg:
	protoc --cpp_out=. msg.proto


write: msg.pb.cc write.cc
	g++  msg.pb.cc write.cc -o write  `pkg-config --cflags --libs protobuf`

reader: msg.pb.cc reader.cc
	g++  msg.pb.cc reader.cc -o reader  `pkg-config --cflags --libs protobuf`

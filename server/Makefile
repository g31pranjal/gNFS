
PROTO_PATH = ../proto/
UTILS_PATH = ../utils/

all :
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o nfsServer.o nfsServer.cc
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o server.o server.cc
	g++ -std=c++11 $(PROTO_PATH)schema.pb.o $(PROTO_PATH)schema.grpc.pb.o nfsServer.o server.o $(UTILS_PATH)utils.o -L/usr/local/lib  -lstdc++fs `pkg-config --libs --cflags fuse3 protobuf grpc++` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -o server

clean:
	rm nfsServer.o server.o server
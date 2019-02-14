
PROTO_PATH = ../proto/
UTILS_PATH = ../utils/

all:
	g++ -std=c++11 `pkg-config --cflags --libs fuse3` -c wrapper.cc -o wrapper.o
	g++ -std=c++11 -Wall  `pkg-config --cflags --libs fuse3` -c client.cc -o client.o
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o nfsClient.o nfsClient.cc
	g++ -std=c++11 $(PROTO_PATH)schema.pb.o $(PROTO_PATH)schema.grpc.pb.o client.o wrapper.o nfsClient.o $(UTILS_PATH)utils.o -L/usr/local/lib -lstdc++fs `pkg-config --libs --cflags fuse3 protobuf grpc++` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -o client

clean: 
	rm wrapper.o client client.o nfsClient.o
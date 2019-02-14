
PROTO_PATH = "../proto/"

all: 
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` utils.cc -c -o utils.o

clean:
	rm utils.o

	
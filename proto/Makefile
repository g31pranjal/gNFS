
all :
	protoc --cpp_out=. schema.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` schema.proto
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o schema.pb.o schema.pb.cc
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o schema.grpc.pb.o schema.grpc.pb.cc


clean :
	rm schema.grpc.pb.cc schema.grpc.pb.h
	rm schema.pb.cc schema.pb.h
	rm schema.pb.o
	rm schema.grpc.pb.o

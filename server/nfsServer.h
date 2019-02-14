#ifndef NFSSERVER
#define NFSSERVER

#include <sys/stat.h>
#include <dirent.h>
#include <map>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "../proto/schema.grpc.pb.h"
#include "../utils/utils.h"

#include "../utils/utils.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using NfsProtocol::attribute_request;
using NfsProtocol::attribute_response;
using NfsProtocol::attributes;
using NfsProtocol::c_response;
using NfsProtocol::truncate_request;
using NfsProtocol::create_request;
using NfsProtocol::d_response;
using NfsProtocol::flush_request;
using NfsProtocol::flush_response;
using NfsProtocol::fsync_request;
using NfsProtocol::fsync_response;
using NfsProtocol::mkdir_request;
using NfsProtocol::NfsServer;
using NfsProtocol::open_request;
using NfsProtocol::read_directory_single_object;
using NfsProtocol::read_request;
using NfsProtocol::read_response;
using NfsProtocol::readdir_request;
using NfsProtocol::readdir_response;
using NfsProtocol::rename_request;
using NfsProtocol::rmdir_request;
using NfsProtocol::unlink_request;
using NfsProtocol::lookup_request;
using NfsProtocol::lookup_response;
using NfsProtocol::write_request;
using NfsProtocol::write_response;


class serverImplementation final : public NfsServer::Service
{

  private:
	std::string base;
	std::map<std::string, int> stofh;
	std::map<int, std::string> fhtos;
	int fhcount;
	std::vector<SingleWrite> datastore;

	int addToLookup(std::string path);
	std::string back_lookup(int fh);
	void print_lookup();
	void print_store();

  public:
	serverImplementation(std::string path);

	Status server_mkdir(ServerContext *context, const mkdir_request *request, c_response *response) override;
	Status server_rmdir(ServerContext *context, const rmdir_request *request, c_response *response) override;
	Status server_rename(ServerContext *context, const rename_request *request, c_response *response) override;
	Status server_open(ServerContext *context, const open_request *request, d_response *response) override;
	Status server_create(ServerContext *context, const create_request *request, d_response *response) override;
	Status server_truncate(ServerContext *context, const truncate_request *request, d_response *response) override;
	Status server_unlink(ServerContext *context, const unlink_request *request, c_response *response) override;
	Status server_read(ServerContext *context, const read_request *request, read_response *response) override;
	Status read_directory(ServerContext *context, const readdir_request *request, readdir_response *response) override;
	Status server_mknod(ServerContext *context, const read_directory_single_object *request, c_response *response) override;
	Status get_attributes(ServerContext *context, const attribute_request *request, attribute_response *response) override;
	Status server_release(ServerContext *context, const read_request *request, d_response *response) override;
	Status server_write(ServerContext *context, const write_request *request, write_response *response) override;	
	Status server_lookup(ServerContext *context, const lookup_request *request, lookup_response *response) override;
	Status server_commit(ServerContext *context, const read_request *request, c_response *response) override;
	Status server_fsync(ServerContext *context, const fsync_request *request, fsync_response *response) override;
	Status server_flush(ServerContext *context, const flush_request *request, flush_response *response) override;

};

#endif
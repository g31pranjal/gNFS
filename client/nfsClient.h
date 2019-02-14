#ifndef NFSCLIENT
#define NFSCLIENT

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <experimental/filesystem>

#include "../proto/schema.grpc.pb.h"
#include "../proto/schema.pb.h"
#include "../utils/utils.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using std::experimental::filesystem::path;

using NfsProtocol::proto_file_info;
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



class clientImplementation
{

  private:
	std::unique_ptr<NfsServer::Stub> stub_;
	std::map<std::string, int> stofh;
	std::map<int, std::string> fhtos;
	int lookup(std::string path);
	bool renewed;
	std::vector<SingleWrite> datastore;
	int client_commit(std::string path, struct fuse_file_info *fi);
	int client_retransmit_writes(std::string path, struct fuse_file_info *fi);
	std::string id;
	void reset_lookups();
	void renewing_connection();

  public:
	clientImplementation(std::shared_ptr<Channel> channel);

	int client_mkdir(std::string pth, mode_t mode);
	int client_rmdir(std::string path);
	int client_rename(std::string from, std::string to);
	int client_open(std::string path, struct fuse_file_info *fi);
	int client_create(std::string pth, mode_t mode, struct fuse_file_info *fi);
	int client_truncate(std::string path, off_t size, struct fuse_file_info *fi);
	int client_unlink(std::string path);
	int client_read(std::string path, char *buffer, int size, int offset, struct fuse_file_info *fi);
	int client_mknod(std::string path, mode_t mode, dev_t rdev);
	std::list<DirEntry> read_directory(std::string path, int &responseCode);
	int get_attributes(std::string path, struct stat *st);
	int client_release(std::string path, struct fuse_file_info *fi);
	int client_write(std::string path, const char *buf, int size, int offset, struct fuse_file_info *fi);
	int fsync(std::string path, int isdatasync, struct fuse_file_info *fi);
	int flush(std::string path, struct fuse_file_info *fi);

	void print_store();


	// int getAttributes(std::string path, struct stat *st);
	// std::list<DirEntry> readDirectory(std::string path, int &responseCode);
	// int release(std::string path, struct fuse_file_info *fi);
	// int write(std::string path, const char *buf, int size, int offset, struct fuse_file_info* fi);
	// int utimens(std::string path,const struct timespec *ts, struct fuse_file_info *fi);
};

#endif
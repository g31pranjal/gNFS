#include "nfsClient.h"
#include <chrono>
#include <thread>
#include <unistd.h>

#define LOG true


clientImplementation::clientImplementation(std::shared_ptr<Channel> channel)
	: stub_(NfsServer::NewStub(channel)) {
		this->reset_lookups();
		char a[6];
		alphanum_random(a, 6);
		std::string tmp(a);
		this->id = a;
		std::cout << this->id << "\n";
		this->renewed = true;
	}

void clientImplementation::renewing_connection() {
	this->stub_ = NfsServer::NewStub(grpc::CreateChannel("localhost:3110", grpc::InsecureChannelCredentials()));
	this->renewed = true;
}

void clientImplementation::reset_lookups() {
	this->stofh.clear();
	this->fhtos.clear();
	this->stofh[""] = 0;
	this->fhtos[0] = "";
}


void clientImplementation::print_store() {
	std::cout << "---\n";
	for(auto& i : this->datastore) {
		std::cout << i.fh << ", " << i.size << ", " << i.offset << "\n";
	}
	std::cout << "---\n";
}


int clientImplementation::lookup(std::string ref) {

	path refpath(ref);
	std::string refpathstring = refpath.string();
	if(refpathstring.length() == 1)
		refpathstring = "";

	if ( this->stofh.find(refpathstring) == this->stofh.end() ) {
		
		path parentpath = refpath.parent_path();
		std::string parentpathstring = parentpath.string();
		if(parentpathstring.length() == 1)
			parentpathstring = "";

		this->lookup(parentpathstring);

		// send the lookup request to the server
		lookup_request request;
		lookup_response response;
		ClientContext context;

		request.set_cid(this->id);
		request.set_dirfh(this->stofh[parentpathstring]);
		std::string last;
		for(auto& e : refpath)
			last = e.string();
		request.set_name(last);
		std::cout << "calling lookup " << "\n";
		Status status = stub_->server_lookup(&context, request, &response);

		if(status.ok()) {
			this->stofh[refpathstring] = response.fh();
			this->fhtos[response.fh()] = refpathstring;
			return this->stofh[refpathstring];
		}
		else {
			this->renewed = false;
			std::cout << "cannot reach, sleeping\n";
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			std::cout << "retrying\n";
			if(!renewed) {
				this->reset_lookups();
				this->renewing_connection();
			}
			return this->lookup(ref);
		}

	} else {
	// if the path exists in the lookup, give the fh 
		return this->stofh[refpathstring];
	}
}

int clientImplementation::get_attributes(std::string path, struct stat *st)
{
	std::cout << "calling getattr: " << std::endl;
	
	attribute_request request;
	attribute_response response;
	ClientContext context;

	int fh = this->lookup(path);
	request.set_fh(fh);
	request.set_path(path);
	*request.mutable_attr() = toGstat(st);
	request.set_cid(this->id);

	std::cout << "actual " << std::endl;
	Status status = stub_->get_attributes(&context, request, &response);

	std::cout << "reaching \n";
	
	if (status.ok())
	{
		toCstat(response.attr(), st);
		return response.status();
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->get_attributes( path, st);
	}
}


std::list<DirEntry> clientImplementation::read_directory(std::string path, int &responseCode)
{
	readdir_request request;
	readdir_response response;
	ClientContext context;

	int dirfh = this->lookup(path);
	request.set_dirfh(dirfh);
	request.set_cid(this->id);

	std::cout << "calling readdir: " << std::endl;
	Status status = stub_->read_directory(&context, request, &response);

	std::list<DirEntry> entries;
	if (status.ok())
	{
		responseCode = response.status();
		for (int i = 0; i < response.objects_size(); i++)
		{
			DirEntry dirEntry;
			toCstat(response.objects(i).attr(), &dirEntry.st);
			dirEntry.name = response.objects(i).name();
			entries.push_back(dirEntry);
		}
		return entries;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->read_directory(path, responseCode);
	}
}


int clientImplementation::client_mkdir(std::string pth, mode_t mode)
{

	mkdir_request request;
	c_response response;
	attributes atr;
	ClientContext context;

	path ref = pth;
	path parent = ref.parent_path();
	std::string parentstring = parent.string();
	if(parentstring.length() == 1)
		parentstring = "";
	int dirfh = this->lookup(parentstring);
	std::string last;
	for(auto& e : ref)
		last = e.string();

	request.set_dirfh(dirfh);
	request.set_name(last);
	atr.set_st_mode(mode);
	request.mutable_attr()->CopyFrom(atr);
	request.set_cid(this->id);

	std::cout << "calling mkdir: " << std::endl;
	Status status = stub_->server_mkdir(&context, request, &response);
	
	if (status.ok())
	{
		if (response.success() != 0)
			return (-response.ern());
		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_mkdir(pth, mode);
	}
}

int clientImplementation::client_rmdir(std::string path)
{

	rmdir_request request;
	c_response response;
	ClientContext context;

	int dirfh = this->lookup(path);
	request.set_dirfh(dirfh);
	request.set_cid(this->id);

	std::cout << "calling rmdir: " << std::endl;
	Status status = stub_->server_rmdir(&context, request, &response);

	if (status.ok())
	{
		if (response.success() != 0)
			return (-response.ern());
		this->stofh.erase(path);
		this->fhtos.erase(dirfh);
		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_rmdir(path);
	}
}

int clientImplementation::client_rename(std::string from, std::string to)
{

	rename_request request;
	c_response response;
	ClientContext context;

	request.set_fromfh( this->lookup(from) );
	path ref = to;
	path parent = ref.parent_path();
	std::string parentstring = parent.string();
	if(parentstring.length() == 1)
		parentstring = "";
	int dirfh = this->lookup(parentstring);
	std::string last;
	for(auto& e : ref)
		last = e.string();

	request.set_todirfh(dirfh);
	request.set_name(last);
	request.set_cid(this->id);

	std::cout << "calling rename: " << std::endl;
	Status status = stub_->server_rename(&context, request, &response);

	if (status.ok())
	{
		if (response.success() != 0)
			return (-response.ern());
		
		if(this->stofh.find( to ) != this->stofh.end()  ) {
			//renaming to an existing file, case of overwrite

			int afh = this->stofh[from];
			int bfh = this->stofh[to];

			this->stofh.erase( this->stofh.find(from) );
			this->fhtos.erase( this->fhtos.find(bfh) );
			this->fhtos[afh] = to;
			this->stofh[to] = afh;
		} 
		else {
			this->fhtos.erase( this->fhtos.find(this->stofh[from] ) );
			this->stofh.erase( this->stofh.find(from)  );
		}

		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_rename(from, to);
	}
}

int clientImplementation::client_open(std::string path, struct fuse_file_info *fi)
{

	open_request request;
	d_response response;
	ClientContext context;

	request.set_fh(this->lookup(path));
	request.mutable_pfi()->CopyFrom(toProtoFileInfo(fi));
	request.set_cid(this->id);

	std::cout << "calling open: " << std::endl;
	Status status = stub_->server_open(&context, request, &response);

	if (status.ok())
	{
		toFuseFileInfo(response.pfi(), fi);
		if (response.success() != 0)
			return (-response.ern());
		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_open(path, fi);
	}
}

int clientImplementation::client_create(std::string pth, mode_t mode, struct fuse_file_info *fi)
{

	create_request request;
	d_response response;
	attributes atr;
	ClientContext context;

	path ref = pth;
	path parent = ref.parent_path();
	std::string parentstring = parent.string();
	if(parentstring.length() == 1)
		parentstring = "";
	int dirfh = this->lookup(parentstring);
	std::string last;
	for(auto& e : ref)
		last = e.string();

	request.set_dirfh(dirfh);
	request.set_name(last);
	request.set_cid(this->id);

	atr.set_st_mode(mode);
	request.mutable_attr()->CopyFrom(atr);
	request.mutable_pfi()->CopyFrom(toProtoFileInfo(fi));

	std::cout << "calling create: " << std::endl;
	Status status = stub_->server_create(&context, request, &response);

	if (status.ok())
	{
		toFuseFileInfo(response.pfi(), fi);
		if (response.success() != 0)
			return (-response.ern());
		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_create(pth, mode, fi);
	}
}

int clientImplementation::client_truncate(std::string path, off_t size, struct fuse_file_info *fi)
{

	truncate_request request;
	attributes atr;
	d_response response;
	ClientContext context;

	request.set_fh( this->lookup(path) );
	atr.set_st_size(size);
	request.mutable_attr()->CopyFrom(atr);
	request.mutable_pfi()->CopyFrom(toProtoFileInfo(fi));
	request.set_cid(this->id);

	std::cout << "calling truncate: " << std::endl;
	Status status = stub_->server_truncate(&context, request, &response);

	if (status.ok())
	{
		toFuseFileInfo(response.pfi(), fi);
		if (response.success() != 0)
			return (-response.ern());
		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_truncate(path, size, fi);
	}
}

int clientImplementation::client_unlink(std::string path)
{

	unlink_request request;
	c_response response;
	ClientContext context;

	request.set_fh( this->lookup(path) );
	request.set_cid(this->id);

	std::cout << "calling unlink: " << std::endl;
	Status status = stub_->server_unlink(&context, request, &response);

	if (status.ok())
	{
		if (response.success() != 0)
			return (-response.ern());
		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_unlink(path);
	}
}





int clientImplementation::client_read(std::string path, char* buffer,int size, int offset, struct fuse_file_info *fi) 
{
	read_request request;
	read_response response;
	ClientContext context;

	std::cout << "reading: path, " << path << " size, " << size << " offset, " << offset << "\n";

	request.set_fh( this->lookup(path));
	request.set_offset(offset);
	request.set_size(size);
	request.mutable_pfi()->CopyFrom(toProtoFileInfo(fi));
	request.set_cid(this->id);

	std::cout << "calling readdir: " << std::endl;
	Status status = stub_->server_read(&context, request, &response);

	toFuseFileInfo(response.pfi(), fi);
	if (status.ok() )
	{
		strncpy(buffer, response.data().c_str(), size);
		return response.size();
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_read(path, buffer, size, offset, fi);
	}
}


int clientImplementation::client_mknod(std::string path, mode_t mode, dev_t rdev) {

	read_directory_single_object request;
	attributes atr;
	c_response response;
	ClientContext context;
	
	request.set_name(path);
	atr.set_st_mode(mode);
	atr.set_st_dev(rdev);
	request.mutable_attr()->CopyFrom(atr);

	std::cout << "calling mknod: " << std::endl;
	Status status = stub_->server_mknod(&context, request, &response);

	if(status.ok()){
		if(response.success() != 0)
			return (- response.ern());
		return 0;
	}
	else  {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_mknod(path, mode, rdev);
	}
}

int clientImplementation::client_retransmit_writes(std::string path, struct fuse_file_info *fi) { 

	int fh = this->lookup(path);

	for(auto a = this->datastore.begin(); a != this->datastore.end();) {
		if( (*a).fh == fh ) {
			write_request request;
			write_response response;
			ClientContext context;
			request.set_size((*a).size);
			request.set_offset((*a).offset);
			request.set_data((*a).data.c_str());
			request.set_cid(this->id);
			proto_file_info pfi = (*a).pfi;
			pfi.set_fh(0);
			request.mutable_pfi()->CopyFrom(pfi);

			Status status = stub_->server_write(&context, request, &response);

			if(status.ok()) {
				if(response.status() == 0){
				}
				else {
					continue;
				}
			}
			else {
				this->renewed = false;
				std::cout << "cannot reach, sleeping\n";
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				std::cout << "retrying\n";
				if(!renewed) {
					this->reset_lookups();
					this->renewing_connection();
				}
				return this->client_retransmit_writes(path, fi);
			}
		}
		a++;
	}
}


int clientImplementation::client_commit(std::string path, struct fuse_file_info *fi) {

	read_request request;
	c_response response;
	ClientContext context;

	int fh = this->lookup(path);
	bool isDirty = false;

	for(auto& a : this->datastore) {
		if( a.fh == fh ) {
			isDirty = true;
			break;
		}
	}
	if(!isDirty)
		return 0;

	request.set_fh( this->lookup(path) );
	request.mutable_pfi()->CopyFrom(toProtoFileInfo(fi));
	request.set_cid(this->id);

	std::cout << "calling commit: " << std::endl;
	Status status = stub_->server_commit(&context, request, &response);

	if(status.ok()) {
		if(response.success() == 0) {
			for(auto a = this->datastore.begin(); a != this->datastore.end();) 
				if((*a).fh == request.fh()) {
					a = this->datastore.erase(a);
				}
				else {
					a++;
				} 
			return 0;
		}
		else if(response.success() == 1){
			// client feels it has something to write, but server refuses. hence, retransmit.
			// and commit again.
			this->client_retransmit_writes(path, fi);
			return this->client_commit(path, fi);
		}
		else{
			// write operation on server fails, return failure.
			return -1;
		}
	} 
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_commit(path, fi);
	}
}


int clientImplementation::client_release(std::string path, struct fuse_file_info *fi) {

	int cResult = this->client_commit(path, fi);
	if(cResult == -1)
		return -1;

	read_request request;
	d_response response;
	ClientContext context2;

	request.set_fh( this->lookup(path));
	request.mutable_pfi()->CopyFrom(toProtoFileInfo(fi));
	request.set_cid(this->id);
		
	std::cout << "calling release: " << std::endl;
	Status status = stub_->server_release(&context2, request, &response);

	toFuseFileInfo(response.pfi(), fi);

	if(status.ok()){
		// mainDataStore.clear();
		if(response.success() != 0)
			return (- response.ern());
		return 0;
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->client_release(path, fi);
	}
}


int clientImplementation::client_write(std::string path, const char *buf, int size, int offset, struct fuse_file_info *fi)
{
	write_request request;
	write_response response;
	ClientContext context;

	std::cout << "write: " << path << " @ " << this->lookup(path) << ", " << size << ", " << offset << "\n";	
	
	proto_file_info pfi = toProtoFileInfo(fi);

	request.set_size(size);
	request.set_offset(offset);
	request.set_fh( this->lookup(path) );
	request.mutable_pfi()->CopyFrom(pfi);
	std::string buffer(buf);
	request.set_data(buffer);
	request.set_cid(this->id);

	SingleWrite swr;
	swr.size = size;
	swr.offset = offset;
	swr.fh = this->lookup(path);
	swr.data = buffer;
	swr.pfi = pfi;

	this->datastore.push_back(swr);

	Status status = stub_->server_write(&context, request, &response);

	if(status.ok()) {
		if(response.status() == 0){
			toFuseFileInfo(response.pfi(), fi);
			return response.datasize();
		}
		return -1;
	}
	else {
		return size;
	}

}

int clientImplementation::fsync(std::string path, int isdatasync, struct fuse_file_info *fi)
{
	fsync_request fsyncRequestObject;
	fsyncRequestObject.set_path(path);
	fsyncRequestObject.set_cid(this->id);
	fsyncRequestObject.set_isdatasync(isdatasync);
	*fsyncRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	fsync_response fsyncResponseObject;

	// Call
	Status status = stub_->server_fsync(&context, fsyncRequestObject, &fsyncResponseObject);

	toCFileInfo(fsyncResponseObject.fileinfo(), fi);

	if (status.ok())
	{
		return fsyncResponseObject.status();
	}
	else {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->fsync(path, isdatasync, fi);
	}
}

int clientImplementation::flush(std::string path, struct fuse_file_info *fi)
{
	flush_request flushRequestObject;
	flushRequestObject.set_path(path);
	*flushRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	flushRequestObject.set_cid(this->id);
	ClientContext context;

	// Container response
	flush_response flushResponseObject;

	// Call
	Status status = stub_->server_flush(&context, flushRequestObject, &flushResponseObject);

	toCFileInfo(flushResponseObject.fileinfo(), fi);

	if (status.ok())
	{
		return flushResponseObject.status();
	}
	else  {
		this->renewed = false;
		std::cout << "cannot reach, sleeping\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "retrying\n";
		if(!renewed) {
			this->reset_lookups();
			this->renewing_connection();
		}
		return this->flush(path, fi);
	}
}

#define FUSE_USE_VERSION 31

#include "fuse.h"
#include "wrapper.h"


struct fuse_operations nfsOps;

int main(int argc, char ** argv) {

	nfsOps.getattr = &wrapper_getattr;
	nfsOps.readdir = &wrapper_readdir;
	nfsOps.read = &wrapper_read; 
	nfsOps.mknod = &wrapper_mknod;
	nfsOps.mkdir = &wrapper_mkdir;
	nfsOps.rmdir = &wrapper_rmdir;
	nfsOps.rename = &wrapper_rename;
	nfsOps.truncate = &wrapper_truncate;
	nfsOps.create = &wrapper_create;
	nfsOps.open = &wrapper_open;
	nfsOps.release = &wrapper_release;
	// nfsOps.fsync = &wrapper_fsync;
	nfsOps.unlink = &wrapper_unlink;
	nfsOps.write = &wrapper_write;
	// nfsOps.flush = &wrapper_flush;

	return fuse_main(argc, argv, &nfsOps, NULL);
	
}
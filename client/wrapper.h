#ifndef WRAPPER
#define WRAPPER

#define FUSE_USE_VERSION 31

#include <sys/stat.h>
#include <fuse.h>
#include "nfsClient.h"

// void set_rootdir(const char *path);


	int wrapper_getattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi);
	int wrapper_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo, fuse_readdir_flags rflags);
	int wrapper_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
	int wrapper_mknod(const char *path, mode_t mode, dev_t dev);
	int wrapper_mkdir(const char *path, mode_t mode);
	int wrapper_rmdir(const char *path);
int wrapper_rename(const char *path, const char *newpath, unsigned int flag);
	int wrapper_truncate(const char *path, off_t newSize, struct fuse_file_info *fi);
	int wrapper_create(const char * path, mode_t mode, struct fuse_file_info * fileInfo);
	int wrapper_open(const char *path, struct fuse_file_info *fileInfo);
	int wrapper_release(const char *path, struct fuse_file_info *fileInfo);
			int wrapper_fsync(const char *path, int datasync, struct fuse_file_info *fi);
			int wrapper_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
			int wrapper_flush(const char *path, struct fuse_file_info *fileInfo);
	int wrapper_unlink(const char *path);

// int wrapper_init(struct fuse_conn_info *conn);
// int wrapper_opendir(const char *path, struct fuse_file_info *fileInfo);
// int wrapper_symlink(const char *path, const char *link);
// int wrapper_utime(const char *path, struct utimbuf *ubuf);
// int wrapper_setxattr(const char *path, const char *name, const char *value, size_t size, int flags);
// int wrapper_chmod(const char *path, mode_t mode);
// int wrapper_chown(const char *path, uid_t uid, gid_t gid);
// int wrapper_link(const char *path, const char *newpath);
// int wrapper_statfs(const char *path, struct statvfs *statInfo);
// int wrapper_getxattr(const char *path, const char *name, char *value, size_t size);
// int wrapper_listxattr(const char *path, char *list, size_t size);
// int wrapper_removexattr(const char *path, const char *name);
// int wrapper_readlink(const char *path, char *link, size_t size);
// int wrapper_releasedir(const char *path, struct fuse_file_info *fileInfo);
// int wrapper_fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);

#endif

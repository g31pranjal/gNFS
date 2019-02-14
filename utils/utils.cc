
#include <sys/stat.h>
#include "utils.h"

std::string toStr(const char *path)
{
	std::string a(path);
	return a;
}

proto_file_info toProtoFileInfo(struct fuse_file_info *fi)
{
	proto_file_info pfi;
	pfi.set_fh(fi->fh);
	pfi.set_flags(fi->flags);
	return pfi;
}

void toFuseFileInfo(proto_file_info pfi, struct fuse_file_info *fi)
{
	fi->fh = pfi.fh();
	fi->flags = pfi.flags();
}

attributes toGstat(struct stat *st)
{
	attributes gstat;
	gstat.set_st_dev(st->st_dev);
	gstat.set_st_ino(st->st_ino);
	gstat.set_st_mode(st->st_mode);
	gstat.set_st_nlink(st->st_nlink);
	gstat.set_st_uid(st->st_uid);
	gstat.set_st_gid(st->st_gid);
	gstat.set_st_rdev(st->st_rdev);
	gstat.set_st_size(st->st_size);
	// gstat.set_st_atime(st->st_atime);
	// gstat.set_st_mtime(st->st_mtime);
	// gstat.set_st_ctime(st->st_ctime);
	gstat.set_st_blksize(st->st_blksize);
	gstat.set_st_blocks(st->st_blocks);
	// gstat.set_st_attr(st->st_attr);
	return gstat;
}

void toCstat(attributes gstat, struct stat *st)
{

	st->st_dev = gstat.st_dev();
	st->st_ino = gstat.st_ino();
	st->st_mode = gstat.st_mode();
	st->st_nlink = gstat.st_nlink();
	st->st_uid = gstat.st_uid();
	st->st_gid = gstat.st_gid();
	st->st_rdev = gstat.st_rdev();
	st->st_size = gstat.st_size();
	// st->st_atime = gstat.st_atime();
	// st->st_mtime = gstat.st_mtime();
	// st->st_ctime = gstat.st_ctime();
	st->st_blksize = gstat.st_blksize();
	st->st_blocks = gstat.st_blocks();
	// st->st_attr = gstat.st_attr();
}

void toCFileInfo(proto_file_info fuseFileInfo, struct fuse_file_info *fi)
{
	fi->fh = fuseFileInfo.fh();
	fi->flags = fuseFileInfo.flags();
}


proto_file_info toGFileInfo(struct fuse_file_info *fi){
	proto_file_info fuseFileInfo;
	fuseFileInfo.set_fh(fi->fh);
	fuseFileInfo.set_flags(fi->flags);
	return fuseFileInfo;
}


void alphanum_random(char *s, const int len) 
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;

}

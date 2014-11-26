#include "common.h"


int Flags=0;
char *DiffHook=NULL;
char *Key=NULL;
char *LocalHost=NULL;





void HashratCtxDestroy(void *p_Ctx)
{
HashratCtx *Ctx;

Ctx=(HashratCtx *) p_Ctx;
STREAMClose(Ctx->Out);
DestroyString(Ctx->HashType);
DestroyString(Ctx->ListPath);
DestroyString(Ctx->HashStr);
HashDestroy(Ctx->Hash);
free(Ctx);
}


int HashratOutputInfo(HashratCtx *Ctx, STREAM *Out, char *Path, struct stat *Stat, char *Hash)
{
char *Tempstr=NULL; 



if (Flags & FLAG_TRAD_OUTPUT) Tempstr=MCopyStr(Tempstr,Hash, "  ", Path,NULL);
else
{
	/*
		struct stat {
    dev_t     st_dev;     // ID of device containing file 
    ino_t     st_ino;     // inode number 
    mode_t    st_mode;    // protection 
    nlink_t   st_nlink;   // number of hard links 
    uid_t     st_uid;     // user ID of owner
    gid_t     st_gid;     // group ID of owner
    dev_t     st_rdev;    // device ID (if special file) 
    off_t     st_size;    // total size, in bytes 
    blksize_t st_blksize; // blocksize for file system I/O 
    blkcnt_t  st_blocks;  // number of 512B blocks allocated 
    time_t    st_atime;   // time of last access 
    time_t    st_mtime;   // time of last modification 
    time_t    st_ctime;   // time of last status change 
	*/

#ifdef _LARGEFILE64_SOURCE
	Tempstr=FormatStr(Tempstr,"hash='%s:%s' mode='%o' uid='%lu' gid='%lu' size='%llu' mtime='%lu' inode='%llu' path='%s'",Ctx->HashType,Hash,Stat->st_mode,Stat->st_uid,Stat->st_gid,Stat->st_size,Stat->st_mtime,Stat->st_ino,Path);
#else
	Tempstr=FormatStr(Tempstr,"hash='%s:%s' mode='%o' uid='%lu' gid='%lu' size='%lu' mtime='%lu' inode='%lu' path='%s'",Ctx->HashType,Hash,Stat->st_mode,Stat->st_uid,Stat->st_gid,Stat->st_size,Stat->st_mtime,Stat->st_ino,Path);
#endif
}

Tempstr=CatStr(Tempstr,"\n");

STREAMWriteString(Tempstr,Out);

DestroyString(Tempstr);
}




void HashratStoreHash(HashratCtx *Ctx, char *Path, struct stat *Stat, char *Hash)
{
char *Tempstr=NULL;

	//if CTX_CACHED is set, then unset. Otherwise update XATTR for this item
//	if (Ctx->Flags & CTX_CACHED) Ctx->Flags &= ~CTX_CACHED;
//	else 

	
	if (Ctx->Flags & CTX_STORE_XATTR) HashRatSetXAttr(Ctx->Out, Path, Stat, Ctx->HashType, Hash);

	if (Ctx->Flags & CTX_STORE_MEMCACHED)
	{
		if (Flags & FLAG_NET) Tempstr=MCopyStr(Tempstr, Path);
		else Tempstr=MCopyStr(Tempstr,"hashrat://",LocalHost,Path,NULL);
		MemcachedSet(Hash, 0, Tempstr);
		MemcachedSet(Tempstr, 0, Hash);
	}

	if (Ctx->Aux) HashratOutputInfo(Ctx, Ctx->Aux, Path, Stat, Hash);
	DestroyString(Tempstr);
}



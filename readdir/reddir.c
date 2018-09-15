#include<stdlib.h>
#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
//定义一个函数类型，Myfunc,参数为char * stat * int 返回值类型为int
typedef int Myfunc(const char *, const struct stat * , int);
//声明一个函数，函数类型为Myfunc
static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;
int main(int argc, char *argv[])
{
	int ret;
	//如果传入参数不为2，路径没传递
	if(argc != 2)
	{
		printf("usage: ftw <starting-pathname>\n");	
		exit(0);
	}
	ret = myftw(argv[1], myfunc);
	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	if(ntot == 0)
		ntot = 1;
	printf("regular files = %7ld, %5.2f %% \n", nreg, nreg*100.0/ntot);
	printf("directories  = %7ld, %5.2f %% \n", ndir, ndir*100.0/ntot);
	printf("block special = %7ld, %5.2f %% \n", nblk, nblk*100.0/ntot);
	printf("char special = %7ld, %5.2f %% \n", nchr, nchr*100.0/ntot);
	printf("FIFOs = %7ld, %5.2f %% \n", nfifo, nfifo*100.0/ntot);
	printf("symbolic links = %7ld, %5.2f %% \n", nslink, nslink*100.0/ntot);
	printf("sockets = %7ld, %5.2f %% \n", nsock, nsock*100.0/ntot);
	exit(ret);
}
static char fullpath[100] = {0} ;//路径
#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

static int myftw(char *pathname, Myfunc *func)
{
	memset(fullpath, 0 ,100);
	strcpy(fullpath, pathname);
	return(dopath(func));
}
static int dopath(Myfunc* func)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	int ret, n;
	if(lstat(fullpath, &statbuf)< 0)
		return(func(fullpath, &statbuf, FTW_NS));
	//如果不是目录
	if(S_ISDIR(statbuf.st_mode) == 0)
	{
		//如果不所目录进入到这
		//printf("into S_ISDIR\n");
		return(func(fullpath, &statbuf, FTW_F));
	}
	if((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return ret;
	//printf("into next\n");
	n = strlen(fullpath);
	fullpath[n++] = '/';
	fullpath[n] = 0;
	if((dp = opendir(fullpath)) == NULL)
		return(func(fullpath, &statbuf, FTW_DNR));
	while ((dirp = readdir(dp)) != NULL) {
		if(strcmp(dirp->d_name, ".") == 0 ||
			strcmp(dirp->d_name, "..") == 0)
			continue;
		strcpy(&fullpath[n], dirp->d_name);
		//printf("fullpath = %s \n",fullpath);
		if((ret = dopath(func)) != 0)
			break;
	}
	//遍历完当前目录，路径恢复到这一级
	fullpath[n-1] = 0;
	if(closedir(dp) < 0)
	{
		printf("can't close directory %s", fullpath);
	}
	return(ret);
}
static int myfunc(const char* pathname, const struct stat *statptr, int type)
{ 
	switch(type){
	case FTW_F:
		switch(statptr->st_mode & S_IFMT){
		//普通文件
		case S_IFREG: nreg++; break;
		//块设备文件
		case S_IFBLK: nblk++; break;
		//字符设备文件
		case S_IFCHR: nchr++; break;
		//
		case S_IFIFO: nfifo++; break;
		//字符链接文件
		case S_IFLNK: nslink++; break;
		//
		case S_IFSOCK: nsock++; break;
		case S_IFDIR:
			printf("for S_IFDIR for %s\n", pathname);
		}
		//printf("into FTW_F\n");
		break;
	case FTW_D:
		ndir++;
		break;
	case FTW_DNR:
		printf("can't read directory %s", pathname); 
		break;
	case FTW_NS:
		printf("stat error for %s", pathname);
		break;
	default:
		printf("unknow type %d for pathname %s", type, pathname);
	}
	return 0;
}

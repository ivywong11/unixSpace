#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
int main(int arg,char *argv[])
{
	DIR *dp;
	struct dirent *dirp;
	if(arg != 2)
	{
		printf("usage : ls directory_name\n");
		exit(0);
	}
	//打开一个目录，失败返回NULL
	if((dp = opendir(argv[1])) == NULL)
	{
		printf("can't open %s\n",argv[1]);
		exit(0);
	}
	while((dirp = readdir(dp)) != NULL)
		printf("%s\n" , dirp->d_name);
	closedir(dp);
	exit(0);
}

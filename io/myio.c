#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define BUFFSIZE 4096
int main(void)
{
	int n;
	char buf[BUFFSIZE];
	//STDIN_FILENO 接收键盘到输入
	while((n == read(STDIN_FILENO, buf , BUFFSIZE)) > 0){
		//STDOUT_FILENO 向屏幕输出
		if(write(STDOUT_FILENO, buf , n) != n)
		{
			printf("write error\n");
			exit(0);
		}
	}
	if(n < 0)
	{
		printf("read error\n");
		exit(0);
	} 
	exit(0);
}

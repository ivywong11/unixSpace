#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#define	MAXLINE	4096			/* max line length */
int main(void)
{
	printf("process ID %ld\n",(long)getpid());
	char buf[MAXLINE];
	pid_t pid;
	int status;
	printf("%% ");
	//读取一行
	while(fgets(buf, MAXLINE, stdin) != NULL)
	{
		if(buf[strlen(buf) - 1] == '\n')
		{
			buf[strlen(buf) - 1] = 0;
		}
		//子进程返回0 ，父进程返回子进程标记
		if((pid = fork()) < 0)
		{
			printf("fork error\n");
			exit(0);
		}else if(pid == 0)
		{
			execlp(buf, buf, (char *)0);
			printf("could't execute: %s\n", buf);
			exit(127);	
		}
		printf("parent pid = %d\n",pid);
		if((pid = waitpid(pid, &status ,0)) <0)
		{
			printf("waitpid error");
			exit(0);
		}
		else{
			printf("pid = %d , status = %d\n", pid, status); 
		} 
		printf("%% ");
	}	
	exit(0);
}

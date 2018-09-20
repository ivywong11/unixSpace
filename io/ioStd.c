#include<stdio.h>
#include<stdlib.h>
int main(void)
{
	FILE *fp;
	fputs("enter any charachter\n",stdout);
	if(getchar() == EOF)
	{
		printf("getchar error\n");
		exit(1);
	}
	fputs("one line to standard error\n",stderr);
	pr_stdio("stdin",stdin);
	pr_stdio("stout",stdout);
	pr_stdio("stderr",stderr);
	exit(0);
}
void pr_stdio(const char *name, FILE *fp)
{
	printf("stream = %s, ",name);
	if(is_unbuffered(fp))
		printf("unbuffered");
	else if(is_linebuffered(fp))
		printf("line buffered");
	else
		printf("fully buffered");
	printf(", buffer size =%d\n", buffer_size(fp));
		
}

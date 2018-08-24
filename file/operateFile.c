#include<fcntl.h>
#include<stdlib.h>
#include<stdio.h>
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";
int main(void)
{
  int fd;
  if((fd = creat("file.hole", FILE_MODE)) < 0)
    {
      printf("create error\n");
      exit(0);
    }
  if(write(fd , buf1 , 10) != 10)
    {
      printf("buf1 write error\n");
      exit(0);
    }
  if(lseek(fd, 16384, SEEK_SET) == -1)
  {
    printf("lseek error\n");
    exit(0);
  }
  if(write(fd , buf2 , 10) != 10)
    {
      printf("buf2 write error\n");
      exit(0);
    }
  exit(0);
}

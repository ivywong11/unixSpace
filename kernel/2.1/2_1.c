/*内核源码头文件*/
#include<linux/init.h>//init_module和cleanup_module函数原型声明
#include<linux/kernel.h>//printk函数在这
#include<linux/module.h>
int init_module(void)
{
	//模块的初始化函数，模块被动态加载到内核时被调用
	//不是必须，内核在加载模块时，如果没有发现该函数，则不会调用
	printk("module init\n");
	//返回0表示模块的初始化函数执行成功,负数->失败
	return 0;		
}
//模块的清除函数,模块从内核卸载时被调用,也非必须
void cleanup_module(void)
{
	printk("cleanup module\n");
}

#include<linux/kernel.h>
#include<linux/module.h>

static int expval = 5;
EXPORT_SYMBOL(expval);

static void expfun(void)
{	
	printk("extern fun\n");
}	

EXPORT_SYMBOL_GPL(expfun);
//export_SYMBOL宏生成一个特定的结构在ELF文件中的一个特定的段中
MODULE_LICENSE("GPL");
MODULE_AUTHOR("yemin");

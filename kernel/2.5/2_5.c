/*内核源码头文件*/
#include<linux/init.h>//init_module和cleanup_module函数原型声明
#include<linux/kernel.h>//printk函数在这
#include<linux/module.h>
//整形变量、整形数组、字符串指针
static int baudrate = 9600;
static int port[4] = {0, 1, 2, 3};
static char *name = "2.5t";
//将三个变量声明为模块参数
module_param(baudrate, int, S_IRUGO);
module_param_array(port, int, NULL, S_IRUGO);
module_param(name, charp, S_IRUGO);

//函数名前加static,防止重名
//__init标记函数放到ELF文件特定代码段
//模块加载时单独分配内存，调用成功后释放
//__exit如果模块不允许卸载，这段不会加载
static int __init init_2_5(void)
{
	//模块的初始化函数，模块被动态加载到内核时被调用
	//不是必须，内核在加载模块时，如果没有发现该函数，则不会调用
	printk("module init\n");
	//返回0表示模块的初始化函数执行成功,负数->失败
	int i;
	printk("baudrate: %d\n", baudrate);
	printk("port: ");
	for(i = 0; i<ARRAY_SIZE(port); i++){
		printk("%d ", port[i]);
	}
	printk("\n");
	printk("name: %s\n", name);
	return 0;		
}
//模块的清除函数,模块从内核卸载时被调用,也非必须
static void __exit exit_2_5(void)
{
	printk("cleanup module\n");
}

//module_init宏指定init_module函数别名
//module_exit宏指定cleanup_module函数别名
module_init(init_2_5);
module_exit(exit_2_5);

//接受GPL协议,MODULE_LICENSE宏在include/linux/module.h中
//会生成一些信息放在ELF文件特殊段，模块加载的时候会将信息复制到内存，并检查这些信息
MODULE_LICENSE("GPL");
//作者
MODULE_AUTHOR("yemin");
//描述
MODULE_DESCRIPTION("demo");

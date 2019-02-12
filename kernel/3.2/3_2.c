/*内核源码头文件*/
#include<linux/init.h>//init_module和cleanup_module函数原型声明
#include<linux/kernel.h>//printk函数在这
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
//主设备号
#define _3_2_MAJOR 256
//次设备号
#define _3_2_MINOR 0
//设备号注册到内核的个数
#define _3_2_DEV_CNT 1
//标记主设备号的名称
#define _3_2_DEV_NAME "3_2"
//定义一个cdev结构对象
static struct cdev _3_2_dev;

//定义了一个struct file_operations类型的操作方法集合
static struct file_operations _3_2_ops = {
	.owner = THIS_MODULE,
};
//函数名前加static,防止重名
//__init标记函数放到ELF文件特定代码段
//模块加载时单独分配内存，调用成功后释放
//__exit如果模块不允许卸载，这段不会加载
static int __init init_3_2(void)
{
	//模块的初始化函数，模块被动态加载到内核时被调用
	//不是必须，内核在加载模块时，如果没有发现该函数，则不会调用
	printk("module init\n");
	int ret;
	//设备号
	dev_t dev;
	//用宏定义，主次设备号合一个设备号
	//主设备左移20位和次设备相或
	dev = MKDEV(_3_2_MAJOR, _3_2_MINOR);
	//设备号合成了，那么将它注册到内核
	ret = register_chrdev_region(dev, _3_2_DEV_CNT, _3_2_DEV_NAME);
	if(ret)
		goto reg_err;
	//cdev_init 初始化cdev,并且将cdev的ops指针指向ops，这样后面打开设备时，找到cdev后，将file结构中的f_op指向它的ops，而这个ops又指向了驱动程序中具体实现的操作
	cdev_init(&_3_2_dev, &_3_2_ops);
	//这部分和ops里面的owner都指向了THIS_MODULE,这样ops和owner都能找到对于的模块
	//owner是struct module结构
	_3_2_dev.owner = THIS_MODULE;
	//将cdev加到内核的cdev_map中
	ret = cdev_add(&_3_2_dev, dev, _3_2_DEV_CNT);
	if(ret)
		goto add_err;
	//返回0表示模块的初始化函数执行成功,负数->失败
	return 0;
add_err:
	//代表添加cdev失败，将设备号注销
	unregister_chrdev_region(dev, _3_2_DEV_CNT);
reg_err:
	//代表注册设备号失败
	return ret;		
}
//模块的清除函数,模块从内核卸载时被调用,也非必须
static void __exit exit_3_2(void)
{
	printk("cleanup module\n");
	dev_t dev;
	dev = MKDEV(_3_2_MAJOR, _3_2_MINOR);
	
	cdev_del(&_3_2_dev);
	//模块卸载的时候，将设备号从内核中注销
	unregister_chrdev_region(dev, _3_2_DEV_CNT);
}

//module_init宏指定init_module函数别名
//module_exit宏指定cleanup_module函数别名
module_init(init_3_2);
module_exit(exit_3_2);

//接受GPL协议,MODULE_LICENSE宏在include/linux/module.h中
//会生成一些信息放在ELF文件特殊段，模块加载的时候会将信息复制到内存，并检查这些信息
MODULE_LICENSE("GPL");
//作者
MODULE_AUTHOR("yemin");
//描述
MODULE_DESCRIPTION("demo");

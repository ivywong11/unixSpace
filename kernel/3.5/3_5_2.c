/*内核源码头文件*/
#include<linux/init.h>//init_module和cleanup_module函数原型声明
#include<linux/kernel.h>//printk函数在这
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/kfifo.h>
//主设备号
#define _3_5_MAJOR 256
//次设备号
#define _3_5_MINOR 0
//设备号注册到内核的个数,注册到内核2个设备号，以及将cdev添加到内核的时候，指明管理2个设备
#define _3_5_DEV_CNT 2
//标记主设备号的名称
#define _3_5_DEV_NAME "3_5"
//定义一个cdev结构对象
static struct cdev _3_5_dev;

//初始化一个名叫_3_5_fifo的kfifo对象，这个对象里的每个成员的类型是char,个数为32个，个数必须是2的幂
//这里定义2个FIFO
DEFINE_KFIFO(j_3_5_1_fifo, char, 32);
DEFINE_KFIFO(j_3_5_2_fifo, char, 32);
//这里定义了一个结构体，里面一个cdev,和他对应的kfifo
struct vser_dev {
	struct kfifo * fifo;
	struct cdev cdev;
}
static struct vser_dev vsdev[2];
//实现了设备的打开函数
//第一个参数要打开的文件的inode
//第二个参数是打开对应文件后由内核构造并初始化好的file结构
//这边关闭函数，叫release,文件可以打开多次，open函数会被调用多次，close文件时，release只有在最后一个文件close才会调用
static int j_3_5_open(struct inode *inode, struct file *filp)
{
	printk("drivers open\n");
	//这里根据inode中的i_cdev，因为这个cdev是结构体vser_dev中的cdev，这个函数通过结构体成员的地址，反向得到结构体的起始地址,并保存到file里面
	filp->private_data = container_of(inode->i_cdev, struct vser_dev. cdev);
	return 0;
}
//实现了设备的关闭函数
static int j_3_5_release(struct inode *inode, struct file *filp)
{
	printk("drivers release\n");
	return 0;
}
//read系统调用的驱动实现
static ssize_t j_3_5_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	unsigned int copied = 0;
	//从filp取出对应的FIFO，是在前面open的时候根据次设备号放入的
	struct vser_dev *dev = filp->private_data;
	//将FIFO中的数据取出,
	kfifo_to_user(dev->fifo, buf, count, &copied)
	return copied; 
}
//
static ssize_t j_3_5_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	unsigned int copied = 0;
	//将用户空间数据放入FIFO,长度count,实际放入copied返回
	kfifo_from_user(&j_3_5_fifo, buf, count, &copied);
	return copied;
}
//定义了一个struct file_operations类型的操作方法集合
static struct file_operations _3_5_ops = {
	.owner = THIS_MODULE,
	.open = j_3_5_open,
	.release = j_3_5_release,
	.read = j_3_5_read,
	.write = j_3_5_write,
};

//函数名前加static,防止重名
//__init标记函数放到ELF文件特定代码段
//模块加载时单独分配内存，调用成功后释放
//__exit如果模块不允许卸载，这段不会加载
static int __init init_3_5(void)
{
	//模块的初始化函数，模块被动态加载到内核时被调用
	//不是必须，内核在加载模块时，如果没有发现该函数，则不会调用
	printk("module init\n");
	int i;
	int ret;
	//设备号
	dev_t dev;
	//用宏定义，主次设备号合一个设备号
	//主设备左移20位和次设备相或
	dev = MKDEV(_3_5_MAJOR, _3_5_MINOR);
	//设备号合成了，那么将它注册到内核,这里注册了2个设备号
	ret = register_chrdev_region(dev, _3_5_DEV_CNT, _3_5_DEV_NAME);
	if(ret)
		goto reg_err;
	//cdev_init 初始化cdev,并且将cdev的ops指针指向ops，这样后面打开设备时，找到cdev后，将file结构中的f_op指向它的ops，而这个ops又指向了驱动程序中具体实现的操作
	//cdev_init(&_3_5_dev, &_3_5_ops);
	//这部分和ops里面的owner都指向了THIS_MODULE,这样ops和owner都能找到对于的模块
	//owner是struct module结构
	//_3_5_dev.owner = THIS_MODULE;
	//将cdev加到内核的cdev_map中,这里表面支持2个设备
	//ret = cdev_add(&_3_5_dev, dev, _3_5_DEV_CNT);
	for(i = 0; i<_3_5_DEV_CNT; i++)
	{
		//这里和前面的不一样，这里注册了2次设备号，和将2个cdev添加到内核，不同的cdev
		cdev_init(&vsdev[i].cdev, &_3_5_ops);
		vsdev[i].cdev.ower = THIS_MODULE;
		vsdev[i].fifo = i == 0 ? (struct kfifo*)&j_3_5_1_fifo : (struct kfifo*)&j_3_5_2_fifo;
		ret = cdev_add(&vsdev[i].cdev, dev + i, 1);
	        if(ret)
			goto add_err;
			
	}
	//返回0表示模块的初始化函数执行成功,负数->失败
	return 0;
add_err:
	//代表添加cdev失败，将设备号注销
	unregister_chrdev_region(dev, _3_5_DEV_CNT);
reg_err:
	//代表注册设备号失败
	return ret;		
}
//模块的清除函数,模块从内核卸载时被调用,也非必须
static void __exit exit_3_5(void)
{
	printk("cleanup module\n");
	dev_t dev;
	int i;
	dev = MKDEV(_3_5_MAJOR, _3_5_MINOR);
	for(i = 0; i < VER_DEV_CNT; i++)
	{
		//这里卸载的时候要将2个cdev从内核里删除
		cdev_del(&vsdev[i].cdev);
	}
	//模块卸载的时候，将设备号从内核中注销
	unregister_chrdev_region(dev, _3_5_DEV_CNT);
}

//module_init宏指定init_module函数别名
//module_exit宏指定cleanup_module函数别名
module_init(init_3_5);
module_exit(exit_3_5);

//接受GPL协议,MODULE_LICENSE宏在include/linux/module.h中
//会生成一些信息放在ELF文件特殊段，模块加载的时候会将信息复制到内存，并检查这些信息
MODULE_LICENSE("GPL");
//作者
MODULE_AUTHOR("yemin");
//描述
MODULE_DESCRIPTION("demo");

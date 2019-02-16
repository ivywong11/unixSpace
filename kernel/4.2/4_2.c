#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/kfifo.h>
#include<linux/ioctl.h>
#include<linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "4_2.h"

#define _4_2_MAJOR 256//主设备号
#define _4_2_MINOR 0//次设备号
#define _4_2_DEV_CNT 1
#define _4_2_DEV_NAME "4_2"//设备名

struct _4_2_dev {
	unsigned int baud;//波特率
	struct option opt;
	struct cdev cdev;
	//添加了两个proc_dir_entry类型指针成员
	struct proc_dir_entry *pdir;
	struct proc_dir_entry *pdat;
};
//初始化一个名叫4_1_fifo的kfifo对象
DEFINE_KFIFO(_4_2_fifo, char, 32);
static struct _4_2_dev vsdev;
//实现了设备的打开函数
static int _4_2_open(struct inode *inode, struct file *filp)
{
	printk("drivers open\n");
	return 0;
}
//实现了设备关闭函数
static int _4_2_release(struct inode *inode, struct file *filp)
{
	printk("drivers release\n");
	return 0;
}
//read系统调用的驱动实现
static ssize_t _4_2_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied = 0;
	ret = kfifo_to_user(&_4_2_fifo, buf, count, &copied);
	return ret == 0 ? copied : ret;
}
//write系统调用的驱动实现
static ssize_t _4_2_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied = 0;
	ret = kfifo_from_user(&_4_2_fifo, buf, count, &copied);
	return ret == 0 ?  copied : ret;
}

static long _4_2_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	//通过_IOC_TYPE宏取出命令中的幻数，如果不匹配则返回-ENOTTY表示参数不对
	if(_IOC_TYPE(cmd) != _4_2_MAGIC)
		return -ENOTTY;
	switch(cmd){
		//根据命令匹配相应操作
		case _4_2_SET_BAUD:
			//直接传递波特率数据
			vsdev.baud = arg;
			break;
		case _4_2_GET_BAUD:
			arg = vsdev.baud;
			break;
		case _4_2_SET_FFMT:
			//传递指针,从用户传递过来的数据复制到opt,复制长度struct option,函数返回未复制成功字节数,该函数调用了access_ok来验证用户空间的内存是否真实可读，避免了在内核中嗯的缺页故障带来的问题,该函数可能会使进程休眠，如果只是简单的复制，还可以使用get_user(val, (int __user *)arg); put_user(ret, (int __user *)arg)
			if(copy_from_user(&vsdev.opt, (struct option __user *)arg, sizeof(struct option)))
			return -EFAULT;
			break;
		case _4_2_GET_FFMT:
			if(copy_to_user((struct option __user*)arg, &vsdev.opt, sizeof(struct option)))
			return -EFAULT;
			break;
		default:
			return -ENOTTY;		
	}
	return 0;
}
static struct file_operations _4_2_ops = {
	.owner = THIS_MODULE,
	.open = _4_2_open,
	.release = _4_2_release,
	.read = _4_2_read,
	.write = _4_2_write,
	//添加了.unclocked接口，实现的函数是4_1_ioctl
	.unlocked_ioctl = _4_2_ioctl,
};

static int dat_show(struct seq_file *m, void *v)
{
	struct _4_2_dev *dev = m->private;

	seq_printf(m, "baudrate: %d\n", dev->baud);
	return seq_printf(m, "frame format: %d%c%d\n", dev->opt.datab, \
			dev->opt.parity == 0 ? 'N' : dev->opt.parity == 1 ? 'O': 'E', \
			dev->opt.stopb);
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, dat_show, PDE_DATA(inode));
}

static struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
};
static int __init _4_2_init(void)
{
	int ret;
	dev_t dev;
	//合成设备号，注册到内核
	dev = MKDEV(_4_2_MAJOR, _4_2_MINOR);
	ret = register_chrdev_region(dev, _4_2_DEV_CNT, _4_2_DEV_NAME);
	if(ret)
		goto reg_err;
	//初始化cdev，将cdev中的ops指向4_1_ops
	cdev_init(&vsdev.cdev, &_4_2_ops);
	//cdev中的owner指向THIS_MODULE
	vsdev.cdev.owner = THIS_MODULE;
	//初始化vsdev中的成员
	vsdev.baud = 115200;//波特率115200
	vsdev.opt.datab = 8;//波特率
	vsdev.opt.parity = 0;//奇偶校验
	vsdev.opt.stopb = 1;//停止位
	//将cdev添加到内核cdev_map中
	ret = cdev_add(&vsdev.cdev, dev, _4_2_DEV_CNT);
	if(ret)
		goto add_err;
	//创建vser目录，第二个参数为NULL,表示在/proc目录下
	vsdev.pdir = proc_mkdir("vser", NULL);
	if(!vsdev.pdir)
		goto dir_err;
	//创建info文件的目录项，文件名，权限，目录的目录项指针,文件的操作集合,其中release,read,llseek都使用内核已有的函数是西安，而open使用自定义方法，从似有数据中获取设备结构地址，在proc_create_data传递，文件的私有数据
	vsdev.pdat = proc_create_data("info", 0, vsdev.pdir, &proc_ops, &vsdev);
	if(!vsdev.pdat)
		goto dat_err;
	return 0;
dat_err:
	remove_proc_entry("vser", NULL);
dir_err:
	cdev_del(&vsdev.cdev);
add_err:
	//如果添加失败，则将设备号注销
	unregister_chrdev_region(dev, _4_2_DEV_CNT);
reg_err:
	return ret;
}

static void __exit _4_2_exit(void)
{
	dev_t dev;
	dev = MKDEV(_4_2_MAJOR, _4_2_MINOR);
	cdev_del(&vsdev.cdev);
	unregister_chrdev_region(dev, _4_2_DEV_CNT);
}

module_init(_4_2_init);
module_exit(_4_2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yemin");

//测试
//mknod /dev/vser4_1 c 256 0#创建一个节点，名字随意，主次设备号要和驱动中一样
//gcc -o test test.c
//./test
//baud rate : 9600
//frame formata: 801
 

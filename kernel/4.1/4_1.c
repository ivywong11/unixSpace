#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/kfifo.h>
#include<linux/ioctl.h>
#include<linux/uaccess.h>
#include<4_1.h>

#define 4_1_MAJOR 256//主设备号
#define 4_1_MINOR 0//次设备号
#define 4_1_DEV_CNT 1
#define 4_1_DEV_NAME "4_1"//设备名

struct 4_1_dev {
	unsigned int baud;//波特率
	struct option opt;
	struct cdev cdev;
}
//初始化一个名叫4_1_fifo的kfifo对象
DEFINE_KFIFO(4_1_fifo, char, 32);
static struct 4_1_dev vsdev;
//实现了设备的打开函数
static int 4_1_open(struct inode *inode, struct file *filp)
{
	printk("drivers open\n")
	return 0;
}
//实现了设备关闭函数
static int 4_1_release(struct indoe *inode, struct file *filp)
{
	printk("drivers release\n")
	return 0;
}
//read系统调用的驱动实现
static ssize_t 4_1_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied = 0;
	ret = kfifo_to_user(&4_1_fifo, buf, count, &copied);
	return ret == 0 ? copied : ret;
}
//write系统调用的驱动实现
static ssize_t 4_1_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied = 0;
	ret = kfifo_from_user(&4_1_fifo, buf, count, &copied);
	return ret == 0 ?  copied : ret;
}

static long 4_1_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	//通过_IOC_TYPE宏取出命令中的幻数，如果不匹配则返回-ENOTTY表示参数不对
	if(_IOC_TYPE(cmd) != 4_1_MAGIC)
		return -ENOTTY;
	switch(cmd){
		//根据命令匹配相应操作
		case 4_1_SET_BAUD:
			//直接传递波特率数据
			vsdev.baud = arg;
			break;
		case 4_1_GET_BAUD:
			arg = vsdev.baud;
			break;
		case 4_1_SET_FFMT:
			//传递指针,从用户传递过来的数据复制到opt,复制长度struct option,函数返回未复制成功字节数,该函数调用了access_ok来验证用户空间的内存是否真实可读，避免了在内核中嗯的缺页故障带来的问题,该函数可能会使进程休眠，如果只是简单的复制，还可以使用get_user(val, (int __user *)arg); put_user(ret, (int __user *)arg)
			if((copy_from_user(&vsdev.opt, (struct option __user *)arg, sizeof(struct option)))
			return -EFAULT;
			break;
		case 4_1_GET_FFMT:
			if(copy_to_user(struct option __user*)arg, &vsdev.opt, sizeof(struct option));
			return -EFAULT;
			break;
		default:
			return -ENOTTY;		
	}
	return 0;
}
static struct file_operations 4_1_ops = {
	.owner = THIS_MODULE,
	.open = 4_1_open,
	.release = 4_1_release,
	.read = 4_1_read,
	.write = 4_1_write,
	//添加了.unclocked接口，实现的函数是4_1_ioctl
	.unclocked = 4_1_ioctl,
}

static int __init 4_1_inint(void)
{
	int ret;
	dev_t dev;
	//合成设备号，注册到内核
	dev = MKDEV(4_1_MAJOR, 4_1_MINOR);
	ret = register_chrdev_region(dev, VSER_DEV_CNT, 4_1_NAME);
	if(ret)
		goto reg_err;
	//初始化cdev，将cdev中的ops指向4_1_ops
	cdev_init(&vsdev.cdev, &4_1_ops);
	//cdev中的owner指向THIS_MODULE
	vsdev.cdev.owner = THIS_MODULE;
	//初始化vsdev中的成员
	vsdev.baud = 115200;//波特率115200
	vsdev.opt.datab = 8;//波特率
	vsdev.opt.parity = 0;//奇偶校验
	vsdev.opt.stopb = 1;//停止位
	//将cdev添加到内核cdev_map中
	ret = cdev_add(&vsdev.cdev, dev, VSER_DEV_CNT);
	if(ret)
		goto add_err;
	return 0;
add_err:
	//如果添加失败，则将设备号注销
	unregister_chrdev_region(dev, VSER_DEV_CNT);
reg_err:
	return ret;
}

static void __exit 4_1_exit(void)
{
	dev_t dev;
	dev = MKDEV(4_1_MAJOR, 4_1_MINOR);
	cdev_del(&vsdev.cdev);
	unregister_chrdev_region(dev, VSER_DEV_CNT);
}

modult_init(4_1_init);
modult_exit(4_1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yemin");

//测试
//mknod /dev/vser4_1 c 256 0#创建一个节点，名字随意，主次设备号要和驱动中一样
//gcc -o test test.c
//./test
//baud rate : 9600
//frame formata: 801
 

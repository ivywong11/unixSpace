#ifndef _4_2_H
#define _4_2_H

struct option {
	unsigned int datab;
	unsigned int parity;
	unsigned int stopb;
};

#define _4_2_MAGIC	's'//幻数
#define _4_2_SET_BAUD _IOW(_4_2_MAGIC, 0, unsigned int)//设置波特率，写方向，命令码0,需要写入参数大小unsigned int
#define _4_2_GET_BAUD _IOR(_4_2_MAGIC, 1, unsigned int)//获取波特率，读方向，命令码1,需要读取的参数大小
#define _4_2_SET_FFMT _IOW(_4_2_MAGIC, 2, struct option)//设置帧格式，写方向，命令码2,需要写入参数大小struct option
#define _4_2_GET_FFMT _IOR(_4_2_MAGIC, 3, struct option)//获取帧格式，读方向，命令码3,需要读取参数大小struct option

#endif

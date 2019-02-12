#Makefile
ifeq ($(KERNELRELEASE),)

#这里判断KERNELRELEASE变量有木有被定义
#第一次解释这个脚本执行的时候，这个变量未定义,进入这个分支

#定义内核源码目录，下面会用到
#MAKE的时候，指定-C 将当前goldfish内核源码的本地路径指定为编译环境
KERNELDIR ?= /home/mini/Android/kernel/goldfish-android-goldfish-2.6.29
#这里将当前模块所在目录定义为PWD变量
PWD := $(shell pwd)

#目标modules
modules:
	#make不带参数会默认生成该目标
	#这里意思，进入内核目录，编译M指定的目录中模块
	#编译M指定的模块的时候，推出内核目录，进入到模块目录，这个时候
	#KERNELRELEASE不会空，则执行else分支，将obj-m后面跟的目标编译成一个模块
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INSTALL_MOD_PATH=$(PWD) modules_install
clean:
	rm -rf *.o *.ko .*.cmd *.mod.* modules.order Module.symvers .tmp_versions

else

#obj-m表示将后面的跟的目标编译成一个模块
obj-m := 2_5.o
#表示2_4模块是由2_4_1.o和2_4_2.o两个目标文件共同生成
#2_4-objs = 2_4_1.o 2_4_2.o

endif


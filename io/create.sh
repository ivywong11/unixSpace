#!/bin/sh
#如果参数总数,不等于3 
if [ $# -ne 3 ];then
    echo "usage : $0 character out_file file_size(Byte)"
    exit 1
fi
#如果第一个参数 不是英文，则退出
echo "$1" | grep -q "^[a-zA-Z]$"
if [ $? -ne 0 ];then
    echo "arg1 must be character"
    exit 1
fi
 
character=$1
out_file=$2
target_size=$3
 
# echo输出默认是带'\n'字符的，所以需要通过dd指定输入字节数
# 写一个字节到out_file ibs输入文件1字节 * count 1 
echo "$character" | dd of=$out_file ibs=1 count=1
while true
do
	# 当前输出文件的大小
    cur_size=`du -b $out_file | awk '{print $1}'`
	# 当前大小 大于等于 目标大小
    if [ $cur_size -ge $target_size ];then
		echo 1
        break
    fi
	# 目标大小减去当前大小的值
    remain_size=$((target_size-$cur_size))
	echo 2
	# 剩下需要填充的比当前大，需要输入字节数，就是当前的字节数
	# 否则就是剩下的字节数
    if [ $remain_size -ge $cur_size ];then
		echo 3
        input_size=$cur_size
    else
		echo 4
        input_size=$remain_size
    fi
	echo 5
	echo $remain_size
	echo $cur_size
    dd if=$out_file ibs=$input_size count=1 of=$out_file seek=1 obs=$cur_size || exit 1
done

echo 6
exit 1
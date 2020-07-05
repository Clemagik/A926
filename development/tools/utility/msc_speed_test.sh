#!/bin/sh

LOG_MSC_SPEED_TEST=$(pwd)/msc_speed_test.log
count_num=51200
file_name=test_file
file_path=/mnt/sd
TEST_SIZE=`expr $count_num \/ 1024 `

output_env()
{
	echo
	echo "*******************************************************" 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	echo "TEST DATE: $(date)" 2>&1| tee -a $LOG_MSC_SPEED_TEST
	echo "TEST FILE SIZE: $TEST_SIZE MB" 2>&1| tee -a $LOG_MSC_SPEED_TEST
	echo "***************** TEST ENVIRONMENT ********************" 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	cat /proc/jz/clock/clocks |grep cclk 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	cat /proc/jz/clock/clocks |grep l2clk 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	cat /proc/jz/clock/clocks |grep h0clk 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	cat /proc/jz/clock/clocks |grep h2clk 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	cat /proc/jz/clock/clocks |grep pclk | head -1 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	cat /proc/jz/clock/clocks |grep cgu_msc0 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	echo "*******************************************************" 2>&1 |tee -a $LOG_MSC_SPEED_TEST
}

write_speed_test()
{
	echo "Write Test ,Please Wait ..." 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	sync
	echo 3 > /proc/sys/vm/drop_caches
	echo "time dd if=/dev/zero of=$1/$file_name bs=1024 count=$2"
	real_time=$( { time dd if=/dev/zero of=$1/$file_name bs=1024 count=$2;} 2>&1 |awk 'NR==3{print $3}')
	sync_time=$( { time sync;} 2>&1 |awk 'NR==1{print $3}')
	clean_time=$( { time echo 3 > /proc/sys/vm/drop_caches;} 2>&1 |awk 'NR==1{print $3}')
	real_time=${real_time%?}
	sync_time=${sync_time%?}
	clean_time=${clean_time%?}
	spend_time=$(awk 'BEGIN{printf "%0.3f\n",'$real_time'+'$sync_time'+'$clean_time'}')
	echo "dd real time is: $real_time"
	echo "sync time is :$sync_time"
	echo "clean time is :$clean_time"
	echo "spend all time is : $spend_time" 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	WRITE_SPEED=$(awk 'BEGIN{printf "%0.3f\n",'$count_num'/'$spend_time'/'1024'}')
	echo "the write speed is $WRITE_SPEED MB/s" 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	echo
}

read_speed_test()
{
	echo "Read Test ,Please Wait ..." 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	sync
	echo 3 > /proc/sys/vm/drop_caches
	echo "time cat $1/$file_name >/dev/null "
	real_time=$( { time cat $1/$file_name >/dev/null;} 2>&1 |awk 'NR==1{print $3}')
	sync_time=$( { time sync;} 2>&1 |awk 'NR==1{print $3}')
	clean_time=$( { time echo 3 > /proc/sys/vm/drop_caches;} 2>&1 |awk 'NR==1{print $3}')
	real_time=${real_time%?}
	sync_time=${sync_time%?}
	clean_time=${clean_time%?}
	spend_time=$(awk 'BEGIN{printf "%0.3f\n",'$real_time'+'$sync_time'+'$clean_time'}')
	echo "dd real time is: $real_time"
	echo "sync time is :$sync_time"
	echo "clean time is :$clean_time"
	echo "spend all time is : $spend_time" 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	READ_SPEED=$(awk 'BEGIN{printf "%0.3f\n",'$count_num'/'$spend_time'/'1024'}')
	echo "the read speed is $READ_SPEED MB/s" 2>&1 |tee -a $LOG_MSC_SPEED_TEST
	echo
}

print_table()
{
	echo " " 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo "TEST FILE SIZE: $TEST_SIZE MB" 2>&1| tee -a $LOG_MSC_SPEED_TEST
	echo "----------------------------------" 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo "TEST RESULT:" 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo "----------------------------------" 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo " | WRITE SPEED is: $WRITE_SPEED MB/s  " 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo "----------------------------------" 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo " | READ SPEED is:  $READ_SPEED MB/s  " 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo "----------------------------------" 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo " " 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	echo " " 2>&1 | tee -a $LOG_MSC_SPEED_TEST
}

main()
{
	output_env
	write_speed_test "$file_path" "$count_num"
	echo "" 2>&1 | tee -a $LOG_MSC_SPEED_TEST
	read_speed_test $file_path
	rm $file_path/$file_name
	print_table

}

main

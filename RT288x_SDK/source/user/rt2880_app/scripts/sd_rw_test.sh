#!/bin/sh

OPTION=$1
usage()
{
	echo "Usage: sd_rw_test.sh [OPTION] [mount] [file1] [file2]"
	echo "	loopback test: sd_rw_test.sh lo /media/file"
	echo "	read/write test: sd_rw_test.sh rw"
	echo "	read only: sd_rw_test.sh ro"
	echo "	write only: sd_rw_test.sh wo"
	exit 1
}

if [ "$OPTION" != "lo" -a "$OPTION" != "rw" -a "$OPTION" != "ro" -a "$OPTION" != "wo" -a "$OPTION" != "detect" -a "$OPTION" != "fillup" ]; then
	usage
	exit 1
fi

if [ "$OPTION" == "lo" -o "$OPTION" == "detect" -o "$OPTION" == "fillup" ]; then
	if [ ! -f "$2" ]; then
		usage
		exit 1
	fi
	PASS=0
	FAIL=0
	TestFile=$2
	TestFileCRC=`md5sum $TestFile | sed 's/\/media\/.*//'`
	if [ "$OPTION" == "detect" ]; then
		InitCardState=`reg s b0130000;reg p 8`
		reg s b0000600;reg w 24 500;reg w 20 500
		card_state=`reg s b0130000;reg p 8`
	fi
fi

COUNT=1
MOUNT=`mount | grep mmc | sed 's/^\/dev.*\/media/\/media/g' | sed 's/\ type.*$//g'`

echo 3 > /proc/sys/vm/drop_caches
echo 8192 > /proc/sys/vm/min_free_kbytes

doLoopBackTest()
{
	echo "`date` --- The Loopback $COUNT time ---"
	rm -rf $MOUNT/backup
	sync
	mkdir $MOUNT/backup
	echo "cp $TestFile $MOUNT/backup"
#	time cp $TestFile $MOUNT/backup
	cp $TestFile $MOUNT/backup
	sync
	bk_file=`ls $MOUNT/backup`
#	ls -l $TestFile
#	ls -l $MOUNT/backup/$bk_file
	bk_file_crc=`md5sum $MOUNT/backup/$bk_file | sed 's/\/media\/.*//'`
	if [ "$bk_file_crc" != "$TestFileCRC" ]; then
		FAIL=`expr $FAIL + 1`
	else
		PASS=`expr $PASS + 1`
	fi
	rm -rf $MOUNT/backup
	sync
	echo "`date` --- Loopback Test: Pass-$PASS ,Failed-$FAIL ---"
}

doRWTest()
{
	echo "`date` --- The Read/Write $COUNT time ---"
	if [ "$OPTION" == "rw" -o "$OPTION" == "wo" ]; then
	echo "Memory ===> SD"
		for i in 16m 32m 64m 128m 256m 512m 1024m
		do
			rm -rf $MOUNT/DDFile
			sync
			lmdd if=internal of=$MOUNT/DDFile$i move=$i fsync=1
		done
	fi
	if [ ! -f $MOUNT/DDFile1024m ]; then
		lmdd if=internal of=$MOUNT/DDFile1024m move=1024m fsync=1
	else
		ls -l $MOUNT/DDFile
	fi
	if [ "$OPTION" == "rw" -o "$OPTION" == "ro" ]; then
		echo "SD ===> Memory"
		for i in 1024m 512m 256m 128m 64m 32m 16m
		do
			lmdd if=$MOUNT/DDFile1024m of=internal move=$i fsync=1
		done
	fi
}

doCardDetectTest()
{
	echo "`date` --- The Card Detect $COUNT time ---"
	card_state=`reg p 8`
	if [ "$card_state" != "$InitCardState" ]; then
		sleep 3
		if [ -d "$MOUNT" ]; then
			echo "Card detected and mounted! Do loop back test ..." 
			doLoopBackTest
			echo "[ Please remove card ... ]" 
			sleep 1
			reg s b0000600;reg w 24 500;reg w 20 500
		else
			echo "Card removed! [ Please insert card ... ]"
			sleep 1
			reg s b0000600;reg w 24 500;reg w 20 0
		fi
		reg s b0130000
	fi
	InitCardState=$card_state
}

doFillUpTest()
{
	echo "`date` --- The Card Detect $COUNT time ---"
	echo 
	cp $TestFile $TestFile$COUNT
	sync
}

echo "***** Start Test *****"
if [ "$OPTION" == "detect" -a $card_state == $InitCardState ]; then
	reg s b0000600;reg w 24 500;reg w 20 0
	card_state=`reg s b0130000;reg p 8`
fi
while [ 1 ]
do
	if [ "$OPTION" == "lo" ]; then
		if [ -d "$MOUNT" ]; then
			doLoopBackTest 
		else
			echo "*** Error: mount device fail ***"
			exit 1
		fi
	fi
	if [ "$OPTION" == "rw" -o "$OPTION" == "wo" -o "$OPTION" == "ro" ]; then
		if [ -d "$MOUNT" ]; then
			doRWTest
		else
			echo "*** Error: mount device fail ***"
			exit 1
		fi
	fi
	if [ "$OPTION" == "detect" ]; then
		doCardDetectTest
	fi
	if [ "$OPTION" == "fillup" ]; then
		doFillUpTest
	fi
	COUNT=`expr $COUNT + 1`
done

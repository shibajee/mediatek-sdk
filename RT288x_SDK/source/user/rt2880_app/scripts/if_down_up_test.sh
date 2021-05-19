#!/bin/sh

OP=$1
IF=$2
IF_STATE=
COUNT=0

check_if_state()
{
	IF_STATE=`ifconfig | sed -n "/$1/"p` 
	if [ "$IF_STATE" == "" ]; then
		echo -e "\n!!! Interface $1 is DOWN !!!\n"
	else
		echo -e "\n!!! Interface $1 is UP !!!\n"
	fi
}

if_up_down_test()
{
	ifconfig $IF down
	sleep 2
	check_if_state $IF
	sleep 1
	ifconfig $IF up
	sleep 5
	check_if_state $IF
	sleep 1
}

radio_on_off_test()
{
	echo "!!! RF is OFF !!!"
	iwpriv ra0 set RadioOn=0
	sleep 2
	echo "!!! RF is ON !!!"
	iwpriv ra0 set RadioOn=1
	sleep 2
}

if [ "$IF" == "" -o "$OP" == "" ]; then
	echo "Usage:"
	echo "	$0 [operation] [interface]"
	echo "	example:"
	echo "	  if_down_up_test.sh device ra0   - just do 'interface down-up' test"
	echo "	  if_down_up_test.sh radio ra0    - just do 'Radio off-on' test"
	echo "	  if_down_up_test.sh all ra0      - do all test"
	exit
fi

check_if_state $IF > /dev/null
if [ "$IF_STATE" == "" ]; then
	echo "!!! Please up $IF First !!!"
	exit
fi

while [ 1 ]
do
	COUNT=`expr $COUNT + 1`
	echo "!!! The $COUNT time !!!"
	if [ "$OP" == "device" -o "$OP" == "all" ]; then
		if_up_down_test
	fi
	if [ "$OP" == "radio" -o "$OP" == "all" ]; then
		radio_on_off_test
	fi
done

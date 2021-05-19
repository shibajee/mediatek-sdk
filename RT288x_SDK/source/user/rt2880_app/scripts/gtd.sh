#!/bin/sh

if [ ! -n "$2" ]; then
  echo "================================"
  echo "Gigabit Tx Distortion Test"
  echo "================================"
  echo ""
  echo "insufficient arguments!"
  echo "Usage: $0 <mode:1~4> <port:0~4>"
  echo ""
  exit 0
fi

MODE="$1"
PORT="$2"
NORMAL_MODE_VAL="0x600"

if [ $MODE -lt "1" -o $MODE -gt "4" ]; then
  echo ""
  echo " WARNING!! mode=$MODE is invalid (should be 1 ~ 4)"
  echo ""
  exit 0
fi

if [ $PORT != "0" -a $PORT != "1" -a $PORT != "2" -a $PORT != "3"  -a $PORT != "4" ]; then
  echo ""
  echo " WARNING!! port=$PORT is invalid (should be 0 ~ 4)"
  echo ""
  exit 0
fi

if [ $MODE == "1" ]; then
	TEST_MODE_VAL="0x2700"
elif [ $MODE == "2" ]; then
	TEST_MODE_VAL="0x4700"
elif [ $MODE == "3" ]; then
	TEST_MODE_VAL="0x6700"
elif [ $MODE == "4" ]; then
	TEST_MODE_VAL="0x8700"
fi


for port in 0 1 2 3 4; do
	if [ $port == $PORT ]; then
		mii_mgr -s -p $port -r 9 -v $TEST_MODE_VAL 1>/dev/null 2>&1
	else
		mii_mgr -s -p $port -r 9 -v $NORMAL_MODE_VAL 1>/dev/null 2>&1
	fi
done

#!/bin/sh

if [ ! -n "$2" ]; then
  echo "================================"
  echo "Gigabit Templete Pair Test"
  echo "================================"
  echo ""
  echo "insufficient arguments!"
  echo "Usage: $0 <pair:a~d> <port:0~4>"
  echo ""
  exit 0
fi

PAIR="$1"
PORT="$2"

if [ $PAIR != "a" -a $PAIR != "b" -a $PAIR != "c" -a $PAIR != "d" ]; then
  echo ""
  echo " WARNING!! pair=$PAIR is invalid (should be a ~ d)"
  echo ""
  exit 0
fi

if [ $PORT != "0" -a $PORT != "1" -a $PORT != "2" -a $PORT != "3"  -a $PORT != "4" ]; then
  echo ""
  echo " WARNING!! port=$PORT is invalid (should be 0 ~ 4)"
  echo ""
  exit 0
fi

if [ $PAIR == "b" ]; then
	mii_mgr -s -p $PORT -r 13 -v 0x001f 1>/dev/null 2>&1
	mii_mgr -s -p $PORT -r 14 -v 0x0044 1>/dev/null 2>&1
	mii_mgr -s -p $PORT -r 13 -v 0x401f 1>/dev/null 2>&1
	mii_mgr -s -p $PORT -r 14 -v 0x0000 1>/dev/null 2>&1
fi


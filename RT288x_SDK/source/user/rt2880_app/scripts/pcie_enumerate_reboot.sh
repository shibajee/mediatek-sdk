#!/bin/sh
. /sbin/config.sh

PCIE_CHECH_FAIL=0
if [ "$CONFIG_RALINK_MT7621" == "y" ]; then
	PCIE_BASE_REG=be140000
else
	PCIE_BASE_REG=b0140000
fi

PORT0_STATUS=`reg s $PCIE_BASE_REG; reg p 2050 > /etc/pcie_status`
PORT0_STATUS=`cat /etc/pcie_status`
echo "PCIE PORT0 Link: $PORT0_STATUS"

if [ "$CONFIG_RALINK_MT7621" == "y" ]; then
	PORT1_STATUS=`reg s $PCIE_BASE_REG; reg p 3050 > /etc/pcie_status`
	PORT1_STATUS=`cat /etc/pcie_status`
	echo "PCIE PORT1 Link: $PORT1_STATUS"
	PORT2_STATUS=`reg s $PCIE_BASE_REG; reg p 4050 > /etc/pcie_status`
	PORT2_STATUS=`cat /etc/pcie_status`
	echo "PCIE PORT2 Link: $PORT2_STATUS"
fi


if [ "$PORT0_STATUS" != "0x1" ]; then
	echo "PCIE Link Down"
	PCIE_CHECH_FAIL=1
fi
if [ "$CONFIG_RALINK_MT7621" == "y" ]; then
	if [ "$CONFIG_PCIE_PORT1" == "y" -a "$PORT1_STATUS" != "0x1" ]; then
		echo "PCIE Link Down"
		PCIE_CHECH_FAIL=1
	fi
	if [ "$CONFIG_PCIE_PORT2" == "y" -a "$PORT2_STATUS" != "0x1" ]; then
		echo "PCIE Link Down"
		PCIE_CHECH_FAIL=1
	fi
fi
LSPCI=`lspci`
if [ "$LSPCI" == "" ]; then
	echo "lspci fail"
	PCIE_CHECH_FAIL=1
else
	lspci
fi

if [ "$CONFIG_RLT_WIFI" == "y" ]; then
	CHECK_WIFI=`ifconfig ra0`
	if [ "$LSPCI" == "" ]; then
		echo "check 1st WIFI fail"
		PCIE_CHECH_FAIL=1
	else
		ifconfig ra0
	fi
	CHECK_WIFI=`ifconfig rai0`
	if [ "$LSPCI" == "" ]; then
		echo "check 2nd WIFI fail"
		PCIE_CHECH_FAIL=1
	else
		ifconfig rai0
	fi
fi

if [ "$PCIE_CHECH_FAIL" == "0" ]; then
	reboot
fi


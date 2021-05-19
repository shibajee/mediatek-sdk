#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <linux/if.h>
#include <linux/mii.h>
#include <linux/types.h>
#include <linux/autoconf.h>
#include <config/autoconf.h>
#include "ra_ioctl.h"


int mii_mgr_cl45_read(int port_num, int dev, int reg){
	int sk, method, ret, i;
	struct ifreq ifr;
	ra_mii_ioctl_data mii;

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Open socket failed\n");
	}

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &mii;

	method = RAETH_MII_WRITE;
	mii.phy_id = port_num;
	mii.reg_num = 13;
	mii.val_in =  dev;
	ret = ioctl(sk, method, &ifr);

	method = RAETH_MII_WRITE;
	mii.phy_id = port_num;
	mii.reg_num = 14;
	mii.val_in =  reg;
	ret = ioctl(sk, method, &ifr);

	method = RAETH_MII_WRITE;
	mii.phy_id = port_num;
	mii.reg_num = 13;
	mii.val_in =  (0x6000 | dev);
	ret = ioctl(sk, method, &ifr);

        usleep(1000);

	method = RAETH_MII_READ;
	mii.phy_id = port_num;
	mii.reg_num = 14;
	ret = ioctl(sk, method, &ifr);

	close(sk);

	return mii.val_out;
}

int main(int argc, char *argv[])
{
	int port_num = 5;
	int cl45_start_reg = 0x9B;
	int cl45_end_reg = 0xA2;
	int cl22_reg[6] = {0x00, 0x01, 0x04, 0x05, 0x09, 0x0A};
	int i, j, ret;


	int sk, method;
	struct ifreq ifr;
	ra_mii_ioctl_data mii;

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Open socket failed\n");
	}

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &mii;
	/* dump CL45 reg first*/

	for(i=0; i<5 ; i++){
		printf("== Port %d ==\n", i);
		for(j=cl45_start_reg; j<(cl45_end_reg+1); j++){
			ret = mii_mgr_cl45_read(i, 0x1E, j);
			printf("dev1Eh_reg%xh = 0x%x\n", j, ret);
		}

	}

	printf("== Global ==\n");
	for( i=0; i<6; i++){
		method = RAETH_MII_READ;
		mii.phy_id = 0;
		mii.reg_num = cl22_reg[i];
		ret = ioctl(sk, method, &ifr);
		printf("Reg%xh = 0x%x\n",cl22_reg[i] , mii.val_out);
	}

	return 0;
}

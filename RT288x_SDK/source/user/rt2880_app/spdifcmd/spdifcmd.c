#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include <fcntl.h>
#include <signal.h>

#include "spdif_ctrl.h"

/* AC3 sync info: for parsing ac3 sample rate and frame size */
typedef struct ac3_si
{
	uint16_t syncword: 16;
	uint16_t crc1: 16;
	uint16_t frmsizecod: 8;
} ac3_si_t;

typedef struct ac3_si_bswap
{
	uint16_t syncword: 16;
	uint16_t crc1: 16;
	uint16_t others: 8;
	uint16_t frmsizecod: 8;
} ac3_si_bswap_t;

int spdif_fd;
int spdif_pcm;
int pause_stat;
char* txbuffer;
void *fdm;
char* txrawbuffer;

int pause_cnt;
int resume_cnt;

struct stat spdif_stat;

#define swab16(x) \
	((unsigned short)( \
		(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) | \
		(((unsigned short)(x) & (unsigned short)0xff00U) >> 8) ))

void usage()
{
	printf("Usage: [fmt=0] [srate] [wordlen] [pathname]\n");
	printf("       [fmt=1] [srate] [rawtype] [pathname]\n");
	printf("       fmt = 0|1 - spdif pcm| raw data\n");
	printf("       srate = 22050| 24000| 32000| 44100| 48000| 88200| 96000| 176400| 192000 Hz sampling freqency \n");
	printf("       rawtype = for raw data (fmt = 1) -- (0: Null data;)  1: AC3 data; (3: Pause) \n");
	printf("       wordlen = 16| 24 bits per sample\n");
	printf("       downsample = 1: no down sample; 2: 2x down sample; 4: 4x down sample\n\n");
	printf("       [fmt=2] [pathname]\n");
}

int get_ac3_frame_size_48ksrate(int frmsizecod)
{
	int frmsize; //unit: words

	if ((frmsizecod == 0) || (frmsizecod == 1))
	{
		frmsize = 64;
	}
	else if ((frmsizecod == 2) || (frmsizecod == 3))
	{
		frmsize = 80;
	}
	else if ((frmsizecod == 4) || (frmsizecod == 5))
	{
		frmsize = 96;
	}
	else if ((frmsizecod == 6) || (frmsizecod == 7))
	{
		frmsize = 112;
	}
	else if ((frmsizecod == 8) || (frmsizecod == 9))
	{
		frmsize = 128;
	}
	else if ((frmsizecod == 10) || (frmsizecod == 11))
	{
		frmsize = 160;
	}
	else if ((frmsizecod == 12) || (frmsizecod == 13))
	{
		frmsize = 192;
	}
	else if ((frmsizecod == 14) || (frmsizecod == 15))
	{
		frmsize = 224;
	}
	else if ((frmsizecod == 16) || (frmsizecod == 17))
	{
		frmsize = 256;
	}
	else if ((frmsizecod == 18) || (frmsizecod == 19))
	{
		frmsize = 320;
	}
	else if ((frmsizecod == 20) || (frmsizecod == 21))
	{
		frmsize = 384;
	}
	else if ((frmsizecod == 22) || (frmsizecod == 23))
	{
		frmsize = 448;
	}
	else if ((frmsizecod == 24) || (frmsizecod == 25))
	{
		frmsize = 512;
	}
	else if ((frmsizecod == 26) || (frmsizecod == 27))
	{
		frmsize = 640;
	}
	else if ((frmsizecod == 28) || (frmsizecod == 29))
	{
		frmsize = 768;
	}
	else if ((frmsizecod == 30) || (frmsizecod == 31))
	{
		frmsize = 896;
	}
	else if ((frmsizecod == 32) || (frmsizecod == 33))
	{
		frmsize = 1024;
	}
	else if ((frmsizecod == 34) || (frmsizecod == 35))
	{
		frmsize = 1152;
	}
	else if ((frmsizecod == 36) || (frmsizecod == 37))
	{
		frmsize = 1280;
	}
	else 
	{
		printf("Not support this spec!\n");
		return -1;
	}
	return frmsize;


}

int get_ac3_frame_size_44ksrate(int frmsizecod)
{
	int frmsize; //unit:words

	if (frmsizecod == 0)
	{
		frmsize = 69;
	}
	else if (frmsizecod == 1)
	{
		frmsize = 70;
	}
	else if (frmsizecod == 2)
	{
		frmsize = 87;
	}
	else if (frmsizecod == 3)
	{
		frmsize = 88;
	}
	else if (frmsizecod == 4)
	{
		frmsize = 104;
	}
	else if (frmsizecod == 5)
	{
		frmsize = 105;
	}
	else if (frmsizecod == 6)
	{
		frmsize = 121;
	}
	else if (frmsizecod == 7)
	{
		frmsize = 122;
	}
	else if (frmsizecod == 8)
	{
		frmsize = 139;
	}
	else if (frmsizecod == 9)
	{
		frmsize = 140;
	}
	else if (frmsizecod == 10)
	{
		frmsize = 174;
	}
	else if (frmsizecod == 11)
	{
		frmsize = 175;
	}
	else if (frmsizecod == 12)
	{
		frmsize = 208;
	}
	else if (frmsizecod == 13)
	{
		frmsize = 209;
	}
	else if (frmsizecod == 14)
	{
		frmsize = 243;
	}
	else if (frmsizecod == 15)
	{
		frmsize = 244;
	}
	else if (frmsizecod == 16)
	{
		frmsize = 278;
	}
	else if (frmsizecod == 17)
	{
		frmsize = 279;
	}
	else if (frmsizecod == 18)
	{
		frmsize = 348;
	}
	else if (frmsizecod == 19)
	{
		frmsize = 349;
	}
	else if (frmsizecod == 20)
	{
		frmsize = 417;
	}
	else if (frmsizecod == 21)
	{
		frmsize = 418;
	}
	else if (frmsizecod == 22)
	{
		frmsize = 487;
	}
	else if (frmsizecod == 23)
	{
		frmsize = 488;
	}
	else if (frmsizecod == 24)
	{
		frmsize = 557;
	}
	else if (frmsizecod == 25)
	{
		frmsize = 558;
	}
	else if (frmsizecod == 26)
	{
		frmsize = 696;
	}
	else if (frmsizecod == 27)
	{
		frmsize = 697;
	}
	else if (frmsizecod == 28)
	{
		frmsize = 835;
	}
	else if (frmsizecod == 29)
	{
		frmsize = 836;
	}
	else if (frmsizecod == 30)
	{
		frmsize = 975;
	}
	else if (frmsizecod == 31)
	{
		frmsize = 976;
	}
	else if (frmsizecod == 32)
	{
		frmsize = 1114;
	}
	else if (frmsizecod == 33)
	{
		frmsize = 1115;
	}
	else if (frmsizecod == 34)
	{
		frmsize = 1253;
	}
	else if (frmsizecod == 35)
	{
		frmsize = 1254;
	}
	else if (frmsizecod == 36)
	{
		frmsize = 1393;
	}
	else if (frmsizecod == 37)
	{
		frmsize = 1397;
	}
	else
	{
		printf("Not support this spec!\n");
		return -1;
	}
    	return frmsize;
}

int get_ac3_frame_size_32ksrate(int frmsizecod)
{
	int frmsize; //unit: words

	if ((frmsizecod == 0) || (frmsizecod == 1))
	{
		frmsize = 96;
	}
	else if ((frmsizecod == 2) || (frmsizecod == 3))
	{
		frmsize = 120;
	}
	else if ((frmsizecod == 4) || (frmsizecod == 5))
	{
		frmsize = 144;
	}
	else if ((frmsizecod == 6) || (frmsizecod == 7))
	{
		frmsize = 168;
	}
	else if ((frmsizecod == 8) || (frmsizecod == 9))
	{
		frmsize = 192;
	}
	else if ((frmsizecod == 10) || (frmsizecod == 11))
	{
		frmsize = 240;
	}
	else if ((frmsizecod == 12) || (frmsizecod == 13))
	{
		frmsize = 288;
	}
	else if ((frmsizecod == 14) || (frmsizecod == 15))
	{
		frmsize = 336;
	}
	else if ((frmsizecod == 16) || (frmsizecod == 17))
	{
		frmsize = 384;
	}
	else if ((frmsizecod == 18) || (frmsizecod == 19))
	{
		frmsize = 480;
	}
	else if ((frmsizecod == 20) || (frmsizecod == 21))
	{
		frmsize = 576;
	}
	else if ((frmsizecod == 22) || (frmsizecod == 23))
	{
		frmsize = 672;
	}
	else if ((frmsizecod == 24) || (frmsizecod == 25))
	{
		frmsize = 768;
	}
	else if ((frmsizecod == 26) || (frmsizecod == 27))
	{
		frmsize = 960;
	}
	else if ((frmsizecod == 28) || (frmsizecod == 29))
	{
		frmsize = 1152;
	}
	else if ((frmsizecod == 30) || (frmsizecod == 31))
	{
		frmsize = 1344;
	}
	else if ((frmsizecod == 32) || (frmsizecod == 33))
	{
		frmsize = 1536;
	}
	else if ((frmsizecod == 34) || (frmsizecod == 35))
	{
		frmsize = 1728;
	}
	else if ((frmsizecod == 36) || (frmsizecod == 37))
	{
		frmsize = 1920;
	}
	else 
	{
		printf("Not support this spec!\n");
		return -1;
	}
	return frmsize;
}

int get_ac3_frame_len(int fscod, int frmsizecod)
{
	int value;

	if(fscod == 0) //48kHz sampling rate
	{
		value = get_ac3_frame_size_48ksrate(frmsizecod);
	}
	else if (fscod == 1) // 44.1kHz sampling rate
	{
		value = get_ac3_frame_size_44ksrate(frmsizecod);
	}
	else if (fscod == 2) // 32kHz sampling rate
	{
		value = get_ac3_frame_size_32ksrate(frmsizecod);
	}
	else
	{
		printf("Not support this spec!\n");
		return -1;
	}

	return value; //unit: 1 word = 16 bit
}

void spdif_tx_handler(int signum)
{
        int i;
        switch(signum)
        {
        case SIGINT:
                ioctl(spdif_fd, SPDIF_TX_STOP, 0);
        
		if(spdif_pcm == 1)
		{        
			free(txbuffer);
		}
		else
		{
			free(txrawbuffer);
		}

		munmap(fdm, spdif_stat.st_size);
                close(spdif_fd);
                printf("SPDIF STOP!!\n");
                break;
        case SIGUSR1:
                if(pause_stat == 0)
                {
                        ioctl(spdif_fd, SPDIF_TX_PAUSE, 0);
                        pause_stat = 1;
                        printf("SPDIF PAUSE....\n");
                }
                else if (pause_stat == 1)
                {
                        ioctl(spdif_fd, SPDIF_TX_RESUME, 0);
                        pause_stat = 0;
                        printf("SPDIF RESUME....\n");
                }
                break;
        default:
                break;
        }

        return;
}

int main(int argc, char *argv[])
{
	//void *fdm;
	void *fdmSwap;
	int fd;
	int pos;
	int fsize;
	u32 byte_num; 
	u32 burst_sample;
	u32 buf_size;
	int left;
	int index;
	int i;
	
	printf("This is Mediatek SPDIF Command Program...\n");
	fflush(stdout);
	
	spdif_fd = open("/dev/spdif0", O_RDWR|O_SYNC); 
	if(spdif_fd<0)
	{
		printf("spdifcmd:open spdif driver failed (%d)...exit\n",spdif_fd);
		return -1;
	}
	if(argc < 2)
	{
		usage();    
		goto EXIT;	    
	}
	
	switch(strtoul(argv[1], NULL ,10))
	{
	case 0:
		if(argc < 4)
		{
			usage();
	 		goto EXIT;		
		}
		if (fstat(STDIN_FILENO, &spdif_stat) == -1 ||spdif_stat.st_size == 0)
			goto EXIT;
		ioctl(spdif_fd, SPDIF_SAMPLE_FREQ, strtoul(argv[2], NULL ,10));
		ioctl(spdif_fd, SPDIF_WORD_LEN, strtoul(argv[3], NULL ,10));
		
		/* set burst sapmle and byte num for calculating buf size */
		burst_sample = PCM_BURST_SAMPLE;
		if (strtoul(argv[3], NULL ,10) == 16)
		{
			byte_num = 2;
		}
		else if (strtoul(argv[3], NULL ,10) == 24)
		{
			byte_num = 3;
		}
		else
		{
			printf("Not suuport this word length!\n");
			return -1;
		}
		buf_size = burst_sample * byte_num * 2;
		printf("burst_sample=%d; byte_num=%d, buf_size=%d\n", burst_sample, byte_num, buf_size);

		//char* txbuffer;
		txbuffer = (char*)calloc(buf_size, sizeof(char));

		spdif_pcm =1; 

		fdm = mmap(NULL, spdif_stat.st_size, PROT_READ, MAP_SHARED, STDIN_FILENO, 0);
		if (fdm == MAP_FAILED)
		{
			free(txbuffer);
			goto EXIT;
		}

		signal(SIGINT, spdif_tx_handler);
		signal(SIGUSR1, spdif_tx_handler);

		ioctl(spdif_fd, SPDIF_INIT_PARAMETER, 0);
		//ioctl(spdif_fd, SPDIF_TX_PCM_ENABLE, 0);
		pos = 0;
		index = 0;
		i = 0;
		left = spdif_stat.st_size;
		while(left > buf_size)
		{
			if (pause_stat == 0)
			{
				ioctl(spdif_fd, SPDIF_PUT_AUDIO, (char*)fdm+pos);
				i++;
				if(i==6)
					ioctl(spdif_fd, SPDIF_TX_PCM_ENABLE, 0);

				left -= buf_size;
				pos += buf_size;

				resume_cnt ++;
				//if(resume_cnt%100 == 0)
				//	printf("resume_cnt=%d\n", resume_cnt);
			}
			else
			{
				pause_cnt++;
				//if(pause_cnt%100 == 0)
				//	printf("pause_cnt=%d\n", pause_cnt);
				//printf("At PAUSE state\n");
			}
		}
	
		if (pause_stat == 0)
		{	
			memcpy(txbuffer, (char*)fdm+pos, left);
			ioctl(spdif_fd, SPDIF_PUT_AUDIO, txbuffer);
			resume_cnt++;
		}
		else
		{
			pause_cnt++;
			//printf("At Pause state\n");
		}
		ioctl(spdif_fd, SPDIF_TX_DISABLE, 0);
		free(txbuffer);
		munmap(fdm, spdif_stat.st_size)==0;
		break;	
  	case 1:
		if(argc < 4)
		{
			usage();
	 		goto EXIT;		
		}	
		if (fstat(STDIN_FILENO, &spdif_stat) == -1 ||spdif_stat.st_size == 0)
			goto EXIT;

		ioctl(spdif_fd, SPDIF_SAMPLE_FREQ, strtoul(argv[2], NULL ,10));
		ioctl(spdif_fd, SPDIF_RAW_DATA_TYPE, strtoul(argv[3], NULL ,10));
		//ioctl(spdif_fd, SPDIF_WORD_LEN, strtoul(argv[4], NULL ,10));
		//ioctl(spdif_fd, SPDIF_BYTE_SWAP_SET, strtoul(argv[5], NULL ,10));

		/* set burst sapmle and byte num for calculating buf size */
		if (strtoul(argv[3], NULL ,10) == 0)
		{
			burst_sample = NULL_BURST_SAMPLE;
		}
		else if (strtoul(argv[3], NULL ,10) == 1)
		{
			burst_sample = AC3_BURST_SAMPLE;
		}
		else if (strtoul(argv[3], NULL ,10) == 3)
		{
			burst_sample = PAUSE_BURST_SAMPLE;
		}
		else
		{	
			printf("Not support this raw data type!\n");
			return -1;
		}
		
		spdif_pcm = 0;

		fdm = mmap(0, spdif_stat.st_size, PROT_READ, MAP_SHARED, STDIN_FILENO, 0);
		if (fdm == MAP_FAILED)
			goto EXIT;
			
		/* Parsing ac3 frame size */
		int nb_len;

		if (strtoul(argv[3], NULL ,10) == 1)
		{
			ac3_si_t* pac3si;
			int frmsizecod;
			int fscod;

			pac3si = fdm;
			printf("syncword=0x%4x\n", pac3si->syncword);
			printf("syncword=0x%4x\n", htons(pac3si->syncword));
			printf("byte swap test: 0x%4x\n", swab16((u16)(pac3si->syncword)));
			if (htons(pac3si->syncword) == 0x0b77)
			{ 
				fscod = (pac3si->frmsizecod & 0xc0) >> 6;
				frmsizecod = (pac3si->frmsizecod)&(0x3f);

				nb_len = (get_ac3_frame_len(fscod, frmsizecod)) << 1; // unit: byte
				printf("nb_len(no Bswap)=%d\n", nb_len);
				ioctl(spdif_fd, SPDIF_BYTE_SWAP_SET, 0);  // Set Byte swap
				ioctl(spdif_fd, SPDIF_NB_LEN, (nb_len << 3)); // parsing nb_len in the unit of "bit" 
			}
			else if (htons(pac3si->syncword) == 0x770b)
			{
				ac3_si_bswap_t* pac3si_bswap;
				pac3si_bswap = fdm;
				
				fscod = (pac3si_bswap->frmsizecod & 0xc0) >> 6;
				frmsizecod = (pac3si_bswap->frmsizecod)&(0x3f);

				nb_len = (get_ac3_frame_len(fscod, frmsizecod)) << 1; // unit: byte
				printf("nb_len(Bswap mode)=%d\n", nb_len);
				ioctl(spdif_fd, SPDIF_BYTE_SWAP_SET, 1); // Set Byte swap
				ioctl(spdif_fd, SPDIF_NB_LEN, (nb_len << 3)); // parsing nb_len in the unit of "bit" 
			}
			else
			{
				printf("Wrong syncword for AC3 data!\n");
				return -1;
			}
		}
		else
		{
			printf("Not support this data format!\n");
			return -1;
		}
		
		signal(SIGINT, spdif_tx_handler);
		signal(SIGUSR1, spdif_tx_handler);	


		ioctl(spdif_fd, SPDIF_TX_RAW_ENABLE, 0);
		pos = 0;
		index = 0;
		
		//char* txrawbuffer;
		txrawbuffer = (char*)calloc(nb_len, sizeof(char));
		
		left = spdif_stat.st_size;
		while(left > nb_len)
		{
			if (pause_stat == 0)
			{
				ioctl(spdif_fd, SPDIF_PUT_AUDIO_FOR_RAW_DATA, (char*)fdm+pos);
				left -= nb_len;
				pos += nb_len;
				resume_cnt++;
			}
    			else
			{
				pause_cnt++;
				//printf("At PAUSE state\n");
			}
		}
	
		if(pause_stat == 0)
		{	
			if(left !=0)
			{
				memcpy(txrawbuffer, (char*)fdm+pos, left);
			}	
			ioctl(spdif_fd, SPDIF_PUT_AUDIO_FOR_RAW_DATA, txrawbuffer);
			resume_cnt++;
		}
		else
		{
			pause_cnt++;
			//printf("At PAUSE state\n");
		}
		ioctl(spdif_fd, SPDIF_TX_DISABLE, 0);	
		munmap(fdm, spdif_stat.st_size);
                
                free(txrawbuffer);
		break;
	case 2:
		printf("case 2 test\n");
		/* get the size of original file */		
		if(fstat(STDIN_FILENO, &spdif_stat) == -1 || spdif_stat.st_size == 0)
		{
			printf("**** error1 *****\n");
			goto EXIT;
		}
		fdm = mmap(0, spdif_stat.st_size, PROT_READ, MAP_SHARED, STDIN_FILENO, 0);
		if(fdm == MAP_FAILED)
		{
			printf("**** error2 *****\n");
			goto EXIT;
		}
		
		fd = open("audiout.ac3", (O_CREAT| O_RDWR), (S_IRUSR | S_IRUSR));
		if(fd < 2)
		{
			printf("open fd error\n");
                        close(fd);
			goto EXIT;
		}
		
		if (spdif_stat.st_size%2 == 1)	
			fsize = spdif_stat.st_size + 1;
		else
			fsize = spdif_stat.st_size;
		
		if (lseek(fd, fsize-1, SEEK_SET) == -1)
		{
			printf("lseek failed\n");
			close(fd);
			goto EXIT;
		}
		write(fd, "\0", 1);
		lseek(fd, 0, SEEK_SET);
		
		printf("file size = %d\n", fsize);

		fdmSwap = mmap(0, fsize, PROT_WRITE|PROT_READ, MAP_FILE|MAP_SHARED, fd, 0);
		if (fdmSwap == MAP_FAILED)
		{
			printf("mmap fdmSwap failed\n");
                        close(fd);
			goto EXIT; 
		}

		pos = 0;
		char pSwpbuf[2];
		unsigned short tmp;
		int x=0;
	 	while((pos+2)<=fsize)
		{
			memcpy(pSwpbuf, (char*)fdm+pos, 2);
			tmp = ((pSwpbuf[0]&(0x00ff))<<8) | (pSwpbuf[1]&(0x00ff));
			tmp = swab16(tmp);
			pSwpbuf[0]=(tmp&(0x00ff00))>>8;
			pSwpbuf[1]=tmp&(0x00ff);
			memcpy((char*)fdmSwap+pos, pSwpbuf, 2);
			pos=pos+2;
		}


		munmap(fdm, spdif_stat.st_size);
		munmap(fdmSwap, fsize);

                close(fd);

		break;
	default: 	
		break;	
	}
			
EXIT:
	close(spdif_fd);
	
	printf("spdifcmd ...quit\n");
	return 0;
}

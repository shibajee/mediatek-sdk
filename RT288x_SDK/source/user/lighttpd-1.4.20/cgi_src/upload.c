#include "upload.h"

#define REFRESH_TIMEOUT		"40000"		/* 40000 = 40 secs*/

#define RFC_ERROR "RFC1867 error"
#define UPLOAD_FILE "/var/tmpFW"
#define MEM_SIZE	1024
#define MEM_HALT	512


void *memmem(const void *buf, size_t buf_len, const void *byte_line, size_t byte_line_len)
{
	unsigned char *bl = (unsigned char *)byte_line;
	unsigned char *bf = (unsigned char *)buf;
	unsigned char *p  = bf;

	while (byte_line_len <= (buf_len - (p - bf))){
		unsigned int b = *bl & 0xff;
		if ((p = (unsigned char *) memchr(p, b, buf_len - (p - bf))) != NULL){
			if ( (memcmp(p, byte_line, byte_line_len)) == 0)
				return p;
			else
				p++;
		}else{
			break;
		}
	}
	return NULL;
}

#if defined (UPLOAD_FIRMWARE_SUPPORT)

/*
 *  taken from "mkimage -l" with few modified....
 */
int check(char *imagefile, int offset, int len, char *err_msg)
{
	struct stat sbuf;

	int  data_len;
	char *data;
	unsigned char *ptr;
	unsigned long checksum;

	image_header_t header;
	image_header_t *hdr = &header;

	int ifd;

	if ((unsigned)len < sizeof(image_header_t)) {
		sprintf (err_msg, "Bad size: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	ifd = open(imagefile, O_RDONLY);
	if(!ifd){
		sprintf (err_msg, "Can't open %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	if (fstat(ifd, &sbuf) < 0) {
		close(ifd);
		sprintf (err_msg, "Can't stat %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	ptr = (unsigned char *) mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	if ((caddr_t)ptr == (caddr_t)-1) {
		close(ifd);
		sprintf (err_msg, "Can't mmap %s: %s\n", imagefile, strerror(errno));
		return 0;
	}
	ptr += offset;

	/*
	 *  handle Header CRC32
	 */
	memcpy (hdr, ptr, sizeof(image_header_t));

	if (ntohl(hdr->ih_magic) != IH_MAGIC) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "Bad Magic Number: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	data = (char *)hdr;

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = htonl(0);	/* clear for re-calculation */

	if (crc32 (0, data, sizeof(image_header_t)) != checksum) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has bad header checksum!\n", imagefile);
		return 0;
	}

	/*
	 *  handle Data CRC32
	 */
	data = (char *)(ptr + sizeof(image_header_t));
	data_len  = len - sizeof(image_header_t) ;

	if (crc32 (0, data, data_len) != ntohl(hdr->ih_dcrc)) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has corrupted data!\n", imagefile);
		return 0;
	}

#if 1
	/*
	 * compare MTD partition size and image size
	 */
#if defined (CONFIG_RT2880_ROOTFS_IN_RAM)
	if(len > getMTDPartSize("\"Kernel\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than Kernel MTD partition.\n", len);
		return 0;
	}
#elif defined (CONFIG_RT2880_ROOTFS_IN_FLASH)
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
	if(len > getMTDPartSize("\"Kernel_RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than Kernel_RootFS MTD partition.\n", len);
		return 0;
	}
#else
	if(len < CONFIG_MTD_KERNEL_PART_SIZ){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) size doesn't make sense.\n", len);
		return 0;
	}

	if((len - CONFIG_MTD_KERNEL_PART_SIZ) > getMTDPartSize("\"RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than RootFS MTD partition.\n", len - CONFIG_MTD_KERNEL_PART_SIZ);
		return 0;
	}
#endif
#else
#error "goahead: no CONFIG_RT2880_ROOTFS defined!"
#endif
#endif

	munmap(ptr, len);
	close(ifd);

	return 1;
}


#endif /* UPLOAD_FIRMWARE_SUPPORT */

/*
 * I'm too lazy to use popen() instead of system()....
 * ( note:  static buffer used)
 */
#define DEFAULT_LAN_IP "10.10.10.254"
char *getLanIP(void)
{
	static char buf[64];
	char *nl;
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	if( (fp = popen("nvram_get 2860 lan_ipaddr", "r")) == NULL )
		goto error;

	if(!fgets(buf, sizeof(buf), fp)){
		pclose(fp);
		goto error;
	}

	if(!strlen(buf)){
		pclose(fp);
		goto error;
	}
	pclose(fp);

	if(nl = strchr(buf, '\n'))
		*nl = '\0';

	return buf;

error:
	fprintf(stderr, "warning, cant find lan ip\n");
	return DEFAULT_LAN_IP;
}


void javascriptUpdate(int success)
{
	printf("<script language=\"JavaScript\" type=\"text/javascript\">");
	if(success){
		printf(" \
function refresh_all(){	\
  top.location.href = \"http://%s\"; \
} \
function update(){ \
  self.setTimeout(\"refresh_all()\", %s);\
}", getLanIP(), REFRESH_TIMEOUT);
	}else{
		printf("function update(){ parent.menu.setLockMenu(0);}");
	}
	printf("</script>");
}

inline void webFoot(void)
{
	printf("</body></html>\n");
}


int main (int argc, char *argv[])
{
	char *file_begin, *file_end;
	char *line_begin, *line_end;
	char err_msg[256];
	char *boundary; int boundary_len;
	FILE *fp = fopen(UPLOAD_FILE, "w");
	long inLen;
	char *inStr;

	inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
	inStr = malloc(inLen);
	memset(inStr, 0, inLen);
	fread(inStr, 1, inLen, stdin);
	printf(
"\
Server: %s\n\
Pragma: no-cache\n\
Content-type: text/html\n",
getenv("SERVER_SOFTWARE"));

	printf("\n\
<html>\n\
<head>\n\
<TITLE>Upload Firmware</TITLE>\n\
<link rel=stylesheet href=/style/normal_ws.css type=text/css>\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n\
</head>\n\
<body onload=\"update()\"> <h1> Upload Firmware</h1>");

	line_begin = inStr;
	if ((line_end = strstr(line_begin, "\r\n")) == 0) {
		printf("%s %d", RFC_ERROR, 1);
		return -1;
	}
	boundary_len = line_end - line_begin;
	boundary = malloc(boundary_len+1);
	if (NULL == boundary) {
		printf("boundary allocate %d faul!\n", boundary_len);
		goto err;
	}
	memset(boundary, 0, boundary_len+1);
	memcpy(boundary, line_begin, boundary_len);

	// sth like this..
	// Content-Disposition: form-data; name="filename"; filename="\\192.168.3.171\tftpboot\a.out"
	//
	line_begin = line_end + 2;
	if ((line_end = strstr(line_begin, "\r\n")) == 0) {
		printf("%s %d", RFC_ERROR, 2);
		goto err;
	}
	if(strncasecmp(line_begin, 
		       "content-disposition: form-data;", 
		       strlen("content-disposition: form-data;"))) {
		printf("%s %d", RFC_ERROR, 3);
		goto err;
	}
	line_begin += strlen("content-disposition: form-data;") + 1;
	if (!(line_begin = strchr(line_begin, ';'))){
		printf("We dont support multi-field upload.\n");
		goto err;
	}
	line_begin += 2;
	if (strncasecmp(line_begin, "filename=", strlen("filename="))) {
		printf("%s %d", RFC_ERROR, 4);
		goto err;
	}

	// We may check a string  "Content-Type: application/octet-stream" here,
	// but if our firmware extension name is the same with other known ones, 
	// the browser would use other content-type instead.
	// So we dont check Content-type here...
	line_begin = line_end + 2;
	if ((line_end = strstr(line_begin, "\r\n")) == 0) {
		printf("%s %d", RFC_ERROR, 5);
		goto err;
	}
	line_begin = line_end + 2;
	if ((line_end = strstr(line_begin, "\r\n")) == 0) {
		printf("%s %d", RFC_ERROR, 6);
		goto err;
	}
	file_begin = line_end + 2;
	file_end = (char*)memmem(file_begin, inLen - (file_begin - inStr), 
				 boundary, boundary_len);
	file_end -= 2;		// back 2 chars.(\r\n);
	fwrite(file_begin, 1, file_end - file_begin, fp);

	// examination
#if defined (UPLOAD_FIRMWARE_SUPPORT)
		if(!check(UPLOAD_FILE, 0, file_end - file_begin, err_msg) ){
			printf("Not a valid firmware. %s", err_msg);
			javascriptUpdate(0);
			goto err;
		}

	/*
	 * write the current linux version into flash.
	 */
	write_flash_kernel_version(UPLOAD_FILE, 0);
#ifdef CONFIG_RT2880_DRAM_8M
	system("killall goahead");
#endif

	// flash write
	if( mtd_write_firmware(UPLOAD_FILE, 0, file_end - file_begin) == -1){
		printf("mtd_write fatal error! The corrupted image has ruined the flash!!");
		javascriptUpdate(0);
		goto err;
	}
#elif defined (UPLOAD_BOOTLOADER_SUPPORT)
		mtd_write_bootloader(UPLOAD_FILE, 0, file_end - file_begin);
#else
#error "no upload support defined!"
#endif

	printf("Done...rebooting");
	javascriptUpdate(1);
	webFoot();
#if 0
	free(boundary);
	free(inStr);
	fclose(fp);
#endif
	system("sleep 3 && reboot &");
	exit(0);

err:
	webFoot();
	free(boundary);
	free(inStr);
	fclose(fp);
	exit(-1);
}


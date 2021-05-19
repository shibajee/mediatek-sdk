
#include <stdio.h>


typedef unsigned short __u16;
typedef __u16 __le16;




int main(void)
{
	__le16 i;
	printf("%ld\n", sizeof(i));
	return 0;
}

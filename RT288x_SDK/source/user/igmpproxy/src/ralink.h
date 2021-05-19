


#if defined (CONFIG_WAN_AT_P0)
#define WANPORT			0x1			/* 0000001 */
#define LANPORT_RANGE		{1,2,3,4}
#elif defined (CONFIG_WAN_AT_P4)
#define WANPORT			0x10		/* 0010000 */
#define LANPORT_RANGE		{0,1,2,3}
#else
#define WANPORT			0x0			/* no wan port */
#define LANPORT_RANGE		{0,1,2,3,4}
#endif

#define OTHER_INTERFACE				7		/* port 7  (wifi)  */

/* delay mac table deletion */
#define DELETED			1
#define ZEROED			2

#define ADDENTRY		1
#define DELENTRY		2

struct group_member
{
	unsigned long 		ip_addr;
	unsigned char		a0;		// class D, the 1st byte.
	char 				port_num;
	char				has_report;	// 0 or 1
	struct group_member *next;
};

struct group
{
	unsigned char	a1;			// class D, the 2nd byte
	unsigned char	a2;			// class D, the 3rd byte
	unsigned char	a3;			// class D, the 4th byte
	unsigned char	port_map;	// port map
	struct group_member	*members;
	struct group	*next;
};


#define READMODE        0x0
#define WRITEMODE       0x1
#define WRITE_DELAY     100                     /* ms */
unsigned int rareg(int mode, unsigned int addr, long long int new_value);

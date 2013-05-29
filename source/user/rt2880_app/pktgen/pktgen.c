#include <stdio.h>
#include <string.h>
#include <errno.h>  
#include <sys/socket.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <sys/ioctl.h>
#include <getopt.h>

#define TARGET_IFNAME "br0"
unsigned char dest_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char src_mac[6] = {0x00, 0x0c, 0x43, 0x11, 0x22, 0x33};
unsigned char prot[2] = {0x88, 0xe1};
unsigned char mmv = 1;
unsigned short mmtype = 0xa000;
unsigned short fmi = 0;
unsigned short length = 60 - 14 - 5;
unsigned char pkt[1514];
unsigned char buffer[2048];

struct __attribute__ ((packed)) plc_vsm
{
	unsigned char		mmv;
	unsigned short		mmtype;
	unsigned short		fmi;
	unsigned short		len;
};

static int sock = -1;


void usage()
{
    fprintf(stderr, "usage: pktgen -d <destination MAC> -s <source MAC>\n");
    fprintf(stderr, "              -p <protocol> -m <MMTYPE> -f <fmi>\n");
    fprintf(stderr, "              -v <mmv> -l <length>\n");
    fprintf(stderr, "              -c <content>\n");
    fprintf(stderr, "MAC address format, 6-bytes hex value seprated by ':', example ==>  00:0c:43:22:33:44\n");
    fprintf(stderr, "protocol/MMTYPE/fmi is a 2-bytes hex value, example ==>  88e1\n");
    fprintf(stderr, "mmv / length is a decimal value, example ==>  1\n");
    fprintf(stderr, "content is hex values(2 bytes alighnment), example ==>  14a587c92301aa42\n");
}

int atohex(unsigned char *string, unsigned char *val)
{
	unsigned char *p = string, d;
	int i;
	
	
	if ((*p <= 0x39) && (*p >= 0x30))
		d = *p - 0x30;
	else if ((*p <= 0x46) && (*p >= 0x41))
		d = *p - 0x37;
	else if ((*p <= 0x66) && (*p >= 0x61))
		d = *p - 0x57;
	else
		return -1;
	*val = d * 0x10;
	if ((*(p+1) <= 0x39) && (*(p+1) >= 0x30))
		d = *(p+1) - 0x30;
	else if ((*(p+1) <= 0x46) && (*(p+1) >= 0x41))
		d = *(p+1) - 0x37;
	else if ((*(p+1) <= 0x66) && (*(p+1) >= 0x61))
		d = *(p+1) - 0x57;
	else
		return -1;
	*val += d;
	return 0;
}



int main(int argc, char *argv[])
{
	int ret, fd, i, count;
	int opt;
	char options[] = "d:s:p:m:v:f:l:c:?";
	struct sockaddr_ll sa;
	struct ifreq ifr;
        unsigned char buf[2000], *p;
        struct plc_vsm *pvsm;
        fd_set readfds;
        struct timeval tv;
        int force_src_mac = 0;
	
	if (argc < 3)
	{
		usage();
		return 0;
	}
	
	while ((opt = getopt (argc, argv, options)) != -1) 
	{
		p = optarg;
		if (!p)
			return -1;
		switch (opt) 
		{
		case 'd':
			{
				unsigned char d;
		
				for (i = 0; i < 6; i++)
				{
					if (atohex(p, &dest_mac[i]) != 0)
					{
						printf("error dest mac\n");
						return -1;
					}
					//printf("dest_mac[%d] = %x\n", i, dest_mac[i]);
					p += 3;
				}
			}
			break;
		case 's':
			{
				unsigned char d;
				
				for (i = 0; i < 6; i++)
				{

					if (atohex(p, &src_mac[i]) != 0)
					{
						printf("error src mac\n");
						return -1;
					}

					//printf("src_mac[%d] = %x\n", i, src_mac[i]);
					p += 3;
				}
				force_src_mac = 1;
			}
			break;
		case 'p':
			if (atohex(p, &prot[0]) != 0)
			{
				printf("error protocol\n");
				return -1;
			}
			p += 2;
			if (atohex(p, &prot[1]) != 0)
			{
				printf("error protocol\n");
				return -1;
			}
			break;
		case 'm':
			{
				unsigned char m;
				if (atohex(p, &m) != 0)
				{
					printf("error mmtype\n");
					return -1;
				}
				mmtype = m << 8;
				p += 2;
				if (atohex(p, &m) != 0)
				{
					printf("error mmtype\n");
					return -1;
				}
				mmtype |= m;
				//printf("mmtype = %x\n", mmtype);
			}
			break;
		case 'v':
				mmv = atoi(p);
			break;
		case 'l':
			{
				int l = atoi(p);
				if (length < l)
					length = l;
			}
			break;
		case 'f':
			{
				unsigned char f;
				if (atohex(p, &f) != 0)
				{
					printf("error fmi\n");
					return -1;
				}
				fmi = f << 8;
				p += 2;
				if (atohex(p, &f) != 0)
				{
					printf("error mmtype\n");
					return -1;
				}
				fmi |= f;
				printf("fmi = %x\n", fmi);
			}
			break;
		case 'c':
			{
				int x = strlen(optarg);

				for (i = 0; i < (x >> 1); i++)
				{
					if (atohex(p, &pkt[21+i]) != 0)
					{
						printf("content error\n");
						return -1;
					}
					p += 2;
				}
				
				if (x & 1)
					printf("the last bytes is not sent, please enter 2 bytes alighnment content\n");
				
			}
			
			break;
		case '?':
		default:
			usage();
			break;

		}
	}
	
	memcpy(&pkt[0], dest_mac, 6);
	memcpy(&pkt[6], src_mac, 6);
	memcpy(&pkt[12], prot, 2);
	
	
	pvsm = (struct plc_vsm *) &pkt[14];
	pvsm->mmtype = mmtype;
	pvsm->mmv = mmv;
	pvsm->fmi = fmi;
	pvsm->len = length;
	
	fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd == -1) {
		printf("Can not allocate fd\n");
		return -1;
	}
	
	memset(&ifr, 0, sizeof(ifr));
	strcpy (ifr.ifr_name, TARGET_IFNAME);
	
	if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1) {
		printf("%s not exist\n", TARGET_IFNAME);
		close(fd);
		return -1;
	}

	if (!force_src_mac) {
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
		{
			memcpy(&pkt[6], ifr.ifr_hwaddr.sa_data, 6);
		}
	}



	
	ioctl(fd, SIOCGIFFLAGS, &ifr);
	if ( (ifr.ifr_flags & IFF_UP) == 0) {
		printf("%s is down\n", TARGET_IFNAME);
		close(fd);
		return -1;
	}
	
	ioctl(fd, SIOCGIFINDEX, &ifr);

	memset(&sa, 0, sizeof (sa));
	sa.sll_family    = AF_PACKET;
	sa.sll_ifindex   = ifr.ifr_ifindex;
	sa.sll_protocol  = htons(ETH_P_ALL);

	ret = sendto(fd, pkt, length+14+5, 0, (struct sockaddr *)&sa, sizeof (sa));
	close(fd);

	return 0;
}



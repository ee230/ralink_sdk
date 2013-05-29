/*
 * RT2880/RT3052 read/write register utility.
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <sys/mman.h> 
#include <fcntl.h> 
#include <errno.h>

#define PAGE_SIZE		0x1000 	/* 4096 */

#define READMODE	0x0
#define WRITEMODE	0x1
#define WRITE_DELAY	100			/* ms */

unsigned int rareg(int mode, unsigned int addr, long long int new_value)
{
	int fd; 
	unsigned int round;
	void *start;
	volatile unsigned int *v_addr;
	unsigned int rc;

	fd = open("/dev/mem", O_RDWR | O_SYNC );
	if ( fd < 0 ) { 
		printf("open file /dev/mem error. %s\n", strerror(errno)); 
		exit(-1);
	} 

	// round addr to PAGE_SIZE
	round = addr;								// keep old value
	addr = (addr / PAGE_SIZE) * PAGE_SIZE;
	round = round - addr;

	start = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
	if(	(int)start == -1 ){
		printf("mmap() failed at phsical address:%d %s\n", addr, strerror(errno)); 
		close(fd);
		exit(-1);
	}
	//printf("mmap() starts at 0x%08x successfuly\n", (unsigned int) start);

	v_addr = (void *)start + round;
	addr = addr + round;
	printf("0x%08x: 0x%08x\n", addr, *v_addr);

	if(mode == WRITEMODE){
		*v_addr = new_value;
		usleep(WRITE_DELAY * 1000);
		printf("0x%08x: 0x%08x\n", addr, *v_addr);
	}

	rc = *v_addr;
	munmap(start, PAGE_SIZE);
	close(fd);
	return rc;
}


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>

#include "nvram.h"

#if defined (CONFIG_RALINK_GPIO) || defined (CONFIG_RALINK_GPIO_MODULE)
#include "ralink_gpio.h"
#define GPIO_DEV "/dev/gpio"
#endif


#ifdef CONFIG_RT2880_L2_MANAGE
int ramad_start(void);
#endif
static char *saved_pidfile;

void loadDefault(int chip_id)
{
    switch(chip_id)
    {
    case 2860:
	system("ralink_init clear 2860");
#if defined CONFIG_LAN_WAN_SUPPORT || defined CONFIG_MAC_TO_MAC_MODE 
	system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_vlan");
#elif defined(CONFIG_ICPLUS_PHY)
	system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_oneport");
#else
	system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_novlan");
#endif
	break;
    case 2880:
	system("ralink_init clear rtdev");
#if defined (CONFIG_RTDEV)
	system("ralink_init renew rtdev /etc_ro/Wireless/iNIC/RT2860AP.dat");
#elif defined (CONFIG_RTDEV_PLC)
	system("ralink_init renew rtdev /etc_ro/PLC/plc_default.dat");
#endif
	break;
    case 2561: 
	system("ralink_init clear rtdev");
	system("ralink_init renew rtdev /etc_ro/Wireless/RT61AP/RT2561_default");
	break;
    default:
	printf("%s:Wrong chip id\n",__FUNCTION__);
	break;
    }
}

/*
 * gpio interrupt handler -
 *   SIGUSR1 - notify goAhead to start WPS (by sending SIGUSR1)
 *   SIGUSR2 - restore default value
 */
static void nvramIrqHandler(int signum)
{
	if (signum == SIGUSR1) {
#ifdef CONFIG_RALINK_RT2880
		int gopid;
		FILE *fp = fopen("/var/run/goahead.pid", "r");

		if (NULL == fp) {
			printf("nvram: goAhead is not running\n");
			return;
		}
		fscanf(fp, "%d", &gopid);
		if (gopid < 2) {
			printf("nvram: goAhead pid(%d) <= 1\n", gopid);
			return;
		}

		//send SIGUSR1 signal to goAhead for WPSPBCStart();
		printf("notify goahead to start WPS PBC..\n");
		kill(gopid, SIGUSR1);
		fclose(fp);
#else
		//RT2883, RT3052, RT3883 use a different gpio number for WPS,
		//which will be registered in goahead instead
#endif
	} else if (signum == SIGUSR2) {
		printf("load default and reboot..\n");
		loadDefault(2860);
#if defined (CONFIG_RTDEV) || \
    defined (CONFIG_RTDEV_PLC)
		loadDefault(2880);
#elif defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		loadDefault(2561);
#endif
		system("reboot");
	}
}

/*
 * init gpio interrupt -
 *   1. config gpio interrupt mode
 *   2. register my pid and request gpio pin 0
 *   3. issue a handler to handle SIGUSR1 and SIGUSR2
 */
int initGpio(void)
{
#if !defined (CONFIG_RALINK_GPIO) && !defined (CONFIG_RALINK_GPIO_MODULE)
	signal(SIGUSR1, nvramIrqHandler);
	signal(SIGUSR2, nvramIrqHandler);
	return 0;
#else
	int fd;
	ralink_gpio_reg_info info;

	info.pid = getpid();
#if defined (CONFIG_RALINK_RT2880)
	info.irq = 0;
#elif defined (CONFIG_RALINK_RT3052) && ((CONFIG_RALINK_I2S) || defined (CONFIG_RALINK_I2S_MODULE))
	info.irq = 43;
#elif defined (CONFIG_RALINK_RT3883)
	//RT3883 uses gpio 27 for load-to-default
	info.irq = 27;
#elif defined (CONFIG_RALINK_RT6855A)
#if defined (CONFIG_RT6855A_PCIE_PORT0_ENABLE)
	info.irq = 0;	// rt6855 reset default
#else
	info.irq = 2;	// rt6856 reset default
#endif
#else
	//RT2883, RT3052, RT3352, RT6855 use gpio 10 for load-to-default
	info.irq = 10;
#endif	

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	//set gpio direction to input
#if !defined (CONFIG_RALINK_RT6855A)
	if (info.irq < 24) {
		if (ioctl(fd, RALINK_GPIO_SET_DIR_IN, (1<<info.irq)) < 0)
			goto ioctl_err;
	}
#if defined (RALINK_GPIO_HAS_2722)
	else if (22 <= info.irq) {
		if (ioctl(fd, RALINK_GPIO2722_SET_DIR_IN, (1<<(info.irq-22))) < 0)
			goto ioctl_err;
	}
#elif defined (RALINK_GPIO_HAS_4524)
	else if (24 <= info.irq && info.irq < 40) {
		if (ioctl(fd, RALINK_GPIO3924_SET_DIR_IN, (1<<(info.irq-24))) < 0)
			goto ioctl_err;
	}
	else {
		if (ioctl(fd, RALINK_GPIO4540_SET_DIR_IN, (1<<(info.irq-40))) < 0)
			goto ioctl_err;
	}
#elif defined (RALINK_GPIO_HAS_5124)
	else if (24 <= info.irq && info.irq < 40) {
		if (ioctl(fd, RALINK_GPIO3924_SET_DIR_IN, (1<<(info.irq-24))) < 0)
			goto ioctl_err;
	}
	else {
		if (ioctl(fd, RALINK_GPIO5140_SET_DIR_IN, (1<<(info.irq-40))) < 0)
			goto ioctl_err;
	}
#elif defined (RALINK_GPIO_HAS_9524)
	else if (24 <= info.irq && info.irq < 40) {
		if (ioctl(fd, RALINK_GPIO3924_SET_DIR_IN, (1<<(info.irq-24))) < 0)
			goto ioctl_err;
	}
	else if (40 <= info.irq && info.irq < 72) {
		if (ioctl(fd, RALINK_GPIO7140_SET_DIR_IN, (1<<(info.irq-40))) < 0)
			goto ioctl_err;
	}
	else {
		if (ioctl(fd, RALINK_GPIO9572_SET_DIR_IN, (1<<(info.irq-72))) < 0)
			goto ioctl_err;
	}
#endif
#else
	if (info.irq < 16) {
		if (ioctl(fd, RALINK_GPIO_SET_DIR_IN, info.irq) < 0)
			goto ioctl_err;
	} else {
		goto ioctl_err;
	}
#endif
	//enable gpio interrupt
	if (ioctl(fd, RALINK_GPIO_ENABLE_INTP) < 0)
		goto ioctl_err;

	//register my information
	if (ioctl(fd, RALINK_GPIO_REG_IRQ, &info) < 0)
		goto ioctl_err;
	close(fd);

	//issue a handler to handle SIGUSR1 and SIGUSR2
	signal(SIGUSR1, nvramIrqHandler);
	signal(SIGUSR2, nvramIrqHandler);
	return 0;

ioctl_err:
	perror("ioctl");
	close(fd);
	return -1;
#endif
}

static void pidfile_delete(void)
{
	if (saved_pidfile) unlink(saved_pidfile);
}

int pidfile_acquire(const char *pidfile)
{
	int pid_fd;
	if (!pidfile) return -1;

	pid_fd = open(pidfile, O_CREAT | O_WRONLY, 0644);
	if (pid_fd < 0) {
		printf("Unable to open pidfile %s: %m\n", pidfile);
	} else {
		lockf(pid_fd, F_LOCK, 0);
		if (!saved_pidfile)
			atexit(pidfile_delete);
		saved_pidfile = (char *) pidfile;
	}
	return pid_fd;
}

void pidfile_write_release(int pid_fd)
{
	FILE *out;

	if (pid_fd < 0) return;

	if ((out = fdopen(pid_fd, "w")) != NULL) {
		fprintf(out, "%d\n", getpid());
		fclose(out);
	}
	lockf(pid_fd, F_UNLCK, 0);
	close(pid_fd);
}

int main(int argc,char **argv)
{
	pid_t pid;
	int fd;

	if (strcmp(nvram_bufget(RT2860_NVRAM, "WebInit"),"1")) {
		loadDefault(2860);
	}
	
	if (strcmp(nvram_bufget(RTDEV_NVRAM, "WebInit"),"1")) {
#if defined (CONFIG_RTDEV) || \
    defined (CONFIG_RTDEV_PLC)
		loadDefault(2880);
#elif defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		loadDefault(2561);
#endif
	}
	nvram_close(RT2860_NVRAM);
#if defined (CONFIG_RTDEV) || \
	defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	nvram_close(RTDEV_NVRAM);
#endif

	if (initGpio() != 0)
		exit(EXIT_FAILURE);

	fd = pidfile_acquire("/var/run/nvramd.pid");
	pidfile_write_release(fd);

#ifdef CONFIG_RT2880_L2_MANAGE
	//start the management daemon (blocking)
	ramad_start();
#else
	while (1) {
		pause();
	}
#endif

	exit(EXIT_SUCCESS);
}


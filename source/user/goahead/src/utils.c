/* vi: set sw=4 ts=4 sts=4: */
/*
 *	utils.c -- System Utilities 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: utils.c,v 1.125 2012-02-03 03:37:08 chhung Exp $
 */
#include	<time.h>
#include	<signal.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>

#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>

#include	"nvram.h"
#include	"ralink_gpio.h"
#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config

#include	"webs.h"
#include	"internet.h"
#include	"utils.h"
#include	"wireless.h"

#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
extern void setFirmwarePath(void);
#endif

static int  getModIns(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgToHTML(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfg2General(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfg2NthGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfg2Zero(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfg2NthZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfg3General(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfg3Zero(int eid, webs_t wp, int argc, char_t **argv);
static int  getDpbSta(int eid, webs_t wp, int argc, char_t **argv);
static int  getLangBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getMiiInicBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getPlatform(int eid, webs_t wp, int argc, char_t **argv);
static int  getStationBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getSysBuildTime(int eid, webs_t wp, int argc, char_t **argv);
static int  getSdkVersion(int eid, webs_t wp, int argc, char_t **argv);
static int  getSysUptime(int eid, webs_t wp, int argc, char_t **argv);
static int  getPortStatus(int eid, webs_t wp, int argc, char_t **argv);
static int  isOnePortOnly(int eid, webs_t wp, int argc, char_t **argv);
static void forceMemUpgrade(webs_t wp, char_t *path, char_t *query);
static void setOpMode(webs_t wp, char_t *path, char_t *query);
#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
static void ScanUSBFirmware(webs_t wp, char_t *path, char_t *query);
#endif
static int  getHWNATBuilt(int eid, webs_t wp, int argc, char_t **argv);

/*********************************************************************
 * System Utilities
 */

void arplookup(char *ip, char *arp)
{
    char buf[256];
    FILE *fp = fopen("/proc/net/arp", "r");
    if(!fp){
        trace(0, T("no proc fs mounted!\n"));
        return;
    }
    strcpy(arp, "00:00:00:00:00:00");
    while(fgets(buf, 256, fp)){
        char ip_entry[32], hw_type[8],flags[8], hw_address[32];
        sscanf(buf, "%s %s %s %s", ip_entry, hw_type, flags, hw_address);
        if(!strcmp(ip, ip_entry)){
            strcpy(arp, hw_address);
            break;
        }
    }

    fclose(fp);
    return;
}


/*
 * description: kill process whose pid was recorded in file
 *              (va is supported)
 */
int doKillPid(char_t *fmt, ...)
{
	va_list		vargs;
	char_t		*pid_fname = NULL;
	struct stat	st;

	va_start(vargs, fmt);
	if (fmtValloc(&pid_fname, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
		trace(0, T("doKillPid: lost data, buffer overflow\n"));
	}
	va_end(vargs);

	if (pid_fname) {
		if (-1 == stat(pid_fname, &st)) //check the file existence
			return 0;
		doSystem("kill `cat %s`", pid_fname);
		doSystem("rm -f %s", pid_fname);
		bfree(B_L, pid_fname);
	}
	return 0;
}

/*
 * description: parse va and do system
 */
int doSystem(char_t *fmt, ...)
{
	va_list		vargs;
	char_t		*cmd = NULL;
	int			rc = 0;
	
	va_start(vargs, fmt);
	if (fmtValloc(&cmd, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
		trace(0, T("doSystem: lost data, buffer overflow\n"));
	}
	va_end(vargs);

	if (cmd) {
		trace(0, T("%s\n"), cmd);
		rc = system(cmd);
		bfree(B_L, cmd);
	}
	return rc;
}

/*
 * arguments: index - index of the Nth value
 *            values - un-parsed values
 * description: parse values delimited by semicolon, and return the value
 *              according to given index (starts from 0)
 * WARNING: the original values will be destroyed by strtok
 */
char *getNthValue(int index, char *values)
{
	int i;
	static char *tok;

	if (NULL == values)
		return NULL;
	for (i = 0, tok = strtok(values, ";");
			(i < index) && tok;
			i++, tok = strtok(NULL, ";")) {
		;
	}
	if (NULL == tok)
		return "";
	return tok;
}

/*
 * arguments: index - index of the Nth value (starts from 0)
 *            old_values - un-parsed values
 *            new_value - new value to be replaced
 * description: parse values delimited by semicolon,
 *              replace the Nth value with new_value,
 *              and return the result
 * WARNING: return the internal static string -> use it carefully
 */
char *setNthValue(int index, char *old_values, char *new_value)
{
	int i;
	char *p, *q;
	static char ret[2048];
	char buf[8][256];

	memset(ret, 0, 2048);
	for (i = 0; i < 8; i++)
		memset(buf[i], 0, 256);

	//copy original values
	for ( i = 0, p = old_values, q = strchr(p, ';')  ;
	      i < 8 && q != NULL                         ;
	      i++, p = q + 1, q = strchr(p, ';')         )
	{
		strncpy(buf[i], p, q - p);
	}
	strcpy(buf[i], p); //the last one

	//replace buf[index] with new_value
	strncpy(buf[index], new_value, 256);

	//calculate maximum index
	index = (i > index)? i : index;

	//concatenate into a single string delimited by semicolons
	strcat(ret, buf[0]);
	for (i = 1; i <= index; i++) {
		strncat(ret, ";", 2);
		strncat(ret, buf[i], 256);
	}

	return ret;
}

/*
 * arguments: values - values delimited by semicolon
 * description: parse values delimited by semicolon, and return the number of
 *              values
 */
int getValueCount(char *values)
{
	int cnt = 0;

	if (NULL == values)
		return 0;
	while (*values++ != '\0') {
		if (*values == ';')
			++cnt;
	}
	return (cnt + 1);
}

/*
 * check the existence of semicolon in str
 */
int checkSemicolon(char *str)
{
	char *c = strchr(str, ';');
	if (c)
		return 1;
	return 0;
}

/*
 * argument: ip address
 * return: 1 = the given ip address is within LAN's scope
 *         0 = otherwise
 */
int isInLan(char *radius_ip_addr)
{
    char lan_if_addr[16];
    char lan_if_netmask[16];

    struct in_addr lan_ip;
    struct in_addr lan_netmask;
    struct in_addr radius_ip;

    if ((getIfIp(getLanIfName(), lan_if_addr)) == -1) {
        printf("getLanIP error\n");
        return 0;
    }
    if ((getIfNetmask(getLanIfName(), lan_if_netmask)) == -1) {
        printf("getLanNetmask error\n");
        return 0;
    }

    inet_aton(lan_if_addr, &lan_ip);
    inet_aton(lan_if_netmask, &lan_netmask);
    inet_aton(radius_ip_addr, &radius_ip);

    printf("lan_ip:%08x\n", lan_ip.s_addr);
    printf("lan_netmask:%08x\n", lan_netmask.s_addr);
    printf("radius_ip:%08x\n", radius_ip.s_addr);

    if ((lan_ip.s_addr & lan_netmask.s_addr) == (radius_ip.s_addr & lan_netmask.s_addr) ){
        printf("in Lan\n");
        return 1;
    } else {
        printf("not in lan\n");
        return 0;
    }
}

/*
 * substitution of getNthValue which dosen't destroy the original value
 */
int getNthValueSafe(int index, char *value, char delimit, char *result, int len)
{
    int i=0, result_len=0;
    char *begin, *end;

    if(!value || !result || !len)
        return -1;

    begin = value;
    end = strchr(begin, delimit);

    while(i<index && end){
        begin = end+1;
        end = strchr(begin, delimit);
        i++;
    }

    //no delimit
    if(!end){
		if(i == index){
			end = begin + strlen(begin);
			result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
		}else
			return -1;
	}else
		result_len = (len-1) < (end-begin)? (len-1) : (end-begin);

	memcpy(result, begin, result_len );
	*(result+ result_len ) = '\0';

	return 0;
}

/*
 *  argument:  [IN]     index -- the index array of deleted items(begin from 0)
 *             [IN]     count -- deleted itmes count.
 *             [IN/OUT] value -- original string/return string
 *             [IN]     delimit -- delimitor
 */
int deleteNthValueMulti(int index[], int count, char *value, char delimit)
{
	char *begin, *end;
	int i=0,j=0;
	int need_check_flag=0;
	char *buf = strdup(value);

	begin = buf;

	end = strchr(begin, delimit);
	while(end){
		if(i == index[j]){
			memset(begin, 0, end - begin );
			if(index[j] == 0)
				need_check_flag = 1;
			j++;
			if(j >=count)
				break;
		}
		begin = end;

		end = strchr(begin+1, delimit);
		i++;
	}

	if(!end && index[j] == i)
		memset(begin, 0, strlen(begin));

	if(need_check_flag){
		for(i=0; i<strlen(value); i++){
			if(buf[i] == '\0')
				continue;
			if(buf[i] == ';')
				buf[i] = '\0';
			break;
		}
	}

	for(i=0, j=0; i<strlen(value); i++){
		if(buf[i] != '\0'){
			value[j++] = buf[i];
		}
	}
	value[j] = '\0';

	free(buf);
	return 0;
}



/*
 * nanosleep(2) don't depend on signal SIGALRM and could cooperate with
 * other SIGALRM-related functions(ex. setitimer(2))
 */
unsigned int Sleep(unsigned int secs)
{
	int rc;
	struct timespec ts, remain;
	ts.tv_sec  = secs;
	ts.tv_nsec = 0;

sleep_again:
	rc = nanosleep(&ts, &remain);
	if(rc == -1 && errno == EINTR){
		ts.tv_sec = remain.tv_sec;
		ts.tv_nsec = remain.tv_nsec;
		goto sleep_again;
	}	
	return 0;
}

/*
 * The setitimer() is Linux-specified.
 */
int setTimer(int microsec, void ((*sigroutine)(int)))
{
	struct itimerval value, ovalue;
   
	signal(SIGALRM, sigroutine);
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = microsec;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = microsec;
	return setitimer(ITIMER_REAL, &value, &ovalue);
}

void stopTimer(void)
{
	struct itimerval value, ovalue;
   
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);
}

/*
 * configure LED blinking with proper frequency (privatly use only)
 *   on: number of ticks that LED is on
 *   off: number of ticks that LED is off
 *   blinks: number of on/off cycles that LED blinks
 *   rests: number of on/off cycles that LED resting
 *   times: stop blinking after <times> times of blinking
 * where 1 tick == 100 ms
 */
static int gpioLedSet(int gpio, unsigned int on, unsigned int off,
		unsigned int blinks, unsigned int rests, unsigned int times)
{
	int fd;
	ralink_gpio_led_info led;

	//parameters range check
	if (gpio < 0 || gpio >= RALINK_GPIO_NUMBER ||
			on > RALINK_GPIO_LED_INFINITY ||
			off > RALINK_GPIO_LED_INFINITY ||
			blinks > RALINK_GPIO_LED_INFINITY ||
			rests > RALINK_GPIO_LED_INFINITY ||
			times > RALINK_GPIO_LED_INFINITY) {
		return -1;
	}
	led.gpio = gpio;
	led.on = on;
	led.off = off;
	led.blinks = blinks;
	led.rests = rests;
	led.times = times;

	fd = open("/dev/gpio", O_RDONLY);
	if (fd < 0) {
		perror("/dev/gpio");
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_LED_SET, &led) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int ledAlways(int gpio, int on)
{
	if (on)
		return gpioLedSet(gpio, RALINK_GPIO_LED_INFINITY, 0, 1, 1, RALINK_GPIO_LED_INFINITY);
	else
		return gpioLedSet(gpio, 0, RALINK_GPIO_LED_INFINITY, 1, 1, RALINK_GPIO_LED_INFINITY);
}

int ledWps(int gpio, int mode)
{
	switch (mode) {
		case WPS_LED_RESET:
			return gpioLedSet(gpio, 0, RALINK_GPIO_LED_INFINITY, 1, 1, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_PROGRESS:
			return gpioLedSet(gpio, 2, 1, RALINK_GPIO_LED_INFINITY, 1, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_ERROR:
			return gpioLedSet(gpio, 1, 1, RALINK_GPIO_LED_INFINITY, 1, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_SESSION_OVERLAP:
			return gpioLedSet(gpio, 1, 1, 10, 5, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_SUCCESS:
			gpioLedSet(gpio, 3000, 1, 1, 1, 1);
			break;
	}
	return 0;
}

/*
 * concatenate a string with an integer
 * ex: racat("SSID", 1) will return "SSID1"
 */
char *racat(char *s, int i)
{
	static char str[32];
	snprintf(str, 32, "%s%1d", s, i);
	return str;
}

void websLongWrite(webs_t wp, char *longstr)
{
    char tmp[513] = {0};
    int len = strlen(longstr);
    char *end = longstr + len;

    while(longstr < end){
        strncpy(tmp, longstr, 512);
        websWrite(wp, T("%s"), tmp);
        longstr += 512;
    }
    return;
}

/*********************************************************************
 * Web Related Utilities
 */

void formDefineUtilities(void)
{
	websAspDefine(T("getModIns"), getModIns);
	websAspDefine(T("getCfgGeneral"), getCfgGeneral);
	websAspDefine(T("getCfgToHTML"), getCfgToHTML);
	websAspDefine(T("getCfgNthGeneral"), getCfgNthGeneral);
	websAspDefine(T("getCfgZero"), getCfgZero);
	websAspDefine(T("getCfgNthZero"), getCfgNthZero);
	websAspDefine(T("getCfg2General"), getCfg2General);
	websAspDefine(T("getCfg2NthGeneral"), getCfg2NthGeneral);
	websAspDefine(T("getCfg2Zero"), getCfg2Zero);
	websAspDefine(T("getCfg2NthZero"), getCfg2NthZero);
	websAspDefine(T("getCfg3General"), getCfg3General);
	websAspDefine(T("getCfg3Zero"), getCfg3Zero);
	websAspDefine(T("getDpbSta"), getDpbSta);
	websAspDefine(T("getLangBuilt"), getLangBuilt);
	websAspDefine(T("getMiiInicBuilt"), getMiiInicBuilt);
	websAspDefine(T("getPlatform"), getPlatform);
	websAspDefine(T("getStationBuilt"), getStationBuilt);
	websAspDefine(T("getSysBuildTime"), getSysBuildTime);
	websAspDefine(T("getSdkVersion"), getSdkVersion);
	websAspDefine(T("getSysUptime"), getSysUptime);
	websAspDefine(T("getPortStatus"), getPortStatus);
	websAspDefine(T("isOnePortOnly"), isOnePortOnly);
	websFormDefine(T("forceMemUpgrade"), forceMemUpgrade);
	websFormDefine(T("setOpMode"), setOpMode);
#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
	websFormDefine(T("ScanUSBFirmware"), ScanUSBFirmware);
#endif
	websAspDefine(T("getHWNATBuilt"), getHWNATBuilt);
}


/* 
 * arguments: type - 0 = return the status of module insertion (default)
 *                   1 = write the status of module insertion
 *            modname - module name
 * description: test the insertion of module through /proc/modules
 * result: -1 = fopen error, 1 = module inserted, 0 = not inserted yet
 */
static int getModIns(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, type, result = 0;
	char_t *modname;
	FILE *fp;
	char buf[128];

	if (ejArgs(argc, argv, T("%d %s"), &type, &modname) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}

	if (NULL == (fp = fopen("/proc/modules", "r"))) {
		//error(E_L, E_LOG, T("getModIns: open /proc/modules error"));
		if (1 == type)
			return websWrite(wp, T("-1"));
		else {
			ejSetResult(eid, "-1");
			return 0;
		}
	}

	while (NULL != fgets(buf, 128, fp)) {
		i = 0;
		while (!isspace(buf[i++]))
			;
		buf[i - 1] = '\0';
		if (!strcmp(buf, modname)) {
			result = 1;
			break;
		}
	}
	fclose(fp);

	if (1 == type)
		return websWrite(wp, T("%d"), result);
	ejSetResult(eid, result);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), value);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

static int getCfgToHTML(int eid, webs_t wp, int argc, char_t **argv)
{
	int type; 
	unsigned int offset, len;
	char_t *field;
	char *value, *head, *tok;
	char ret[32+1];

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RT2860_NVRAM, field);
	memset(ret, 0, 33);
	tok = value;
	head = tok;
	do {
		if (*tok == 34) {
			offset = (unsigned int) head - (unsigned int) value;
			len = (unsigned int) tok - (unsigned int) head;
			strncat(ret, value+offset, len);
			strcat(ret, "&#34;");
			head = tok+1;
		} 
		else if (*tok == 39) {
			offset = head - (unsigned int) value;
			len = (unsigned int) tok - (unsigned int) head;
			strncat(ret, value+offset, len);
			strcat(ret, "&#39;");
			head = tok+1;
		}
		tok++;
	} while (*tok != '\0');
	offset = (unsigned int) head - (unsigned int) value;
	strcat(ret, value+offset);
	if (1 == type) {
		if (NULL == ret)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), ret);
	}
	if (NULL == ret)
		ejSetResult(eid, "");
	ejSetResult(eid, ret);
	return 0;
}
/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char nth[128];

	memset(nth, 0, 128);
	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		getNthValueSafe(idx, value, ';', nth, 128);
		if (NULL == nth)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), nth);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	getNthValueSafe(idx, value, ';', nth, 128);
	if (NULL == nth)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfgZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "0")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char nth[128];

	memset(nth, 0, 128);
	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		getNthValueSafe(idx, value, ';', nth, 128);
		if (NULL == nth)
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), nth);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	getNthValueSafe(idx, value, ';', nth, 128);
	if (NULL == nth)
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
#if defined (RTDEV_SUPPORT)
static int getCfg2General(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RTDEV_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), value);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfg2NthGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char nth[128];

	memset(nth, 0, 128);
	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RTDEV_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		getNthValueSafe(idx, value, ';', nth, 128);
		if (NULL == nth)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), nth);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	getNthValueSafe(idx, value, ';', nth, 128);
	if (NULL == nth)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfg2Zero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RTDEV_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "0")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfg2NthZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char nth[128];

	memset(nth, 0, 128);
	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RTDEV_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		getNthValueSafe(idx, value, ';', nth, 128);
		if (NULL == nth)
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), nth);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	getNthValueSafe(idx, value, ';', nth, 128);
	if (NULL == nth)
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}
#else
static int  getCfg2General(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}

static int  getCfg2NthGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}

static int  getCfg2Zero(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}

static int  getCfg2NthZero(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}
#endif

#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfg3General(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;
	
	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_bufget(RTDEV_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), value);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfg3Zero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args"));
	}
	value = (char *) nvram_bufget(RTDEV_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}
#else
static int  getCfg3General(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}
static int  getCfg3Zero(int eid, webs_t wp, int argc, char_t **argv)
{
	return 0;
}
#endif

static int getDpbSta(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_RT2860V2_STA_DPB) || defined (CONFIG_RT2860V2_STA_ETH_CONVERT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLangBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *lang;

	if (ejArgs(argc, argv, T("%s"), &lang) < 1) {
		return websWrite(wp, T("0"));
	}
	if (!strncmp(lang, "en", 3))
#ifdef CONFIG_USER_GOAHEAD_LANG_EN
		return websWrite(wp, T("1"));
#else
		return websWrite(wp, T("0"));
#endif
	else if (!strncmp(lang, "zhtw", 5))
#ifdef CONFIG_USER_GOAHEAD_LANG_ZHTW
		return websWrite(wp, T("1"));
#else
		return websWrite(wp, T("0"));
#endif
	else if (!strncmp(lang, "zhcn", 5))
#ifdef CONFIG_USER_GOAHEAD_LANG_ZHCN
		return websWrite(wp, T("1"));
#else
		return websWrite(wp, T("0"));
#endif

	return websWrite(wp, T("0"));
}

static int getMiiInicBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_RTDEV_MII)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write the current system platform accordingly
 */
static int getPlatform(int eid, webs_t wp, int argc, char_t **argv)
{
	char platform[8];
#if defined CONFIG_RALINK_RT2883
	strcpy(platform, "RT2883");
#elif defined CONFIG_RALINK_RT3052
	strcpy(platform, "RT3052");
#elif defined CONFIG_RALINK_RT3883
	strcpy(platform, "RT3883");
#elif defined CONFIG_RALINK_RT3352
	strcpy(platform, "RT3352");
#elif defined CONFIG_RALINK_RT5350
	strcpy(platform, "RT5350");
#elif defined CONFIG_RALINK_RT6855
	strcpy(platform, "RT6855");
#elif defined CONFIG_RALINK_RT6352
	strcpy(platform, "RT6352");
#elif defined CONFIG_RALINK_RT71100
	strcpy(platform, "RT71100");
#else
	strcpy(platform, "RT2880");
#endif

#ifdef CONFIG_RAETH_ROUTER
	return websWrite(wp, T("%s with IC+ MACPHY"), platform);
#endif
#ifdef CONFIG_ICPLUS_PHY
    return websWrite(wp, T("%s with IC+ PHY"), platform);
#endif
#ifdef CONFIG_RT_MARVELL
	return websWrite(wp, T("%s with MARVELL"), platform);
#endif
#ifdef CONFIG_MAC_TO_MAC_MODE
	return websWrite(wp, T("%s with Vitesse"), platform);
#endif
#ifdef CONFIG_RT_3052_ESW
	return websWrite(wp, T("%s embedded switch"), platform);
#endif
#if defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
	return websWrite(wp, T("%s with Two Giga PHY"), platform);
#endif
    
	return 0;
}

static int getHWNATBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RA_NAT_HW
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getStationBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_STA || defined CONFIG_RT2860V2_STA_MODULE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write System build time
 */
static int getSysBuildTime(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"), __DATE__);
}

/*
 * description: write RT288x SDK version
 */
static int getSdkVersion(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = fopen("/etc_ro/web/cgi-bin/History", "r");
	char version[16] = "";

	if (fp != NULL)
	{
		char buf[30];
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strncasecmp(buf, "Version", 7) != 0)
				continue;
			sscanf(buf, "%*s%s", version);
			break;
		}
		fclose(fp);
	}

	return websWrite(wp, T("%s"), version);
}

/*
 * description: write System Uptime
 */
static int getSysUptime(int eid, webs_t wp, int argc, char_t **argv)
{
	struct tm *utime;
	time_t usecs;

	time(&usecs);
	utime = localtime(&usecs);

	if (utime->tm_hour > 0)
		return websWrite(wp, T("%d hour%s, %d min%s, %d sec%s"),
				utime->tm_hour, (utime->tm_hour == 1)? "" : "s",
				utime->tm_min, (utime->tm_min == 1)? "" : "s",
				utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	else if (utime->tm_min > 0)
		return websWrite(wp, T("%d min%s, %d sec%s"),
				utime->tm_min, (utime->tm_min == 1)? "" : "s",
				utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	else
		return websWrite(wp, T("%d sec%s"),
				utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	return 0;
}

static int getPortStatus(int eid, webs_t wp, int argc, char_t **argv)
{
#if (defined (CONFIG_RAETH_ROUTER) || defined CONFIG_RT_3052_ESW) && defined (CONFIG_USER_ETHTOOL)
	int port, rc;
	FILE *fp;
	char buf[1024];

	for(port=0; port<5; port++){
		char *pos;
		char link = '0';
		int speed = 100;
		char duplex = 'F';
		FILE *proc_file = fopen("/proc/rt2880/gmac", "w");
		if(!proc_file){
			websWrite(wp, T("-1"));		// indicate error
			return 0;
		}
		fprintf(proc_file, "%d", port);
		fclose(proc_file);

		if((fp = popen("ethtool eth2", "r")) == NULL){
			websWrite(wp, T("-1"));		// indicate error
			return 0;
		}
		rc = fread(buf, 1, 1024, fp);
		pclose(fp);
		if(rc == -1){
			websWrite(wp, T("-1"));		// indicate error
			return 0;
		}else{
			// get Link status
			if((pos = strstr(buf, "Link detected: ")) != NULL){
				pos += strlen("Link detected: ");
				if(*pos == 'y')
					link = '1';
			}
			// get speed
			if((pos = strstr(buf, "Speed: ")) != NULL){
				pos += strlen("Speed: ");
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == 'M')
					speed = 10;
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == '0' && *(pos+3) == '0' && *(pos+4) == 'M')
					speed = 1000;
			}
			// get duplex
			if((pos = strstr(buf, "Duplex: ")) != NULL){
				pos += strlen("Duplex: ");
				if(*pos == 'H')
					duplex = 'H';
			}

			websWrite(wp, T("%c,%d,%c,"), link, speed, duplex);
		}
	}
	return 0;
#else
	websWrite(wp, T("-1"));
	return 0;
#endif

}

inline int getOnePortOnly(void)
{
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
	return 0;
#elif defined CONFIG_ICPLUS_PHY
	return 1;
#else
	return 0;
#endif
	return 0;
}

static int isOnePortOnly(int eid, webs_t wp, int argc, char_t **argv)
{
	if( getOnePortOnly() == 1)
		websWrite(wp, T("true"));
	else
		websWrite(wp, T("false"));		 
	return 0;
}

void redirect_wholepage(webs_t wp, const char *url)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("<html><head><script language=\"JavaScript\">"));
	websWrite(wp, T("parent.location.replace(\"%s\");"), url);
	websWrite(wp, T("</script></head></html>"));
}

int netmask_aton(const char *ip)
{
	int i, a[4], result = 0;
	sscanf(ip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	for(i=0; i<4; i++){	//this is dirty
		if(a[i] == 255){
			result += 8;
			continue;
		}
		if(a[i] == 254)
			result += 7;
		if(a[i] == 252)
			result += 6;
		if(a[i] == 248)
			result += 5;
		if(a[i] == 240)
			result += 4;
		if(a[i] == 224)
			result += 3;
		if(a[i] == 192)
			result += 2;
		if(a[i] == 128)
			result += 1;
		//if(a[i] == 0)
		//	result += 0;
		break;
	}
	return result;
}
static void forceMemUpgrade(webs_t wp, char_t *path, char_t *query)
{
	char_t *mode  = websGetVar(wp, T("ForceMemUpgradeSelect"), T("0"));
	if(!mode)
		return;
	if(!strcmp(mode, "1"))
		nvram_bufset(RT2860_NVRAM, "Force_mem_upgrade", "1");
	else
		nvram_bufset(RT2860_NVRAM, "Force_mem_upgrade", "0");
	nvram_commit(RT2860_NVRAM);
    websHeader(wp);
    websWrite(wp, T("<h2>force mem upgrade</h2>\n"));
    websWrite(wp, T("mode: %s<br>\n"), mode);
    websFooter(wp);
    websDone(wp, 200);	
}

#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
static void ScanUSBFirmware(webs_t wp, char_t *path, char_t *query)
{
	setFirmwarePath();
	printf("%s enter\n", __FUNCTION__);

	websRedirect(wp, "adm/upload_firmware.asp");
}
#endif

/* goform/setOpMode */
static void setOpMode(webs_t wp, char_t *path, char_t *query)
{
	char_t	*mode, *nat_en, *tcp_timeout, *udp_timeout;
	const char	*old_mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*old_nat = nvram_bufget(RT2860_NVRAM, "natEnabled");
	int		need_commit = 0;
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
	char	*wan_ip, *lan_ip;
#endif
#if defined (CONFIG_RT2860V2_STA_DPB) || defined (CONFIG_RT2860V2_STA_ETH_CONVERT)
	char_t	*econv = "", *econv_mode, *econv_mac;
#endif
#if defined (RT2860_APCLI_SUPPORT)
	char_t	*apcli = "";
#endif
#if defined (CONFIG_RTDEV_MII)
	char_t	*mii;
#endif

	mode = websGetVar(wp, T("opMode"), T("0")); 
	nat_en = websGetVar(wp, T("natEnbl"), T("0"));
	tcp_timeout = websGetVar(wp, T("tcp_timeout"), T(""));
	udp_timeout = websGetVar(wp, T("udp_timeout"), T(""));
#ifdef CONFIG_RA_NAT_HW
	char_t	*hwnat_en = websGetVar(wp, T("hwnatEnbl"), T("0"));;
	const char	*old_hwnat = nvram_bufget(RT2860_NVRAM, "hwnatEnabled");
#endif
	if (strcmp(tcp_timeout, nvram_get(RT2860_NVRAM, "TcpTimeout")) || 
		strcmp(udp_timeout, nvram_get(RT2860_NVRAM, "UdpTimeout")))
	{
		nvram_set(RT2860_NVRAM, "TcpTimeout", tcp_timeout); 
		nvram_set(RT2860_NVRAM, "UdpTimeout", udp_timeout);
		need_commit = 1;
	}

	if (!strncmp(old_mode, "0", 2)) {
	}
	else if (!strncmp(old_mode, "1", 2) || !strncmp(old_mode, "3", 2)) {
		if (!strncmp(mode, "0", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: gateway (or ap-client) -> bridge
			 * config: wan_ip(wired) overwrites lan_ip(bridge)
			 */
			wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", wan_ip);
			need_commit = 1;
#endif
		}
		if (!strncmp(mode, "2", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: gateway (or ap-client) -> ethernet-converter
			 * config: wan_ip(wired) overwrites lan_ip(wired) 
			 *         lan_ip(wireless) overwrites wan_ip(wireless)
			 */
			wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
			lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", wan_ip);
			nvram_bufset(RT2860_NVRAM, "wan_ipaddr", lan_ip);
			need_commit = 1;
#endif
		}
	}
	else if (!strncmp(old_mode, "2", 2)) {
		if (!strncmp(mode, "0", 2)) {
			/*
			 * mode: wireless-isp -> bridge
			 * config: lan_ip(wired) overwrites lan_ip(bridge) -> the same
			 */
		}
		else if (!strncmp(mode, "1", 2) || !strncmp(mode, "3", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: ethernet-converter -> gateway (or ap-client)
			 * config: lan_ip(wired) overwrites wan_ip(wired) 
			 *         wan_ip(wireless) overwrites lan_ip(wireless)
			 */
			wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
			lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", wan_ip);
			nvram_bufset(RT2860_NVRAM, "wan_ipaddr", lan_ip);
			need_commit = 1;
#endif
		}
	}

#if defined (CONFIG_RT2860V2_STA_DPB) || defined (CONFIG_RT2860V2_STA_ETH_CONVERT)
	if (!strncmp(mode, "0", 2)) {
		const char *old;

		econv = websGetVar(wp, T("ethConv"), T("0"));
		econv_mode = websGetVar(wp, T("oEthConvMode"), T("0"));
		econv_mac = websGetVar(wp, T("oEthConvMac"), T(""));
		old = nvram_bufget(RT2860_NVRAM, "ethConvert");
		if (strncmp(old, econv, 2)) {
			nvram_bufset(RT2860_NVRAM, "ethConvert", econv);
			need_commit = 1;
		}
		if (!strcmp(econv_mode, "0"))
			nvram_bufset(RT2860_NVRAM, "ethConvertMAC", "00:00:00:00:00:00");
		else if (!strcmp(econv_mode, "1"))
			nvram_bufset(RT2860_NVRAM, "ethConvertMAC", "FF:FF:FF:FF:FF:FF");
		else
			nvram_bufset(RT2860_NVRAM, "ethConvertMAC", econv_mac);
		if (!strncmp(econv, "1", 2)) {
			//disable dhcp server in this mode
			old = nvram_bufget(RT2860_NVRAM, "dhcpEnabled");
			if (!strncmp(old, "1", 2)) {
				nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
				need_commit = 1;
			}
		}
	}
#endif
#if defined (RT2860_APCLI_SUPPORT)
	if (!strncmp(mode, "0", 2)) {
		apcli = websGetVar(wp, T("apcliEnbl"), T("0"));
		nvram_bufset(RT2860_NVRAM, "apClient", apcli);
		need_commit = 1;
	}
#endif

	//new OperationMode
	if (strncmp(mode, old_mode, 2)) {
		nvram_bufset(RT2860_NVRAM, "OperationMode", mode);

		//from or to ap client mode
		if (!strncmp(mode, "3", 2))
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
		else if (!strncmp(old_mode, "3", 2))
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
		need_commit = 1;
	}

	if (strncmp(nat_en, old_nat, 2)) {
		nvram_bufset(RT2860_NVRAM, "natEnabled", nat_en);
		need_commit = 1;
	}
#ifdef CONFIG_RA_NAT_HW
	if (strncmp(hwnat_en, old_hwnat, 2)) {
		nvram_bufset(RT2860_NVRAM, "hwnatEnabled", hwnat_en);
		need_commit = 1;
	}
#endif

	// For 100PHY  ( Ethernet Convertor with one port only)
	// If this is one port only board(IC+ PHY) then redirect
	// the user browser to our alias ip address.
	if( getOnePortOnly() ){
		//     old mode is Gateway, and new mode is BRIDGE/WirelessISP/Apcli
		if (    (!strcmp(old_mode, "1") && !strcmp(mode, "0"))  ||
				(!strcmp(old_mode, "1") && !strcmp(mode, "2"))  ||
				(!strcmp(old_mode, "1") && !strcmp(mode, "3"))  ){
			char redirect_url[512];
			const char *lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");

			if(! strlen(lan_ip))
				lan_ip = "10.10.10.254";
			snprintf(redirect_url, 512, "http://%s", lan_ip);
			redirect_wholepage(wp, redirect_url);
			goto final;
        }

		//     old mode is BRIDGE/WirelessISP/Apcli, and new mode is Gateway
		if (    (!strcmp(old_mode, "0") && !strcmp(mode, "1"))  ||
				(!strcmp(old_mode, "2") && !strcmp(mode, "1"))  ||
				(!strcmp(old_mode, "3") && !strcmp(mode, "1"))  ){
			redirect_wholepage(wp, "http://172.32.1.254");
			goto final;
		}
	}
    
#if defined (CONFIG_RTDEV_MII)
	mii = websGetVar(wp, T("miiMode"), T("0"));
	if (!strncmp(mode, "0", 2)) {
		const char *old_mii = nvram_bufget(RTDEV_NVRAM, "InicMiiEnable");

		if (strncmp(mii, old_mii, 2)) {
			nvram_set(RTDEV_NVRAM, "InicMiiEnable", mii);
			need_commit = 1; //force to run initInternet
		}
	}
	else {
		nvram_set(RTDEV_NVRAM, "InicMiiEnable", "0");
		need_commit = 1; //force to run initInternet
	}
#endif

	websHeader(wp);
	websWrite(wp, T("<h2>Operation Mode</h2>\n"));
	websWrite(wp, T("mode: %s<br>\n"), mode);
	if (strncmp(mode, "0", 2))
		websWrite(wp, T("NAT enabled: %s<br>\n"), nat_en);
#if defined (CONFIG_RT2860V2_STA_DPB) || defined (CONFIG_RT2860V2_STA_ETH_CONVERT)
	else
		websWrite(wp, T("DPB station: %s<br>\n"), econv);
#endif
#if defined (CONFIG_RTDEV_MII)
	websWrite(wp, T("INIC MII mode: %s<br>\n"), mii);
#endif
	websFooter(wp);
	websDone(wp, 200);

final:
	sleep(1);	// wait for websDone() to finish tcp http session(close socket)

	//restart internet if any changes
	if (need_commit) {
		nvram_commit(RT2860_NVRAM);
		updateFlash8021x(RT2860_NVRAM);
		initInternet();
	}
}

void ConverterStringToDisplay(char *str)
{
    int  len, i;
    char buffer[193];
    char *pOut;

    memset(buffer,0,193);
    len = strlen(str);
    pOut = &buffer[0];

    for (i = 0; i < len; i++) {
		switch (str[i]) {
			case 38:
				strcpy (pOut, "&amp;");
				pOut += 5;
				break;

			case 60: 
				strcpy (pOut, "&lt;");
				pOut += 4;
				break;

			case 62: 
				strcpy (pOut, "&gt;");
				pOut += 4;
				break;

			case 34:
				strcpy (pOut, "&#34;");
				pOut += 5;
				break;

			case 39:
				strcpy (pOut, "&#39;");
				pOut += 5;
				break;
			case 32:
				strcpy (pOut, "&nbsp;");
				pOut += 6;
				break;

			default:
				if ((str[i]>=0) && (str[i]<=31)) {
					//Device Control Characters
					sprintf(pOut, "&#%02d;", str[i]);
					pOut += 5;
				} else if ((str[i]==39) || (str[i]==47) || (str[i]==59) || (str[i]==92)) {
					// ' / ; (backslash)
					sprintf(pOut, "&#%02d;", str[i]);
					pOut += 5;
				} else if (str[i]>=127) {
					//Device Control Characters
					sprintf(pOut, "&#%03d;", str[i]);
					pOut += 6;
				} else {
					*pOut = str[i];
					pOut++;
				}
				break;
		}
    }
    *pOut = '\0';
    strcpy(str, buffer);
}


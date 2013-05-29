/*********************************/
/* additional error return codes */
/*********************************/

#define ENOERR		0	/* no error */
#define EREBOOT		-1	/* unconditional reboot */
#define ERESET		-2	/* unconditional hard reset */
#define EMAXLOAD	-3	/* load average too high */
#define ETOOHOT		-4	/* too hot inside */
#define ENOLOAD		-5	/* /proc/loadavg contains no data */
#define ENOCHANGE	-6	/* file wasn't changed in the given interval */
#define EINVMEM		-7	/* /proc/meminfo contains invalid data */
#define ECHKILL		-8	/* child was killed by signal */
#define ETOOLONG	-9	/* child didn't return in time */

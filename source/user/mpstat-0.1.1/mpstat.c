/* Started to convert the vmstat program to a mpstat like program for use with 
SMP Linux. Although not complete the output data is correct. Added cpuid and 
multiple cpu handling.*/
/* removed dependance ont the procps package */
/* June 11, 1999 added function to get per processor interupts */ 
/* June 18, 1999 combined intr and ithr see FAQ.txt */
/* Feb 23, 2001 ump fixes, fixes for kernel 2.4 */

#include "mpstat.h"

#define BOOLTRUE 1
#define BOOLFALSE 0

int main(int, char **);

void getstat(unsigned *, unsigned *, unsigned *);

void getcpu_prcnt(int, int, unsigned *, 
		unsigned *, unsigned *, unsigned long *);

/***************************************************************
                             Main 
***************************************************************/

int main(int argc, char *argv[]) {

  const char format[]="cpu%-2d %6u %6u %4u %9u %3u %4u %4u %4u %3u %5u %3u %3u %3u %3u\n";
  unsigned int height=22; /* window height, reset later if needed. */
  unsigned long int args[2]={0,0};
  unsigned int moreheaders=TRUE;
  unsigned int tog=0; /* toggle switch for cleaner code */
#ifdef __SMP__
  /* SMP tog switches */
  int status_tog = 0;
  int status_nottog;
#endif
  unsigned int i,hz;
  unsigned int running[MAX_NR_CPUS],blocked[MAX_NR_CPUS]; /* running and blocked processes */	
  unsigned int memfree = 0; /* dummy var set to zero for space holder */
  unsigned int inter[2],ticks[2],ctxt[2];
  unsigned int per=0, pero2; 
  unsigned long num=0;
  unsigned int cpu_wait[MAX_NR_CPUS], cpu_inter[MAX_NR_CPUS];
  static int num_cpus;
  unsigned int cpu_useage[MAX_NR_CPUS], cpu_nicage[MAX_NR_CPUS], cpu_sysage[MAX_NR_CPUS];
  unsigned long cpu_idlage[MAX_NR_CPUS];
  int ret_stat;
  int ii;
  unsigned int duser[MAX_NR_CPUS],dsystem[MAX_NR_CPUS],
		  didle[MAX_NR_CPUS],divid[MAX_NR_CPUS],divo22[MAX_NR_CPUS];
  int *cpuid;
  
  cpuid = (int*)calloc(sizeof(MAX_NR_CPUS+1), sizeof(TWOBITE));
  if (cpuid == NULL) {
	  printf("could not allocate memory exiting \n");
	  exit(-1);
  }
  
  setlinebuf(stdout);
  argc=0; /* redefined as number of integer arguments */
  for (argv++;*argv;argv++) {
    if ('-' ==(**argv)) {
      switch (*(++(*argv))) {
	case 'V':
	printf("%s\n%s\n%s\n",PROGNAME, VERSION, REQUIRES);
	exit(0);
      case 'n':
	/* print only one header */
	moreheaders=FALSE;
      break;
      default:
	/* no other aguments defined yet. */
	usage();
      }
    }
    else {
      if (!sscanf(*argv,"%lu",args+argc++)) usage();
    }
  }
  switch (argc) {
  case 0:  /* no numeric args */
    per=1; 
    num=0;
    break;
  case 1:
    per=(unsigned) args[0];
    num=ULONG_MAX;
    break;
  case 2:
    per=(unsigned) args[0];
    num=args[1];
    break;
  default : 
    usage();
    break;
  }
  /* here we get the number of cpus and also get the cpu id 
   * the number of cpu's is returned by the function while the 
   * the cpu id is passed by reference */
  num_cpus = read_nr_cpus(cpuid); 
  if (num_cpus == 0) {
	  printf("NO CPUS DETECTED!!!! FATAL ERROR EXITING!!!\n");
	  exit (-1);
  } else if (num_cpus > NR_CPUS) {
      printf("***************WARNING!!!***************\n");
      printf("This version of mpstat only accepts %d cpu's and may segfault \n",NR_CPUS);
      printf("or show improper information.\n");
      printf("If you have the source code edit the mpstat.c file and change the\n");
      printf("#define MAX_NR_CPUS ((NR_CPUS*2)+2) line to reflect the number \n");
      printf("of cpu's in your machine.\n");
      exit (-1);
  }

  if (moreheaders) {
      int tmp=winhi()-3;
      height=(((tmp>0)?tmp:22)/num_cpus);
  }    

  pero2=(per/2); 

  showheader();
  getfaults(running,blocked, num_cpus, cpuid);
  getstat(inter,ticks,ctxt);
  get_inter(cpu_inter,num_cpus);
  getcpu_prcnt(BOOLFALSE, num_cpus, cpu_useage, 
		  cpu_nicage, cpu_sysage, cpu_idlage); 
  hz=sysconf(_SC_CLK_TCK); /* get ticks/s from system */
 // code here needs to be cleaned up here

#ifdef __SMP__
  
       for (ii=1; ii<=num_cpus; ii++) {
			  duser[ii] = cpu_useage[ii]+cpu_nicage[ii];
              dsystem[ii] = (cpu_sysage[ii]);
              didle[ii] = (cpu_idlage[ii])%UINT_MAX;
              divid[ii] = (duser[ii] + dsystem[ii] + didle[ii]);
              divo22[ii] = divid[ii]/2;
              cpu_wait[ii]=0;
	      
  printf(format, (int)cpuid[ii-1], running[ii-1], blocked[ii-1],memfree,
	 cpu_inter[ii-1], (*(ctxt)*hz+divo22[ii])/divid[ii],
	 memfree, memfree,memfree, memfree,memfree,
	 (100*duser[ii]+divo22[ii])/divid[ii],
	 (100*dsystem[ii]+divo22[ii])/divid[ii], 
	 memfree,
         (100*didle[ii] + divo22[ii])/divid[ii]);
           }  	   
#else	   
		
	duser[0] = cpu_useage[0]+cpu_nicage[0];
        dsystem[0] = (cpu_sysage[0]);
        didle[0] = (cpu_idlage[0])%UINT_MAX;
        divid[0] = (duser[0] + dsystem[0] + didle[0]);
        divo22[0] = divid[0]/2;
        cpu_wait[0]=0;
	
  printf(format, (int)cpuid[0],running[0], blocked[0],
	 memfree,
	 cpu_inter[0],
	 (*(ctxt)*hz+divo22[0])/divid[0],
	 memfree,
	 memfree,memfree,memfree,memfree,
	 (100*duser[0]+divo22[0])/divid[0], (100*dsystem[0]+divo22[0])/divid[0],
		  memfree,
                 (100*didle[0] + divo22[0])/divid[0]);
#endif
 
	
  for(i=1;i<num;i++) { /* \\\\\\\\\\\\\\\\\\\\ main loop ////////////////// */
    sleep(per);

#ifdef __SMP__
    if (status_tog == 0) {
       status_tog = num_cpus*2+2;
       status_nottog = 0;
    }
    else {
        status_tog = 0;
        status_nottog = num_cpus*2+2;
   } 
#endif

   if (moreheaders && ((i%height)==0)) showheader();
    tog= !tog;
    getfaults(running,blocked, num_cpus, cpuid);
    get_inter(cpu_inter,num_cpus);
    getstat(inter+tog,ticks+tog,ctxt+tog);

#ifdef __SMP__
    getcpu_prcnt(BOOLTRUE, num_cpus, cpu_useage+status_tog, 
			cpu_nicage+status_tog, cpu_sysage+status_tog, 
			cpu_idlage+status_tog);
#else 
    getcpu_prcnt(BOOLTRUE, num_cpus, cpu_useage+tog, 
			cpu_nicage+tog, cpu_sysage+tog, 
			cpu_idlage+tog);
#endif
	
// code here must be refined 
  
#ifdef __SMP__
         for (ii=1; ii<=num_cpus; ii++) {
	      
              duser[ii] = cpu_useage[ii + status_tog] - cpu_useage[ii + status_nottog] + cpu_nicage[ii+ status_tog] -cpu_nicage[ii+ status_nottog];
              dsystem[ii] = (cpu_sysage[ii + status_tog]) - (cpu_sysage[ii + status_nottog]);
              didle[ii] = ((cpu_idlage[ii + status_tog]) - (cpu_idlage[ii + status_nottog]))%UINT_MAX;
              divid[ii] = (duser[ii] + dsystem[ii] + didle[ii]);
              divo22[ii] = divid[ii]/2;
              cpu_wait[ii]=0;
	      
               printf(format,(int)cpuid[ii-1], running[ii-1], blocked[ii-1], 
	   memfree,
	   cpu_inter[ii-1],
	   (*(ctxt+tog)-*(ctxt+(!tog))+pero2)/per,
	   memfree,
	   memfree,memfree,memfree,memfree,
	   (100*duser[ii]+divo22[ii])/divid[ii], 
                   (100*dsystem[ii]+divo22[ii])/divid[ii],
		    memfree,
                   (100*didle[ii] + divo22[ii])/divid[ii]); 

	    }
#else
	    
    duser[0] = cpu_useage[0 + tog] - cpu_useage[0 + !tog] + cpu_nicage[0+ tog] -cpu_nicage[0+ !tog];
          dsystem[0] = (cpu_sysage[0 + tog]) - (cpu_sysage[0 + !tog]);
          didle[0] = ((cpu_idlage[0 + tog]) - (cpu_idlage[0 + !tog]))%UINT_MAX;
          divid[0] = (duser[0] + dsystem[0] + didle[0]);
          divo22[0] = divid[0]/2;
          cpu_wait[0] = 0;
	  
          printf(format,(int)cpuid[0], running[0], blocked[0],
	   memfree,
	   cpu_inter[0],
	   (*(ctxt+tog)-*(ctxt+(!tog))+pero2)/per,
	   memfree,
	memfree,memfree,memfree,memfree,
	   (100*duser[0]+divo22[0])/divid[0], 
                   (100*dsystem[0]+divo22[0])/divid[0],
		memfree,
                   (100*didle[0] + divo22[0])/divid[0]); 
#endif

  }
  free(cpuid);
  exit(EXIT_SUCCESS);
}

/**************************** others ***********************************/


void getstat(unsigned *itot, unsigned *i1, unsigned *ct) {
  static int stat;

  if ((stat=open("/proc/stat", O_RDONLY, 0)) != -1) {
    char* b;
    buff[BUFFSIZE-1] = 0;  /* ensure null termination in buffer */
    read(stat,buff,BUFFSIZE-1);
    close(stat);
    *itot = 0; 
    *i1 = 1;   /* ensure assert below will fail if the sscanf bombs */
    b = strstr(buff, "intr ");
    sscanf(b, "intr %u %u", itot, i1);
    b = strstr(buff, "ctxt ");
    sscanf(b, "ctxt %u", ct);
    assert(*itot>*i1);
  }
  else {
    crash("/proc/stat");
  }
}

void getcpu_prcnt(int withtog, int cpunumber, 
		unsigned *cpu_useage, unsigned *cpu_nicage, 
		unsigned *cpu_sysage,  unsigned long *cpu_idlage) {
	
	FILE *statPtr;
	int ii;
	char *name;
	
	name = (char*)calloc(6,sizeof(ONEBITE));
 
   if (((statPtr=fopen("/proc/stat", "r")) == NULL) || (name == NULL))  {
        printf("Could not open file /proc/stat. \n");
   }
   else {
	   	
        fscanf(statPtr, "%s %u %u %u %lu", name, &cpu_useage[0], &cpu_nicage[0], &cpu_sysage[0], &cpu_idlage[0]);

#ifdef __SMP__
  
       for (ii=1; ii<=cpunumber; ii++) {
           fscanf(statPtr, "%s %u %u %u %lu", name, &cpu_useage[ii], &cpu_nicage[ii], &cpu_sysage[ii], &cpu_idlage[ii]);
       }  	   
	free(name);
#endif

      fclose(statPtr);   
   } 
}

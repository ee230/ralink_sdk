/* this file contains some of the actual functions used by mpstat */

#include "mpstat.h"

void usage(void) {
  fprintf(stderr,"usage: %s [-V] [-n] [delay [count]]\n",PROGNAME);
  fprintf(stderr,"              -V prints version.\n");
  fprintf(stderr,"              -n causes the headers not to be reprinted regularly.\n");
  fprintf(stderr,"              delay is the delay between updates in seconds. \n");
  fprintf(stderr,"              count is the number of updates.\n");
  exit(EXIT_FAILURE);
}

void crash(char *filename) {
    perror(filename);
    exit(EXIT_FAILURE);
}

int winhi(void) {
    struct winsize win;
    int rows = 24;
 
    if (ioctl(1, TIOCGWINSZ, &win) != -1 && win.ws_row > 0)
      rows = win.ws_row;
 
    return rows;
}


void showheader(void){
  printf("%-5s %6s %6s %4s %8s %3s %4s %4s %4s %3s %5s %3s %3s %3s %3s\n",
	 "cpuid","majf","minf","xcal","intr/ithr","csw","icsw","migr",
	 "smtx","srw","syscl","us","sy","wt","idl");
}

int read_nr_cpus(int *cpurealid) {
	FILE *cpuinfofile;
	int cpunum = 0;
	
	if ((cpuinfofile = fopen("/proc/cpuinfo","r")) == NULL ) {
		printf("could not find /proc/cpuinfo!!!\n");
	} else {
		char *buffer;
		int nchars;
		buffer = calloc(BUFFSIZE,sizeof(ONEBITE)); 
		while (!feof(cpuinfofile)) 
		{
			fscanf(cpuinfofile,"%s       : %d",buffer,&nchars);
			if ((strcmp(PROCESSOR,buffer) == 0)) {
				*cpurealid = nchars;
				cpurealid++;
				cpunum++;
			}
		}
		free(buffer); 
	}
	return(cpunum);			
}

void get_inter (unsigned *inters, int number_cpus) {
	
	FILE *pFile;
	unsigned i = 1 ,k, j =0, l=0, m=0;
	unsigned *cpu;
	
	cpu = (unsigned *)calloc(MAX_NR_CPUS+1, sizeof(ONEBITE));
	*inters = 0;
	
	if (((pFile=fopen("/proc/interrupts", "r")) == NULL)
		|| (cpu == NULL)) {
		printf("error file not found\n");
	} else {
		char *buffer, *tempbuffer;
		buffer = (char*)calloc(4+(number_cpus*11)+30+1,sizeof(ONEBITE));
		tempbuffer = (char*)calloc(11+1,sizeof(ONEBITE));
				
		if (buffer != NULL && tempbuffer != NULL) { 
           fgets(buffer, 4+(number_cpus*11)+30, pFile);
		   while (!feof(pFile)) {
			fgets(buffer,4+(number_cpus*11)+30,pFile);
			if(strncmp(buffer,"NMI: ",5) == 0) {		
			   break;
			}
#ifdef __SMP__
			for (j = 1; j<=number_cpus; j++) {
#endif
			   for (i=6+l; i<=16+l;i++) {
				   (int)tempbuffer[m] = (int)buffer[i];
				   m++;
			   }
			   k = strtoul(tempbuffer, NULL,0);
			   cpu[j] = cpu[j] + k;
			   m=0;
#ifdef __SMP__
			   l=l+11;
		    }	
			l=0;
#endif
		   }
		   free(tempbuffer);
		   free(buffer); 
		}
		fclose(pFile);
	}
	/* set value of pointer to value of array */
#ifdef __SMP__
	for (i = 1; i<= number_cpus; i++) {
		*inters = cpu[i];
		inters++;
	}
#else
		*inters = cpu[0];
#endif
	free(cpu);
} 

void getfaults(unsigned int *majorflt, unsigned int *minorflt, int maxnumcpus, int *cpurealid ) { 
		
  static struct direct *ent;
  static char *filename;
  static int fd;
  static unsigned size;
  DIR *proc;
  unsigned minors, majors; 
  int cproc, i;
  unsigned *minortotcpu;
  unsigned *majortotcpu; 
  
  filename = (char *)calloc(80+1,sizeof(ONEBITE));
  minortotcpu = (unsigned *)calloc(MAX_NR_CPUS+1,sizeof(ONEBITE));
  majortotcpu = (unsigned *)calloc(MAX_NR_CPUS+1,sizeof(ONEBITE));
  
  *majorflt=0; 
  *minorflt=0;
 
  if ((proc=opendir("/proc"))==NULL) crash("/proc");

  while((ent=readdir(proc))) {
    if (isdigit(ent->d_name[0])) {  /*just to weed out the obvious junk*/
      sprintf(filename, "/proc/%s/stat", ent->d_name);
      if (((fd = open(filename, O_RDONLY, 0)) != -1) && 
		  (filename != NULL) &&
		  (minortotcpu != NULL) &&
		  (majortotcpu != NULL) ){ /*this weeds the rest*/
	read(fd,buff,BUFFSIZE-1);
#ifdef __SMP__
	sscanf(buff,  "%*d %*s %*c %*d %*d %*d %*d %*d %*u %u %*u %u %*u %*d %*d %*u %*u %*d %*d %*u %*u %*d %*u %u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %d", &minors, &majors ,&size, &cproc);
#else
	sscanf(buff,  "%*d %*s %*c %*d %*d %*d %*d %*d %*u %u %*u %u %*u %*d %*d %*d %*d %*d %*d %*u %*u %*d %*u %u %*u %*u %*u %*u %*u %*u %*u %*u ", &minors, &majors ,&size);

#endif

close(fd);
#ifdef __SMP__   
   majortotcpu[cproc] = majortotcpu[cproc] + majors;
   minortotcpu[cproc] = minortotcpu[cproc] + minors; 
#else 
   majortotcpu[0] = majortotcpu[0] + majors;
   minortotcpu[0] = minortotcpu[0] + minors;
#endif  
      }
    }
  }
  for (i=0; i<maxnumcpus; i++) {
       *majorflt=majortotcpu[*cpurealid]; 
       majorflt++;
       *minorflt=minortotcpu[*cpurealid]; 
       minorflt++; 
       cpurealid++; 
  } 
  free(filename); 
  free(minortotcpu); 
  free(majortotcpu); 
closedir(proc);
}




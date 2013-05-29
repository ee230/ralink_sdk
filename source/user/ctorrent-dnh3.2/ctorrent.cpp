#include "./def.h"
#include <sys/types.h>

#ifdef WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "btconfig.h"
#include "btcontent.h"
#include "downloader.h"
#include "peerlist.h"
#include "tracker.h"
#include "ctcs.h"
#include "console.h"

#include "./config.h"

#ifndef WINDOWS
#include "sigint.h"
#endif

void usage();
int param_check(int argc, char **argv);

#ifdef WINDOWS

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrzevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
}

#else

void Random_init()
{
  unsigned long seed;
#ifdef HAVE_GETTIMEOFDAY
  struct timeval tv; 
  gettimeofday(&tv,(struct timezone*) 0);
  seed = tv.tv_usec + tv.tv_sec + getpid();
#else
  seed = (unsigned long)time((time_t *)0);
#endif
  srandom(seed);
}

int main(int argc, char **argv)
{
  char *s;

  Random_init();
  arg_user_agent = new char[MAX_PF_LEN+1];
  strcpy(arg_user_agent,PEER_PFX);

  cfg_user_agent = new char[strlen(PACKAGE_NAME)+strlen(PACKAGE_VERSION)+1];
#ifndef WINDOWS
  if( !cfg_user_agent ) return -1;
#endif
  sprintf(cfg_user_agent, "%s/%s", PACKAGE_NAME, PACKAGE_VERSION);
  while(s = strchr(cfg_user_agent, ' ')) *s = '-';

  if( argc < 2 ){
    usage();
    exit(1);
  } else if( param_check(argc,argv) < 0 ) exit(1);

  if( arg_flg_make_torrent ){
    if( !arg_announce ){
      CONSOLE.Warning(1, "please use -u to specify a announce url!");
      exit(1);
    }
    if( !arg_save_as ){
      CONSOLE.Warning(1, "please use -s to specify a metainfo file name!");
      exit(1);
    }
    if( BTCONTENT.InitialFromFS(arg_metainfo_file, arg_announce,
                                arg_piece_length) < 0 ||
        BTCONTENT.CreateMetainfoFile(arg_save_as) < 0 ){
      CONSOLE.Warning(1, "create metainfo failed.");
      exit(1);
    }
    CONSOLE.Print("create metainfo file %s successful.", arg_save_as);
    exit(0);
  }

  if( arg_daemon ) CONSOLE.Daemonize();

  if( !arg_flg_exam_only && (!arg_flg_check_only || arg_flg_force_seed_mode) )
    if( arg_ctcs ) CTCS.Initial();
  
  if( BTCONTENT.InitialFromMI(arg_metainfo_file, arg_save_as) < 0){
    CONSOLE.Warning(1, "error, initial meta info failed.");
    exit(1);
  }

  if( !arg_flg_exam_only && (!arg_flg_check_only || arg_flg_force_seed_mode) ){
    if(WORLD.Initial_ListenPort() < 0){
      CONSOLE.Warning(2, "warn, you can't accept connections.");
    }

    Tracker.Initial();

    sig_setup();  // setup signal handling
    CONSOLE.Interact(
      "Press 'h' or '?' for help (display/control client options)." );
    Downloader();
    if( cfg_cache_size ) BTCONTENT.FlushCache();
  }
  if( !arg_flg_exam_only ) BTCONTENT.SaveBitfield();
  WORLD.CloseAll();

  if(arg_verbose)
    CONSOLE.Debug( "%.2f CPU seconds used; %lu seconds elapsed (%.2f%% usage)",
      clock() / (double)CLOCKS_PER_SEC,
      (unsigned long)(time((time_t *)0) - BTCONTENT.GetStartTime()),
      clock() / (double)CLOCKS_PER_SEC /
        (time((time_t *)0) - BTCONTENT.GetStartTime()) * 100 );

  exit(0);
}

#endif

int param_check(int argc, char **argv)
{
  int c, l;
  char *s;
  while( (c=getopt(argc,argv,
            "aA:b:cC:dD:e:E:fi:l:M:m:n:P:p:s:S:tTu:U:vxX:z:hH"))
           != -1 )
    switch( c ){
    case 'a':
      arg_allocate = 1;
      break;

    case 'b':
      arg_bitfield_file = new char[strlen(optarg) + 1];
#ifndef WINDOWS
      if( !arg_bitfield_file ) return -1;
#endif
      strcpy(arg_bitfield_file, optarg);
      break;

    case 'i':			// listen on ip XXXX
      cfg_listen_ip = inet_addr(optarg);
      break;

    case 'p':			// listen on Port XXXX
      cfg_listen_port = atoi(optarg);
      break;

    case 's':			// Save as FILE/DIR NAME
      if( arg_save_as ) return -1;  // specified twice
      arg_save_as = new char[strlen(optarg) + 1];
#ifndef WINDOWS
      if( !arg_save_as ) return -1;
#endif
      strcpy(arg_save_as,optarg);
      break;

    case 'e':			// Exit while complete
      cfg_seed_hours = (time_t)strtoul(optarg, NULL, 10);
      break;

    case 'E':			// target seed ratio
      cfg_seed_ratio = atof(optarg);
      break;

    case 'c':			// Check exist only
      arg_flg_check_only = 1;
      break;

    case 'C':			// Max cache size
      cfg_cache_size = atoi(optarg);
      break;
      
    case 'M':			// Max peers
      cfg_max_peers = atoi(optarg);
      if( cfg_max_peers > 1000 || cfg_max_peers < 20 ){
        CONSOLE.Warning(1, "-%c argument must be between 20 and 1000", c);
        return -1;
      }
      break;
      
    case 'm':			// Min peers
      cfg_min_peers = atoi(optarg);
      if( cfg_min_peers > 1000 || cfg_min_peers < 1 ){
        CONSOLE.Warning(1, "-%c argument must be between 1 and 1000", c);
        return -1;
      }
      break;

    case 'z':			// slice size
      cfg_req_slice_size = atoi(optarg) * 1024;
      if(cfg_req_slice_size < 1024 || cfg_req_slice_size > cfg_max_slice_size){
        CONSOLE.Warning(1, "-%c argument must be between 1 and %d",
          c, cfg_max_slice_size / 1024);
        return -1;
      }
      break;

    case 'n':			// Which file download
      if( arg_file_to_download ) return -1;  // specified twice
      arg_file_to_download = new char[strlen(optarg) + 1];
#ifndef WINDOWS
      if( !arg_file_to_download ) return -1;
#endif
      strcpy(arg_file_to_download,optarg);
      break;

    case 'f':			// force seed mode, skip sha1 check when startup.
      arg_flg_force_seed_mode = 1;
      break;
      
    case 'D':			// download bandwidth limit
      cfg_max_bandwidth_down = (int)(strtod(optarg, NULL) * 1024);
      break;

    case 'U':			// upload bandwidth limit
      cfg_max_bandwidth_up = (int)(strtod(optarg, NULL) * 1024);
      break;

    case 'P':			// peer ID prefix
      l = strlen(optarg);
      if (l > MAX_PF_LEN) {
        CONSOLE.Warning(1, "-P arg must be %d or less characters", MAX_PF_LEN);
        return -1;
      }
      if (l == 1 && *optarg == '-') *arg_user_agent = (char) 0;
      else strcpy(arg_user_agent,optarg);
      break;

    case 'A':			// HTTP user-agent header string
      if( cfg_user_agent ) delete []cfg_user_agent;
      cfg_user_agent = new char[strlen(optarg) + 1];
#ifndef WINDOWS
      if( !cfg_user_agent ) return -1;
#endif
      strcpy(cfg_user_agent, optarg);
      break;

    case 'T':			// convert foreign filenames to printable text
      arg_flg_convert_filenames = 1;
      break;

     // BELOW OPTIONS USED FOR CREATE TORRENT.
    case 'u':			// Announce url
      if( arg_announce ) return -1;  // specified twice
      arg_announce = new char[strlen(optarg) + 1];
#ifndef WINDOWS
      if( !arg_announce ) return -1;
#endif
      strcpy(arg_announce, optarg);
      break;

    case 't':			// make Torrent
      arg_flg_make_torrent = 1;
      break;

    case 'l':			// piece Length (default 262144)
      arg_piece_length = atoi(optarg);
      if( arg_piece_length < 65536 || arg_piece_length > 4096*1024 ){
        CONSOLE.Warning(1, "-%c argument must be between 65536 and %d",
          c, 4096*1024);
        return -1;
      }
      break;
     // ABOVE OPTIONS USED FOR CREATE TORRENT.

    case 'x':			// print torrent information only
      arg_flg_exam_only = 1;
      break;

    case 'S':			// CTCS server
      if( arg_ctcs ) return -1;  // specified twice
      arg_ctcs = new char[strlen(optarg) + 1];
#ifndef WINDOWS
      if( !arg_ctcs ) return -1;
#endif
      if( !strchr(optarg, ':') ){
        CONSOLE.Warning(1, "-%c argument requires a port number", c);
        return -1;
      }
      strcpy(arg_ctcs, optarg);
      break;

    case 'X':			// "user exit" on download completion
      if( arg_completion_exit ) return -1;  // specified twice
      arg_completion_exit = new char[strlen(optarg) + 1];
#ifndef WINDOWS
      if( !arg_completion_exit ) return -1;
#endif
#ifndef HAVE_SYSTEM
      CONSOLE.Warning(1, "-X is not supported on your system");
      return -1;
#endif
#ifndef HAVE_WORKING_FORK
      CONSOLE.Warning(2,
        "No working fork function; be sure the -X command is brief!");
#endif
      strcpy(arg_completion_exit, optarg);
      break;

    case 'v':			// verbose output
      arg_verbose = 1;
      break;

    case 'd':			// daemon mode (fork to background)
      arg_daemon = 1;
      break;

    case 'h':
    case 'H':			// help
      usage();
      return -1;

    default:
      //unknown option.
      CONSOLE.Warning(1, "Use -h for help/usage.");
      return -1;
    }

  argc -= optind; argv += optind;
  if( cfg_min_peers >= cfg_max_peers ) cfg_min_peers = cfg_max_peers - 1;
  if( argc != 1 ){
    if( arg_flg_make_torrent )
      CONSOLE.Warning(1,
        "Must specify torrent contents (one file or directory)");
    else CONSOLE.Warning(1, "Must specify one torrent file");
    return -1;
  }
  arg_metainfo_file = new char[strlen(*argv) + 1];
#ifndef WINDOWS
  if( !arg_metainfo_file ) return -1;
#endif
  strcpy(arg_metainfo_file, *argv);

  if( !arg_bitfield_file ){
    arg_bitfield_file = new char[strlen(arg_metainfo_file) + 4];
#ifndef WINDOWS
    if( !arg_bitfield_file ) return -1;
#endif
    strcpy(arg_bitfield_file, arg_metainfo_file);
    strcat(arg_bitfield_file, ".bf");
  }
  return 0;
}

void usage()
{
  fprintf(stderr,"%s   Original code Copyright: YuHong(992126018601033)\n",
    PACKAGE_STRING);
  fprintf(stderr,"WARNING: THERE IS NO WARRANTY FOR CTorrent. USE AT YOUR OWN RISK!!!\n");
  fprintf(stderr,"\nGeneral Options:\n");
  fprintf(stderr, "%-15s %s\n", "-h/-H", "Show this message");
  fprintf(stderr, "%-15s %s\n", "-x",
    "Decode metainfo (torrent) file only, don't download");
  fprintf(stderr, "%-15s %s\n", "-c", "Check pieces only, don't download");
  fprintf(stderr, "%-15s %s\n", "-v", "Verbose output (for debugging)");

  fprintf(stderr,"\nDownload Options:\n");
  fprintf(stderr, "%-15s %s\n", "-e int",
    "Exit while seed <int> hours later (default 72 hours)");
  fprintf(stderr, "%-15s %s\n", "-E num",
    "Exit after seeding to <num> ratio (UL:DL)");
  fprintf(stderr, "%-15s %s\n", "-i ip",
    "Listen for connections on specific IP address (default all/any)");
  fprintf(stderr, "%-15s %s\n", "-p port",
    "Listen port (default 2706 -> 2106)");
  fprintf(stderr, "%-15s %s\n", "-s filename",
    "Download (\"save as\") to a different file or directory");
  fprintf(stderr, "%-15s %s\n", "-C cache_size",
    "Cache size, unit MB (default 16MB)");
  fprintf(stderr, "%-15s %s\n", "-f",
    "Force saved bitfield or seed mode (skip initial hash check)");
  fprintf(stderr, "%-15s %s\n", "-b filename",
    "Specify bitfield save file (default is torrent+\".bf\")");
  fprintf(stderr, "%-15s %s\n", "-M max_peers",
    "Max peers count (default 100)");
  fprintf(stderr, "%-15s %s\n", "-m min_peers", "Min peers count (default 1)");
  fprintf(stderr, "%-15s %s\n", "-z slice_size",
    "Download slice/block size, unit KB (default 16, max 128)");
  fprintf(stderr, "%-15s %s\n", "-n file_list",
    "Specify file number(s) to download");
  fprintf(stderr, "%-15s %s\n", "-D rate", "Max bandwidth down (unit KB/s)");
  fprintf(stderr, "%-15s %s\n", "-U rate", "Max bandwidth up (unit KB/s)");
  fprintf(stderr, "%-15s %s%s\")\n", "-P peer_id",
    "Set Peer ID prefix (default \"", PEER_PFX);
  fprintf(stderr, "%-15s %s%s\")\n", "-A user_agent",
    "Set User-Agent header (default \"", cfg_user_agent);
  fprintf(stderr, "%-15s %s\n", "-S host:port",
    "Use CTCS server at host:port");
  fprintf(stderr, "%-15s %s\n", "-a", "Preallocate files on disk");
  fprintf(stderr, "%-15s %s\n", "-T",
    "Convert foreign filenames to printable text");
  fprintf(stderr, "%-15s %s\n", "-X command",
    "Run command upon download completion (\"user exit\")");
  fprintf(stderr, "%-15s %s\n", "-d", "Daemon mode (fork to background)");

  fprintf(stderr,"\nMake metainfo (torrent) file options:\n");
  fprintf(stderr, "%-15s %s\n", "-t", "Create a new torrent file");
  fprintf(stderr, "%-15s %s\n", "-u url", "Tracker's url");
  fprintf(stderr, "%-15s %s\n", "-l piece_len",
    "Piece length (default 262144)");
  fprintf(stderr, "%-15s %s\n", "-s filename", "Specify metainfo file name");

  fprintf(stderr,"\nExample:\n");
  fprintf(stderr,"ctorrent -s new_filename -e 12 -C 32 -p 6881 example.torrent\n");
  fprintf(stderr,"\nhome page: http://ctorrent.sourceforge.net/\n");
  fprintf(stderr,"see also: http://www.rahul.net/dholmes/ctorrent/\n");
  fprintf(stderr,"bug report: %s\n",PACKAGE_BUGREPORT);
  fprintf(stderr,"original author: bsdi@sina.com\n\n");
}


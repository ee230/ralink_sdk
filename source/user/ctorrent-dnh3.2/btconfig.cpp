#include <sys/types.h>
#include "btconfig.h"

size_t cfg_req_slice_size = DEFAULT_SLICE_SIZE;
size_t cfg_req_queue_length = 74;

size_t cfg_cache_size = 16;

size_t cfg_max_peers = 100;
size_t cfg_min_peers = 1;

unsigned long cfg_listen_ip = 0;
int cfg_listen_port = 0;
int cfg_max_listen_port = 2706;
int cfg_min_listen_port = 2106;

int cfg_max_bandwidth_down = 0;
int cfg_max_bandwidth_up = 0;

time_t cfg_seed_hours = 72;
double cfg_seed_ratio = 0;

// arguments global value
char *arg_metainfo_file = (char*) 0;
char *arg_bitfield_file = (char*) 0;
char *arg_save_as = (char*) 0;
char *arg_user_agent = (char*) 0;

unsigned char arg_flg_force_seed_mode = 0;
unsigned char arg_flg_check_only = 0;
unsigned char arg_flg_exam_only = 0;
unsigned char arg_flg_make_torrent = 0;
unsigned char arg_flg_convert_filenames = 0;
char *arg_file_to_download = (char *)0;
unsigned char arg_verbose = 0;
unsigned char arg_allocate = 0;
unsigned char arg_daemon = 0;

size_t arg_piece_length = 262144;
char *arg_announce = (char*) 0;

char *arg_ctcs = (char*) 0;
char *arg_completion_exit = (char*) 0;

char *cfg_user_agent = (char*) 0;


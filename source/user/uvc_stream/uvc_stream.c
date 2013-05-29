/*******************************************************************************
# "uvc_stream" is a command line application to stream JPEG files over an      #
# IP-based network from the webcam to a viewer like Firefox, Cambozola,        #
# Videolanclient or even to a Windows Mobile device running the TCPMP-Player.  #
#                                                                              #
# It was written for embedded devices with very limited ressources in terms of #
# RAM and CPU. The decision for Linux-UVC was done, because supported cameras  #
# directly produce JPEG-data, allowing fast and perfomant M-JPEG streams even  #
# from slow embedded devices like those supported by OpenWRT.                  #
#                                                                              #
# I would suggest not to open this server to the internet. Use it as input to  #
# the programm "motion" [http://motion.sf.net] running at your DMZ instead.    #
# Motion has many users and i expect it to be checked more often for security  #
# issues. Keep in mind, that motions advanced capabilties like                 #
# motion-detection, writing of avifiles etc require much more ressources.      #
#                                                                              #
# In contrast to the better known SPCA5XX-LE, UVC-cameras in average produce   #
# better image quality (See Michel XHaards comparison table and rating at      #
# his site) [http://mxhaard.free.fr/embedded.html].                            #
#                                                                              #
# This programm was written in 2007 by Tom Stöveken, basing on luvcview.       #
# The luvcview sources were modified using the tool "indent" and afterwards    #
# SDL dependencies were removed to reduce dependencies to other packages.      #
#                                                                              #
# This package work with the Logitech UVC based webcams with the mjpeg feature.#
#                                                                              #
#     Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard               #
#     Copyright (C) 2007      Tom Stöveken                                     #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/videodev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <pthread.h>

#include "v4l2uvc.h"
#include "utils.h"

#define SOURCE_VERSION "1.0"

#define BOUNDARY "arflebarfle"

/* globals */
struct vdIn *videoIn;
int stop=0;

/* reader writer solution */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t db = PTHREAD_MUTEX_INITIALIZER;
int reader_cnt = 0;

/* global JPG frame */
unsigned char *g_buf = NULL;
int g_size = 0;

/* thread for clients that connected to this server */
void *client_thread( void *arg ) {
  FILE *fp = (FILE*)arg;
  unsigned char *frame = (unsigned char *) calloc(1, (size_t)videoIn->framesizeIn);
  int ok = 1, frame_size=0;

  ok = fputs("HTTP/1.0 200 OK\r\n" \
             "Server: UVC Streamer\r\n" \
             "Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n" \
             "\r\n" \
             "--" BOUNDARY "\n", fp);

  while ( ok >= 0 && !stop ) {
    pthread_mutex_lock( &mutex );
    reader_cnt += 1;
    if ( reader_cnt == 1 ) {
      pthread_mutex_lock( &db );
    }
    pthread_mutex_unlock( &mutex );

    /* read buffer */
    frame_size = g_size;
    memcpy(frame, g_buf, frame_size);

    pthread_mutex_lock( &mutex );
    reader_cnt -= 1;
    if ( reader_cnt == 0 ) {
      pthread_mutex_unlock( &db );
    }
    pthread_mutex_unlock( &mutex );

    /* send picture to client */
    ok = fputs("Content-type: image/jpeg\n\n", fp);
    if( ok < 0 ) break;

    ok = print_picture(fp, frame, frame_size);
    if( ok < 0 ) break;

    ok = fputs("\n--" BOUNDARY "\n", fp);
    if( ok < 0 ) break;

    usleep(1000*1000/videoIn->fps);
  }

  fclose(fp);
  free(frame);

  return NULL;
}

/* the single writer thread */
void *cam_thread( void *arg ) {
  while( !stop ) {
    /* grab a frame */
    if( uvcGrab(videoIn) < 0 ) {
      fprintf(stderr, "Error grabbing\n");
      exit(1);
    }

    /* copy frame to global buffer */
    pthread_mutex_lock( &db );
    g_size = videoIn->buf.bytesused;
    memcpy(g_buf, videoIn->tmpbuffer, videoIn->buf.bytesused);
    pthread_mutex_unlock( &db );
  }

  return NULL;
}

void help(char *progname)
{
  fprintf(stderr, "------------------------------------------------------------------\n");
  fprintf(stderr, "Usage: %s\n" \
                  " [-h | --help ]........: display this help\n" \
                  " [-d | --device ]......: video device to open (your camera)\n" \
                  " [-r | --resolution ]..: 640x480, 320x240, 160x120\n" \
                  " [-f | --fps ].........: frames per second\n" \
                  " [-m | --mode ]........: YUV, MJPG\n" \
                  " [-p | --port ]........: TCP-port for the server\n" \
                  " [-v | --version ].....: display version information\n" \
                  " [-b | --background]...: fork to the background, daemon mode\n", progname);
  fprintf(stderr, "------------------------------------------------------------------\n");
}

void signal_handler(int sig) {
  /* signal "stop" to threads */
  stop = 1;

  /* cleanup most important structures */
  fprintf(stderr, "shutdown...wait for 3secs \n");
  usleep(3000*1000);
  close_v4l2(videoIn);
  free(videoIn);

  exit(0);
  return;
}

void daemon_mode(void) {
  int fr=0;

  fr = fork();
  if( fr < 0 ) {
    fprintf(stderr, "fork() failed\n");
    exit(1);
  }
  if ( fr > 0 ) {
    exit(0);
  }

  if( setsid() < 0 ) {
    fprintf(stderr, "setsid() failed\n");
    exit(1);
  }

  fr = fork();
  if( fr < 0 ) {
    fprintf(stderr, "fork() failed\n");
    exit(1);
  }
  if ( fr > 0 ) {
    fprintf(stderr, "forked to background (%d)\n", fr);
    exit(0);
  }

  umask(0);

  chdir("/");
  close(0);
  close(1);
  close(2);

  open("/dev/null", O_RDWR);
  dup(0);
  dup(0);
}

/* #########################################################################
Main
######################################################################### */
int main(int argc, char *argv[])
{
  struct sockaddr_in addr;
  int sd, port = htons(8080), cfd, on;
  FILE *fp;
  pthread_t client, cam;
  char *dev = "/dev/video0";
  int mode, width=640, height=480, fps=25, daemon=0;
  int format = V4L2_PIX_FMT_MJPEG;

  while(1) {
    int option_index = 0, c=0;
    static struct option long_options[] = \
    {
      {"h", no_argument, 0, 0},
      {"help", no_argument, 0, 0},
      {"d", required_argument, 0, 0},
      {"device", required_argument, 0, 0},
      {"r", required_argument, 0, 0},
      {"resolution", required_argument, 0, 0},
      {"f", required_argument, 0, 0},
      {"fps", required_argument, 0, 0},
      {"p", required_argument, 0, 0},
      {"port", required_argument, 0, 0},
      {"v", no_argument, 0, 0},
      {"version", no_argument, 0, 0},
      {"b", no_argument, 0, 0},
      {"background", no_argument, 0, 0},
      {"m", required_argument, 0, 0},
      {"mode", required_argument, 0, 0},
      {0, 0, 0, 0}
    };

    c = getopt_long_only(argc, argv, "", long_options, &option_index);

    /* no more options to parse */
    if (c == -1) break;

    /* unrecognized option */
    if(c=='?'){ help(argv[0]); return 0; }

    switch (option_index) {
      /* h, help */
      case 0:
      case 1:
        help(argv[0]);
        return 0;
        break;

      /* d, device */
      case 2:
      case 3:
        dev = strdup(optarg);
        break;

      /* r, resolution */
      case 4:
      case 5:
        if ( strcmp("640x480", optarg) == 0 ) { width=640; height=480; }
        else if ( strcmp("320x240", optarg) == 0 ) { width=320; height=240; }
        else if ( strcmp("160x120", optarg) == 0 ) { width=160; height=120; }
        else fprintf(stderr, "ignoring unsupported resolution\n");
        break;

      /* f, fps */
      case 6:
      case 7:
        fps=atoi(optarg);
        break;

      /* p, port */
      case 8:
      case 9:
        port=htons(atoi(optarg));
        break;

      /* v, version */
      case 10:
      case 11:
        printf("UVC Streamer Version: %s\n" \
               "Compilation Date....: %s\n" \
               "Compilation Time....: %s\n", SOURCE_VERSION, __DATE__, __TIME__);
        return 0;
        break;

      /* b, background */
      case 12:
      case 13:
        daemon=1;
        break;
      case 14:
      case 15:
	if (strcasecmp(optarg, "yuv") == 0) {
	    format = V4L2_PIX_FMT_YUYV;
	} else if (strcasecmp(optarg, "mjpg") == 0) {
	    format = V4L2_PIX_FMT_MJPEG;
	} else {
	    printf("Unknown format specified. Aborting.\n");
	    return 0;
	}
	break;
      default:
        help(argv[0]);
        return 0;
    }
  }

  /* ignore SIGPIPE (send if transmitting to closed sockets) */
  signal(SIGPIPE, SIG_IGN);
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    fprintf(stderr, "could not register signal handler\n");
    exit(1);
  }

  /* fork to the background */
  if ( daemon ) {
    daemon_mode();
  }

  /* allocate webcam datastructure */
  videoIn = (struct vdIn *) calloc(1, sizeof(struct vdIn));

  fprintf(stderr, "Using V4L2 device: %s\n", dev);
  fprintf(stderr, "Resolution.......: %i x %i\n", width, height);
  fprintf(stderr, "frames per second: %i\n", fps);
  fprintf(stderr, "TCP port.........: %i\n", ntohs(port));

  /* open video device and prepare data structure */
  if (init_videoIn(videoIn, dev, width, height, fps, format, 1) < 0) {
    fprintf(stderr, "init_VideoIn failed\n");
    exit(1);
  }

  /* open socket for server */
  sd = socket(PF_INET, SOCK_STREAM, 0);
  if ( sd < 0 ) {
    fprintf(stderr, "socket failed\n");
    exit(1);
  }

  /* ignore "socket already in use" errors */
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(1);
  }

  /* configure server address to listen to all local IPs */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = INADDR_ANY;
  if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ) {
    fprintf(stderr, "bind failed\n");
    exit(1);
  }

  /* start listening on socket */
  if ( listen(sd, 10) != 0 ) {
    fprintf(stderr, "listen failed\n");
    exit(1);
  }

  /* start to read the camera, push picture buffers into global buffer */
  videoIn->tmpbuffer = (unsigned char *) calloc(1, (size_t)videoIn->framesizeIn);
  g_buf = (unsigned char *) calloc(1, (size_t)videoIn->framesizeIn);
  pthread_create(&cam, 0, cam_thread, NULL);
  pthread_detach(cam);

  /* create a child for every client that connects */
  while ( 1 ) {
    cfd = accept(sd, 0, 0);
    fp = fdopen(cfd, "r+");
    pthread_create(&client, NULL, &client_thread, fp);
    pthread_detach(client);
  }

  return 0;
}

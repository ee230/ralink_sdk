/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SYSTEM = 258,
     TUNNEL = 259,
     SESSION = 260,
     PROFILE = 261,
     PEER = 262,
     PPP = 263,
     CREATE = 264,
     MODIFY = 265,
     DIGITAL_ANALOG = 266,
     TUNNEL_AUTH_MODE = 267,
     PEER_IPADDR = 268,
     LAC_LNS = 269,
     TUNNEL_ID = 270,
     TUNNEL_NAME = 271,
     SESSION_ID = 272,
     SESSION_NAME = 273,
     UDP_PORT = 274,
     TRACE_FLAGS = 275,
     MAX_TUNNELS = 276,
     MAX_SESSIONS = 277,
     DRAIN_TUNNELS = 278,
     TUNNEL_ESTABLISH_TIMEOUT = 279,
     SESSION_ESTABLISH_TIMEOUT = 280,
     TUNNEL_PERSIST_PEND_TIMEOUT = 281,
     SESSION_PERSIST_PEND_TIMEOUT = 282,
     DENY_LOCAL_TUNNEL_CREATES = 283,
     DENY_REMOTE_TUNNEL_CREATES = 284,
     HIDE_AVPS = 285,
     OUR_ADDR = 286,
     PEER_ADDR = 287,
     OUR_UDP_PORT = 288,
     PEER_UDP_PORT = 289,
     CONFIG_ID = 290,
     AUTH_MODE = 291,
     FRAMING_CAP = 292,
     BEARER_CAP = 293,
     USE_TIEBREAKER = 294,
     ALLOW_PPP_PROXY = 295,
     USE_PPP_PROXY = 296,
     USE_UDP_CHECKSUMS = 297,
     HELLO_TIMEOUT = 298,
     MAX_RETRIES = 299,
     RX_WINDOW_SIZE = 300,
     TX_WINDOW_SIZE = 301,
     RETRY_TIMEOUT = 302,
     IDLE_TIMEOUT = 303,
     DO_PMTU_DISCOVERY = 304,
     PERSIST = 305,
     MTU = 306,
     SECRET = 307,
     HOST_NAME = 308,
     TUNNEL_PROFILE_NAME = 309,
     PEER_PROFILE_NAME = 310,
     SESSION_PROFILE_NAME = 311,
     PPP_PROFILE_NAME = 312,
     INTERFACE_NAME = 313,
     SESSION_TYPE = 314,
     USER_NAME = 315,
     USER_PASSWORD = 316,
     PRIV_GROUP_ID = 317,
     PROFILE_NAME = 318,
     SEQUENCING_REQUIRED = 319,
     USE_SEQUENCE_NUMBERS = 320,
     NO_PPP = 321,
     REORDER_TIMEOUT = 322,
     FRAMING_TYPE = 323,
     BEARER_TYPE = 324,
     MINIMUM_BPS = 325,
     MAXIMUM_BPS = 326,
     CONNECT_SPEED = 327,
     PEER_PORT = 328,
     WE_CAN_BE_LAC = 329,
     WE_CAN_BE_LNS = 330,
     DEFAULT_TUNNEL_PROFILE_NAME = 331,
     DEFAULT_SESSION_PROFILE_NAME = 332,
     DEFAULT_PPP_PROFILE_NAME = 333,
     NETMASK = 334,
     ASYNCMAP = 335,
     MRU = 336,
     AUTH_FLAGS = 337,
     SYNC_MODE = 338,
     CHAP_INTERVAL = 339,
     CHAP_MAX_CHALLENGE = 340,
     CHAP_RESTART = 341,
     PAP_MAX_AUTH_REQUESTS = 342,
     PAP_RESTART_INTERVAL = 343,
     PAP_TIMEOUT = 344,
     IPCP_MAX_CONFIG_REQUESTS = 345,
     IPCP_MAX_CONFIG_NAKS = 346,
     IPCP_MAX_TERMINATE_REQUESTS = 347,
     IPCP_RETRANSMIT_INTERVAL = 348,
     LCP_ECHO_FAILURE_COUNT = 349,
     LCP_ECHO_INTERVAL = 350,
     LCP_MAX_CONFIG_REQUESTS = 351,
     LCP_MAX_CONFIG_NAKS = 352,
     LCP_MAX_TERMINATE_REQUESTS = 353,
     LCP_RETRANSMIT_INTERVAL = 354,
     MAX_CONNECT_TIME = 355,
     MAX_FAILURE_COUNT = 356,
     LOCAL_IPADDR = 357,
     REMOTE_IPADDR = 358,
     DNS_IPADDR_PRI = 359,
     DNS_IPADDR_SEC = 360,
     WINS_IPADDR_PRI = 361,
     WINS_IPADDR_SEC = 362,
     IP_POOL_NAME = 363,
     USE_RADIUS = 364,
     RADIUS_HINT = 365,
     USE_AS_DEFAULT_ROUTE = 366,
     MULTILINK = 367,
     PROXY_ARP = 368,
     EOT = 369,
     SLASH = 370,
     BLCL = 371,
     ELCL = 372,
     EQUALS = 373,
     DECSTRING = 374,
     STRING = 375,
     HEXSTRING = 376,
     QUOTEDSTRING = 377,
     BOOL = 378,
     IPADDRESS = 379,
     INITIAL_RCVD_LCP_CONFREQ = 380,
     CALLING_NUMBER = 381,
     PROXY_AUTH_NAME = 382,
     SUB_ADDRESS = 383,
     PROXY_AUTH_TYPE = 384,
     PROXY_AUTH_RESPONSE = 385,
     LAST_RCVD_LCP_CONFREQ = 386,
     CALLED_NUMBER = 387,
     PROXY_AUTH_CHALLENGE = 388,
     LAST_SENT_LCP_CONFREQ = 389,
     AUTH_PAP = 390,
     AUTH_CHAP = 391,
     AUTH_MSCHAPV1 = 392,
     AUTH_MSCHAPV2 = 393,
     AUTH_EAP = 394,
     AUTH_NONE = 395,
     AUTH_PEER = 396,
     SRC_IPADDR = 397,
     DEST_IPADDR = 398,
     LOCAL_NAME = 399,
     REMOTE_NAME = 400
   };
#endif
/* Tokens.  */
#define SYSTEM 258
#define TUNNEL 259
#define SESSION 260
#define PROFILE 261
#define PEER 262
#define PPP 263
#define CREATE 264
#define MODIFY 265
#define DIGITAL_ANALOG 266
#define TUNNEL_AUTH_MODE 267
#define PEER_IPADDR 268
#define LAC_LNS 269
#define TUNNEL_ID 270
#define TUNNEL_NAME 271
#define SESSION_ID 272
#define SESSION_NAME 273
#define UDP_PORT 274
#define TRACE_FLAGS 275
#define MAX_TUNNELS 276
#define MAX_SESSIONS 277
#define DRAIN_TUNNELS 278
#define TUNNEL_ESTABLISH_TIMEOUT 279
#define SESSION_ESTABLISH_TIMEOUT 280
#define TUNNEL_PERSIST_PEND_TIMEOUT 281
#define SESSION_PERSIST_PEND_TIMEOUT 282
#define DENY_LOCAL_TUNNEL_CREATES 283
#define DENY_REMOTE_TUNNEL_CREATES 284
#define HIDE_AVPS 285
#define OUR_ADDR 286
#define PEER_ADDR 287
#define OUR_UDP_PORT 288
#define PEER_UDP_PORT 289
#define CONFIG_ID 290
#define AUTH_MODE 291
#define FRAMING_CAP 292
#define BEARER_CAP 293
#define USE_TIEBREAKER 294
#define ALLOW_PPP_PROXY 295
#define USE_PPP_PROXY 296
#define USE_UDP_CHECKSUMS 297
#define HELLO_TIMEOUT 298
#define MAX_RETRIES 299
#define RX_WINDOW_SIZE 300
#define TX_WINDOW_SIZE 301
#define RETRY_TIMEOUT 302
#define IDLE_TIMEOUT 303
#define DO_PMTU_DISCOVERY 304
#define PERSIST 305
#define MTU 306
#define SECRET 307
#define HOST_NAME 308
#define TUNNEL_PROFILE_NAME 309
#define PEER_PROFILE_NAME 310
#define SESSION_PROFILE_NAME 311
#define PPP_PROFILE_NAME 312
#define INTERFACE_NAME 313
#define SESSION_TYPE 314
#define USER_NAME 315
#define USER_PASSWORD 316
#define PRIV_GROUP_ID 317
#define PROFILE_NAME 318
#define SEQUENCING_REQUIRED 319
#define USE_SEQUENCE_NUMBERS 320
#define NO_PPP 321
#define REORDER_TIMEOUT 322
#define FRAMING_TYPE 323
#define BEARER_TYPE 324
#define MINIMUM_BPS 325
#define MAXIMUM_BPS 326
#define CONNECT_SPEED 327
#define PEER_PORT 328
#define WE_CAN_BE_LAC 329
#define WE_CAN_BE_LNS 330
#define DEFAULT_TUNNEL_PROFILE_NAME 331
#define DEFAULT_SESSION_PROFILE_NAME 332
#define DEFAULT_PPP_PROFILE_NAME 333
#define NETMASK 334
#define ASYNCMAP 335
#define MRU 336
#define AUTH_FLAGS 337
#define SYNC_MODE 338
#define CHAP_INTERVAL 339
#define CHAP_MAX_CHALLENGE 340
#define CHAP_RESTART 341
#define PAP_MAX_AUTH_REQUESTS 342
#define PAP_RESTART_INTERVAL 343
#define PAP_TIMEOUT 344
#define IPCP_MAX_CONFIG_REQUESTS 345
#define IPCP_MAX_CONFIG_NAKS 346
#define IPCP_MAX_TERMINATE_REQUESTS 347
#define IPCP_RETRANSMIT_INTERVAL 348
#define LCP_ECHO_FAILURE_COUNT 349
#define LCP_ECHO_INTERVAL 350
#define LCP_MAX_CONFIG_REQUESTS 351
#define LCP_MAX_CONFIG_NAKS 352
#define LCP_MAX_TERMINATE_REQUESTS 353
#define LCP_RETRANSMIT_INTERVAL 354
#define MAX_CONNECT_TIME 355
#define MAX_FAILURE_COUNT 356
#define LOCAL_IPADDR 357
#define REMOTE_IPADDR 358
#define DNS_IPADDR_PRI 359
#define DNS_IPADDR_SEC 360
#define WINS_IPADDR_PRI 361
#define WINS_IPADDR_SEC 362
#define IP_POOL_NAME 363
#define USE_RADIUS 364
#define RADIUS_HINT 365
#define USE_AS_DEFAULT_ROUTE 366
#define MULTILINK 367
#define PROXY_ARP 368
#define EOT 369
#define SLASH 370
#define BLCL 371
#define ELCL 372
#define EQUALS 373
#define DECSTRING 374
#define STRING 375
#define HEXSTRING 376
#define QUOTEDSTRING 377
#define BOOL 378
#define IPADDRESS 379
#define INITIAL_RCVD_LCP_CONFREQ 380
#define CALLING_NUMBER 381
#define PROXY_AUTH_NAME 382
#define SUB_ADDRESS 383
#define PROXY_AUTH_TYPE 384
#define PROXY_AUTH_RESPONSE 385
#define LAST_RCVD_LCP_CONFREQ 386
#define CALLED_NUMBER 387
#define PROXY_AUTH_CHALLENGE 388
#define LAST_SENT_LCP_CONFREQ 389
#define AUTH_PAP 390
#define AUTH_CHAP 391
#define AUTH_MSCHAPV1 392
#define AUTH_MSCHAPV2 393
#define AUTH_EAP 394
#define AUTH_NONE 395
#define AUTH_PEER 396
#define SRC_IPADDR 397
#define DEST_IPADDR 398
#define LOCAL_NAME 399
#define REMOTE_NAME 400




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 35 "l2tp_config_parse.y"
{
	int num;
	unsigned long ulnum;
	l2tp_byte_vector_t val;
}
/* Line 1489 of yacc.c.  */
#line 345 "l2tp_config_parse.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;


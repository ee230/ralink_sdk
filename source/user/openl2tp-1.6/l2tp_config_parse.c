/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 2 "l2tp_config_parse.y"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <syslog.h>

#include "l2tp_config_types.h"

#include "l2tp_rpc.h"

static struct l2tp_api_system_msg_data system_config;
static struct l2tp_api_peer_profile_msg_data peer_profile;
static struct l2tp_api_tunnel_profile_msg_data tunnel_profile;
static struct l2tp_api_session_profile_msg_data session_profile;
static struct l2tp_api_ppp_profile_msg_data ppp_profile;
static struct l2tp_api_tunnel_msg_data tunnel;
static struct l2tp_api_session_msg_data session;

extern void l2tp_log(int level, char *fmt, ...);
extern void yyfatal(const char *s);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 35 "l2tp_config_parse.y"
{
	int num;
	unsigned long ulnum;
	l2tp_byte_vector_t val;
}
/* Line 187 of yacc.c.  */
#line 425 "l2tp_config_parse.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 438 "l2tp_config_parse.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   733

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  146
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  223
/* YYNRULES -- Number of states.  */
#define YYNSTATES  592

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   400

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     9,    11,    13,    15,    17,
      19,    21,    23,    28,    29,    32,    36,    40,    44,    48,
      52,    56,    60,    64,    68,    72,    74,    76,    82,    88,
      89,    92,    96,   100,   104,   108,   112,   116,   120,   124,
     126,   128,   134,   140,   141,   144,   148,   152,   156,   160,
     164,   168,   172,   176,   180,   184,   188,   192,   196,   200,
     204,   208,   212,   216,   220,   224,   228,   232,   236,   240,
     244,   248,   252,   254,   256,   262,   268,   269,   272,   276,
     280,   284,   288,   292,   296,   300,   304,   308,   312,   316,
     320,   324,   328,   332,   334,   336,   342,   348,   349,   352,
     356,   360,   364,   368,   372,   376,   380,   384,   388,   392,
     396,   400,   404,   408,   412,   416,   420,   424,   428,   432,
     436,   440,   444,   448,   452,   456,   460,   464,   468,   472,
     476,   480,   484,   488,   492,   496,   500,   504,   508,   512,
     516,   520,   524,   528,   532,   536,   538,   540,   545,   550,
     551,   554,   558,   562,   566,   570,   574,   578,   582,   586,
     590,   594,   598,   602,   606,   610,   614,   618,   622,   626,
     630,   634,   638,   642,   646,   650,   654,   658,   662,   666,
     670,   674,   678,   682,   684,   686,   691,   696,   697,   700,
     704,   708,   712,   716,   720,   724,   728,   732,   736,   740,
     744,   748,   752,   756,   760,   764,   768,   772,   776,   780,
     784,   788,   792,   796,   800,   804,   808,   812,   816,   820,
     824,   828,   832,   836
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     147,     0,    -1,    -1,   114,    -1,   147,   148,    -1,   149,
      -1,   152,    -1,   157,    -1,   162,    -1,   167,    -1,   172,
      -1,   177,    -1,     3,    10,   150,   114,    -1,    -1,   150,
     151,    -1,    20,   118,   119,    -1,    21,   118,   119,    -1,
      22,   118,   119,    -1,    23,   118,   123,    -1,    24,   118,
     119,    -1,    25,   118,   119,    -1,    26,   118,   119,    -1,
      27,   118,   119,    -1,    28,   118,   123,    -1,    29,   118,
     123,    -1,   153,    -1,   154,    -1,     7,     6,     9,   155,
     114,    -1,     7,     6,    10,   155,   114,    -1,    -1,   155,
     156,    -1,    63,   118,   120,    -1,    13,   118,   124,    -1,
      73,   118,   119,    -1,    79,   118,   124,    -1,    14,   118,
     120,    -1,    54,   118,   120,    -1,    56,   118,   120,    -1,
      57,   118,   120,    -1,   158,    -1,   159,    -1,     4,     6,
       9,   160,   114,    -1,     4,     6,    10,   160,   114,    -1,
      -1,   160,   161,    -1,    63,   118,   120,    -1,    30,   118,
     123,    -1,    36,   118,   120,    -1,    37,   118,   120,    -1,
      38,   118,   120,    -1,    39,   118,   123,    -1,    43,   118,
     119,    -1,    44,   118,   119,    -1,    45,   118,   119,    -1,
      46,   118,   119,    -1,    47,   118,   119,    -1,    48,   118,
     119,    -1,    52,   118,   120,    -1,    40,   118,   123,    -1,
      20,   118,   119,    -1,    42,   118,   123,    -1,    53,   118,
     120,    -1,    22,   118,   119,    -1,   142,   118,   124,    -1,
     143,   118,   124,    -1,    33,   118,   119,    -1,    34,   118,
     119,    -1,    55,   118,   120,    -1,    56,   118,   120,    -1,
      57,   118,   120,    -1,    49,   118,   123,    -1,    51,   118,
     119,    -1,   163,    -1,   164,    -1,     5,     6,     9,   165,
     114,    -1,     5,     6,    10,   165,   114,    -1,    -1,   165,
     166,    -1,    63,   118,   120,    -1,    20,   118,   119,    -1,
      64,   118,   123,    -1,    57,   118,   120,    -1,    59,   118,
     120,    -1,    62,   118,   120,    -1,    68,   118,   120,    -1,
      69,   118,   120,    -1,    70,   118,   119,    -1,    71,   118,
     119,    -1,    72,   118,   120,    -1,    41,   118,   123,    -1,
      65,   118,   123,    -1,    66,   118,   123,    -1,    67,   118,
     119,    -1,   168,    -1,   169,    -1,     8,     6,     9,   170,
     114,    -1,     8,     6,    10,   170,   114,    -1,    -1,   170,
     171,    -1,    63,   118,   120,    -1,    20,   118,   119,    -1,
      80,   118,   119,    -1,    81,   118,   119,    -1,    51,   118,
     119,    -1,   109,   118,   123,    -1,   110,   118,   120,    -1,
     135,   118,   123,    -1,   136,   118,   123,    -1,   137,   118,
     123,    -1,   138,   118,   123,    -1,   139,   118,   123,    -1,
     140,   118,   123,    -1,   141,   118,   123,    -1,    83,   118,
     120,    -1,    84,   118,   119,    -1,    85,   118,   119,    -1,
      86,   118,   119,    -1,    87,   118,   119,    -1,    88,   118,
     119,    -1,    89,   118,   119,    -1,    48,   118,   119,    -1,
      90,   118,   119,    -1,    91,   118,   119,    -1,    92,   118,
     119,    -1,    93,   118,   119,    -1,    94,   118,   119,    -1,
      95,   118,   119,    -1,    96,   118,   119,    -1,    97,   118,
     119,    -1,    98,   118,   119,    -1,    99,   118,   119,    -1,
     100,   118,   119,    -1,   101,   118,   119,    -1,   104,   118,
     124,    -1,   105,   118,   124,    -1,   106,   118,   124,    -1,
     107,   118,   124,    -1,   102,   118,   124,    -1,   103,   118,
     124,    -1,   108,   118,   120,    -1,   111,   118,   123,    -1,
     112,   118,   123,    -1,   113,   118,   123,    -1,   144,   118,
     120,    -1,   145,   118,   120,    -1,   173,    -1,   174,    -1,
       4,     9,   175,   114,    -1,     4,    10,   175,   114,    -1,
      -1,   175,   176,    -1,    30,   118,   123,    -1,   142,   118,
     124,    -1,   143,   118,   124,    -1,    35,   118,   119,    -1,
      33,   118,   119,    -1,    36,   118,   120,    -1,    37,   118,
     120,    -1,    38,   118,   120,    -1,    39,   118,   123,    -1,
      50,   118,   123,    -1,    43,   118,   119,    -1,    44,   118,
     119,    -1,    45,   118,   119,    -1,    46,   118,   119,    -1,
      47,   118,   119,    -1,    48,   118,   119,    -1,    34,   118,
     119,    -1,    52,   118,   120,    -1,    40,   118,   123,    -1,
      20,   118,   119,    -1,    42,   118,   123,    -1,    53,   118,
     120,    -1,    15,   118,   119,    -1,    63,   118,   120,    -1,
      22,   118,   119,    -1,    16,   118,   120,    -1,    55,   118,
     120,    -1,    56,   118,   120,    -1,    57,   118,   120,    -1,
      49,   118,   123,    -1,    51,   118,   119,    -1,    58,   118,
     120,    -1,   178,    -1,   179,    -1,     5,     9,   180,   114,
      -1,     5,    10,   180,   114,    -1,    -1,   180,   181,    -1,
      20,   118,   119,    -1,    64,   118,   123,    -1,    57,   118,
     120,    -1,    59,   118,   120,    -1,    62,   118,   120,    -1,
      68,   118,   120,    -1,    69,   118,   120,    -1,    70,   118,
     119,    -1,    71,   118,   119,    -1,    72,   118,   120,    -1,
      41,   118,   123,    -1,   129,   118,   120,    -1,   127,   118,
     120,    -1,   133,   118,   121,    -1,   130,   118,   121,    -1,
     126,   118,   120,    -1,   126,   118,   119,    -1,   132,   118,
     120,    -1,   132,   118,   119,    -1,   128,   118,   120,    -1,
     128,   118,   119,    -1,   125,   118,   121,    -1,   134,   118,
     121,    -1,   131,   118,   121,    -1,    60,   118,   120,    -1,
      61,   118,   120,    -1,    65,   118,   123,    -1,    66,   118,
     123,    -1,    67,   118,   119,    -1,    16,   118,   120,    -1,
      15,   118,   119,    -1,    18,   118,   120,    -1,    17,   118,
     119,    -1,    58,   118,   120,    -1,    63,   118,   120,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   178,   178,   180,   181,   187,   188,   189,   190,   191,
     192,   193,   197,   207,   209,   213,   218,   223,   228,   233,
     238,   243,   248,   253,   258,   266,   267,   271,   286,   300,
     302,   306,   310,   317,   322,   329,   346,   352,   358,   367,
     368,   372,   387,   401,   403,   407,   411,   416,   429,   445,
     461,   466,   471,   476,   481,   486,   491,   496,   502,   507,
     512,   517,   523,   528,   535,   542,   547,   552,   558,   564,
     570,   575,   583,   584,   588,   603,   617,   619,   623,   627,
     632,   637,   643,   659,   665,   682,   699,   704,   709,   726,
     731,   736,   741,   749,   750,   754,   769,   783,   785,   789,
     793,   798,   803,   808,   813,   818,   824,   833,   842,   851,
     860,   869,   878,   887,   900,   905,   910,   915,   920,   925,
     930,   935,   940,   945,   950,   955,   960,   965,   970,   975,
     980,   985,   990,   995,  1002,  1009,  1016,  1023,  1030,  1037,
    1043,  1048,  1053,  1058,  1064,  1073,  1074,  1078,  1091,  1103,
    1105,  1109,  1114,  1121,  1128,  1133,  1138,  1151,  1167,  1183,
    1188,  1193,  1198,  1203,  1208,  1213,  1218,  1223,  1228,  1234,
    1239,  1244,  1249,  1255,  1260,  1266,  1271,  1277,  1283,  1289,
    1295,  1300,  1305,  1314,  1315,  1319,  1332,  1344,  1346,  1350,
    1355,  1360,  1366,  1381,  1387,  1403,  1419,  1424,  1429,  1445,
    1450,  1467,  1473,  1479,  1485,  1491,  1499,  1505,  1513,  1519,
    1527,  1533,  1539,  1545,  1551,  1557,  1562,  1567,  1572,  1578,
    1582,  1588,  1592,  1598
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SYSTEM", "TUNNEL", "SESSION", "PROFILE",
  "PEER", "PPP", "CREATE", "MODIFY", "DIGITAL_ANALOG", "TUNNEL_AUTH_MODE",
  "PEER_IPADDR", "LAC_LNS", "TUNNEL_ID", "TUNNEL_NAME", "SESSION_ID",
  "SESSION_NAME", "UDP_PORT", "TRACE_FLAGS", "MAX_TUNNELS", "MAX_SESSIONS",
  "DRAIN_TUNNELS", "TUNNEL_ESTABLISH_TIMEOUT", "SESSION_ESTABLISH_TIMEOUT",
  "TUNNEL_PERSIST_PEND_TIMEOUT", "SESSION_PERSIST_PEND_TIMEOUT",
  "DENY_LOCAL_TUNNEL_CREATES", "DENY_REMOTE_TUNNEL_CREATES", "HIDE_AVPS",
  "OUR_ADDR", "PEER_ADDR", "OUR_UDP_PORT", "PEER_UDP_PORT", "CONFIG_ID",
  "AUTH_MODE", "FRAMING_CAP", "BEARER_CAP", "USE_TIEBREAKER",
  "ALLOW_PPP_PROXY", "USE_PPP_PROXY", "USE_UDP_CHECKSUMS", "HELLO_TIMEOUT",
  "MAX_RETRIES", "RX_WINDOW_SIZE", "TX_WINDOW_SIZE", "RETRY_TIMEOUT",
  "IDLE_TIMEOUT", "DO_PMTU_DISCOVERY", "PERSIST", "MTU", "SECRET",
  "HOST_NAME", "TUNNEL_PROFILE_NAME", "PEER_PROFILE_NAME",
  "SESSION_PROFILE_NAME", "PPP_PROFILE_NAME", "INTERFACE_NAME",
  "SESSION_TYPE", "USER_NAME", "USER_PASSWORD", "PRIV_GROUP_ID",
  "PROFILE_NAME", "SEQUENCING_REQUIRED", "USE_SEQUENCE_NUMBERS", "NO_PPP",
  "REORDER_TIMEOUT", "FRAMING_TYPE", "BEARER_TYPE", "MINIMUM_BPS",
  "MAXIMUM_BPS", "CONNECT_SPEED", "PEER_PORT", "WE_CAN_BE_LAC",
  "WE_CAN_BE_LNS", "DEFAULT_TUNNEL_PROFILE_NAME",
  "DEFAULT_SESSION_PROFILE_NAME", "DEFAULT_PPP_PROFILE_NAME", "NETMASK",
  "ASYNCMAP", "MRU", "AUTH_FLAGS", "SYNC_MODE", "CHAP_INTERVAL",
  "CHAP_MAX_CHALLENGE", "CHAP_RESTART", "PAP_MAX_AUTH_REQUESTS",
  "PAP_RESTART_INTERVAL", "PAP_TIMEOUT", "IPCP_MAX_CONFIG_REQUESTS",
  "IPCP_MAX_CONFIG_NAKS", "IPCP_MAX_TERMINATE_REQUESTS",
  "IPCP_RETRANSMIT_INTERVAL", "LCP_ECHO_FAILURE_COUNT",
  "LCP_ECHO_INTERVAL", "LCP_MAX_CONFIG_REQUESTS", "LCP_MAX_CONFIG_NAKS",
  "LCP_MAX_TERMINATE_REQUESTS", "LCP_RETRANSMIT_INTERVAL",
  "MAX_CONNECT_TIME", "MAX_FAILURE_COUNT", "LOCAL_IPADDR", "REMOTE_IPADDR",
  "DNS_IPADDR_PRI", "DNS_IPADDR_SEC", "WINS_IPADDR_PRI", "WINS_IPADDR_SEC",
  "IP_POOL_NAME", "USE_RADIUS", "RADIUS_HINT", "USE_AS_DEFAULT_ROUTE",
  "MULTILINK", "PROXY_ARP", "EOT", "SLASH", "BLCL", "ELCL", "EQUALS",
  "DECSTRING", "STRING", "HEXSTRING", "QUOTEDSTRING", "BOOL", "IPADDRESS",
  "INITIAL_RCVD_LCP_CONFREQ", "CALLING_NUMBER", "PROXY_AUTH_NAME",
  "SUB_ADDRESS", "PROXY_AUTH_TYPE", "PROXY_AUTH_RESPONSE",
  "LAST_RCVD_LCP_CONFREQ", "CALLED_NUMBER", "PROXY_AUTH_CHALLENGE",
  "LAST_SENT_LCP_CONFREQ", "AUTH_PAP", "AUTH_CHAP", "AUTH_MSCHAPV1",
  "AUTH_MSCHAPV2", "AUTH_EAP", "AUTH_NONE", "AUTH_PEER", "SRC_IPADDR",
  "DEST_IPADDR", "LOCAL_NAME", "REMOTE_NAME", "$accept", "commands",
  "command", "system_command", "system_statements", "system_statement",
  "peer_profile_command", "peer_profile_create_command",
  "peer_profile_modify_command", "peer_profile_statements",
  "peer_profile_statement", "tunnel_profile_command",
  "tunnel_profile_create_command", "tunnel_profile_modify_command",
  "tunnel_profile_statements", "tunnel_profile_statement",
  "session_profile_command", "session_profile_create_command",
  "session_profile_modify_command", "session_profile_statements",
  "session_profile_statement", "ppp_profile_command",
  "ppp_profile_create_command", "ppp_profile_modify_command",
  "ppp_profile_statements", "ppp_profile_statement", "tunnel_command",
  "tunnel_create_command", "tunnel_modify_command", "tunnel_statements",
  "tunnel_statement", "session_command", "session_create_command",
  "session_modify_command", "session_statements", "session_statement", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   146,   147,   147,   147,   148,   148,   148,   148,   148,
     148,   148,   149,   150,   150,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   152,   152,   153,   154,   155,
     155,   156,   156,   156,   156,   156,   156,   156,   156,   157,
     157,   158,   159,   160,   160,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   162,   162,   163,   164,   165,   165,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   167,   167,   168,   169,   170,   170,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   172,   172,   173,   174,   175,
     175,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   177,   177,   178,   179,   180,   180,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     4,     0,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     5,     5,     0,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     5,     5,     0,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     1,     5,     5,     0,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     5,     5,     0,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     4,     4,     0,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     4,     4,     0,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     3,     0,     1,     0,     0,     0,     0,     0,     4,
       5,     6,    25,    26,     7,    39,    40,     8,    72,    73,
       9,    93,    94,    10,   145,   146,    11,   183,   184,    13,
       0,   149,   149,     0,   187,   187,     0,     0,     0,    43,
      43,     0,     0,    76,    76,     0,     0,    29,    29,    97,
      97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    12,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   147,     0,     0,   150,   148,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   185,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   188,   186,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    41,     0,     0,    44,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    74,    77,
      75,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    27,    30,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    95,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    98,    96,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     173,   176,   170,   175,   151,   155,   167,   154,   156,   157,
     158,   159,   169,   171,   161,   162,   163,   164,   165,   166,
     180,   160,   181,   168,   172,   177,   178,   179,   182,   174,
     152,   153,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   219,   218,   221,
     220,   189,   199,   191,   222,   192,   213,   214,   193,   223,
     190,   215,   216,   217,   194,   195,   196,   197,   198,   210,
     205,   204,   201,   209,   208,   200,   203,   212,   207,   206,
     202,   211,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    62,    46,    65,
      66,    47,    48,    49,    50,    58,    60,    51,    52,    53,
      54,    55,    56,    70,    71,    57,    61,    67,    68,    69,
      45,    63,    64,    79,    89,    81,    82,    83,    78,    80,
      90,    91,    92,    84,    85,    86,    87,    88,    32,    35,
      36,    37,    38,    31,    33,    34,   100,   120,   103,    99,
     101,   102,   113,   114,   115,   116,   117,   118,   119,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   137,   138,   133,   134,   135,   136,   139,   104,   105,
     140,   141,   142,   106,   107,   108,   109,   110,   111,   112,
     143,   144
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     9,    10,    38,    62,    11,    12,    13,   137,
     272,    14,    15,    16,    63,   179,    17,    18,    19,   100,
     229,    20,    21,    22,   139,   321,    23,    24,    25,    41,
      98,    26,    27,    28,    45,   135
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -111
static const yytype_int16 yypact[] =
{
    -110,  -111,     6,  -111,    -2,    99,   105,    20,    33,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
      -7,  -111,  -111,     7,  -111,  -111,    37,    45,   109,  -111,
    -111,   -15,    29,  -111,  -111,   414,   447,  -111,  -111,  -111,
    -111,  -106,   -68,   -62,   -48,   -35,   -24,   -21,   -14,   -12,
      -8,  -111,  -111,   314,   352,     1,     2,     3,     4,     8,
      21,    22,    24,    27,    28,    30,    31,    32,    34,    36,
      41,    42,    43,    46,    47,    48,    49,    50,    51,    52,
      57,    91,    92,    94,    95,  -111,    96,    97,  -111,  -111,
     382,   529,    98,   100,   101,   102,   103,   104,   106,   107,
     108,   117,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   134,  -111,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,  -111,  -111,    39,    44,    93,
     188,   -16,    -3,     5,    40,    38,   145,   146,   147,    88,
     180,   149,   152,   186,   187,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   212,   213,   217,   219,   220,  -111,   221,   222,  -111,
    -111,   223,    35,   224,   226,   183,   227,   230,   236,   244,
     248,   253,   218,   252,   255,   257,   260,   261,   262,   264,
     265,   270,   283,   268,   290,   291,   292,   293,   294,   296,
     297,    23,   295,   300,   302,   303,   304,   306,   307,   308,
     309,   315,   317,   318,   319,   320,   322,   324,  -111,  -111,
    -111,   339,   323,   340,   341,   349,   337,   350,   367,   369,
     370,   371,   372,   373,   346,   374,   375,   380,   381,   383,
     401,   402,   403,   379,   -59,   404,   -31,   405,   406,   408,
     -29,   409,   410,   384,   415,   416,   417,   418,   419,   420,
     432,  -111,  -111,  -111,   433,   434,   435,   436,   437,   438,
     439,   440,   441,   442,   444,   445,   446,   448,   449,   450,
     451,   453,   464,   465,   466,   467,   469,   471,   472,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,  -111,   496,   497,   498,   499,   500,   501,   502,   503,
     504,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,   407,   413,   399,   505,   506,   507,   508,
     509,   510,   511,   512,   513,   517,   518,   519,   520,   521,
     522,   523,   524,   526,   527,   528,   530,   531,   525,   532,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,   533,   534,   535,   538,   539,   540,   541,   542,
     543,   544,   547,   548,   550,   551,   552,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,   537,   553,   554,   555,   556,   557,   559,   558,
     560,   561,   562,   563,   565,   566,   567,   569,   570,   571,
     572,   573,   574,   575,   576,   577,   578,   579,   580,   581,
     582,   583,   584,   585,   586,   587,   588,   589,   590,   591,
     592,   597,   595,   599,   598,   600,   601,   602,   603,   604,
     605,   606,   607,   608,   612,   613,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,    77,
    -111,  -111,  -111,  -111,   111,  -111,  -111,  -111,  -111,   118,
    -111,  -111,  -111,  -111,   167,  -111,  -111,  -111,  -111,   231,
    -111,  -111,  -111,  -111,   596,  -111
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      65,    66,    39,    40,     1,    67,     3,    68,    29,     4,
       5,     6,   141,     7,     8,    69,    43,    44,    70,    71,
      72,    73,    74,    75,    76,    77,    36,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    37,
      90,    91,    92,    93,    65,    66,    47,    48,    94,    67,
     142,    68,   263,   264,    49,    50,   143,   263,   264,    69,
     430,   431,    70,    71,    72,    73,    74,    75,    76,    77,
     144,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,   145,    90,    91,    92,    93,   433,   434,
     438,   439,    94,   265,   146,   266,   267,   147,   265,    95,
     266,   267,   268,   323,   148,    30,   149,   268,    31,    32,
     150,    33,   269,   274,    34,    35,   324,   269,   270,   181,
     182,   183,   184,   270,   325,   138,   185,    96,    97,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,   186,
     187,   275,   188,    99,   276,   189,   190,   390,   191,   192,
     193,    64,   194,   271,   195,   361,   277,   327,   273,   196,
     197,   198,   101,   326,   199,   200,   201,   202,   203,   204,
     205,    96,    97,   278,   279,   206,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   274,   207,
     208,   331,   209,   210,   211,   212,   231,   140,   232,   233,
     234,   235,   236,    61,   237,   238,   239,     0,   312,   313,
     314,   315,   316,   317,   318,   240,   275,   319,   320,   276,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   277,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,    42,   328,   329,   330,   333,   278,   279,
     334,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   322,   332,   335,   336,   364,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   312,   313,   314,   315,   316,   317,   318,
     353,   354,   319,   320,   151,   355,   152,   356,   357,   358,
     359,   371,   360,   362,   153,   363,   365,   154,   155,   366,
     156,   157,   158,   159,   160,   367,   161,   162,   163,   164,
     165,   166,   167,   168,   368,   169,   170,   171,   369,   172,
     173,   174,   151,   370,   152,   372,   374,   175,   373,   375,
     376,   377,   153,   378,   379,   154,   155,   382,   156,   157,
     158,   159,   160,   380,   161,   162,   163,   164,   165,   166,
     167,   168,   213,   169,   170,   171,   381,   172,   173,   174,
     383,   384,   385,   386,   387,   175,   388,   389,   392,   391,
     393,   394,   395,   214,   396,   397,   398,   399,   176,   102,
     103,   104,   105,   400,   106,   401,   402,   403,   404,   215,
     405,   216,   406,   408,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   107,   177,   178,   407,   409,
     412,   410,   102,   103,   104,   105,   180,   106,   411,   420,
     413,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   414,   107,   415,
     416,   417,   418,   419,   177,   178,   228,   421,   422,   423,
     429,   424,   442,   425,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     426,   427,   498,   428,   432,   435,   496,   436,   124,   437,
     440,   441,   497,   443,   444,   445,   446,   447,   448,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   213,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   136,   460,   461,   462,     0,   463,   464,   465,   466,
     214,   467,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   468,   469,   470,   471,   215,   472,   216,   473,
     474,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   475,   476,   477,   478,   479,   480,   481,   482,
     483,   484,   485,   486,   487,   488,   489,   490,   491,   492,
     493,   494,   495,     0,   499,   500,     0,   501,   502,   503,
       0,    46,   507,   504,   505,   506,   508,   509,   510,   511,
     512,     0,   514,   230,   515,   513,   516,   517,   518,   521,
     519,   520,   523,     0,     0,   525,   522,   524,   526,   527,
     528,   538,     0,   532,   529,   530,   531,   533,   534,   535,
     536,     0,   537,   539,   540,   541,   542,   543,   544,   546,
     547,   548,   545,   549,   550,   551,     0,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,     0,     0,     0,     0,
       0,   571,   572,   573,   574,   575,   576,   577,   578,   579,
       0,   580,     0,   581,   582,   583,   584,   585,   586,   587,
     588,   589,   590,   591
};

static const yytype_int16 yycheck[] =
{
      15,    16,     9,    10,   114,    20,     0,    22,    10,     3,
       4,     5,   118,     7,     8,    30,     9,    10,    33,    34,
      35,    36,    37,    38,    39,    40,     6,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,     6,
      55,    56,    57,    58,    15,    16,     9,    10,    63,    20,
     118,    22,    13,    14,     9,    10,   118,    13,    14,    30,
     119,   120,    33,    34,    35,    36,    37,    38,    39,    40,
     118,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,   118,    55,    56,    57,    58,   119,   120,
     119,   120,    63,    54,   118,    56,    57,   118,    54,   114,
      56,    57,    63,   119,   118,     6,   118,    63,     9,    10,
     118,     6,    73,    20,     9,    10,   119,    73,    79,   118,
     118,   118,   118,    79,   119,    48,   118,   142,   143,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,   118,
     118,    48,   118,   114,    51,   118,   118,   124,   118,   118,
     118,    40,   118,   114,   118,   120,    63,   119,   114,   118,
     118,   118,    44,   123,   118,   118,   118,   118,   118,   118,
     118,   142,   143,    80,    81,   118,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,    20,   118,
     118,   123,   118,   118,   118,   118,   118,    50,   118,   118,
     118,   118,   118,   114,   118,   118,   118,    -1,   135,   136,
     137,   138,   139,   140,   141,   118,    48,   144,   145,    51,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,    63,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,    32,   119,   119,   119,   118,    80,    81,
     118,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   123,   118,   118,   123,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   135,   136,   137,   138,   139,   140,   141,
     118,   118,   144,   145,    20,   118,    22,   118,   118,   118,
     118,   123,   119,   119,    30,   119,   119,    33,    34,   119,
      36,    37,    38,    39,    40,   119,    42,    43,    44,    45,
      46,    47,    48,    49,   120,    51,    52,    53,   120,    55,
      56,    57,    20,   120,    22,   123,   119,    63,   123,   119,
     119,   119,    30,   119,   119,    33,    34,   119,    36,    37,
      38,    39,    40,   123,    42,    43,    44,    45,    46,    47,
      48,    49,    20,    51,    52,    53,   123,    55,    56,    57,
     120,   120,   120,   120,   120,    63,   120,   120,   118,   124,
     118,   118,   118,    41,   118,   118,   118,   118,   114,    15,
      16,    17,    18,   118,    20,   118,   118,   118,   118,    57,
     118,    59,   118,   120,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    41,   142,   143,   119,   119,
     123,   120,    15,    16,    17,    18,   114,    20,   119,   123,
     120,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,   120,    41,   120,
     120,   120,   120,   120,   142,   143,   114,   123,   123,   119,
     121,   120,   118,   120,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
     119,   119,   123,   120,   120,   120,   119,   121,   114,   121,
     121,   121,   119,   118,   118,   118,   118,   118,   118,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,    20,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   114,   118,   118,   118,    -1,   118,   118,   118,   118,
      41,   118,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   118,   118,   118,   118,    57,   118,    59,   118,
     118,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,    -1,   119,   119,    -1,   120,   120,   120,
      -1,    35,   119,   123,   123,   123,   119,   119,   119,   119,
     119,    -1,   119,   114,   120,   123,   120,   120,   120,   124,
     120,   120,   119,    -1,    -1,   120,   124,   123,   120,   120,
     120,   124,    -1,   119,   123,   123,   123,   120,   120,   119,
     119,    -1,   120,   120,   120,   120,   120,   120,   119,   119,
     119,   119,   124,   120,   119,   119,    -1,   120,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,    -1,    -1,    -1,    -1,
      -1,   124,   124,   124,   124,   124,   124,   120,   123,   120,
      -1,   123,    -1,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   120,   120
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   114,   147,     0,     3,     4,     5,     7,     8,   148,
     149,   152,   153,   154,   157,   158,   159,   162,   163,   164,
     167,   168,   169,   172,   173,   174,   177,   178,   179,    10,
       6,     9,    10,     6,     9,    10,     6,     6,   150,     9,
      10,   175,   175,     9,    10,   180,   180,     9,    10,     9,
      10,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,   114,   151,   160,   160,    15,    16,    20,    22,    30,
      33,    34,    35,    36,    37,    38,    39,    40,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      55,    56,    57,    58,    63,   114,   142,   143,   176,   114,
     165,   165,    15,    16,    17,    18,    20,    41,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,   114,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   181,   114,   155,   155,   170,
     170,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,    20,    22,    30,    33,    34,    36,    37,    38,    39,
      40,    42,    43,    44,    45,    46,    47,    48,    49,    51,
      52,    53,    55,    56,    57,    63,   114,   142,   143,   161,
     114,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,    20,    41,    57,    59,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,   114,   166,
     114,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,    13,    14,    54,    56,    57,    63,    73,
      79,   114,   156,   114,    20,    48,    51,    63,    80,    81,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   135,   136,   137,   138,   139,   140,   141,   144,
     145,   171,   114,   119,   119,   119,   123,   119,   119,   119,
     119,   123,   123,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     119,   120,   119,   119,   123,   119,   119,   119,   120,   120,
     120,   123,   123,   123,   119,   119,   119,   119,   119,   119,
     123,   123,   119,   120,   120,   120,   120,   120,   120,   120,
     124,   124,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   119,   120,   119,
     120,   119,   123,   120,   120,   120,   120,   120,   120,   120,
     123,   123,   123,   119,   120,   120,   119,   119,   120,   121,
     119,   120,   120,   119,   120,   120,   121,   121,   119,   120,
     121,   121,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   119,   119,   123,   119,
     119,   120,   120,   120,   123,   123,   123,   119,   119,   119,
     119,   119,   119,   123,   119,   120,   120,   120,   120,   120,
     120,   124,   124,   119,   123,   120,   120,   120,   120,   123,
     123,   123,   119,   120,   120,   119,   119,   120,   124,   120,
     120,   120,   120,   120,   119,   124,   119,   119,   119,   120,
     119,   119,   120,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   124,   124,   124,   124,   124,   124,   120,   123,   120,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     120,   120
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 182 "l2tp_config_parse.y"
    {
		;}
    break;

  case 12:
#line 198 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			status = l2tp_system_modify_1_svc(system_config, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "system: command failed: rc=%d", l2tp_strerror(-result));
		;}
    break;

  case 15:
#line 214 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_TRACE_FLAGS;
			system_config.config.trace_flags = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 16:
#line 219 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_MAX_TUNNELS;
			system_config.config.max_tunnels = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 17:
#line 224 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_MAX_SESSIONS;
			system_config.config.max_sessions = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 18:
#line 229 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_DRAIN_TUNNELS;
			system_config.config.drain_tunnels = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 19:
#line 234 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_TUNNEL_ESTABLISH_TIMEOUT;
			system_config.config.tunnel_establish_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 20:
#line 239 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_SESSION_ESTABLISH_TIMEOUT;
			system_config.config.session_establish_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 21:
#line 244 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_TUNNEL_PERSIST_PEND_TIMEOUT;
			system_config.config.tunnel_persist_pend_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 22:
#line 249 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_SESSION_PERSIST_PEND_TIMEOUT;
			system_config.config.session_persist_pend_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 23:
#line 254 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_DENY_LOCAL_TUNNEL_CREATES;
			system_config.config.deny_local_tunnel_creates = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 24:
#line 259 "l2tp_config_parse.y"
    {
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_DENY_REMOTE_TUNNEL_CREATES;
			system_config.config.deny_remote_tunnel_creates = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 27:
#line 272 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (peer_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_peer_profile_create_1_svc(peer_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "peer profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&peer_profile, 0, sizeof(peer_profile));
		;}
    break;

  case 28:
#line 287 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (peer_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_peer_profile_modify_1_svc(peer_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "peer profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&peer_profile, 0, sizeof(peer_profile));
		;}
    break;

  case 31:
#line 307 "l2tp_config_parse.y"
    {
			peer_profile.profile_name = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 32:
#line 311 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR;
			peer_profile.peer_addr.s_addr = addr.s_addr;
		;}
    break;

  case 33:
#line 318 "l2tp_config_parse.y"
    {
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_PEER_PORT;
			peer_profile.peer_port = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 34:
#line 323 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_NETMASK;
			peer_profile.netmask.s_addr = addr.s_addr;
		;}
    break;

  case 35:
#line 330 "l2tp_config_parse.y"
    {
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_LACLNS;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "lac") == 0) {
				peer_profile.we_can_be_lac = 1;
				peer_profile.we_can_be_lns = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "lns") == 0) {
				peer_profile.we_can_be_lac = 0;
				peer_profile.we_can_be_lns = 1;
			} else if ((strcasecmp((yyvsp[(3) - (3)].val).buf, "laclns") == 0) ||
				   (strcasecmp((yyvsp[(3) - (3)].val).buf, "lnslac") == 0)) {
				peer_profile.we_can_be_lac = 1;
				peer_profile.we_can_be_lns = 1;
			} else {
				yyfatal("invalid lac_lns value");
			}
		;}
    break;

  case 36:
#line 347 "l2tp_config_parse.y"
    {
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME;
			peer_profile.default_tunnel_profile_name.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			peer_profile.default_tunnel_profile_name.valid = 1;
		;}
    break;

  case 37:
#line 353 "l2tp_config_parse.y"
    {
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME;
			peer_profile.default_session_profile_name.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			peer_profile.default_session_profile_name.valid = 1;
		;}
    break;

  case 38:
#line 359 "l2tp_config_parse.y"
    {
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME;
			peer_profile.default_ppp_profile_name.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			peer_profile.default_ppp_profile_name.valid = 1;
		;}
    break;

  case 41:
#line 373 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (tunnel_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_tunnel_profile_create_1_svc(tunnel_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "tunnel profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel_profile, 0, sizeof(tunnel_profile));
		;}
    break;

  case 42:
#line 388 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (tunnel_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_tunnel_profile_modify_1_svc(tunnel_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "tunnel profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel_profile, 0, sizeof(tunnel_profile));
		;}
    break;

  case 45:
#line 408 "l2tp_config_parse.y"
    {
			tunnel_profile.profile_name = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 46:
#line 412 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS;
			tunnel_profile.hide_avps = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 47:
#line 417 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "none") == 0) {
				tunnel_profile.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_NONE;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "simple") == 0) {
				tunnel_profile.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_SIMPLE;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "challenge") == 0) {
				tunnel_profile.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE;
			} else {
				yyfatal("invalid auth_mode");
			}
		;}
    break;

  case 48:
#line 430 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "sync") == 0) {
				tunnel_profile.framing_cap_sync = 1;
				tunnel_profile.framing_cap_async = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "async") == 0) {
				tunnel_profile.framing_cap_sync = 0;
				tunnel_profile.framing_cap_async = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				tunnel_profile.framing_cap_sync = 1;
				tunnel_profile.framing_cap_async = 1;
			} else {
				yyfatal("invalid framing_cap");
			}
		;}
    break;

  case 49:
#line 446 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "analog") == 0) {
				tunnel_profile.bearer_cap_analog = 1;
				tunnel_profile.bearer_cap_digital = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "digital") == 0) {
				tunnel_profile.bearer_cap_analog = 0;
				tunnel_profile.bearer_cap_digital = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				tunnel_profile.bearer_cap_analog = 1;
				tunnel_profile.bearer_cap_digital = 1;
			} else {
				yyfatal("invalid bearer_cap");
			}
		;}
    break;

  case 50:
#line 462 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER;
			tunnel_profile.use_tiebreaker = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 51:
#line 467 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT;
			tunnel_profile.hello_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 52:
#line 472 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES;
			tunnel_profile.max_retries = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 53:
#line 477 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE;
			tunnel_profile.rx_window_size = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 54:
#line 482 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE;
			tunnel_profile.tx_window_size = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 55:
#line 487 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT;
			tunnel_profile.retry_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 56:
#line 492 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT;
			tunnel_profile.idle_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 57:
#line 497 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SECRET;
			tunnel_profile.secret.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			tunnel_profile.secret.valid = 1;
		;}
    break;

  case 58:
#line 503 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY;
			tunnel_profile.allow_ppp_proxy = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 59:
#line 508 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS;
			tunnel_profile.trace_flags = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 60:
#line 513 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS;
			tunnel_profile.use_udp_checksums = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 61:
#line 518 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME;
			tunnel_profile.host_name.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			tunnel_profile.host_name.valid = 1;
		;}
    break;

  case 62:
#line 524 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS;
			tunnel_profile.max_sessions = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 63:
#line 529 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			tunnel_profile.our_addr.s_addr = addr.s_addr;
		;}
    break;

  case 64:
#line 536 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			tunnel_profile.peer_addr.s_addr = addr.s_addr;
		;}
    break;

  case 65:
#line 543 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT;
			tunnel_profile.our_udp_port = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 66:
#line 548 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT;
			tunnel_profile.peer_udp_port = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 67:
#line 553 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME;
			tunnel_profile.peer_profile_name.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			tunnel_profile.peer_profile_name.valid = 1;
		;}
    break;

  case 68:
#line 559 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME;
			tunnel_profile.session_profile_name.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			tunnel_profile.session_profile_name.valid = 1;
		;}
    break;

  case 69:
#line 565 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME;
			tunnel_profile.ppp_profile_name.optstring_u.value = (yyvsp[(3) - (3)].val).buf;
			tunnel_profile.ppp_profile_name.valid = 1;
		;}
    break;

  case 70:
#line 571 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY;
			tunnel_profile.do_pmtu_discovery = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 71:
#line 576 "l2tp_config_parse.y"
    {
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MTU;
			tunnel_profile.mtu = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 74:
#line 589 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (session_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_session_profile_create_1_svc(session_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "session profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session_profile, 0, sizeof(session_profile));
		;}
    break;

  case 75:
#line 604 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (session_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_session_profile_modify_1_svc(session_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "session profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session_profile, 0, sizeof(session_profile));
		;}
    break;

  case 78:
#line 624 "l2tp_config_parse.y"
    {
			session_profile.profile_name = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 79:
#line 628 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS;
			session_profile.trace_flags = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 80:
#line 633 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_SEQUENCING_REQUIRED;
			session_profile.sequencing_required = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 81:
#line 638 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_PPP_PROFILE_NAME;
			OPTSTRING(session_profile.ppp_profile_name) = (yyvsp[(3) - (3)].val).buf;
			session_profile.ppp_profile_name.valid = 1;
		;}
    break;

  case 82:
#line 644 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_SESSION_TYPE;
			session_profile.flags |= L2TP_API_SESSION_FLAG_SESSION_TYPE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "laic") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LAIC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "laoc") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LAOC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "lnic") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LNIC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "lnoc") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LNOC;
			} else {
				yyfatal("Bad session type: expecting laic|laoc|lnic|lnoc");
			}
		;}
    break;

  case 83:
#line 660 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_PRIV_GROUP_ID;
			OPTSTRING(session_profile.priv_group_id) = (yyvsp[(3) - (3)].val).buf;
			session_profile.priv_group_id.valid = 1;
		;}
    break;

  case 84:
#line 666 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_FRAMING_TYPE;
			session_profile.flags |= L2TP_API_SESSION_FLAG_FRAMING_TYPE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "sync") == 0) {
				session_profile.framing_type_sync = 1;
				session_profile.framing_type_async = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "async") == 0) {
				session_profile.framing_type_sync = 0;
				session_profile.framing_type_async = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				session_profile.framing_type_sync = 1;
				session_profile.framing_type_async = 1;
			} else {
				yyfatal("invalid framing_type");
			}
		;}
    break;

  case 85:
#line 683 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_BEARER_TYPE;
			session_profile.flags |= L2TP_API_SESSION_FLAG_BEARER_TYPE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "analog") == 0) {
				session_profile.bearer_type_analog = 1;
				session_profile.bearer_type_digital = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "digital") == 0) {
				session_profile.bearer_type_analog = 0;
				session_profile.bearer_type_digital = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				session_profile.bearer_type_analog = 1;
				session_profile.bearer_type_digital = 1;
			} else {
				yyfatal("invalid bearer_type");
			}
		;}
    break;

  case 86:
#line 700 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_MINIMUM_BPS;
			session_profile.minimum_bps = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 87:
#line 705 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_MAXIMUM_BPS;
			session_profile.maximum_bps = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 88:
#line 710 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_CONNECT_SPEED;
			int ints[2];
			int num_matches;
			session_profile.flags |= L2TP_API_SESSION_FLAG_CONNECT_SPEED;
			num_matches = sscanf((yyvsp[(3) - (3)].val).buf, "%d:%d", &ints[0], &ints[1]);
			if (num_matches >= 1) {
				session_profile.rx_connect_speed = ints[0];
				session_profile.tx_connect_speed = ints[0];
				if (num_matches == 2) {
					session_profile.tx_connect_speed = ints[1];
				}
			} else {
				yyfatal("Expecting connect_speed[:tx_connect_speed]");
			}
		;}
    break;

  case 89:
#line 727 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_USE_PPP_PROXY;
			session_profile.use_ppp_proxy = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 90:
#line 732 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_USE_SEQUENCE_NUMBERS;
			session_profile.use_sequence_numbers = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 91:
#line 737 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_NO_PPP;
			session_profile.no_ppp = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 92:
#line 742 "l2tp_config_parse.y"
    {
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_REORDER_TIMEOUT;
			session_profile.reorder_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 95:
#line 755 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (ppp_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_ppp_profile_create_1_svc(ppp_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "ppp profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&ppp_profile, 0, sizeof(ppp_profile));
		;}
    break;

  case 96:
#line 770 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			if (ppp_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_ppp_profile_modify_1_svc(ppp_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "ppp profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&ppp_profile, 0, sizeof(ppp_profile));
		;}
    break;

  case 99:
#line 790 "l2tp_config_parse.y"
    {
			ppp_profile.profile_name = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 100:
#line 794 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS;
			ppp_profile.trace_flags = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 101:
#line 799 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP;
			ppp_profile.asyncmap = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 102:
#line 804 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MRU;
			ppp_profile.mru = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 103:
#line 809 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MTU;
			ppp_profile.mtu = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 104:
#line 814 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS;
			ppp_profile.use_radius = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 105:
#line 819 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT;
			OPTSTRING(ppp_profile.radius_hint) = (yyvsp[(3) - (3)].val).buf;
			ppp_profile.radius_hint.valid = 1;
		;}
    break;

  case 106:
#line 825 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP;
			if ((yyvsp[(3) - (3)].num)) {
				ppp_profile.auth_refuse_pap = 0;
			} else {
				ppp_profile.auth_refuse_pap = -1;
			}
		;}
    break;

  case 107:
#line 834 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP;
			if ((yyvsp[(3) - (3)].num)) {
				ppp_profile.auth_refuse_chap = 0;
			} else {
				ppp_profile.auth_refuse_chap = -1;
			}
		;}
    break;

  case 108:
#line 843 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP;
			if ((yyvsp[(3) - (3)].num)) {
				ppp_profile.auth_refuse_mschap = 0;
			} else {
				ppp_profile.auth_refuse_mschap = -1;
			}
		;}
    break;

  case 109:
#line 852 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2;
			if ((yyvsp[(3) - (3)].num)) {
				ppp_profile.auth_refuse_mschapv2 = 0;
			} else {
				ppp_profile.auth_refuse_mschapv2 = -1;
			}
		;}
    break;

  case 110:
#line 861 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP;
			if ((yyvsp[(3) - (3)].num)) {
				ppp_profile.auth_refuse_eap = 0;
			} else {
				ppp_profile.auth_refuse_eap = -1;
			}
		;}
    break;

  case 111:
#line 870 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE;
			if ((yyvsp[(3) - (3)].num)) {
				ppp_profile.auth_none = -1;
			} else {
				ppp_profile.auth_none = 0;
			}
		;}
    break;

  case 112:
#line 879 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER;
			if ((yyvsp[(3) - (3)].num)) {
				ppp_profile.auth_peer = -1;
			} else {
				ppp_profile.auth_peer = 0;
			}
		;}
    break;

  case 113:
#line 888 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "sync") == 0) {
				ppp_profile.sync_mode = L2TP_API_PPP_SYNCMODE_SYNC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "async") == 0) {
				ppp_profile.sync_mode = L2TP_API_PPP_SYNCMODE_ASYNC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				ppp_profile.sync_mode = L2TP_API_PPP_SYNCMODE_SYNC_ASYNC;
			} else {
				yyfatal("Bad sync mode: expecting sync|async|any");
			}
		;}
    break;

  case 114:
#line 901 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL;
			ppp_profile.chap_interval = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 115:
#line 906 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE;
			ppp_profile.chap_max_challenge = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 116:
#line 911 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART;
			ppp_profile.chap_restart = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 117:
#line 916 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS;
			ppp_profile.pap_max_auth_requests = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 118:
#line 921 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL;
			ppp_profile.pap_restart_interval = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 119:
#line 926 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT;
			ppp_profile.pap_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 120:
#line 931 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT;
			ppp_profile.idle_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 121:
#line 936 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS;
			ppp_profile.ipcp_max_config_requests = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 122:
#line 941 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS;
			ppp_profile.ipcp_max_config_naks = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 123:
#line 946 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS;
			ppp_profile.lcp_max_terminate_requests = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 124:
#line 951 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL;
			ppp_profile.ipcp_retransmit_interval = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 125:
#line 956 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT;
			ppp_profile.lcp_echo_failure_count = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 126:
#line 961 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL;
			ppp_profile.lcp_echo_interval = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 127:
#line 966 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS;
			ppp_profile.lcp_max_config_requests = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 128:
#line 971 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS;
			ppp_profile.lcp_max_config_naks = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 129:
#line 976 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS;
			ppp_profile.lcp_max_terminate_requests = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 130:
#line 981 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL;
			ppp_profile.lcp_retransmit_interval = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 131:
#line 986 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME;
			ppp_profile.max_connect_time = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 132:
#line 991 "l2tp_config_parse.y"
    {
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT;
			ppp_profile.max_failure_count = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 133:
#line 996 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			ppp_profile.dns_addr_1.s_addr = addr.s_addr;
		;}
    break;

  case 134:
#line 1003 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			ppp_profile.dns_addr_2.s_addr = addr.s_addr;
		;}
    break;

  case 135:
#line 1010 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			ppp_profile.wins_addr_1.s_addr = addr.s_addr;
		;}
    break;

  case 136:
#line 1017 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			ppp_profile.wins_addr_2.s_addr = addr.s_addr;
		;}
    break;

  case 137:
#line 1024 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			ppp_profile.local_ip_addr.s_addr = addr.s_addr;
		;}
    break;

  case 138:
#line 1031 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			ppp_profile.peer_ip_addr.s_addr = addr.s_addr;
		;}
    break;

  case 139:
#line 1038 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME;
			OPTSTRING(ppp_profile.ip_pool_name) = (yyvsp[(3) - (3)].val).buf;
			ppp_profile.ip_pool_name.valid = 1;
		;}
    break;

  case 140:
#line 1044 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE;
			ppp_profile.use_as_default_route = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 141:
#line 1049 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_MULTILINK;
			ppp_profile.multilink = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 142:
#line 1054 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP;
			ppp_profile.proxy_arp = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 143:
#line 1059 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME;
			OPTSTRING(ppp_profile.local_name) = (yyvsp[(3) - (3)].val).buf;
			ppp_profile.local_name.valid = 1;
		;}
    break;

  case 144:
#line 1065 "l2tp_config_parse.y"
    {
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME;
			OPTSTRING(ppp_profile.remote_name) = (yyvsp[(3) - (3)].val).buf;
			ppp_profile.remote_name.valid = 1;
		;}
    break;

  case 147:
#line 1079 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			status = l2tp_tunnel_create_1_svc(tunnel, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_TUNNEL_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "tunnel create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel, 0, sizeof(tunnel));
		;}
    break;

  case 148:
#line 1092 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			status = l2tp_tunnel_modify_1_svc(tunnel, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "tunnel modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel, 0, sizeof(tunnel));
		;}
    break;

  case 151:
#line 1110 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_HIDE_AVPS;
			tunnel.hide_avps = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 152:
#line 1115 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_OUR_ADDR;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			tunnel.our_addr.s_addr = addr.s_addr;
		;}
    break;

  case 153:
#line 1122 "l2tp_config_parse.y"
    {
			struct in_addr addr;
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PEER_ADDR;
			addr.s_addr = htonl((yyvsp[(3) - (3)].ulnum));
			tunnel.peer_addr.s_addr = addr.s_addr;
		;}
    break;

  case 154:
#line 1129 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_CONFIG_ID;
			tunnel.config_id = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 155:
#line 1134 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_OUR_UDP_PORT;
			tunnel.our_udp_port = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 156:
#line 1139 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_AUTH_MODE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "none") == 0) {
				tunnel.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_NONE;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "simple") == 0) {
				tunnel.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_SIMPLE;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "challenge") == 0) {
				tunnel.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE;
			} else {
				yyfatal("invalid auth_mode");
			}
		;}
    break;

  case 157:
#line 1152 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_FRAMING_CAP;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "sync") == 0) {
				tunnel.framing_cap_sync = 1;
				tunnel.framing_cap_async = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "async") == 0) {
				tunnel.framing_cap_sync = 0;
				tunnel.framing_cap_async = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				tunnel.framing_cap_sync = 1;
				tunnel.framing_cap_async = 1;
			} else {
				yyfatal("invalid framing_cap");
			}
		;}
    break;

  case 158:
#line 1168 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_BEARER_CAP;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "analog") == 0) {
				tunnel.bearer_cap_analog = 1;
				tunnel.bearer_cap_digital = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "digital") == 0) {
				tunnel.bearer_cap_analog = 0;
				tunnel.bearer_cap_digital = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				tunnel.bearer_cap_analog = 1;
				tunnel.bearer_cap_digital = 1;
			} else {
				yyfatal("invalid bearer_cap");
			}
		;}
    break;

  case 159:
#line 1184 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_USE_TIEBREAKER;
			tunnel.use_tiebreaker = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 160:
#line 1189 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PERSIST;
			tunnel.persist = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 161:
#line 1194 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_HELLO_TIMEOUT;
			tunnel.hello_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 162:
#line 1199 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_MAX_RETRIES;
			tunnel.max_retries = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 163:
#line 1204 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_RX_WINDOW_SIZE;
			tunnel.rx_window_size = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 164:
#line 1209 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TX_WINDOW_SIZE;
			tunnel.tx_window_size = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 165:
#line 1214 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_RETRY_TIMEOUT;
			tunnel.retry_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 166:
#line 1219 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_IDLE_TIMEOUT;
			tunnel.idle_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 167:
#line 1224 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PEER_UDP_PORT;
			tunnel.peer_udp_port = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 168:
#line 1229 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_SECRET;
			OPTSTRING(tunnel.secret) = (yyvsp[(3) - (3)].val).buf;
			tunnel.secret.valid = 1;
		;}
    break;

  case 169:
#line 1235 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_ALLOW_PPP_PROXY;
			tunnel.allow_ppp_proxy = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 170:
#line 1240 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TRACE_FLAGS;
			tunnel.trace_flags = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 171:
#line 1245 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_USE_UDP_CHECKSUMS;
			tunnel.use_udp_checksums = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 172:
#line 1250 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_HOST_NAME;
			OPTSTRING(tunnel.host_name) = (yyvsp[(3) - (3)].val).buf;
			tunnel.host_name.valid = 1;
		;}
    break;

  case 173:
#line 1256 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TUNNEL_ID;
			tunnel.tunnel_id = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 174:
#line 1261 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PROFILE_NAME;
			OPTSTRING(tunnel.tunnel_profile_name) = (yyvsp[(3) - (3)].val).buf;
			tunnel.tunnel_profile_name.valid = 1;
		;}
    break;

  case 175:
#line 1267 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_MAX_SESSIONS;
			tunnel.max_sessions = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 176:
#line 1272 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TUNNEL_NAME;
			OPTSTRING(tunnel.tunnel_name) = (yyvsp[(3) - (3)].val).buf;
			tunnel.tunnel_name.valid = 1;
		;}
    break;

  case 177:
#line 1278 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PEER_PROFILE_NAME;
			OPTSTRING(tunnel.peer_profile_name) = (yyvsp[(3) - (3)].val).buf;
			tunnel.peer_profile_name.valid = 1;
		;}
    break;

  case 178:
#line 1284 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_SESSION_PROFILE_NAME;
			OPTSTRING(tunnel.session_profile_name) = (yyvsp[(3) - (3)].val).buf;
			tunnel.session_profile_name.valid = 1;
		;}
    break;

  case 179:
#line 1290 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PPP_PROFILE_NAME;
			OPTSTRING(tunnel.ppp_profile_name) = (yyvsp[(3) - (3)].val).buf;
			tunnel.ppp_profile_name.valid = 1;
		;}
    break;

  case 180:
#line 1296 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_DO_PMTU_DISCOVERY;
			tunnel.do_pmtu_discovery = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 181:
#line 1301 "l2tp_config_parse.y"
    {
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_MTU;
			tunnel.mtu = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 182:
#line 1306 "l2tp_config_parse.y"
    {
			tunnel.flags2 |= L2TP_API_TUNNEL_FLAG_INTERFACE_NAME;
			OPTSTRING(tunnel.interface_name) = (yyvsp[(3) - (3)].val).buf;
			tunnel.interface_name.valid = 1;
		;}
    break;

  case 185:
#line 1320 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			status = l2tp_session_create_1_svc(session, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_SESSION_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "session create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session, 0, sizeof(session));
		;}
    break;

  case 186:
#line 1333 "l2tp_config_parse.y"
    {
			bool_t status;
			int result;
			status = l2tp_session_modify_1_svc(session, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "session modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session, 0, sizeof(session));
		;}
    break;

  case 189:
#line 1351 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_TRACE_FLAGS;
			session.trace_flags = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 190:
#line 1356 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_SEQUENCING_REQUIRED;
			session.sequencing_required = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 191:
#line 1361 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_PPP_PROFILE_NAME;
			OPTSTRING(session.ppp_profile_name) = (yyvsp[(3) - (3)].val).buf;
			session.ppp_profile_name.valid = 1;
		;}
    break;

  case 192:
#line 1367 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_SESSION_TYPE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "laic") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LAIC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "laoc") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LAOC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "lnic") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LNIC;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "lnoc") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LNOC;
			} else {
				yyfatal("Bad session type: expecting laic|laoc|lnic|lnoc");
			}
		;}
    break;

  case 193:
#line 1382 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_PRIV_GROUP_ID;
			OPTSTRING(session.priv_group_id) = (yyvsp[(3) - (3)].val).buf;
			session.priv_group_id.valid = 1;
		;}
    break;

  case 194:
#line 1388 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_FRAMING_TYPE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "sync") == 0) {
				session.framing_type_sync = 1;
				session.framing_type_async = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "async") == 0) {
				session.framing_type_sync = 0;
				session.framing_type_async = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				session.framing_type_sync = 1;
				session.framing_type_async = 1;
			} else {
				yyfatal("invalid framing_type");
			}
		;}
    break;

  case 195:
#line 1404 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_BEARER_TYPE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "analog") == 0) {
				session.bearer_type_analog = 1;
				session.bearer_type_digital = 0;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "digital") == 0) {
				session.bearer_type_analog = 0;
				session.bearer_type_digital = 1;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "any") == 0) {
				session.bearer_type_analog = 1;
				session.bearer_type_digital = 1;
			} else {
				yyfatal("invalid bearer_type");
			}
		;}
    break;

  case 196:
#line 1420 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_MINIMUM_BPS;
			session.minimum_bps = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 197:
#line 1425 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_MAXIMUM_BPS;
			session.maximum_bps = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 198:
#line 1430 "l2tp_config_parse.y"
    {
			int ints[2];
			int num_matches;
			session.flags |= L2TP_API_SESSION_FLAG_CONNECT_SPEED;
			num_matches = sscanf((yyvsp[(3) - (3)].val).buf, "%d:%d", &ints[0], &ints[1]);
			if (num_matches >= 1) {
				session.rx_connect_speed = ints[0];
				session.tx_connect_speed = ints[0];
				if (num_matches == 2) {
					session.tx_connect_speed = ints[1];
				}
			} else {
				yyfatal("Expecting connect_speed[:tx_connect_speed]");
			}
		;}
    break;

  case 199:
#line 1446 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_USE_PPP_PROXY;
			session.use_ppp_proxy = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 200:
#line 1451 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_TYPE;
			if (strcasecmp((yyvsp[(3) - (3)].val).buf, "text") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PLAIN_TEXT;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "chap") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_CHAP;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "pap") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_PAP;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "none") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_NO_AUTH;
			} else if (strcasecmp((yyvsp[(3) - (3)].val).buf, "mschap") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_MSCHAPV1;
			} else {
				yyfatal("Bad auth type: expecting none|text|chap|pap|mschap");
			}
		;}
    break;

  case 201:
#line 1468 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_NAME;
			OPTSTRING(session.proxy_auth_name) = (yyvsp[(3) - (3)].val).buf;
			session.proxy_auth_name.valid = 1;
		;}
    break;

  case 202:
#line 1474 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_CHALLENGE;
			session.proxy_auth_challenge.proxy_auth_challenge_len = (yyvsp[(3) - (3)].val).len;
			session.proxy_auth_challenge.proxy_auth_challenge_val = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 203:
#line 1480 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_RESPONSE;
			session.proxy_auth_response.proxy_auth_response_len = (yyvsp[(3) - (3)].val).len;
			session.proxy_auth_response.proxy_auth_response_val = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 204:
#line 1486 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_CALLING_NUMBER;
			OPTSTRING(session.calling_number) = (yyvsp[(3) - (3)].val).buf;
			session.calling_number.valid = 1;
		;}
    break;

  case 205:
#line 1492 "l2tp_config_parse.y"
    {
			char str[16];
			session.flags |= L2TP_API_SESSION_FLAG_CALLING_NUMBER;
			sprintf(str, "%lu", (yyvsp[(3) - (3)].ulnum));
			OPTSTRING(session.calling_number) = strdup(str);
			session.calling_number.valid = 1;
		;}
    break;

  case 206:
#line 1500 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_CALLED_NUMBER;
			OPTSTRING(session.called_number) = (yyvsp[(3) - (3)].val).buf;
			session.called_number.valid = 1;
		;}
    break;

  case 207:
#line 1506 "l2tp_config_parse.y"
    {
			char str[16];
			session.flags |= L2TP_API_SESSION_FLAG_CALLED_NUMBER;
			sprintf(str, "%lu", (yyvsp[(3) - (3)].ulnum));
			OPTSTRING(session.called_number) = strdup(str);
			session.called_number.valid = 1;
		;}
    break;

  case 208:
#line 1514 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_SUB_ADDRESS;
			OPTSTRING(session.sub_address) = (yyvsp[(3) - (3)].val).buf;
			session.sub_address.valid = 1;
		;}
    break;

  case 209:
#line 1520 "l2tp_config_parse.y"
    {
			char str[16];
			session.flags |= L2TP_API_SESSION_FLAG_SUB_ADDRESS;
			sprintf(str, "%lu", (yyvsp[(3) - (3)].ulnum));
			OPTSTRING(session.sub_address) = strdup(str);
			session.sub_address.valid = 1;
		;}
    break;

  case 210:
#line 1528 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_INITIAL_RCVD_LCP_CONFREQ;
			session.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_len = (yyvsp[(3) - (3)].val).len;
			session.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 211:
#line 1534 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_LAST_SENT_LCP_CONFREQ;
			session.last_sent_lcp_confreq.last_sent_lcp_confreq_len = (yyvsp[(3) - (3)].val).len;
			session.last_sent_lcp_confreq.last_sent_lcp_confreq_val = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 212:
#line 1540 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_LAST_RCVD_LCP_CONFREQ;
			session.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_len = (yyvsp[(3) - (3)].val).len;
			session.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val = (yyvsp[(3) - (3)].val).buf;
		;}
    break;

  case 213:
#line 1546 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_USER_NAME;
			OPTSTRING(session.user_name) = (yyvsp[(3) - (3)].val).buf;
			session.user_name.valid = 1;
		;}
    break;

  case 214:
#line 1552 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_USER_PASSWORD;
			OPTSTRING(session.user_password) = (yyvsp[(3) - (3)].val).buf;
			session.user_password.valid = 1;
		;}
    break;

  case 215:
#line 1558 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_USE_SEQUENCE_NUMBERS;
			session.use_sequence_numbers = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 216:
#line 1563 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_NO_PPP;
			session.no_ppp = (yyvsp[(3) - (3)].num);
		;}
    break;

  case 217:
#line 1568 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_REORDER_TIMEOUT;
			session.reorder_timeout = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 218:
#line 1573 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_TUNNEL_NAME;
			OPTSTRING(session.tunnel_name) = (yyvsp[(3) - (3)].val).buf;
			session.tunnel_name.valid = 1;
		;}
    break;

  case 219:
#line 1579 "l2tp_config_parse.y"
    {
			session.tunnel_id = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 220:
#line 1583 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_SESSION_NAME;
			OPTSTRING(session.session_name) = (yyvsp[(3) - (3)].val).buf;
			session.session_name.valid = 1;
		;}
    break;

  case 221:
#line 1589 "l2tp_config_parse.y"
    {
			session.session_id = (yyvsp[(3) - (3)].ulnum);
		;}
    break;

  case 222:
#line 1593 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_INTERFACE_NAME;
			OPTSTRING(session.interface_name) = (yyvsp[(3) - (3)].val).buf;
			session.interface_name.valid = 1;
		;}
    break;

  case 223:
#line 1599 "l2tp_config_parse.y"
    {
			session.flags |= L2TP_API_SESSION_FLAG_PROFILE_NAME;
			OPTSTRING(session.profile_name) = (yyvsp[(3) - (3)].val).buf;
			session.profile_name.valid = 1;
		;}
    break;


/* Line 1267 of yacc.c.  */
#line 4045 "l2tp_config_parse.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1606 "l2tp_config_parse.y"




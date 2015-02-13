/***********************************************************************
 * connect.c -- Make socket connection using SOCKS4/5 and HTTP tunnel.
 *
 * Copyright (c) 2000, 2001 Shun-ichi Goto
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * ---------------------------------------------------------
 * PROJECT:  My Test Program
 * AUTHOR:   Shun-ichi GOTO <gotoh@taiyo.co.jp>
 * CREATE:   Wed Jun 21, 2000
 * REVISION: $Revision: 619 $
 * ---------------------------------------------------------
 *
 * Get Source:
 *
 *   Recent version of 'connect.c' is available from
 *     http://www.imasy.or.jp/~gotoh/ssh/connect.c
 *
 *   Pre-compiled biniary for Win32 is also available:
 *     http://www.imasy.or.jp/~gotoh/ssh/connect.exe.gz
 *
 *   Related tool, ssh-askpass.exe (alternative ssh-askpass on UNIX)
 *   is available:
 *     http://www.imasy.or.jp/~gotoh/ssh/ssh-askpass.exe.gz
 *     
 * How To Compile:
 *
 *  On UNIX environment:
 *      $ gcc connect.c -o connect
 * 
 *  On SOLARIS:
 *      $gcc -o connect -lresolv -lsocket -lnsl connect.c
 *
 *  on Win32 environment:
 *      $ cl connect.c wsock32.lib advapi32.lib
 *    or
 *      $ bcc32 connect.c wsock32.lib advapi32.lib
 *
 * How To Use:
 *   You can specify proxy method by environment variable or command
 *   line option.
 * 
 *   usage:  connect [-dnhs45] [-R resolve] [-p local-port]
 *                   [-H [user@]proxy-server:port]
 *                   [-S [user@]socks-server[:port]]
 *                   host port
 *
 *   "host" and "port" is for target hostname and port-number to connect.
 *   The -p option will forward a local TCP port instead of using the
 *   standard input and output.
 *
 *   -H option specify hostname and port number of http proxy server to
 *   relay. If port is omitted, 80 is used. You can specify this value by
 *   environment variable HTTP_PROXY and give -h option to use it.
 *
 *   -S option specify hostname and port number of SOCKS server to relay.
 *   Like -H, port number can be omit and default is 1080. You can also
 *   specify this value pair by environment variable SOCKS5_SERVER and
 *   give -s option to use it.
 *
 *   -4 and -5 is for specifying SOCKS protocol version. It is valid only
 *   using with -s or -S. Default is -5 (protocol version 5)
 *
 *   -R is for specifying method to resolve hostname. 3 keywords ("local",
 *   "remote", "both") or dot-notation IP address is acceptable.  Keyword
 *   "both" means, "Try local first, then remote". If dot-notation IP
 *   address is specified, use this host as nameserver. Default is
 *   "remote" for SOCKS5 or "local" for others. On SOCKS4 protocol, remote
 *   resolving method ("remote" and "both") requires protocol 4a supported
 *   server.
 *
 *   -d option is used for debug. If you fail to connect, use this and
 *   check request to and response from server.
 *
 *   You can omit "port" argument when program name is special format
 *   containing port number itself. For example, 
 *     $ ln -s connect connect-25
 *   means this connect-25 command is spcifying port number 25 already
 *   so you need not 2nd argument (and ignored if specified).
 *
 *   To use proxy, this example is for SOCKS5 connection to connect to
 *   'host' at port 25 via SOCKS5 server on 'firewall' host.
 *     $ connect -S firewall  host 25
 *   or
 *     $ SOCKS5_SERVER=firewall; export SOCKS5_SERVER
 *     $ connect -s host 25
 *
 *   And this is for HTTP-PROXY connection:
 *     $ connect -H proxy-server:8080  host 25
 *   or
 *     $ HTTP_PROXY=proxy-server:8080; export HTTP_PROXY
 *     $ connect -h host 25
 *   To forward a local port, for example to use ssh:
 *     $ connect -p 5550 -H proxy-server:8080  host 22
 *    ($ ssh -l user -p 5550 localhost )
 *
 * SOCKS5 authentication:
 *
 *   Only USER/PASS authentication is supported.
 *
 * Proxy authentication:
 *
 *   Only BASIC scheme is supported.
 *
 * Authentication informations:
 * 
 *   User name for authentication is specifed by environment variable or
 *   system login name.  And Password is specified from environment
 *   variable or external program (specified in $SSH_ASKPASS) or tty.
 *
 *   Following environment variable is used for specifying user name.
 *     SOCKS: $SOCKS5_USER, $LOGNAME, $USER
 *     HTTP Proxy: $HTTP_PROXY_USER, $LOGNAME, $USER
 *
 * ssh-askpass support:
 *
 *   You can use ssh-askpass (came from OpenSSH or else) to specify
 *   password on graphical environment (X-Window or MS Windows). To use
 *   this, set program name to environment variable SSH_ASKPASS. On UNIX,
 *   X-Window must be required, so $DISPLAY environment variable is also
 *   needed.  On Win32 environment, $DISPLAY is not mentioned.
 *
 * For Your Information
 *
 *   SOCKS5 -- RFC 1928, RFC 1929, RFC 1961
 *             NEC SOCKS Reference Implementation is available from:
 *               http://www.socks.nec.com
 *             DeleGate version 5 or earlier can be SOCKS4 server,
 *             and version 6 canbe SOCKS5 and SOCKS4 server.
 *               http://www.delegate.org/delegate/
 *
 *   HTTP-Proxy --
 *             Many http proxy servers supports this, but https should
 *             be allowed as configuration on your host.
 *             For example on DeleGate, you should add "https" to 
 *             "REMITTABLE" parameter to allow HTTP-Proxy like this:
 *               delegated -Pxxxx ...... REMITTABLE="+,https" ...
 *
 *  Hypertext Transfer Protocol -- HTTP/1.1  -- RFC 2616
 *  HTTP Authentication: Basic and Digest Access Authentication -- RFC 2617
 *             For proxy authentication, refer these documents.
 *
 ***********************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>

#ifdef __CYGWIN32__
#undef _WIN32
#endif

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#include <sys/stat.h>
#include <io.h>
#include <conio.h>
#else /* !_WIN32 */
#include <unistd.h>
#include <pwd.h>
#include <termios.h>
#include <sys/time.h>
#ifndef __hpux
#include <sys/select.h>
#endif /* __hpux */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifndef __CYGWIN32__
#include <arpa/nameser.h>
#include <resolv.h>
#endif /* !__CYGWIN32__ */
#endif /* !_WIN32 */

#ifndef HAVE_SOCKLEN_T
  typedef int socklen_t;
#endif

 /* available authentication types */
#define SOCKS_ALLOW_NO_AUTH
#define SOCKS_ALLOW_USERPASS_AUTH

 /* consider Borland C */
#ifdef __BORLANDC__
#define _kbhit kbhit
#endif

 /* help message */
 static char *usage =
 "usage: %s [-dnhs45] [-R resolve] \n"
 "          [-H proxy-server[:port]] [-S [user@]socks-server[:port]] \n"
 "          host port\n";

 /* name of this program */
 char *progname = NULL;

 /* options */
 int f_debug = 0;

 /* local input type */
#define LOCAL_STDIO	0
#define LOCAL_SOCKET	1
 char *local_type_names[] = { "stdio", "socket" };
int   local_type = LOCAL_STDIO;
u_short local_port = 0;

/* relay method, server and port */
#define METHOD_UNDECIDED 0
#define METHOD_DIRECT    1
#define METHOD_SOCKS     2
#define METHOD_HTTP      3
char *method_names[] = { "UNDECIDED", "DIRECT", "SOCKS", "HTTP" };

int   relay_method = METHOD_UNDECIDED;		/* relaying method */
char *relay_host = NULL;			/* hostname of relay server */
u_short relay_port = 0;				/* port of relay server */
char *relay_user = NULL;			/* user name for auth */

/* destination target host and port */
char *dest_host = NULL;
struct in_addr dest_addr;
u_short dest_port = 0;

/* informations for SOCKS */
#define SOCKS5_REP_SUCCEEDED	0x00	/* succeeded */
#define SOCKS5_REP_FAIL		0x01	/* general SOCKS serer failure */
#define SOCKS5_REP_NALLOWED	0x02	/* connection not allowed by ruleset */
#define SOCKS5_REP_NUNREACH	0x03	/* Network unreachable */
#define SOCKS5_REP_HUNREACH	0x04	/* Host unreachable */
#define SOCKS5_REP_REFUSED	0x05	/* conenction refused */
#define SOCKS5_REP_EXPIRED	0x06	/* TTL expired */
#define SOCKS5_REP_CNOTSUP	0x07	/* Command not supported */
#define SOCKS5_REP_ANOTSUP	0x08	/* Address not supported */
#define SOCKS5_REP_INVADDR	0x09	/* Inalid address */
/* SOCKS5 authentication methods */
#define SOCKS5_AUTH_REJECT	0xFF	/* No acceptable auth method */
#define SOCKS5_AUTH_NOAUTH	0x00	/* without authentication */
#define SOCKS5_AUTH_GSSAPI	0x01	/* GSSAPI */
#define SOCKS5_AUTH_USERPASS	0x02	/* User/Password */
#define SOCKS5_AUTH_CHAP	0x03	/* Challenge-Handshake Auth Proto. */
#define SOCKS5_AUTH_EAP		0x05	/* Extensible Authentication Proto. */
#define SOCKS5_AUTH_MAF		0x08	/* Multi-Authentication Framework */

#define SOCKS4_REP_SUCCEEDED	90	/* rquest granted (succeeded) */
#define SOCKS4_REP_REJECTED	91	/* request rejected or failed */
#define SOCKS4_REP_IDENT_FAIL	92	/* cannot connect identd */
#define SOCKS4_REP_USERID	93	/* user id not matched */

#define RESOLVE_UNKNOWN 0
#define RESOLVE_LOCAL   1
#define RESOLVE_REMOTE  2
#define RESOLVE_BOTH    3
char *resolve_names[] = { "UNKNOWN", "LOCAL", "REMOTE", "BOTH" };

int socks_version = 5;				/* SOCKS protocol version */
int socks_resolve = RESOLVE_UNKNOWN;
struct in_addr socks_ns;

/* Environment variable names */
#define ENV_SOCKS_SERVER  "SOCKS_SERVER"	/* SOCKS server */
#define ENV_SOCKS5_SERVER "SOCKS5_SERVER"
#define ENV_SOCKS4_SERVER "SOCKS4_SERVER"

#define ENV_SOCKS_RESOLVE  "SOCKS_RESOLVE"	/* resolve method */
#define ENV_SOCKS5_RESOLVE "SOCKS5_RESOLVE"
#define ENV_SOCKS4_RESOLVE "SOCKS4_RESOLVE"

#define ENV_SOCKS5_USER	     "SOCKS5_USER"	/* auth user for SOCKS5 */
#define ENV_SOCKS5_PASSWORD "SOCKS5_PASSWORD"	/* auth password for SOCKS5 */

#define ENV_HTTP_PROXY          "HTTP_PROXY"	/* common env var */
#define ENV_HTTP_PROXY_USER     "HTTP_PROXY_USER" /* auth user */
#define ENV_HTTP_PROXY_PASSWORD "HTTP_PROXY_PASSWORD" /* auth password */

#define ENV_CONNECT_USER     "CONNECT_USER"	/* default auth user name */
#define ENV_CONNECT_PASSWORD "CONNECT_PASSWORD"	/* default auth password */

#define ENV_SSH_ASKPASS "SSH_ASKPASS"		/* askpass program */

/* Prefix string of HTTP_PROXY */
#define HTTP_PROXY_PREFIX "http://"
#define PROXY_AUTH_NONE 0
#define PROXY_AUTH_BASIC 1
#define PROXY_AUTH_DIGEST 2
int proxy_auth_type = PROXY_AUTH_NONE;


/* return value of relay start function. */
#define START_ERROR -1
#define START_OK     0
#define START_RETRY  1

/* socket related definitions */
#ifndef _WIN32
#define SOCKET int
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#ifndef FD_ALLOC
#define FD_ALLOC(nfds) ((fd_set*)malloc((nfds+7)/8))
#endif /* !FD_ALLOC */

#ifdef _WIN32
#define socket_errno() WSAGetLastError()
#else /* !_WIN32 */
#define closesocket close
#define socket_errno() (errno)
#endif /* !_WIN32 */

#ifdef _WIN32
#define popen _popen
#endif /* WIN32 */

/* packet operation macro */
#define PUT_BYTE(ptr,data) (*(unsigned char*)ptr = data)

/* debug message output */
    void
debug( const char *fmt, ... )
{
    va_list args;
    if ( f_debug ) {
        va_start( args, fmt );
        fprintf(stderr, "DEBUG: ");
        vfprintf( stderr, fmt, args );
        va_end( args );
    }
}

    void
debug_( const char *fmt, ... )			/* without prefix */
{
    va_list args;
    if ( f_debug ) {
        va_start( args, fmt );
        vfprintf( stderr, fmt, args );
        va_end( args );
    }
}

/* error message output */
    void
error( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    fprintf(stderr, "ERROR: ");
    vfprintf( stderr, fmt, args );
    va_end( args );
}

    void
fatal( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    fprintf(stderr, "FATAL: ");
    vfprintf( stderr, fmt, args );
    va_end( args );
    exit (EXIT_FAILURE);
}

    void
downcase( char *buf )
{
    while ( *buf ) {
        if ( isupper(*buf) )
            *buf += 'a'-'A';
        buf++;
    }
}

    int
lookup_resolve( const char *str )
{
    char *buf = strdup( str );
    int ret;

    downcase( buf );
    if ( strcmp( buf, "both" ) == 0 )
        ret = RESOLVE_BOTH;
    else if ( strcmp( buf, "remote" ) == 0 )
        ret = RESOLVE_REMOTE;
    else if ( strcmp( buf, "local" ) == 0 )
        ret = RESOLVE_LOCAL;
    else if ( strspn(buf, "0123456789.") == strlen(buf) ) {
#if defined(_WIN32) || defined(__CYGWIN32__)
        fatal("Sorry, you can't specify name resolve host with -R option on Win32 environment.");
#endif /* _WIN32 || __CYGWIN32__ */
        ret = RESOLVE_LOCAL;			/* this case is also 'local' */
        socks_ns.s_addr = inet_addr(buf);
    }
    else
        ret = RESOLVE_UNKNOWN;
    free(buf);
    return ret;
}

    char *
getusername(void)
{
#ifdef _WIN32
    static char buf[1024];
    DWORD size = sizeof(buf);
    buf[0] = '\0';
    GetUserName( buf, &size);
    return buf;
#else  /* not _WIN32 */
    struct passwd *pw = getpwuid(getuid());
    if ( pw == NULL )
        fatal("getpwuid() failed for uid: %d\n", getuid());
    return pw->pw_name;
#endif /* not _WIN32 */
}

/** TTY operation **/


int intr_flag = 0;

#ifndef _WIN32
    void
intr_handler(/* int sig */)
{
    intr_flag = 1;
}

    void
tty_change_echo(int fd, int enable)
{
    static struct termios ntio, otio;		/* new/old termios */
    static sigset_t nset, oset;			/* new/old sigset */
    static struct sigaction nsa, osa;		/* new/old sigaction */
    static int disabled = 0;

    if ( disabled && enable ) {
        /* enable echo */
        tcsetattr(fd, TCSANOW, &otio);
        disabled = 0;
        /* resotore sigaction */
        sigprocmask(SIG_SETMASK, &oset, NULL);
        sigaction(SIGINT, &osa, NULL);
        if ( intr_flag != 0 ) {
            /* re-generate signal  */
            kill(getpid(), SIGINT);
            sigemptyset(&nset);
            sigsuspend(&nset);
            intr_flag = 0;
        }
    } else if (!disabled && !enable) {
        /* set SIGINTR handler and break syscall on singal */
        sigemptyset(&nset);
        sigaddset(&nset, SIGTSTP);
        sigprocmask(SIG_BLOCK, &nset, &oset);
        intr_flag = 0;
        memset(&nsa, 0, sizeof(nsa));
        nsa.sa_handler = intr_handler;
        sigaction(SIGINT, &nsa, &osa);
        /* disable echo */
        if (tcgetattr(fd, &otio) == 0 && (otio.c_lflag & ECHO)) {
            disabled = 1;
            ntio = otio;
            ntio.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
            (void) tcsetattr(fd, TCSANOW, &ntio);
        }
    }

    return;
}

#define TTY_NAME "/dev/tty"
    int
tty_readpass( const char *prompt, char *buf, size_t size )
{
    int tty, ret;

    if (0 >= size)
        return -1;				/* no room */
    tty = open(TTY_NAME, O_RDWR);
    if (0 > tty) {
        error("Unable to open %s\n", TTY_NAME);
        return -1;				/* can't open tty */
    }
    if (write(tty, prompt, strlen(prompt)) != (int)strlen(prompt)) {
        close(tty);
        return -1;
    }
    buf[0] = '\0';
    tty_change_echo(tty, 0);			/* disable echo */
    ret = read(tty,buf, size-1);
    tty_change_echo(tty, 1);			/* restore */
    if (write(tty, "\n", 1) != 1) {			/* new line */
        close(tty);
        return -1;
    }
    close(tty);
    if (NULL == strchr(buf,'\n'))
        return -1;
    if (0 <= ret)
        buf[ret] = '\0';
    return ret;
}

#else  /* _WIN32 */

    BOOL __stdcall
w32_intr_handler(DWORD dwCtrlType)
{
    if ( dwCtrlType == CTRL_C_EVENT ) {
        intr_flag = 1;
        return TRUE;
    } else {
        return FALSE;
    }
}

#define tty_readpass w32_tty_readpass
    int
w32_tty_readpass( const char *prompt, char *buf, size_t size )
{
    HANDLE in  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode, bytes;
    int ret;

    WriteFile(out, prompt, strlen(prompt), &bytes, 0);
    SetConsoleCtrlHandler(w32_intr_handler, TRUE ); /* add handler */
    GetConsoleMode(in, &mode);
    SetConsoleMode(in, mode&~ENABLE_ECHO_INPUT); /* disable echo */
    ret = ReadFile(in, buf, size, &bytes, 0);
    SetConsoleMode(in, mode);			/* enable echo */
    SetConsoleCtrlHandler( w32_intr_handler, FALSE ); /* remove handler */
    if ( intr_flag )
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0); /* re-signal */
    puts("");
    return ret;
}

#endif /* _WIN32 */

/* expect
   check STR is begin with substr with case-ignored comparison.
   Return 1 if matched, otherwise 0.
   */
    int
expect( char *str, char *substr)
{
    int len = strlen(substr);
    while ( 0 < len-- ) {
        if ( toupper(*str) != toupper(*substr) )
            return 0;				/* not matched */
        str++, substr++;
    }
    return 1;			/* good, matched */
}

/*** network operations ***/

/* set_relay()
   Determine relay informations:
   method, host, port, and username.
   1st arg, METHOD should be METHOD_xxx.
   2nd arg, SPEC is hostname or hostname:port or user@hostame:port.
   hostname is domain name or dot notation.
   If port is omitted, use 80 for METHOD_HTTP method,
   use 1080 for METHOD_SOCKS method.
   Username is also able to given by 3rd. format.
   2nd argument SPEC can be NULL. if NULL, use environment variable.
   */
    int
set_relay( int method, char *spec )
{
    char *buf, *sep, *resolve;

    relay_method = method;

    switch ( method ) {
        case METHOD_DIRECT:
            return -1;				/* nothing to do */

        case METHOD_SOCKS:
            if ( spec == NULL ) {
                switch ( socks_version ) {
                    case 5:
                        spec = getenv(ENV_SOCKS5_SERVER);
                        break;
                    case 4:
                        spec = getenv(ENV_SOCKS4_SERVER);
                        break;
                }
            }
            if ( spec == NULL )
                spec = getenv(ENV_SOCKS_SERVER);

            if ( spec == NULL )
                fatal("Failed to determine SOCKS server.\n");
            relay_port = 1080;			/* set default first */

            /* determine resolve method */
            if ( socks_resolve == RESOLVE_UNKNOWN ) {
                if ( ((socks_version == 5) &&
                            ((resolve = getenv(ENV_SOCKS5_RESOLVE)) != NULL)) ||
                        ((socks_version == 4) &&
                         ((resolve = getenv(ENV_SOCKS4_RESOLVE)) != NULL)) ||
                        ((resolve = getenv(ENV_SOCKS_RESOLVE)) != NULL) ) {
                    socks_resolve = lookup_resolve( resolve );
                    if ( socks_resolve == RESOLVE_UNKNOWN )
                        fatal("Invalid resolve method: %s\n", resolve);
                } else {
                    /* default */
                    if ( socks_version == 5 )
                        socks_resolve = RESOLVE_REMOTE;
                    else
                        socks_resolve = RESOLVE_LOCAL;
                }
            }
            break;

        case METHOD_HTTP:
            if ( spec == NULL )
                spec = getenv(ENV_HTTP_PROXY);
            if ( spec == NULL )
                fatal("You must specify http proxy server\n");
            relay_port = 80;			/* set default first */
            break;
    }

    if (expect( spec, HTTP_PROXY_PREFIX)) {
        /* URL format like: "http://server:port/" */
        /* extract server:port part */
        buf = strdup( spec + strlen(HTTP_PROXY_PREFIX));
        buf[strcspn(buf, "/")] = '\0';
    } else {
        /* assume spec is aready "server:port" format */
        buf = strdup( spec );
    }
    spec = buf;

    /* check username in spec */ 
    sep = strchr( spec, '@' );
    if ( sep != NULL ) {
        *sep = '\0';
        relay_user = strdup( spec );
        spec = sep +1;
    }
    if ( (relay_user == NULL) &&
            ((relay_user = getenv("LOGNAME")) == NULL) &&
            ((relay_user = getenv("USER")) == NULL) &&
            ((relay_user = getusername()) == NULL)) {
        /* get username from system */
        debug("Cannot determine your username.");
    }

    /* split out hostname and port number from spec */
    sep = strchr(spec,':');
    if ( sep == NULL ) {
        /* hostname only, port is already set as default */
        relay_host = strdup( spec );
    } else {
        /* hostname and port */
        relay_port = atoi(sep+1);
        *sep = '\0';
        relay_host = strdup( spec );
    }
    free(buf);
    return 0;
}


    u_short
resolve_port( const char *service )
{
    int port;
    if ( service[strspn (service, "0123456789")] == '\0'  ) {
        /* all digits, port number */
        port = atoi(service);
    } else {
        /* treat as service name */
        struct servent *ent;
        ent = getservbyname( service, NULL );
        if ( ent == NULL ) {
            debug("Unknown service, '%s'\n", service);
            port = 0;
        } else {
            port = ntohs(ent->s_port);
            debug("service: %s => %d\n", service, port);
        }
    }
    return (u_short)port;
}

    int
getarg( int argc, char **argv )
{
    int err = 0;
    char *ptr, *server = (char*)NULL;
    int method = METHOD_DIRECT;

    progname = *argv;
    argc--, argv++;

    /* check optinos */
    while ( (0 < argc) && (**argv == '-') ) {
        ptr = *argv + 1;
        while ( *ptr ) {
            switch ( *ptr ) {
                case 's':				/* use SOCKS */
                    method = METHOD_SOCKS;
                    break;

                case 'n':				/* no proxy */
                    method = METHOD_DIRECT;
                    break;

                case 'h':				/* use http-proxy */
                    method = METHOD_HTTP;
                    break;

                case 'S':				/* specify SOCKS server */
                    if ( 0 < argc ) {
                        argv++, argc--;
                        method = METHOD_SOCKS;
                        server = *argv;
                    } else {
                        error("option '-%c' needs argument.\n", *ptr);
                        err++;
                    }
                    break;

                case 'H':				/* specify http-proxy server */
                    if ( 0 < argc ) {
                        argv++, argc--;
                        method = METHOD_HTTP;
                        server = *argv;
                    } else {
                        error("option '-%c' needs argument.\n", *ptr);
                        err++;
                    }
                    break;

                case 'P':
                    /* destination port number */
                    if ( 0 < argc ) {
                        argv++, argc--;
                        dest_port = resolve_port(*argv);
                    } else {
                        error("option '-%c' needs argument.\n", *ptr);
                        err++;
                    }
                    break;

                case 'p':                          /* specify port to forward */
                    if ( 0 < argc ) {
                        argv++, argc--;
                        local_type = LOCAL_SOCKET;
                        local_port = resolve_port(*argv);
                    } else {
                        error("option '-%c' needs argument.\n", *ptr);
                        err++;
                    }
                    break;

                case '4':
                    socks_version = 4;
                    break;

                case '5':
                    socks_version = 5;
                    break;

                case 'R':				/* specify resolve method */
                    if ( 0 < argc ) {
                        argv++, argc--;
                        socks_resolve = lookup_resolve( *argv );
                    } else {
                        error("option '-%c' needs argument.\n", *ptr);
                        err++;
                    }
                    break;

                case 'd':				/* debug mode */
                    f_debug++;
                    break;

                default:
                    error("unknown option '-%c'\n", *ptr);
                    err++;
            }
            ptr++;
        }
        argc--, argv++;
    }

    /* check error */
    if ( 0 < err )
        goto quit;

    set_relay( method, server );

    /* check destination HOST (MUST) */
    if ( argc == 0  ) {
        error( "You must specify hostname.\n");
        err++;
        goto quit;
    }
    dest_host = argv[0];
    /* decide port or service name from programname or argument */
    if ( ((ptr=strrchr( progname, '/' )) != NULL) ||
            ((ptr=strchr( progname, '\\')) != NULL) )
        ptr++;
    else
        ptr = progname;
    if ( dest_port == 0 ) {
        /* accept only if -P is not specified. */
        if ( 1 < argc ) {
            /* get port number from argument (prior to progname) */
            /* NOTE: This way is for cvs ext method. */
            dest_port = resolve_port(argv[1]);
        } else if ( strncmp( ptr, "connect-", 8) == 0 ) {
            /* decide port number from program name */
            char *str = strdup( ptr+8 );
            str[strcspn( str, "." )] = '\0';
            dest_port = resolve_port(str);
            free(str);
        }
    }
    /* check port number */
    if ( dest_port <= 0 ) {
        error( "You must specify destination port correctly.\n");
        err++;
        goto quit;
    }
    if ( (relay_method != METHOD_DIRECT) && (relay_port <= 0) ) {
        error("Invalid relay port: %d\n", dest_port);
        err++;
        goto quit;
    }

quit:
    /* report for debugging */
    debug("relay_method = %s (%d)\n",
            method_names[relay_method], relay_method);
    if ( relay_method != METHOD_DIRECT ) {
        debug("relay_host=%s\n", relay_host);
        debug("relay_port=%d\n", relay_port);
        debug("relay_user=%s\n", relay_user);
    }
    if ( relay_method == METHOD_SOCKS ) {
        debug("socks_version=%d\n", socks_version);
        debug("socks_resolve=%s (%d)\n",
                resolve_names[socks_resolve], socks_resolve);
    }
    debug("local_type=%s\n", local_type_names[local_type]);
    if ( local_type == LOCAL_SOCKET ) 
        debug("local_port=%d\n", local_port);
    debug("dest_host=%s\n", dest_host);
    debug("dest_port=%d\n", dest_port);
    if ( 0 < err ) {
        fprintf(stderr, usage, progname);
        exit(1);
    }
    return 0;
}


/* TODO: IPv6 */
    SOCKET
open_connection( const char *host, u_short port )
{
    SOCKET s;
    struct hostent *ent;
    struct sockaddr_in saddr;

    if ( relay_method == METHOD_DIRECT ) {
        host = dest_host;
        port = dest_port;
    } else {
        host = relay_host;
        port = relay_port;
    }

    ent = gethostbyname( host );
    if ( ent == NULL ) {
        error("can't resolve hostname: %s\n", host);
        return SOCKET_ERROR;
    }
    memcpy( &saddr.sin_addr, ent->h_addr, ent->h_length );
    saddr.sin_family = ent->h_addrtype;
    saddr.sin_port = htons(port);

    debug("connecting to %s:%u\n", inet_ntoa(saddr.sin_addr), port);
    s = socket( AF_INET, SOCK_STREAM, 0 );
    if ( connect( s, (struct sockaddr *)&saddr, sizeof(saddr))
            == SOCKET_ERROR) {
        debug( "connect() failed.\n");
        return SOCKET_ERROR;
    }
    return s;
}

int f_report = 1;				/* report flag */

    void
report_text( char *prefix, char *buf )
{
    static char work[1024];
    char *tmp;

    if ( !f_debug )
        return;
    if ( !f_report )
        return;					/* don't report */
    debug("%s \"", prefix);
    while ( *buf ) {
        memset( work, 0, sizeof(work));
        tmp = work;
        while ( *buf && ((size_t)(tmp - work) < sizeof(work)-5) ) {
            switch ( *buf ) {
                case '\t': *tmp++ = '\\'; *tmp++ = 't'; break;
                case '\r': *tmp++ = '\\'; *tmp++ = 'r'; break;
                case '\n': *tmp++ = '\\'; *tmp++ = 'n'; break;
                case '\\': *tmp++ = '\\'; *tmp++ = '\\'; break;
                default:
                           if ( isprint(*buf) ) {
                               *tmp++ = *buf;
                           } else {
                               sprintf( tmp, "\\x%02X", (unsigned char)*buf);
                               tmp += strlen(tmp);
                           }
            }
            buf++;
            *tmp = '\0';
        }
        debug_("%s", work);
    }

    debug_("\"\n");
}


    void
report_bytes( char *prefix, char *buf, int len )
{
    int i = 0;
    if ( ! f_debug )
        return;
    while ( 0 < len ) {
        if ((i%16) == 0) {
            if (i) {
                int j;
                buf -= 16;
                debug_("  '");
                for (j = 0; j < 16; j++, buf++)
                    debug_("%c", isprint(*buf) ? *buf : '.');
                debug_("'");
            }
            debug_("\n%s", prefix );
        }
        debug_(" %02x", *(unsigned char *)buf);
        buf++;
        len--;
        i++;
    }
    debug_("\n");
    return;
}

    int
atomic_out( SOCKET s, char *buf, int size )
{
    int ret, len;

    assert( buf != NULL );
    assert( 0<=size );
    /* do atomic out */
    ret = 0;
    while ( 0 < size ) {
        len = send( s, buf+ret, size, 0 );
        if ( len == -1 ) {
            error("Fail to send(), %d\n", socket_errno());
            return -1;
        }
        ret += len;
        size -= len;
    }
    debug("atomic_out()  [%d bytes]\n", ret);
    report_bytes(">>>", buf, ret);
    return ret;
}

    int
atomic_in( SOCKET s, char *buf, int size )
{
    int ret, len;

    assert( buf != NULL );
    assert( 0<=size );

    /* do atomic in */
    ret = 0;
    while ( 0 < size ) {
        len = recv( s, buf+ret, size, 0 );
        if ( len == -1 ) {
            error("Fail to send(), %d\n", socket_errno());
            return -1;
        } else if ( len == 0 ) {
            /* closed by peer */
            error( "Connection closed by peer.\n");
            return -1;				/* can't complete atomic in */
        }
        ret += len;
        size -= len;
    }
    debug("atomic_in() [%d bytes]\n", ret);
    report_bytes("<<<", buf, ret);
    return ret;
}

    int
line_input( SOCKET s, char *buf, int size )
{
    char *dst = buf;

    if ( size == 0 )
        return 0;				/* no error */
    size--;
    while ( 0 < size ) {
        switch ( recv( s, dst, 1, 0) ) {	/* recv one-by-one */
            case SOCKET_ERROR:
                error("recv() error\n");
                return -1;				/* error */
            case 0:
                size = 0;				/* end of stream */
                break;
            default:
                /* continue reading until last 1 char is EOL? */
                if ( *dst == '\n' ) {
                    /* finished */
                    size = 0;
                } else {
                    /* more... */
                    size--;
                }
                dst++;
        }
    }
    *dst = '\0';
    report_text( "<<<", buf);
    return 0;
}

    void
skip_all_lines (SOCKET s)
{
    char buf[1024];
    while (0 < recv (s, buf, sizeof(buf), 0))
        ;
}

/* cut_token()
   Span token in given string STR until char in DELIM is appeared.
   Then replace contiguous DELIMS with '\0' for string termination
   and returns next pointer.
   If no next token, return NULL.
   */
    char *
cut_token( char *str, char *delim)
{
    char *ptr = str + strcspn(str, delim);
    char *end = ptr + strspn(ptr, delim);
    if ( ptr == str )
        return NULL;
    while ( ptr < end )
        *ptr++ = '\0';
    return ptr;
}

/* readpass()
   password input routine 
   Use ssh-askpass (same mechanism to OpenSSH)
   */
    char *
readpass( const char* prompt, ...)
{
    static char buf[1000];			/* XXX, don't be fix length */
    va_list args;
    va_start(args, prompt);
    vsprintf(buf, prompt, args);
    va_end(args);

    if ( getenv(ENV_SSH_ASKPASS)
#if !defined(_WIN32) && !defined(__CYGWIN32__)
            && getenv("DISPLAY")
#endif /* not _WIN32 && not __CYGWIN32__ */
       ) {
        /* use ssh-askpass to get password */
        FILE *fp;
        char *askpass = getenv(ENV_SSH_ASKPASS), *cmd;
        cmd = malloc(strlen(askpass) +1 +1 +strlen(buf) +1);
        sprintf(cmd, "%s \"%s\"", askpass, buf);
        fp = popen(cmd, "r");
        free(cmd);
        if (NULL == fp)
            return NULL;			/* fail */
        buf[0] = '\0';
        if (NULL == fgets(buf, sizeof(buf), fp)) {
            fclose(fp);
            return NULL;			/* fail */
        }
        fclose(fp);
    } else {
        tty_readpass(buf, buf, sizeof(buf));
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return buf;
}

#ifdef SOCKS_ALLOW_USERPASS_AUTH
    static int
socks5_do_auth_userpass( int s )
{
    unsigned char buf[1024], *ptr;
    char *pass;
    int len;

    /* do User/Password authentication. */
    /* This feature requires username and password from 
       command line argument or environment variable,
       or terminal. */
    if ( relay_user == NULL )
        fatal("User name cannot be decided.\n");

    /* get password from environment variable if exists. */
    if ((pass=getenv(ENV_SOCKS5_PASSWORD)) == NULL &&
            (pass=readpass("Enter SOCKS5 password for %s@%s: ",
                           relay_user, relay_host)) == NULL )
        fatal("Cannot get password for user: %s\n", relay_user);

    /* make authentication packet */
    ptr = buf;
    PUT_BYTE( ptr++, 1 );			/* subnegotiation ver.: 1 */
    len = strlen( relay_user );			/* ULEN and UNAME */
    PUT_BYTE( ptr++, len );
    strcpy( (char *)ptr, relay_user );
    ptr += len;
    len = strlen( pass );			/* PLEN and PASSWD */
    PUT_BYTE( ptr++, strlen(pass));
    strcpy( (char *)ptr, pass );
    ptr += len;

    /* send it and get answer */
    if ( (atomic_out( s, (char *)buf, ptr-buf ) != ptr-buf)
            || (atomic_in( s, (char *)buf, 2 ) != 2) ) {
        error("I/O error\n");
        return -1;
    }

    /* check status */
    if ( buf[1] == 0 )
        return 0;				/* success */
    else
        return -1;				/* fail */
}
#endif /* SOCKS_ALLOW_USERPASS_AUTH */

    static const char *
socks5_getauthname( int auth )
{
    switch ( auth ) {
        case SOCKS5_AUTH_REJECT: return "REJECTED";
        case SOCKS5_AUTH_NOAUTH: return "NO-AUTH";
        case SOCKS5_AUTH_GSSAPI: return "GSSAPI";
        case SOCKS5_AUTH_USERPASS: return "USERPASS";
        case SOCKS5_AUTH_CHAP: return "CHAP";
        case SOCKS5_AUTH_EAP: return "EAP";
        case SOCKS5_AUTH_MAF: return "MAF";
        default: return "(unknown)";
    }
}

/* begin SOCKS5 relaying
   And no authentication is supported.
   */
    int
begin_socks5_relay( SOCKET s )
{
    unsigned char buf[256], *ptr;
    unsigned char n_auth = 0; unsigned char auth_list[10], auth_method;
    int len, auth_result;    

    debug( "begin_socks_relay()\n");

    /* request authentication */
    ptr = buf;
    PUT_BYTE( ptr++, 5);			/* SOCKS version (5) */
#ifdef SOCKS_ALLOW_NO_AUTH
    /* add no-auth authentication */
    auth_list[n_auth++] = 0;
#endif /* SOCKS_ALLOW_NO_AUTH */
#ifdef SOCKS_ALLOW_USERPASS_AUTH
    /* add user/pass authentication */
    auth_list[n_auth++] = 2;
#endif /* SOCKS_ALLOW_USERPASS_AUTH */
    PUT_BYTE( ptr++, n_auth);			/* num auth */
    while (0 < n_auth--)
        PUT_BYTE( ptr++, auth_list[n_auth]);	/* authentications */
    if ( (atomic_out( s, (char *)buf, ptr-buf ) < 0 ) || /* send requst */
            (atomic_in( s, (char *)buf, 2 ) < 0) ||	/* recv response */
            (buf[0] != 5) ||			/* ver5 response */
            (buf[1] == 0xFF) )			/* check auth method */
        return -1;
    auth_method = buf[1];

    debug("auth method: %s\n", socks5_getauthname(auth_method));
    auth_result = -1;
    switch ( auth_method ) {
        case SOCKS5_AUTH_REJECT:
            error("No acceptable authentication method\n");
            return -1;				/* fail */

#ifdef SOCKS_ALLOW_NO_AUTH
        case SOCKS5_AUTH_NOAUTH:
            /* nothing to do */
            auth_result = 0;
            break;
#endif /* SOCKS_ALLOW_NO_AUTH */

#ifdef SOCKS_ALLOW_USERPASS_AUTH
        case SOCKS5_AUTH_USERPASS:
            auth_result = socks5_do_auth_userpass(s);
            break;
#endif /* SOCKS_ALLOW_USERPASS_AUTH */

        default:
            error("Unsupported authentication method: %s\n",
                    socks5_getauthname( auth_method ));
            return -1;				/* fail */
    }
    if ( auth_result != 0 ) {
        error("Authentication faield.\n");
        return -1;
    }
    /* request to connect */
    ptr = buf;
    PUT_BYTE( ptr++, 5);			/* SOCKS version (5) */
    PUT_BYTE( ptr++, 1);			/* CMD: CONNECT */
    PUT_BYTE( ptr++, 0);			/* FLG: 0 */
    if ( dest_addr.s_addr == 0 ) {
        /* resolved by SOCKS server */
        PUT_BYTE( ptr++, 3);			/* ATYP: DOMAINNAME */
        len = strlen(dest_host);
        PUT_BYTE( ptr++, len);			/* DST.ADDR (len) */
        memcpy( ptr, dest_host, len );		/* (hostname) */
        ptr += len;
    } else {
        /* resolved localy */
        PUT_BYTE( ptr++, 1 );			/* ATYP: IPv4 */
        memcpy( ptr, &dest_addr.s_addr, sizeof(dest_addr.s_addr));
        ptr += sizeof(dest_addr.s_addr);
    }
    PUT_BYTE( ptr++, dest_port>>8);	/* DST.PORT */
    PUT_BYTE( ptr++, dest_port&0xFF);
    if ( (atomic_out( s, (char *)buf, ptr-buf) < 0) ||	/* send request */
            (atomic_in( s, (char *)buf, 4 ) < 0) ||	/* recv response */
            (buf[1] != SOCKS5_REP_SUCCEEDED) )	/* check reply code */
        return -1;
    ptr = buf + 4;
    switch ( buf[3] ) {				/* case by ATYP */
        case 1:					/* IP v4 ADDR*/
            atomic_in( s, (char *)ptr, 4+2 );		/* recv IPv4 addr and port */
            break;
        case 3:					/* DOMAINNAME */
            atomic_in( s, (char *)ptr, 1 );			/* recv name and port */
            atomic_in( s, (char *)ptr+1, *(unsigned char*)ptr + 2);
            break;
        case 4:					/* IP v6 ADDR */
            atomic_in( s, (char *)ptr, 16+2 );		/* recv IPv6 addr and port */
            break;
    }

    /* Conguraturation, connected via SOCKS5 server! */
    debug("connected.\n");
    return 0;
}

/* begin SOCKS protocol 4 relaying
   And no authentication is supported.

   There's SOCKS protocol version 4 and 4a. Protocol version
   4a has capability to resolve hostname by SOCKS server, so
   we don't need resolving IP address of destination host on
   local machine.

   Environment variable SOCKS_RESOLVE directs how to resolve
   IP addess. There's 3 keywords allowed; "local", "remote"
   and "both" (case insensitive). Keyword "local" means taht
   target host name is resolved by localhost resolver
   (usualy with gethostbyname()), "remote" means by remote
   SOCKS server, "both" means to try resolving by localhost
   then remote.

   SOCKS4 protocol and authentication of SOCKS5 protocol
   requires user name on connect request.
   User name is determined by following method.

   1. If server spec has user@hostname:port format then
   user part is used for this SOCKS server.

   2. Get user name from environment variable LOGNAME, USER
   (in this order).

*/
    int
begin_socks4_relay( SOCKET s )
{
    unsigned char buf[256], *ptr;

    debug( "begin_socks_relay()\n");

    /* make connect request packet 
       protocol v4:
VN:1, CD:1, PORT:2, ADDR:4, USER:n, NULL:1
protocol v4a:
VN:1, CD:1, PORT:2, DUMMY:4, USER:n, NULL:1, HOSTNAME:n, NULL:1
*/
    ptr = buf;
    PUT_BYTE( ptr++, 4);			/* protocol version (4) */
    PUT_BYTE( ptr++, 1);			/* CONNECT command */
    PUT_BYTE( ptr++, dest_port>>8);	/* destination Port */
    PUT_BYTE( ptr++, dest_port&0xFF);
    /* destination IP */
    memcpy(ptr, &dest_addr.s_addr, sizeof(dest_addr.s_addr));
    ptr += sizeof(dest_addr.s_addr);
    if ( dest_addr.s_addr == 0 )
        *(ptr-1) = 1;				/* fake, protocol 4a */
    /* username */
    strcpy( (char *)ptr, relay_user );
    ptr += strlen( relay_user ) +1;
    /* destination host name (for protocol 4a) */
    if ( (socks_version == 4) && (dest_addr.s_addr == 0)) {
        strcpy( (char *)ptr, dest_host );
        ptr += strlen( dest_host ) +1;
    }
    /* send command and get response
       response is: VN:1, CD:1, PORT:2, ADDR:4 */
    if ( (atomic_out( s, (char *)buf, ptr-buf) < 0) ||	/* send request */
            (atomic_in( s, (char *)buf, 8 ) < 0) ||	/* recv response */
            (buf[1] != SOCKS4_REP_SUCCEEDED) )	/* check reply code */
        return -1;				/* failed */

    /* Conguraturation, connected via SOCKS4 server! */
    debug("connected.\n");
    return 0;
}

    int
sendf(SOCKET s, const char *fmt,...)
{
    static char buf[10240];			/* xxx, enough? */

    va_list args;
    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    report_text(">>>", buf);
    if ( send(s, buf, strlen(buf), 0) == SOCKET_ERROR ) {
        debug("failed to send http request. errno=%d\n", socket_errno());
        return -1;
    }
    return 0;
}

const char *base64_table =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    char *
make_base64_string(const char *str)
{
    static char *buf;
    unsigned char *src;
    char *dst;
    int bits, data, src_len, dst_len;
    /* make base64 string */
    src_len = strlen(str);
    dst_len = (src_len+2)/3*4;
    buf = malloc(dst_len+1);
    bits = data = 0;
    src = (unsigned char *)str;
    dst = buf;
    while ( dst_len-- ) {
        if ( bits < 6 ) {
            data = (data << 8) | *src;
            bits += 8;
            if ( *src != 0 )
                src++;
        }
        *dst++ = base64_table[0x3F & (data >> (bits-6))];
        bits -= 6;
    }
    *dst = '\0';
    /* fix-up tail padding */
    switch ( src_len%3 ) {
        case 1:
            *--dst = '=';
        case 2:
            *--dst = '=';
    }
    return buf;
}


    int
basic_auth( SOCKET s, const char *user, const char *pass )
{
    char *userpass;
    char *cred;
    int len, ret;

    len = strlen(user)+strlen(pass)+1;
    userpass = malloc(len+1);
    sprintf(userpass,"%s:%s", user, pass);
    cred = make_base64_string(userpass);
    memset( userpass, 0, len );

    f_report = 0;				/* don't report for security */
    ret = sendf(s, "Proxy-Authorization: Basic %s\r\n", cred);
    f_report = 1;
    report_text(">>>", "Proxy-Authorization: Basic xxxxx\r\n");

    memset(cred, 0, strlen(cred));
    free(cred);

    return ret;
}

/* begin relaying via HTTP proxy
   Directs CONNECT method to proxy server to connect to
   destination host (and port). It may not be allowed on your
   proxy server.
   */
    int
begin_http_relay( SOCKET s )
{
    char buf[1024];
    int result;
    char *user = NULL, *pass = NULL;

    debug("begin_http_relay()\n");

    if (proxy_auth_type != PROXY_AUTH_NONE) {
        /* Get username/password for authentication */
        if ((user = getenv(ENV_HTTP_PROXY_USER)) == NULL &&
                (user = getenv(ENV_CONNECT_USER)) == NULL &&
                (user = getusername()) == NULL )
            fatal("Cannot decide username for proxy authentication.");
        if ((pass = getenv(ENV_HTTP_PROXY_PASSWORD)) == NULL &&
                (pass = getenv(ENV_CONNECT_PASSWORD)) == NULL &&
                (pass = readpass("Enter proxy authentication password for %s@%s: ",
                                 user, relay_host)) == NULL)
            fatal("Cannot decide password for proxy authentication.");
        user = strdup(user);
        pass = strdup(pass);
    }
    if (sendf(s,"CONNECT %s:%d HTTP/1.0\r\n", dest_host, dest_port) < 0)
        return -1;
    if (proxy_auth_type == PROXY_AUTH_BASIC && basic_auth(s, user, pass) < 0)
        return -1;
    if (sendf(s,"\r\n") < 0)
        return -1;
    if ( user ) { memset(user, 0, strlen(user)); free(user); }
    if ( pass ) { memset(pass, 0, strlen(pass)); free(pass); }

    /* get response */
    if ( line_input(s, buf, sizeof(buf)) < 0 ) {
        debug("failed to read http response.\n");
        return -1;
    }
    /* check status */
    result = atoi(strchr(buf,' '));
    switch ( result ) {
        case 200:
            /* Conguraturation, connected via http proxy server! */
            debug("connected, start user session.\n");
            break;
        case 302:					/* redirect */
            do {
                if (line_input(s, buf, sizeof(buf)))
                    break;
                downcase(buf);
                if (expect(buf, "Location: ")) {
                    relay_host = cut_token(buf, "//");
                    cut_token(buf, "/");
                    relay_port = atoi(cut_token(buf, ":"));
                }
            } while (strcmp(buf,"\r\n") != 0);
            skip_all_lines(s);
            return START_RETRY;

        case 407:					/* Proxy-Auth required */
            /** NOTE: As easy implementation, we support only BASIC scheme
              and ignore realm. */
            /* If proxy_auth_type is PROXY_AUTH_BASIC and get
               this result code, authentication was failed. */
            if (proxy_auth_type != PROXY_AUTH_NONE) {
                error("Authentication failed.\n");
                return -1;
            }
            do {
                if ( line_input(s, buf, sizeof(buf)) ) {
                    break;
                }
                downcase(buf);
                if (expect(buf, "Proxy-Authenticate: ")) {
                    /* parse type and realm */
                    char *scheme, *realm;
                    scheme = cut_token(buf, " ");
                    realm = cut_token(scheme, " ");
                    if ( scheme == NULL || realm == NULL ) {
                        debug("Invalid format of Proxy-Authenticate: field.");
                        return -1;			/* fail */
                    }
                    /* check type */
                    if (expect(scheme, "basic")) {
                        proxy_auth_type = PROXY_AUTH_BASIC;
                    } else {
                        debug("Unsupported authentication type: %s", scheme);
                        return -1;
                    }
                }
            } while (strcmp(buf,"\r\n") != 0);
            skip_all_lines(s);
            if ( proxy_auth_type == PROXY_AUTH_NONE ) {
                debug("Can't find Proxy-Authenticate: in response header.");
                return -1;
            } else {
                return START_RETRY;
            }

        default:
            /* Not allowed */
            debug("http proxy is not allowed.\n");
            return -1;
    }
    /* skip to end of response header */
    do {
        if ( line_input(s, buf, sizeof(buf) ) ) {
            debug("Can't skip response headers\n");
            return -1;
        }
    } while ( strcmp(buf,"\r\n") != 0 );

    return 0;
}


#ifdef _WIN32
/* ddatalen()
   Returns 1 if data is available, otherwise return 0
   */
    int
fddatalen( SOCKET fd )
{
    DWORD len = 0;
    struct stat st;
    fstat( 0, &st );
    if ( st.st_mode & _S_IFIFO ) { 
        /* in case of PIPE */
        if ( !PeekNamedPipe( GetStdHandle(STD_INPUT_HANDLE),
                    NULL, 0, NULL, &len, NULL) ) {
            if ( GetLastError() == ERROR_BROKEN_PIPE ) {
                /* PIPE source is closed */
                /* read() will detects EOF */
                len = 1;
            } else {
                fatal("PeekNamedPipe() failed, errno=%d\n",
                        GetLastError());
            }
        }
    } else if ( st.st_mode & _S_IFREG ) {
        /* in case of regular file (redirected) */
        len = 1;			/* always data ready */
    } else if ( _kbhit() ) {
        /* in case of console */
        len = 1;
    }
    return len;
}
#endif /* _WIN32 */

/* relay byte from stdin to socket and fro socket to stdout */
    int
do_repeater( SOCKET local_in, SOCKET local_out, SOCKET remote )
{
    /** vars for local input data **/
    char lbuf[1024];				/* local input buffer */
    size_t lbuf_len;				/* available data in lbuf */
    int f_local;				/* read local input more? */
    /** vars for remote input data **/
    char rbuf[1024];				/* remote input buffer */
    size_t rbuf_len;				/* available data in rbuf */
    int f_remote;				/* read remote input more? */
    /** other variables **/
    int nfds, len;
    fd_set *ifds, *ofds;
    struct timeval *tmo;
#ifdef _WIN32
    struct timeval win32_tmo;
#endif /* _WIN32 */

    /* repeater between stdin/out and socket  */
    nfds = ((local_in<remote)? remote: local_in) +1;
    ifds = FD_ALLOC(nfds);
    ofds = FD_ALLOC(nfds);
    f_local = 1;				/* yes, read from local */
    f_remote = 1;				/* yes, read from remote */
    lbuf_len = 0;
    rbuf_len = 0;

    while ( f_local || f_remote ) {
        FD_ZERO( ifds );
        FD_ZERO( ofds );
        tmo = NULL;

        /** prepare for reading local input **/
        if ( f_local && (lbuf_len < sizeof(lbuf)) ) {
#ifdef _WIN32
            if ( local_type != LOCAL_SOCKET ) {
                /* select() on Winsock is not accept standard handle.
                   So use select() with short timeout and checking data
                   in stdin by another method. */
                win32_tmo.tv_sec = 0;
                win32_tmo.tv_usec = 10*1000;	/* 10 ms */
                tmo = &win32_tmo;
            } else
#endif /* !_WIN32 */
                FD_SET( local_in, ifds );
        }

        /** prepare for reading remote input **/
        if ( f_remote && (rbuf_len < sizeof(rbuf)) ) {
            FD_SET( remote, ifds );
        }

        /* FD_SET( local_out, ofds ); */
        /* FD_SET( remote, ofds ); */

        if ( select( nfds, ifds, ofds, NULL, tmo ) == -1 ) {
            /* some error */
            error( "select() failed, %d\n", socket_errno());
            return -1;
        }
#ifdef _WIN32
        /* fake ifds if local is stdio handle because
           select() of Winsock does not accept stdio
           handle. */
        if (f_local && (local_type!=LOCAL_SOCKET) && (0<fddatalen(local_in)))
            FD_SET(0,ifds);		/* data ready */
#endif

        /* remote => local */
        if ( FD_ISSET(remote, ifds) && (rbuf_len < sizeof(rbuf)) ) {
            len = recv( remote, rbuf + rbuf_len, sizeof(rbuf)-rbuf_len, 0);
            if ( len == 0 ) {
                debug("connection closed by peer\n");
                f_remote = 0;			/* no more read from socket */
                f_local = 0;
            } else if ( len == -1 ) {
                /* error */
                fatal("recv() faield, %d\n", socket_errno());
            } else {
                debug("recv %d bytes\n", len);
                if ( 1 < f_debug )		/* more verbose */
                    report_bytes( "<<<", rbuf, len);
                rbuf_len += len;
            }
        }

        /* local => remote */
        if ( FD_ISSET(local_in, ifds) && (lbuf_len < sizeof(lbuf)) ) {
            if (local_type == LOCAL_SOCKET)
                len = recv(local_in, lbuf + lbuf_len,
                        sizeof(lbuf)-lbuf_len, 0);
            else
                len = read(local_in, lbuf + lbuf_len, sizeof(lbuf)-lbuf_len);
            if ( len == 0 ) {
                /* stdin is EOF */
                debug("local input is EOF\n");
                shutdown(remote, 1);		/* no-more writing */
                f_local = 0;
            } else if ( len == -1 ) {
                /* error on reading from stdin */
                fatal("recv() failed, errno = %d\n", errno);
            } else {
                /* repeat */
                lbuf_len += len;
            }
        }

        /* flush data in buffer to socket */
        if ( 0 < lbuf_len ) {
            len = send(remote, lbuf, lbuf_len, 0);
            if ( 1 < f_debug )		/* more verbose */
                report_bytes( ">>>", lbuf, lbuf_len);
            if ( len == -1 ) {
                fatal("send() failed, %d\n", socket_errno());
            } else if ( 0 < len ) {
                /* move data on to top of buffer */
                debug("send %d bytes\n", len);
                lbuf_len -= len;
                if ( 0 < lbuf_len )
                    memcpy( lbuf, lbuf+len, lbuf_len );
            }
        }

        /* flush data in buffer to local output */
        if ( 0 < rbuf_len ) {
            if (local_type == LOCAL_SOCKET)
                len = send( local_out, rbuf, rbuf_len, 0);
            else
                len = write( local_out, rbuf, rbuf_len);
            if ( len == -1 ) {
                fatal("output (local) failed, errno=%d\n", errno);
            } 
            rbuf_len -= len;
            if ( (size_t)len < rbuf_len )
                memcpy( rbuf, rbuf+len, rbuf_len );
        }

    }

    return 0;
}


    int
accept_connection (u_short port)
{
    int sock;
    int connection;
    struct sockaddr_in name;

    struct sockaddr client;
    socklen_t socklen;

    /* Create the socket. */
    debug("Creating source port to forward.\n");
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        fatal("socket() failed, errno=%d\n", socket_errno());

    /* Give the socket a name. */
    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);
    if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
        fatal ("bind() failed, errno=%d\n", socket_errno());

    if (listen( sock, 1) < 0)
        fatal ("listen() failed, errno=%d\n", socket_errno());

    socklen = sizeof(client);
    connection = accept(sock, &client, &socklen);
    if ( connection < 0 )
        fatal ("accept() failed, errno=%d\n", socket_errno());

    shutdown(sock, 2);

    return connection;
}


    int
local_resolve(const char *host)
{
    struct hostent *ent;
    if ( strspn(host, "0123456789.") == strlen(host) ) {
        /* given by IP address */
        dest_addr.s_addr = inet_addr(host);
    } else {
#if !defined(_WIN32) && !defined(__CYGWIN32__)
        if (socks_ns.s_addr != 0) {
            res_init();
            _res.nsaddr_list[0].sin_addr.s_addr = socks_ns.s_addr;
            _res.nscount = 1;
            debug("Using nameserver at %s\n", inet_ntoa(socks_ns));
        }
#endif /* !_WIN32 && !__CYGWIN32__ */
        debug("resolving host by name: %s\n", dest_host);
        ent = gethostbyname( dest_host );
        if ( ent ) {
            memcpy( &(dest_addr.s_addr), ent->h_addr, ent->h_length);
            debug("resolved: %s (%s)\n",
                    dest_host, inet_ntoa(dest_addr));
        } else {
            debug("failed to resolve locally.\n", dest_host);
            return -1;				/* failed */
        }
    }
    return 0;					/* good */
}

/** Main of program **/
    int
main( int argc, char **argv )
{
    int ret;
    SOCKET  remote;				/* socket */
    SOCKET  local_in;				/* Local input */
    SOCKET  local_out;				/* Local output */
#ifdef _WIN32
    WSADATA wsadata;
    WSAStartup( 0x101, &wsadata);
#endif /* _WIN32 */

    /* initialization */
    getarg( argc, argv );
    debug("Program is $Revision: 619 $\n");

    /* Open local_in and local_out if forwarding a port */
    if ( local_type == LOCAL_SOCKET ) {
        /* Relay between local port and destination */
        local_in = local_out = accept_connection( local_port );
    } else {
        /* Relay between stdin/stdout and desteination */
        local_in = 0;
        local_out = 1;
#ifdef _WIN32
        _setmode(local_in, O_BINARY);
        _setmode(local_out, O_BINARY);
#endif
    }

retry:
    /* make connection */
    if ( relay_method == METHOD_DIRECT ) {
        remote = open_connection (dest_host, dest_port);
        if ( remote == SOCKET_ERROR )
            fatal( "Unable to connect to destination host, errno=%d\n",
                    socket_errno());
    } else {
        remote = open_connection (relay_host, relay_port);
        if ( remote == SOCKET_ERROR )
            fatal( "Unable to connect to relay host, errno=%d\n",
                    socket_errno());
    }

    /** resolve destination host (SOCKS) **/
    if (relay_method == METHOD_SOCKS &&
            socks_resolve == RESOLVE_LOCAL &&
            local_resolve( dest_host ) < 0) {
        fatal("Unknown host: %s", dest_host);
    }

    /** relay negociation **/
    switch ( relay_method ) {
        case METHOD_SOCKS:
            if ( ((socks_version == 5) && (begin_socks5_relay(remote) < 0)) ||
                    ((socks_version == 4) && (begin_socks4_relay(remote) < 0)) )
                fatal( "failed to begin relaying via SOCKS.\n");
            break;

        case METHOD_HTTP:
            ret = begin_http_relay(remote);
            switch (ret) {
                case START_ERROR:
                    fatal("failed to begin relaying via HTTP.\n");
                case START_OK:
                    break;
                case START_RETRY:
                    /* retry with authentication */
                    goto retry;			
            }
            break;
    }

    /* main loop */
    do_repeater(local_in, local_out, remote);
    closesocket(remote);
    if ( local_type == LOCAL_SOCKET)
        closesocket(local_in);
#ifdef _WIN32
    WSACleanup();
#endif /* _WIN32 */

    return 0;
}

/* ------------------------------------------------------------
   Local Variables:
   compile-command: "cc connect.c -o connect"
   fill-column: 74
   comment-column: 48
End:
------------------------------------------------------------ */

/*** end of connect.c ***/

#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

struct socket {
	int fd;
	bool connected;
	struct sockaddr * addr;
	int addrlen;
};

static bool socket_new(struct socket * sock, const char * host, unsigned short port);
static int socket_read(struct socket * sock, void * buffer, int buflen);
static void socket_write(struct socket * sock, void * buffer, int buflen);
static void socket_close(struct socket * sock);
static const char * socket_sockaddr_to_str(struct sockaddr * addr, int addrlen);
static const char * socket_other_end(struct socket * sock);

#ifdef DEBUG
static FILE * netdump;
#endif

static bool socket_new(struct socket * sock, const char * host, unsigned short port)
{
netdump=fopen("netdump.txt", "wt");
fprintf(netdump,"open %s\n",host?host:"null");
fflush(netdump);
#ifdef _WIN32
	static bool inited=false;
	if (!inited)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		inited=true;
	}
#endif
	
	memset(sock, 0, sizeof(sock));
	sock->fd=-1;
	
	char portstr[16];
	sprintf(portstr, "%i", port);
	
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags=(host?0:AI_PASSIVE);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=IPPROTO_UDP;
	
	struct addrinfo * addr=NULL;
	struct addrinfo * addrwalk;
	
	if (getaddrinfo(host?host:NULL, portstr, &hints, &addr)!=0) goto err;
	
	addrwalk=addr;
	
#ifndef DEBUG
#error no seriously.
#endif
printf("Connecting to %s, family %i\n", socket_sockaddr_to_str(addrwalk->ai_addr, addrwalk->ai_addrlen), addrwalk->ai_family);
	
	do {
		sock->fd=socket(addrwalk->ai_family, addrwalk->ai_socktype, addrwalk->ai_protocol);
		if (sock->fd<0)
			continue;
		
		if (host) break;
		
		if (!host)
		{
			if (bind(sock->fd, addrwalk->ai_addr, addrwalk->ai_addrlen)!=0) goto fail;
			
		}
		
	fail:
		close(sock->fd);
	} while((addrwalk=addrwalk->ai_next) != NULL);
	
	//we want packet loss, not blocking; we handle loss much better than blocks
#ifdef _WIN32
	u_long iMode=1;
	ioctlsocket(sock->fd, FIONBIO, &iMode);
#endif
#ifndef _WIN32
	fcntl(core->fd, F_SETFL, fcntl(core->fd, F_GETFL, 0)|O_NONBLOCK);
#endif
	
	if (sock->fd<0) goto err;
	
	//bah, I want it protocol agnostic. windows, cooperate
	//it can fail, for example on v4 sockets, but 
	int zero=0;
	setsockopt(sock->fd, IPPROTO_IPV6, 27/*IPV6_V6ONLY*/, (char*)&zero, sizeof(zero));
	
	sock->connected=(host);
	
	sock->addrlen=addr->ai_addrlen;
	sock->addr=malloc(addr->ai_addrlen);
	if (sock->connected) memcpy(sock->addr, addr->ai_addr, addr->ai_addrlen);
	
	freeaddrinfo(addr);
	return true;
	
err:
	if (addr) freeaddrinfo(addr);
	if (sock->fd>=0) close(sock->fd);
	sock->fd=-1;
	return false;
}

static int socket_read(struct socket * sock, void * buffer, int buflen)
{
	if (sock->connected)
	{
		char addrcopy[sock->addrlen];
		int ret;
		do {
			ret=recvfrom(sock->fd, buffer, buflen, 0, (struct sockaddr*)addrcopy, &sock->addrlen);
		} while (ret>0 && !!memcmp(sock->addr, addrcopy, sock->addrlen));
		if (ret<=0) ret=0;
if(ret){
fprintf(netdump,"--> ");
int i;for(i=0;i<ret;i++)fprintf(netdump,"%.2X",((unsigned char*)buffer)[i]);
fprintf(netdump,"\n");
fflush(netdump);}
		return ret;
	}
	else
	{
		memset(sock->addr, 0, sock->addrlen);
		int ret=recvfrom(sock->fd, buffer, buflen, 0, sock->addr, &sock->addrlen);
		if (ret<=0) ret=0;
		else sock->connected=true;
if(ret){
fprintf(netdump,"--> ");
int i;for(i=0;i<ret;i++)fprintf(netdump,"%.2X",((unsigned char*)buffer)[i]);
fprintf(netdump,"\n");
fflush(netdump);}
		return ret;
	}
}

static void socket_write(struct socket * sock, void * buffer, int buflen)
{
if(sock->connected){
fprintf(netdump,"<-- ");
int i;for(i=0;i<buflen;i++)fprintf(netdump,"%.2X",((unsigned char*)buffer)[i]);
fprintf(netdump,"\n");
fflush(netdump);}
//#ifndef DEBUG
//#error no seriously.
//#endif
//static int loss=1;
//if (rand()%loss) {loss--;return;}
//loss=3;
	if (sock->connected) sendto(sock->fd, buffer, buflen, 0, sock->addr, sock->addrlen);
}

static void socket_close(struct socket * sock)
{
	if (sock->addr) free(sock->addr);
	sock->addr=0;
	sock->addrlen=0;
	if (sock->fd>=0) close(sock->fd);
	sock->fd=-1;
}

static const char * socket_sockaddr_to_str(struct sockaddr * addr, int addrlen)
{
	static int addrstrlen=16;
	static char * addrstr=NULL;
	if (!addrstr) addrstr=malloc(addrstrlen);
	
retry: ;
	int err=getnameinfo(addr, addrlen, addrstr, addrstrlen, NULL, 0, NI_NUMERICHOST);
	if (!err) return addrstr;
	if (err==WSAEFAULT)
	{
		addrstrlen*=2;
		addrstr=realloc(addrstr, addrstrlen);
		goto retry;
	}
	return NULL;
}

static const char * socket_other_end(struct socket * sock)
{
	if (!sock->connected) return NULL;
	return socket_sockaddr_to_str(sock->addr, sock->addrlen);
}

#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <ws2tcpip.h>

struct socket {
	int fd;
	bool connected;
	struct sockaddr * addr;
	int addrlen;
};

static bool socket_new(struct socket * sock, const char * host, unsigned short port)
{
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
	
	do {
		sock->fd=socket(addrwalk->ai_family, addrwalk->ai_socktype, addrwalk->ai_protocol);
		if (sock->fd<0)
			continue;
		
		if (host || (bind(sock->fd, addrwalk->ai_addr, addrwalk->ai_addrlen)==0)) break;
		
		close(sock->fd);
	} while((addrwalk=addrwalk->ai_next) != NULL);
	
#ifdef _WIN32
	u_long iMode=1;
	ioctlsocket(sock->fd, FIONBIO, &iMode); 
#endif
#ifndef _WIN32
	fcntl(core->fd, F_SETFL, fcntl(core->fd, F_GETFL, 0)|O_NONBLOCK);
#endif
	
	if (sock->fd<0) goto err;
	
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
		return ret;
	}
	else
	{
		memset(sock->addr, 0, sock->addrlen);
		int ret=recvfrom(sock->fd, buffer, buflen, 0, sock->addr, &sock->addrlen);
		if (ret<=0) ret=0;
		else sock->connected=true;
		return ret;
	}
}

static void socket_write(struct socket * sock, void * buffer, int buflen)
{
#ifndef DEBUG
#error no seriously.
#endif
//if (rand()&1) return;//50% packet loss omg panic! extreme circumstances are the best for testing that stuff works
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

static const char * socket_other_end(struct socket * sock)
{
	if (!sock->connected) return NULL;
	static int addrstrlen=16;
	static char * addrstr=NULL;
	if (!addrstr) addrstr=malloc(addrstrlen);
	
retry: ;
	int err=getnameinfo(sock->addr, sock->addrlen, addrstr, addrstrlen, NULL, 0, NI_NUMERICHOST);
	if (!err) return addrstr;
	if (err==WSAEFAULT)
	{
		addrstrlen*=2;
		addrstr=realloc(addrstr, addrstrlen);
		goto retry;
	}
	return NULL;
	
	////wtf, inet_ntop doesn't exist on xp?
	//if (inet_ntop(sock->addr->sa_family, sock->addr, addrstr, addrstrlen)==0/*wtf, this one is documented to return pointer, why doesn't it*/)
	//{
	//	if (WSAGetLastError()==ERROR_INVALID_PARAMETER && addrstrlen<1024)
	//	{
	//		addrstrlen*=2;
	//		addrstr=realloc(addrstr, addrstrlen);
	//		goto retry;
	//	}
	//	return NULL;
	//}
}

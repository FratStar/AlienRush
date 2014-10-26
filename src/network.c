#include <stdlib.h>
#include <string.h>
#include "SDL_net.h"
#include "network.h"

TCPsocket tcpsock, server;	/*The socket that wil be used for game conenctions*/
TCPsocket clientsock[MAXCLIENTS];		/*the sockets reserved for clients*/
int SockinUse[MAXCLIENTS];					/*keeps track of which clients are in use*/
int NumClients;											/*the number of clients that are in use*/

/*
	generic functions

*/

void ShutDownNetwork()
{
	int i;
	SDLNet_TCP_Close(tcpsock);
	for(i = 0;i < MAXCLIENTS;i++)
	{
		if(SockinUse[i])
		SDLNet_TCP_Close(clientsock[i]);
	}
	SDLNet_Quit();
}

void SetUpNetwork()
{
	if(SDLNet_Init()==-1) 
	{
		fprintf(stderr,"SDLNet_Init: %s\n", SDLNet_GetError());
    exit(2);
	}
	atexit(ShutDownNetwork);
	/*initialize all data to zero*/
	NumClients = 0;
	memset(&tcpsock,0,sizeof(TCPsocket));
	memset(&SockinUse,0,sizeof(int) * MAXCLIENTS);
	memset(&clientsock,0,sizeof(TCPsocket) * MAXCLIENTS);
}


void UpdateNetwork(int host)
{
	char string[10];
	static clientconnected;
	if(host == -1)return;		//not set up yet
	if(host == 0)
	{
		ClientGetData(string,10);
		fprintf(stdout,"%s\r",string);
	}
	else if (host == 1)
	{
		if(NumClients <= 0)
		{
			clientconnected = HostAddClient();
		}
		else
		{
			sprintf(string,"Hello");
			HostSendData(clientconnected,string,10);
			fprintf(stdout,"Sent this string: %s\r",string);
		}
	}
}
/*
		Host functions
*/

void HostBegin()
{
	IPaddress ip;
	if(SDLNet_ResolveHost(&ip,NULL,DEFAULTPORT)==-1)
	{
		fprintf(stderr,"SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(1);
	}
	fprintf(stderr,"Host resolved: %i \n", ip.host);
	tcpsock=SDLNet_TCP_Open(&ip);
	if(!tcpsock) 
	{
		fprintf(stderr,"SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(2);
	}
}

int HostAddClient()
{
	int i;
	if(NumClients >= MAXCLIENTS)return -1;
	for(i = 0;i < MAXCLIENTS;i++)
	{
		if(!SockinUse[i])break;
	}
	clientsock[i]=SDLNet_TCP_Accept(tcpsock);
	if(!clientsock[i]) 
	{
		fprintf(stderr,"SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
		return -1;
	}
	else 
	{
		SockinUse[i] = 1;
		return i;
	}
}

void HostGetData(int client,void *data,int len)
{
	if(SDLNet_TCP_Recv(clientsock[client],data,len)<=0)
	{
		fprintf(stderr,"SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
    // An error may have occured, but sometimes you can just ignore it
    // It may be good to disconnect sock because it is likely invalid now.
	}
}

void HostSendData(int client,void *data,int len)
{
	if(SDLNet_TCP_Send(clientsock[client],data,len)<len)
	{
		fprintf(stderr,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
    // It may be good to disconnect sock because it is likely invalid now.
	}

}


/*
	Client Functions
*/

int ClientBegin()
{
	IPaddress ip;
	SDLNet_ResolveHost(&ip, "192.168.0.100", DEFAULTPORT);
	
	tcpsock=SDLNet_TCP_Open(&ip);
	if(!tcpsock)
	{
		fprintf(stderr,"SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		return 0;
	}
	return 1;
}

void ClientGetData(void *data,int len)
{
	if(SDLNet_TCP_Recv(tcpsock,data,len)<=0)
	{
		fprintf(stderr,"SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
    // An error may have occured, but sometimes you can just ignore it
    // It may be good to disconnect sock because it is likely invalid now.
	}
}

void ClientSendData(void *data,int len)
{
	if(SDLNet_TCP_Send(tcpsock,data,len)<len)
	{
		fprintf(stderr,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
    // It may be good to disconnect sock because it is likely invalid now.
	}

}



/*EOF*/

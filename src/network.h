#ifndef __NETWORK__
#define __NETWORK__
/*
		Networking management functions.
		Can set up a host and listen for up to MAXCLIENTS clients
		Can set itself up to connect to a server
		Can send and receive data between client and server

*/
#define MAXCLIENTS 16
#define DEFAULTPORT	1236

void SetUpNetwork();		//Initializes network library, preps shutdown.

void HostBegin();				//Sets up the program as a host.
int HostAddClient();		//listens for a client.  returns -1 if the host didn't add a client.  Client's index otherwise.

void HostGetData(int client,void *data,int len);
void HostSendData(int client,void *data,int len);

int ClientBegin();			//Sets the program up as a client.

void ClientGetData(void *data,int len);
void ClientSendData(void *data,int len);


#endif
/*eof*/

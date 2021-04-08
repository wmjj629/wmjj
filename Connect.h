#ifndef _CONNECT_H_
#define _CONNECT_H_

#include <ras.h>
#include <winsock2.h>
#pragma comment (lib, "Ws2.lib")

#ifdef TEST_MODE
#define CONN_URL			"112.223.69.35"
#else
#define CONN_URL			"180.70.94.41"
#endif

#define DATA_SENDING		500
#define CONNECT_OK			501
#define CONNECT_FAIL		502
#define SOCKET_FAIL			503
#define NETWORK_INIT_FAIL	504
#define SEND_OK				505
#define SEND_FAIL			506
#define RECV_FAIL			507
#define CONNECT_TIMEOUT		508

#define DC1					0x11
#define DC2					0x12
#define DC3					0x13
#define FS					0x1C
#define GS					0x1D
#define US					0x1F
#define EOT					0x04

void RecvData();
void RASConnection();
void RASDisConnect();

BOOL SendPacketMake();
BOOL SendData();
BOOL ConnectionResult();

DWORD WINAPI Connection( LPVOID lpParameter );

void userSet32Bit( int value, char *buf, int offset );
int userGet32Bit(char *buf, int offset);

#endif
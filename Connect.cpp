#include "StdAfx.h"

WSADATA			wsaData;
HRASCONN		rascon;

SOCKET			mySocket;
SOCKADDR_IN		sokAddr;

int				result, iPacketSize, iRecvBytes;
char			*buff;	
BOOL			bConnection;
BOOL			bRecvError;
BOOL			bComplete;

extern char		szPhoneNumber[16];
extern HWND		hWnd;
extern HWND		hStaticTitle;
extern HWND		hStaticMsg;
extern TCHAR	path[MAX_PATH];



//=============================================================================================
//  RAS ��Ʈ��ũ ����
//
//---------------------------------------------------------------------------------------------
// RASConnection - RAS ��Ʈ��ũ ����
//
void RASConnection()
{
	DWORD ret;
	
	RASDIALPARAMS rasdial;
	ZeroMemory( &rasdial, sizeof(RASDIALPARAMS) );
	
	rasdial.dwSize = sizeof( RASDIALPARAMS );
	_tcscpy( rasdial.szEntryName, L"Internet Connection" ); 
	_tcscpy( rasdial.szUserName, L"" ); 
	_tcscpy( rasdial.szPassword, L"") ; 
	
	rascon = NULL;
	
	SystemIdleTimerReset();
	ret = RasDial( NULL, NULL, &rasdial, 0xFFFFFFFF, hWnd, &rascon );
	
}


//=============================================================================================
//  RAS ��Ʈ��ũ ���� ����
//
//---------------------------------------------------------------------------------------------
// RASConnection - RAS ��Ʈ��ũ ���� ����
//
void RASDisConnect()
{
	DWORD ret;
	ret = RasHangUp( rascon );
}







//=============================================================================================
// ���� ��Ʈ��ũ ����
//
//---------------------------------------------------------------------------------------------
// Connection - ��Ʈ��ũ ����
//
DWORD WINAPI Connection( LPVOID lpParameter )
{
	bConnection = TRUE;
	Sleep( 1000 );
	
	// ���� �ʱ�ȭ
	if( WSAStartup( MAKEWORD(2,0), &wsaData ) == 0 )
	{
		// ���� ����
		mySocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 
		if( mySocket != INVALID_SOCKET )
		{
			ZeroMemory( &sokAddr, sizeof(sokAddr) );
			sokAddr.sin_family		= AF_INET;
			
			// ��忡 ���� �ٸ� ��Ʈ��ȣ �Ҵ�
		#ifdef TEST_MODE
			sokAddr.sin_port	= htons( 60008 );
		#else
			sokAddr.sin_port	= htons( 60009 );
		#endif
			
			sokAddr.sin_addr.s_addr	= inet_addr( CONN_URL );
			
			// ������ ����
			int iRet = connect( mySocket, (SOCKADDR*)&sokAddr, sizeof(sokAddr) );

			if( iRet != SOCKET_ERROR )
			{
				SetWindowText( hStaticMsg, L">> �������� ���� <<" );

				// ���� �ʱ�ȭ
				iPacketSize = 0;
				iRecvBytes	= 0;
				bRecvError	= FALSE;
				if( buff != NULL )
				{
					free( buff );
					buff = NULL;
				}

				
				if( SendPacketMake() )
				{
					// ������ ����
					if( SendData() )
					{
						RecvData();
					}
					// ���� ����
					else
					{
						SetWindowText( hStaticMsg, L">> ���� ���� <<\n\nȮ�ι�ư�� ������\n���׷��̵带\n��õ� �մϴ�\n\n\n[Ȯ��]" );
						bRecvError = TRUE;
					}
				}
	
			}
			else
			{
				SetWindowText( hStaticMsg, L">> ���� ���� ���� <<\n\nȮ�ι�ư�� ������\n���׷��̵带\n��õ� �մϴ�\n\n\n[Ȯ��]" );
				bRecvError = TRUE;
			}

			// ���� ����
			shutdown( mySocket, SD_BOTH );
			// ���� �ݱ�
			closesocket( mySocket );
			
		}
		else
		{
			SetWindowText( hStaticMsg, L">> ���� ���� ���� <<\n\nȮ�ι�ư�� ������\n���׷��̵带\n��õ� �մϴ�\n\n\n[Ȯ��]" );
			bRecvError = TRUE;
		}


		// ���� ���̺귯�� ����
		WSACleanup();
	}
	else
	{
		SetWindowText( hStaticMsg, L">> ���� �ʱ�ȭ ���� <<\n\nȮ�ι�ư�� ������\n���׷��̵带\n��õ� �մϴ�\n\n\n[Ȯ��]" );
		bRecvError = TRUE;
	}

	// Ras���� ����
	RASDisConnect();

	bConnection = FALSE;
	ExitThread(0);
	return 0;
}




// ���׷��̵��û��Ŷ �����
BOOL SendPacketMake()
{
	iPacketSize = 51;
	buff = (char*)calloc( iPacketSize, sizeof(char) );
	
	// ��� ( 25����Ʈ ) 
	userSet32Bit( iPacketSize, buff, 0 );
	memset( buff+4, '0', 10 );
	memcpy( buff+14, szPhoneNumber, 11 );

	//////////////////////////////////////////
	memcpy( buff+25, "00000000000000000000000", 23 );
	memcpy( buff+48, "99", 2 );

	buff[50] = EOT;
	return TRUE;
}




// ������ ����
BOOL SendData()
{
	BOOL bRet = TRUE;
	
	if( iPacketSize > 0 )
	{
		
		int ret = send( mySocket, buff, iPacketSize, 0 );
		if( ret == SOCKET_ERROR || ret == 0 ) bRet = FALSE;
	}
	else bRet = FALSE;
	
	return bRet;
}


void RecvData()
{
	SetWindowText( hStaticMsg, L">> �ٿ�ε� �� <<" );

	// ��ü��Ŷ���� ����
	char DataLength[4];
	recv( mySocket, DataLength, 4, 0 );
	
	// ��ü��Ŷ���� int������ ��ȯ
	int len = userGet32Bit( DataLength, 0 );

	// ������ ��Ŷ ���� ������ ���� �����Ҵ�
	free( buff );
	buff = (char*)calloc( len-4, sizeof(char) );

	int cnt = 0;
	iRecvBytes = 0;
	TCHAR tb[64];

	while( (cnt = recv( mySocket, buff+iRecvBytes, len-4-iRecvBytes, 0 )) != 0 )
	{
		if( cnt == SOCKET_ERROR ) break;
		iRecvBytes += cnt;

		wsprintf( tb, L">> �ٿ�ε� �� <<\n\n%d / %d", iRecvBytes, len-4 );
		SetWindowText( hStaticMsg, tb );
	}


	if( cnt == SOCKET_ERROR )
	{
		SetWindowText( hStaticMsg, L">> ���ſ��� �߻� <<\n\nȮ�ι�ư�� ������\n���׷��̵带\n��õ� �մϴ�\n\n\n[Ȯ��]" );
		bRecvError = TRUE;
	}
	else
	{
		if( len-4 == iRecvBytes )
		{
			wsprintf( tb, L">> �ٿ�ε� �Ϸ� <<\n\n%d / %d", iRecvBytes, len-4 );
			SetWindowText( hStaticMsg, tb );
		}
		else
		{
			SetWindowText( hStaticMsg, L">> ���ſ��� �߻� <<\n\nȮ�ι�ư�� ������\n���׷��̵带\n��õ� �մϴ�\n\n\n[Ȯ��]" );
			bRecvError = TRUE;
		}
	}
}




// 32bit int ------> byte array
void userSet32Bit( int value, char *buf, int offset )
{
	buf[offset] = (char)(value >> 24);
	buf[offset+1] = (char)(value >> 16);
	buf[offset+2] = (char)(value >> 8);
	buf[offset+3] = (char)value;
}


// byte array --------------> 32 bit int
int userGet32Bit( char *buf, int offset )
{

	int intValue = 0;
	
	for( int i = offset; i < offset+4; i++ )
	{
		intValue += (buf[i]&(0xff))<<(8*(3-i));
	}
	
	return intValue;
}




BOOL ConnectionResult()
{
	TCHAR NewFilePath[MAX_PATH];
	char *pFrom	= buff+1;
	char *search = NULL;

	while( (search = (char*)memchr( pFrom, US, 100 )) )
	{
		if( search != NULL )
		{
			TCHAR wszFileName[256];
			ZeroMemory( wszFileName, sizeof(wszFileName) );
			mbstowcs( wszFileName, pFrom, search-pFrom );
			
			search++;
			int len = userGet32Bit( search, 0 );
			search += 5;
			
			ZeroMemory( NewFilePath, sizeof(NewFilePath) );
			wcscpy( NewFilePath, path );
			wcscat( NewFilePath, wszFileName );
			
			HANDLE hFile = CreateFile( NewFilePath, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			if( hFile != INVALID_HANDLE_VALUE )
			{	
				DWORD dw;
				WriteFile( hFile, search, len, &dw, NULL );	
				CloseHandle( hFile );
			}

			search += len;
			pFrom = search + 1;
		}
	}

	SetWindowText( hStaticMsg, L">> ���׷��̵�Ϸ� <<\n\nȮ�ι�ư�� ������\n���α׷��� �ٽ�\n�����մϴ�\n\n\n[Ȯ��]" );

	if( buff != NULL )
	{
		free( buff );
		buff = NULL;
	}

	bComplete = TRUE;

	return TRUE;
}



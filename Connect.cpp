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
//  RAS 네트워크 접속
//
//---------------------------------------------------------------------------------------------
// RASConnection - RAS 네트워크 접속
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
//  RAS 네트워크 접속 해재
//
//---------------------------------------------------------------------------------------------
// RASConnection - RAS 네트워크 접속 해재
//
void RASDisConnect()
{
	DWORD ret;
	ret = RasHangUp( rascon );
}







//=============================================================================================
// 소켓 네트워크 접속
//
//---------------------------------------------------------------------------------------------
// Connection - 네트워크 접속
//
DWORD WINAPI Connection( LPVOID lpParameter )
{
	bConnection = TRUE;
	Sleep( 1000 );
	
	// 윈속 초기화
	if( WSAStartup( MAKEWORD(2,0), &wsaData ) == 0 )
	{
		// 소켓 생성
		mySocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 
		if( mySocket != INVALID_SOCKET )
		{
			ZeroMemory( &sokAddr, sizeof(sokAddr) );
			sokAddr.sin_family		= AF_INET;
			
			// 모드에 따라 다른 포트번호 할당
		#ifdef TEST_MODE
			sokAddr.sin_port	= htons( 60008 );
		#else
			sokAddr.sin_port	= htons( 60009 );
		#endif
			
			sokAddr.sin_addr.s_addr	= inet_addr( CONN_URL );
			
			// 서버에 접속
			int iRet = connect( mySocket, (SOCKADDR*)&sokAddr, sizeof(sokAddr) );

			if( iRet != SOCKET_ERROR )
			{
				SetWindowText( hStaticMsg, L">> 서버접속 성공 <<" );

				// 변수 초기화
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
					// 데이터 전송
					if( SendData() )
					{
						RecvData();
					}
					// 전송 실패
					else
					{
						SetWindowText( hStaticMsg, L">> 전송 실패 <<\n\n확인버튼을 누르면\n업그레이드를\n재시도 합니다\n\n\n[확인]" );
						bRecvError = TRUE;
					}
				}
	
			}
			else
			{
				SetWindowText( hStaticMsg, L">> 서버 접속 실패 <<\n\n확인버튼을 누르면\n업그레이드를\n재시도 합니다\n\n\n[확인]" );
				bRecvError = TRUE;
			}

			// 연결 종료
			shutdown( mySocket, SD_BOTH );
			// 소켓 닫기
			closesocket( mySocket );
			
		}
		else
		{
			SetWindowText( hStaticMsg, L">> 소켓 생성 실패 <<\n\n확인버튼을 누르면\n업그레이드를\n재시도 합니다\n\n\n[확인]" );
			bRecvError = TRUE;
		}


		// 윈속 라이브러리 제거
		WSACleanup();
	}
	else
	{
		SetWindowText( hStaticMsg, L">> 윈속 초기화 실패 <<\n\n확인버튼을 누르면\n업그레이드를\n재시도 합니다\n\n\n[확인]" );
		bRecvError = TRUE;
	}

	// Ras연결 종료
	RASDisConnect();

	bConnection = FALSE;
	ExitThread(0);
	return 0;
}




// 업그레이드요청패킷 만들기
BOOL SendPacketMake()
{
	iPacketSize = 51;
	buff = (char*)calloc( iPacketSize, sizeof(char) );
	
	// 헤더 ( 25바이트 ) 
	userSet32Bit( iPacketSize, buff, 0 );
	memset( buff+4, '0', 10 );
	memcpy( buff+14, szPhoneNumber, 11 );

	//////////////////////////////////////////
	memcpy( buff+25, "00000000000000000000000", 23 );
	memcpy( buff+48, "99", 2 );

	buff[50] = EOT;
	return TRUE;
}




// 데이터 전송
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
	SetWindowText( hStaticMsg, L">> 다운로드 중 <<" );

	// 전체패킷길이 수신
	char DataLength[4];
	recv( mySocket, DataLength, 4, 0 );
	
	// 전체패킷길이 int형으로 변환
	int len = userGet32Bit( DataLength, 0 );

	// 나머지 패킷 길이 수신할 공간 동적할당
	free( buff );
	buff = (char*)calloc( len-4, sizeof(char) );

	int cnt = 0;
	iRecvBytes = 0;
	TCHAR tb[64];

	while( (cnt = recv( mySocket, buff+iRecvBytes, len-4-iRecvBytes, 0 )) != 0 )
	{
		if( cnt == SOCKET_ERROR ) break;
		iRecvBytes += cnt;

		wsprintf( tb, L">> 다운로드 중 <<\n\n%d / %d", iRecvBytes, len-4 );
		SetWindowText( hStaticMsg, tb );
	}


	if( cnt == SOCKET_ERROR )
	{
		SetWindowText( hStaticMsg, L">> 수신오류 발생 <<\n\n확인버튼을 누르면\n업그레이드를\n재시도 합니다\n\n\n[확인]" );
		bRecvError = TRUE;
	}
	else
	{
		if( len-4 == iRecvBytes )
		{
			wsprintf( tb, L">> 다운로드 완료 <<\n\n%d / %d", iRecvBytes, len-4 );
			SetWindowText( hStaticMsg, tb );
		}
		else
		{
			SetWindowText( hStaticMsg, L">> 수신오류 발생 <<\n\n확인버튼을 누르면\n업그레이드를\n재시도 합니다\n\n\n[확인]" );
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

	SetWindowText( hStaticMsg, L">> 업그레이드완료 <<\n\n확인버튼을 누르면\n프로그램을 다시\n시작합니다\n\n\n[확인]" );

	if( buff != NULL )
	{
		free( buff );
		buff = NULL;
	}

	bComplete = TRUE;

	return TRUE;
}



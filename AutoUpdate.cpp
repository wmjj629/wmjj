// AutoUpdate.cpp : Defines the entry point for the application.
//

#include "stdafx.h"


HWND		hWnd;
HINSTANCE	hInst;

// 어플리케이션 이름
const TCHAR szAppName[] = TEXT( "X-Ter-Update" );

TCHAR		path[MAX_PATH];				// 어플리케이션 설치 경로

UINT	g_dwOEMKeyMsg;

HWND	hFindHwnd;
HWND	hStaticTitle;
HWND	hStaticMsg;

BOOL	bScanButtonPush;

char	szPhoneNumber[16];

extern char *buff;
extern BOOL bRecvError;
extern BOOL bComplete;

// MainWndProc용 메시지 디스패치 테이블
const struct decodeUINT MainMessages[] = {
	WM_CREATE,			DoCreateMain,
	WM_TIMER,			DoTimerMain,
	WM_KEYDOWN,			DoKeyDownMain,
	WM_DESTROY,			DoDestroyMain,
	WM_PAINT,			DoPaintMain,
	WM_RASDIALEVENT,	DoRasEvent
};



//=================================================================================================
// 프로그램 진입점
//
int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG Message;
	
	// 프로그램 초기화
	hWnd = InitInstance( hInstance, lpCmdLine, nCmdShow );
	if( hWnd == 0 ) return 0x10;
	
	// 메시지 루프
	while( GetMessage( &Message, 0, 0, 0) ) 
	{
		TranslateMessage( &Message );
		DispatchMessage( &Message );
	}
	
	return TermInstance( hInstance,  Message.wParam );
}




//---------------------------------------------------------------------------------------------------
// 인스턴스 초기화
//
HWND InitInstance( HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	HWND hWnd;
	WNDCLASS wc;
	
	hInst = hInstance;
	
	// 주 윈도우 클래스 등록
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	wc.hCursor			= NULL;//LoadCursor( NULL,IDC_ARROW );
	wc.hIcon			= NULL;//LoadIcon(NULL,IDI_APPLICATION);
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= (WNDPROC)MainWndProc;
	wc.lpszClassName	= szAppName;
	wc.lpszMenuName		= NULL;//MAKEINTRESOURCE( IDR_MENU1 );
	wc.style			= 0;	
	
	if( RegisterClass(&wc) == 0 ) return 0;
	
	
	// 주 윈도우 생성
	hWnd = CreateWindow( szAppName, L"자동업그레이드", WS_VISIBLE|WS_CAPTION, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL );
	
	if( !IsWindow(hWnd) ) return 0;
	
	return hWnd;
}


//----------------------------------------------------------------------------------------------------
// 프로그램 클린업
//
int TermInstance( HINSTANCE hInstance, int nDefRC )
{
	if( buff != NULL )
	{
		free( buff );
		buff = NULL;
	}

	RASDisConnect();
	return nDefRC;
}






//=====================================================================================================
// 주 윈도우 메시지 처리 프로시저
//
//-----------------------------------------------------------------------------------------------------
// MainWndProc - 프로그램 윈도우 콜백 함수
//
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	if( wMsg == g_dwOEMKeyMsg ) return DoOSMsgKey( hWnd, wMsg, wParam, lParam );
	if( wMsg == WM_CTLCOLORSTATIC )
	{
		if( (HWND)lParam == hStaticMsg ) SetTextColor( (HDC)wParam,RGB(0,0,255) );
		return (LRESULT)GetStockObject( WHITE_BRUSH );
	}

	for( int i = 0; i < dim(MainMessages); i++ )
	{
		if( wMsg == MainMessages[i].Code )
			return (*MainMessages[i].Fxn)( hWnd, wMsg, wParam, lParam );
	}
	
	return DefWindowProc( hWnd, wMsg, wParam, lParam );
}






//-------------------------------------------------------------------------------------------------------
// DoCreateMain - WM_CREATE 메시지 처리
//
LRESULT DoCreateMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	//------------------------------------------------------------------
	// 프로그램 설치 경로 구함
	//
	GetModuleFileName(0, path, MAX_PATH);
	int len = lstrlen( path );
	for( int i=len; i >=0; i-- )
	{
		if( path[i] == '\\' )
		{
			path[i+1] = 0;
			break;
		}
	}

	
	hStaticTitle = CreateWindow( L"static", L"[업그레이드중]\n\n잠시만 기다려\n주십시오", 
		WS_CHILD|WS_VISIBLE|ES_CENTER|WS_CLIPCHILDREN,
						10, 15, 220, 100, hWnd, (HMENU)100, hInst, NULL ); 

	hStaticMsg = CreateWindow( L"static", NULL, 
		WS_CHILD|WS_VISIBLE|ES_CENTER|WS_CLIPCHILDREN,
						10, 130, 220, 160, hWnd, (HMENU)200, hInst, NULL ); 

	LOGFONT lf;
	ZeroMemory( &lf, sizeof(LOGFONT) );
	lf.lfHeight = 23;
	lf.lfWeight	= 900;
	lf.lfCharSet = HANGEUL_CHARSET;
	SendMessage( hStaticTitle, WM_SETFONT, (WPARAM)CreateFontIndirect( &lf ), MAKELPARAM(FALSE,0) );
	lf.lfHeight = 19;
	SendMessage( hStaticMsg, WM_SETFONT, (WPARAM)CreateFontIndirect( &lf ), MAKELPARAM(FALSE,0) );
	
	// 단말기 키메시지 등록
	g_dwOEMKeyMsg = RegisterWindowMessage( OS_MSG_OEM_KEY );



	// 폰번호 읽기
	TCHAR NewPath[MAX_PATH];
	wcscpy( NewPath, path );
	wcscat( NewPath, L"PhoneNumber.DAT" );

	memset( szPhoneNumber, '0', 11 );

	HANDLE hFile = CreateFile( NewPath, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		int fsize = GetFileSize( hFile, NULL );
		if( fsize > 0 )
		{
			DWORD dw;
			char buff[16] = {0};
			ReadFile( hFile, buff, fsize, &dw, NULL );
			
			if( dw > 0 )
			{
				buff[dw] = NULL;
				strcpy( szPhoneNumber, buff );
			}
		}
		
		CloseHandle( hFile );
	}


	SetTimer( hWnd, ID_TIMER, 3000, NULL );
	return 0;
}



LRESULT DoTimerMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	KillTimer( hWnd, ID_TIMER );
	//if( hFindHwnd != NULL ) SendMessage( hFindHwnd, WM_DESTROY, NULL, NULL );

	RASConnection();
	return 0;
}




LRESULT DoKeyDownMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}




LRESULT DoPaintMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint( hWnd, &ps );
	EndPaint( hWnd, &ps );
	return 0;
}




LRESULT DoDestroyMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	// 메시지루프 종료
	PostQuitMessage(0);
	return 0;
}



LRESULT DoOSMsgKey( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	// SCAN키, 좌우 사이드 키
	if( wParam == VK_OEM_APP1 || wParam == VK_OEM_APP2 || wParam == VK_OEM_APP3 )
	{
		
		// Key Down ////////////////////
		if( lParam == 0 && !bScanButtonPush )
		{ 
			bScanButtonPush = TRUE;

			if( bRecvError )
			{
				bRecvError	= FALSE;
				bComplete	= FALSE;
				RASConnection();
			}
			else if( bComplete )
			{
				bComplete	= FALSE;
				ReStartApp();
			}
			
		}
		// Key Up /////////////////////
		else if( lParam == 1 )
		{
			bScanButtonPush = FALSE;
		}
	}

	return 0;
}


//-------------------------------------------------------------------------------------------------------
// DoRasEvent - WM_RASDIALEVENT 메시지 처리
//
LRESULT DoRasEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	HANDLE hThread;
	RASCONNSTATE RasState = (RASCONNSTATE)wParam;
	
	switch( RasState )
	{
	case RASCS_OpenPort :
		SetWindowText( hStaticMsg, L"포트를 여는중" );
		break;	
	case RASCS_PortOpened :
		SetWindowText( hStaticMsg, L"포트열기 성공" );
		break;
	case RASCS_Authenticate :
		SetWindowText( hStaticMsg, L"인증시도에 응답중" );
		break;
	case RASCS_Authenticated :
		SetWindowText( hStaticMsg, L"네트워크 사용자 인증됨" );
		break;
	case RASCS_Connected :
		SetWindowText( hStaticMsg, L"인터넷 연결성공" );
		
		hThread = CreateThread( NULL, 0, Connection, NULL, 0, NULL );
		CloseHandle( hThread );
		break;
	case RASCS_Disconnected :
		//SetWindowText( hStaticMsg, L"전화접속 연결 해제" );
		Sleep( 1000);
		if( !bRecvError ) ConnectionResult();
		break;
	}
	
	return 0;
}



void ReStartApp()
{
	TCHAR NewFilePath[MAX_PATH];
	PROCESS_INFORMATION pi;
	
	ZeroMemory( NewFilePath, sizeof(NewFilePath) );
	wcscpy( NewFilePath, path );
	wcscat( NewFilePath, L"X-Ter.exe" );
	CreateProcess( NewFilePath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi );
	
	
	Sleep( 2000 );
	SetForegroundWindow( FindWindow( NULL, L"X-Ter-DCT-806" ) );
	DestroyWindow( hWnd );
}
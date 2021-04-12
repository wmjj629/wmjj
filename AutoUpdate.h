// 브랜치 커밋 
// 테스트 되돌리기
#ifndef _AUTO_UPDATE_H_
#define _AUTO_UPDATE_H_

#define ID_TIMER	100

#define DEVICE_NAME		( TEXT("AIP1:") )

#define	VK_OEM_APP1				0xE1	// <SCAN > Xllp Scan Code 0xC1 
#define	VK_OEM_APP2				0xE2	// <SCAN1> Xllp Scan Code 0xC2
#define	VK_OEM_APP3				0xE3	// <SCAN2> Xllp Scan Code 0xC3
#define	VK_OEM_APP4				0xE4	// <ALPHA> Xllp Scan Code 0xC4
#define	VK_OEM_APP5				0xE5	// <CALL > Xllp Scan Code 0xC5
#define OS_MSG_OEM_KEY	TEXT("OS_MSG_OEM_KEY")

// 항목 개수 반환
#define dim(x) ( sizeof(x) / sizeof(x[0]) )

// 메시지와 함수를 묶는 구조체
struct decodeUINT
{
	UINT Code;
	LRESULT (*Fxn)( HWND, UINT, WPARAM, LPARAM );
};

// 윈도우 프로시져
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );


// 메시지 핸들러
LRESULT DoCreateMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
LRESULT DoPaintMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
LRESULT DoTimerMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
LRESULT DoKeyDownMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
LRESULT DoKeyUpMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
//LRESULT DoKillFocus( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
LRESULT DoRasEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
LRESULT DoDestroyMain( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
LRESULT DoOSMsgKey( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );
//LRESULT DoOSMgsPowerOff( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );


int TermInstance( HINSTANCE hInstance, int nDefRC );
HWND InitInstance( HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow ); 

void ReStartApp();

#endif

#include <vcl.h>
#pragma hdrstop
#include <windows.h>
#pragma comment(lib, "Winmm.lib")
#include<Mmsystem.h>
#include "Unit1.h"
#include <vfw.h>
#include <time.h>
#include <Tchar.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

//公共變數宣告。
long dwDriverIndex;	// 電腦的驅動程式代碼。
wchar_t szDeviceName[80];
wchar_t szDeviceVersion[80];
wchar_t *szWindowName;
wchar_t wstrCaptureWindowName[20] = L"My Capture Window";
wchar_t wstrCaptureFileName[80];  // 錄影儲存之檔名。

HWND hwndVideo ; 	// 儲存所建立的視訊擷取視窗之代碼。
long dwFileAlloc;              // 錄影要預先配置的硬碟記憶體空間, 以位元組 (byte) 為單位。


int iCapWindowX1; 	// 視訊擷取視窗左上角座標 ( iCapWindowX1, iCapWindowY1 ) 。
int iCapWindowY1;
int iCapWindowWidth;
int iCapWindowHeight;
int i, j, k,t;
int iFrameCounter;
int iVideoWidth, iVideoHeight;
int iVideoFormatSize;
int iVideoMacroWidth;    // iVideoMacroWidth: 表示一列有多少個宏像素 ( 視訊寬度的一半 )。
int iRowBufferSize;           // iRowBufferSize: 為一列所需的記憶體大小, 宏像素個數乘以 4 。
int iPartner;
int test=1;
int iObjectX1 , iObjectY1;
int iObjectX2 , iObjectY2;
int iObjectWidth , iObjectHeight;
int iR , iG , iB;
int iY , iU , iV;
int iC1, iD , iE, iY1, iY2;
int iVideoBlockWidth, iVideoBlockHeight;  // 視訊擷取視窗的區塊寬與區塊高
int iMinY=100, iMaxY=250 ;
int iMinU=80 , iMaxU=110 ;
int iMinV=90 , iMaxV=135;
int iBlockChangeCounter [60][80] ;
int iThreshold = 1600;
int iY1Difference,iUDifference,iY2Difference,iVDifference,iSquareDifferenceSum1,iSquareDifferenceSum2;
int m,n;

// 用來儲存要交換對象的索引值。
unsigned char ucTempY1, ucTempU, ucTempY2, ucTempV;    // 暫存待交換的色彩值。
unsigned char ucGeckoRGB[100][100][3];
unsigned char ucGeckoYUV[100][100][3];

// 用來儲存要交換對象的索引值。
unsigned char pauseTempY1, pauseTempU, pauseTempY2, pauseTempV;    // 暫存待交換的色彩值。
unsigned char pauseRGB[100][100][3];
unsigned char pauseYUV[100][100][3];

// 用來儲存要交換對象的索引值。
unsigned char diceTempY1, diceTempU, diceTempY2, diceTempV;    // 暫存待交換的色彩值。
unsigned char diceRGB[100][100][3];
unsigned char diceYUV[100][100][3];

// 回呼函數宣告
LRESULT CALLBACK FrameCallbackCounter(HWND hwndVideo , PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackMirror(HWND hwndCapture, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackSingleColorObject ( HWND hwndCapture , PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackSpecificColorFiltering ( HWND hwndVideo, PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackBackgroundRGB ( HWND hwndVideo, PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackDifferenceBinaryYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackDifferenceBlockYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackPlayWithGecko6A( HWND hwndCapture, PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackPlayWithGecko6B( HWND hwndCapture, PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackGecko(HWND hwndVideo , PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackBackgroundYUY2(HWND hwndVideo, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackDifferenceColorYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr );

BITMAPINFO bmiBitmapInfo;

BYTE *ptrCapture;
BYTE * ptrImage;
BYTE * ptrImage1;
BYTE * ptrImage2;

tagCapDriverCaps cdcCapDriverCapability ;  // tagCapDriverCaps 為結構資料型態
CAPTUREPARMS cpCaptureParms;  // CAPTUREPARMS 為錄影參數之結構資料型態

typedef struct stVirtualObject  // 虛擬物件結構資料型態名稱。
{
	int iID;       // 虛擬物件編號。
	int iX1;       // 顯示位置左上角座標 ( X1, Y1 )。
	int iY1;
	int iX2;       // 顯示位置右下點座標 ( X2, Y2 )。
	int iY2;
	int iHeight;     // 虛擬物件之高。
	int iWidth;     // 虛擬物件之寬。
	int iBlockX1;    // 顯示位置左上角區塊座標 ( BlockX1, BlockY1 )。
	int iBlockY1;
	int iBlockX2;    // 顯示位置右下點區塊座標 ( BlockX2, BlockY2 )。
	int iBlockY2;
	bool bVisible;   // 是否顯示在視訊擷取視窗之中
	unsigned char ucBackgroundRed;   // 虛擬物件背景色 ( 紅 )。
	unsigned char ucBackgroundGreen;  // 虛擬物件背景色 ( 綠 )。
	unsigned char ucBackgroundBlue;  // 虛擬物件背景色 ( 藍 )。
} stObject;     // 虛擬物件結構資料型態名稱。
stVirtualObject voGecko;  //宣告虛擬物件。
stVirtualObject pause;
stVirtualObject dice;



//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::On1Click(TObject *Sender)
{
  // 作業系統對於安裝的攝影機驅動程式會從 0 開始編號。
	dwDriverIndex = 0;
	// 用來放 TLabel 元件的 panel 的 Property Visible 設為 true, 看得到了 !
	pnlDriverDescription->Visible = true;
	if ( capGetDriverDescription(dwDriverIndex, szDeviceName, sizeof(szDeviceName),
	   szDeviceVersion, sizeof(szDeviceVersion)))   // 當成功取得相關資訊會傳回 true。
	{
		// 並將該字串變數內容顯示在 lblCapDriverName, lblCapDriverVer
		// 兩個 TLable 元件的 Caption 屬性上。
		lblCapDriverName->Caption = "Name: " + AnsiString(szDeviceName);
		// lblCapDriverName 為 TLabel 元件的 Name。
		lblCapDriverVersion->Caption = szDeviceVersion;
		// lblCapDriverVersion 為 TLabel 元件的 Name。
	}

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Off1Click(TObject *Sender)
{
	// 將 pnlDriverDescription 的 Property Visible 設為 false,
    // pnlDriverDescription 就會從畫面消失, 完全看不見。
	pnlDriverDescription->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Create1Click(TObject *Sender)
{
	iCapWindowX1 = 10;
	iCapWindowY1 = 10;
	iCapWindowWidth = 640; 	// 視訊擷取視窗的寬設定為 320。
	iCapWindowHeight = 480;	// 視訊擷取視窗的高設定為 240。

	// 建立視訊擷取視窗。
	pnlCaptureWindow->Visible = true;
	// pnlCaptureWindow 為 Panel 物件的 Name , 用來放 capture window。
	hwndVideo = capCreateCaptureWindow(
		szWindowName, WS_CHILD | WS_VISIBLE,
		iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight,
		Form1->pnlCaptureWindow->Handle, 1);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Destroy1Click(TObject *Sender)
{
	// 要關閉視訊擷取視窗的方法, 如同要關閉一般視窗一樣,
    // 只要利用 Win32 API 的 DestroyWindow 函數,
    // 並告訴電腦你要關閉的視窗代碼 ( handle ) 即可。
    DestroyWindow(hwndVideo);
	pnlCaptureWindow->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Disconnect1Click(TObject *Sender)
{
   // 將視訊裝置驅動程式與視訊擷取視窗之間的連結取消。
   capDriverDisconnect(hwndVideo);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Connect1Click(TObject *Sender)
{
	// 將所建立的視訊擷取視窗與視訊裝置的驅動程式連結起來,
	// 連起來後, 視訊裝置所擷取到的畫面, 將來就會直接被送到視訊擷取視窗中顯示出來。
	capDriverConnect(hwndVideo, dwDriverIndex);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Enable1Click(TObject *Sender)
{
	// 設定顯示模式為 Preview 模式。
	capPreview(hwndVideo, true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Disable1Click(TObject *Sender)
{
   // 將 Preview 顯示模式取消。
	capPreview(hwndVideo,false);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N30framesec1Click(TObject *Sender)
{
	// 設定顯示速率 33.3 miliseconds, or 30 FPS。
	capPreviewRate(hwndVideo, 33);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N15framesec1Click(TObject *Sender)
{
    // 設定顯示速率 66 miliseconds, or 15 FPS。
	capPreviewRate(hwndVideo, 66);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N3202401Click(TObject *Sender)
{
  // 調整視訊擷取視窗的大小為 320*240。
   pnlCaptureWindow->Width = 330;
   pnlCaptureWindow->Height = 250;
   iCapWindowWidth = 320;
   iCapWindowHeight = 240;
   MoveWindow(hwndVideo, iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight, true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N6404801Click(TObject *Sender)
{
	// 調整視訊擷取視窗的大小為 640 * 480。
	pnlCaptureWindow->Width = 650;
	pnlCaptureWindow->Height = 490;
	iCapWindowWidth = 640;
	iCapWindowHeight = 480;
   	MoveWindow(hwndVideo, iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight, true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::On2Click(TObject *Sender)
{
	// 將視訊畫面設定成可以隨著視訊擷取視窗大小而調整。
	capPreviewScale(hwndVideo, true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Off2Click(TObject *Sender)
{
   // 取消設定視訊畫面隨著視訊擷取視窗大小而調整。
   capPreviewScale(hwndVideo, false);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Autoprocessing1Click(TObject *Sender)
{
	iCapWindowX1 = 10;
	iCapWindowY1 = 10;
	iCapWindowWidth = 640;
	iCapWindowHeight = 480;

	pcWebcam->ActivePage = tsCaptureWindow ;

	 // 作業系統對於安裝的攝影機驅動程式會從 0 開始編號。
	dwDriverIndex = 0;
	// 用來放 TLabel 元件的 panel 的 Property Visible 設為 true, 看得到了 !
	pnlDriverDescription->Visible = true;
	if ( capGetDriverDescription(dwDriverIndex, szDeviceName, sizeof(szDeviceName),
	   szDeviceVersion, sizeof(szDeviceVersion)))   // 當成功取得相關資訊會傳回 true。
	{
			lblCapDriverName->Caption = "Name: " + AnsiString(szDeviceName);
			lblCapDriverVersion->Caption = szDeviceVersion;
	}
	// 建立視訊擷取視窗。
	pnlCaptureWindow->Visible = true;
	// pnlCaptureWindow 為 Panel 物件的 Name , 用來放 capture window。
	hwndVideo = capCreateCaptureWindow(
		szWindowName, WS_CHILD | WS_VISIBLE,
		iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight,
		Form1->pnlCaptureWindow->Handle, 1);

	capDriverConnect(hwndVideo, dwDriverIndex);
	capPreview(hwndVideo, true);
	capPreviewRate(hwndVideo, 300);
	capPreviewScale(hwndVideo, true);

	// 取得視訊裝置驅動程式的效能。
	if ( capDriverGetCaps ( hwndVideo , &cdcCapDriverCapability ,  sizeof ( cdcCapDriverCapability ) ) )
	{
		lblDriverIndex->Caption = AnsiString ( "Driver Index:") + AnsiString ( cdcCapDriverCapability.wDeviceIndex );
		if ( cdcCapDriverCapability.fHasOverlay )
			lblOverlay->Caption = AnsiString  ( " Can device overlay ?  Yes! " ) ;
		else lblOverlay->Caption = AnsiString ( " Can device overlay ? No! " ) ;
		// lblOverlay 為 Label 物件的 Name。

		if ( cdcCapDriverCapability.fHasDlgVideoSource )
		{
			// miVideoSource 為 MainMenu 物件的 MenuItem 之名稱。
			// 將 MenuItem miVideoSource元件設定成可以使用的狀態。
			miVideoSource->Enabled = true ;
			lblVideoSourceDialog->Caption = AnsiString ( " Has video source dialog ? Yes! " );
		}
		else
		{
			// 將 MenuItem miVideoSource元件設定成失效, 無法使用的狀態。
			miVideoSource->Enabled = false ;
			lblVideoSourceDialog->Caption = AnsiString ( " Has video source dialog? No! " );
		}

		if ( cdcCapDriverCapability.fHasDlgVideoFormat )
		{
			// miVideoFormat 為 MainMenu 物件的 MenuItem 之名稱。
			// 將 MenuItem miVideoFormat元件設定成可以使用的狀態。
			miVideoFormat->Enabled = true ;
			lblVideoFormatDialog->Caption = AnsiString ( " Has video format dialog? Yes! " );
		}
		else
		{
			// 將 MenuItem miVideoFormat元件設定成失效, 無法使用的狀態。
			miVideoFormat->Enabled = false ;
			lblVideoFormatDialog->Caption = AnsiString ( " Has video format dialog ? No " ) ;
		}

		if ( cdcCapDriverCapability.fHasDlgVideoDisplay )
		{
            // miVideoDisplay 為 MainMenu 物件下的 MenuItem 之名稱。
			// 將 MenuItem miVideoDisplay 元件設定成可以使用的狀態。
			miVideoDisplay->Enabled = true ;
			lblVideoDisplayDialog->Caption = AnsiString ( " Has video display dialog ? Yes " ) ;
		}
		else
		{
			// 將 MenuItem miVideoDisplay 元件設定成失效, 無法使用的狀態。
			miVideoDisplay->Enabled = false ;
			lblVideoDisplayDialog->Caption = AnsiString ( " Has video display dialog ? No " ) ;
		}

		if ( cdcCapDriverCapability.fCaptureInitialized )
			lblDriverReady->Caption = AnsiString ( " Driver ready to capture ? Yes " ) ;
		else lblDriverReady->Caption = AnsiString ( " Driver ready to capture ? No " ) ;

		if ( cdcCapDriverCapability.fCaptureInitialized )
			lblDriverPalette->Caption = AnsiString (" Can Driver make palette ? Yes " ) ;
		else lblDriverPalette->Caption = AnsiString ( " Can Driver make palette ? No " ) ;
	}
	else ShowMessage ( " Capture Window does not connect to any Capture Driver ! " ) ;

    // 將iFrameCounter 值重設為0。
	iFrameCounter = 0;
	// 開啟預覽回呼函式 FrameCallbackCounter。
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackCounter);

    iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	lblVideoFormatSize->Caption = "Video Format Size : " + AnsiString(iVideoFormatSize);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	lblSize->Caption = "Size : " + AnsiString(bmiBitmapInfo.bmiHeader.biSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	lblWidth->Caption = "Width : " + AnsiString(iVideoWidth);
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	lblHeight->Caption = "Height : " + AnsiString(iVideoHeight);
	lblPlanes->Caption = "Planes : " + AnsiString(bmiBitmapInfo.bmiHeader.biPlanes);
	lblBitCount->Caption = "Bit Count: " + AnsiString(bmiBitmapInfo.bmiHeader.biBitCount);
	lblCompression->Caption = "Compression : " + AnsiString(bmiBitmapInfo.bmiHeader.biCompression );
	lblSizeImage->Caption = "Image Size: " + AnsiString(bmiBitmapInfo.bmiHeader.biSizeImage);
	lblXPelsPerMeter->Caption = "X Pels Per Meter : " + AnsiString(bmiBitmapInfo.bmiHeader.biXPelsPerMeter);
	lblYPelsPerMeter->Caption = "Y Pels Per Meter : " + AnsiString(bmiBitmapInfo.bmiHeader.biYPelsPerMeter);
	lblColorUsed->Caption = "Color Used : " + AnsiString(bmiBitmapInfo.bmiHeader.biClrUsed);
	lblColorImportant->Caption = "Color Important: " + AnsiString(bmiBitmapInfo.bmiHeader.biClrImportant);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Capabilities1Click(TObject *Sender)
{
	 pcWebcam->ActivePage = tsInformation ;
	// 取得視訊裝置驅動程式的效能。
	if ( capDriverGetCaps ( hwndVideo , &cdcCapDriverCapability ,  sizeof ( cdcCapDriverCapability ) ) )
	{
		lblDriverIndex->Caption = AnsiString ( "Driver Index:") + AnsiString ( cdcCapDriverCapability.wDeviceIndex );
		if ( cdcCapDriverCapability.fHasOverlay )
			lblOverlay->Caption = AnsiString  ( " Can device overlay ?  Yes! " ) ;
		else lblOverlay->Caption = AnsiString ( " Can device overlay ? No! " ) ;
		// lblOverlay 為 Label 物件的 Name。

		if ( cdcCapDriverCapability.fHasDlgVideoSource )
		{
			// miVideoSource 為 MainMenu 物件的 MenuItem 之名稱。
			// 將 MenuItem miVideoSource元件設定成可以使用的狀態。
			miVideoSource->Enabled = true ;
			lblVideoSourceDialog->Caption = AnsiString ( " Has video source dialog ? Yes! " );
		}
		else
		{
			// 將 MenuItem miVideoSource元件設定成失效, 無法使用的狀態。
			miVideoSource->Enabled = false ;
			lblVideoSourceDialog->Caption = AnsiString ( " Has video source dialog? No! " );
		}

		if ( cdcCapDriverCapability.fHasDlgVideoFormat )
		{
			// miVideoFormat 為 MainMenu 物件的 MenuItem 之名稱。
			// 將 MenuItem miVideoFormat元件設定成可以使用的狀態。
			miVideoFormat->Enabled = true ;
			lblVideoFormatDialog->Caption = AnsiString ( " Has video format dialog? Yes! " );
		}
		else
		{
			// 將 MenuItem miVideoFormat元件設定成失效, 無法使用的狀態。
			miVideoFormat->Enabled = false ;
			lblVideoFormatDialog->Caption = AnsiString ( " Has video format dialog ? No " ) ;
		}

		if ( cdcCapDriverCapability.fHasDlgVideoDisplay )
		{
            // miVideoDisplay 為 MainMenu 物件下的 MenuItem 之名稱。
			// 將 MenuItem miVideoDisplay 元件設定成可以使用的狀態。
			miVideoDisplay->Enabled = true ;
			lblVideoDisplayDialog->Caption = AnsiString ( " Has video display dialog ? Yes " ) ;
		}
		else
		{
			// 將 MenuItem miVideoDisplay 元件設定成失效, 無法使用的狀態。
			miVideoDisplay->Enabled = false ;
			lblVideoDisplayDialog->Caption = AnsiString ( " Has video display dialog ? No " ) ;
		}

		if ( cdcCapDriverCapability.fCaptureInitialized )
			lblDriverReady->Caption = AnsiString ( " Driver ready to capture ? Yes " ) ;
		else lblDriverReady->Caption = AnsiString ( " Driver ready to capture ? No " ) ;

		if ( cdcCapDriverCapability.fCaptureInitialized )
			lblDriverPalette->Caption = AnsiString (" Can Driver make palette ? Yes " ) ;
		else lblDriverPalette->Caption = AnsiString ( " Can Driver make palette ? No " ) ;
	}
	else ShowMessage ( " Capture Window does not connect to any Capture Driver ! " ) ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miVideoSourceClick(TObject *Sender)
{
   if ( !capDlgVideoSource( hwndVideo ) )
	ShowMessage( " Can't open video source dialog !! " ) ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miVideoFormatClick(TObject *Sender)
{
	// 開啟視訊格式對話盒, 如果無法開啟就秀出訊息通知。
    if ( !capDlgVideoFormat ( hwndVideo )  )
		ShowMessage( " Can't open video format dialog ! ! " ) ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miVideoDisplayClick(TObject *Sender)
{
	 // 開啟視訊顯示對話盒, 如果無法開啟就秀出訊息通知。
    if ( !capDlgVideoDisplay( hwndVideo )  )
		ShowMessage( " Can't open video display dialog ! ! " ) ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miVideoCompressionClick(TObject *Sender)
{
	 // 開啟視訊壓縮對話盒, 如果無法開啟就秀出訊息通知。
    if ( !capDlgVideoCompression( hwndVideo )  )
		ShowMessage( " Can't open video compression dialog ! ! " ) ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnGrabFrameClick(TObject *Sender)
{
	// 擷取單張畫面到記憶體緩衝區之中, 視訊擷取視窗的畫面會暫時靜止,
    // 直到重新啟動 Preview or Overlay 顯示模式。
    // 靜止的好處是可以觀察取得的畫面是否滿意!
    if ( !capGrabFrame ( hwndVideo ) )
        ShowMessage ( " Can't Grab Frame to the video frame buffer ! " );

}
//---------------------------------------------------------------------------

void __fastcall TForm1::GrabFrameNoStopClick(TObject *Sender)
{
	    // 擷取單張畫面到記憶體緩衝區之中, 視訊擷取視窗的畫面不會暫時靜止。
    if ( !capGrabFrameNoStop ( hwndVideo ) )
        ShowMessage ( " Can't Grab Frame to the Video Frame Buffre ! " );

}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnEditCopyClick(TObject *Sender)
{
	 // 將記憶體緩衝區中的影像資料複製到剪貼簿, 可供其他應用程式也能透過剪貼簿取得。
    if ( !capEditCopy ( hwndVideo ) )
        ShowMessage ( " Can't Copy the Video Frame Buffre to the Clipboard ! " );

}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnFileSaveDIBClick(TObject *Sender)
{
	// 分成兩個步驟:
    // 1. 先透過 SavePictureDialog1 元件輸入要儲存的 bmp 檔名。
    // 2. 在透過 capFileSaveDIB 函式直接存檔。
    if ( SavePictureDialog1->Execute() )
	{
		if ( !capFileSaveDIB ( hwndVideo , &(SavePictureDialog1->FileName[1] ) ) )
			ShowMessage ( " Can't Save the Clipboard Image ! " );
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miFrameCounterClick(TObject *Sender)
{
	// 將iFrameCounter 值重設為0。
	iFrameCounter = 0;
	// 開啟預覽回呼函式 FrameCallbackCounter。
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackCounter);
}

LRESULT CALLBACK FrameCallbackCounter(HWND hwndVideo, PVIDEOHDR lpvhdr)
{
	// 每執行一次預覽回呼函式, 就將 iFrameCounter 加 1。
	iFrameCounter++;
	// 將預覽的畫面總數顯示在 Label lblFrameCounter元件之上。
	Form1->lblFrameCounter->Caption = AnsiString(iFrameCounter);
	return (0);
}

//---------------------------------------------------------------------------


void __fastcall TForm1::Off3Click(TObject *Sender)
{
   capSetCallbackOnFrame(hwndVideo, NULL);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::miFormatClick(TObject *Sender)
{
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	lblVideoFormatSize->Caption = "Video Format Size : " + AnsiString(iVideoFormatSize);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	lblSize->Caption = "Size : " + AnsiString(bmiBitmapInfo.bmiHeader.biSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	lblWidth->Caption = "Width : " + AnsiString(iVideoWidth);
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	lblHeight->Caption = "Height : " + AnsiString(iVideoHeight);
	lblPlanes->Caption = "Planes : " + AnsiString(bmiBitmapInfo.bmiHeader.biPlanes);
	lblBitCount->Caption = "Bit Count: " + AnsiString(bmiBitmapInfo.bmiHeader.biBitCount);
	lblCompression->Caption = "Compression : " + AnsiString(bmiBitmapInfo.bmiHeader.biCompression );
	lblSizeImage->Caption = "Image Size: " + AnsiString(bmiBitmapInfo.bmiHeader.biSizeImage);
	lblXPelsPerMeter->Caption = "X Pels Per Meter : " + AnsiString(bmiBitmapInfo.bmiHeader.biXPelsPerMeter);
	lblYPelsPerMeter->Caption = "Y Pels Per Meter : " + AnsiString(bmiBitmapInfo.bmiHeader.biYPelsPerMeter);
	lblColorUsed->Caption = "Color Used : " + AnsiString(bmiBitmapInfo.bmiHeader.biClrUsed);
	lblColorImportant->Caption = "Color Important: " + AnsiString(bmiBitmapInfo.bmiHeader.biClrImportant);

}
//---------------------------------------------------------------------------


LRESULT CALLBACK FrameCallbackGrayMode ( HWND hwndVideo , PVIDEOHDR lpvhdr )
{
	ptrCapture = lpvhdr->lpData;
	k = 0;
	for ( j=0; j< iVideoHeight; j++  )
	{
		for ( i=0; i<iVideoMacroWidth; i++ )
		{
			// 只要將每一個宏像素中的 U、V 色彩値改成 128, 就會呈現灰階畫面。
			ptrCapture[k+1] = 128;
			ptrCapture[k+3] = 128;
			k = k + 4;
		}
	}
	return (0);
}

LRESULT CALLBACK FrameCallbackColorMode ( HWND hwndVideo , PVIDEOHDR lpvhdr )
{
	ptrCapture = lpvhdr->lpData;
	k = 0;
	for ( j=0; j< iVideoHeight; j++  )
	{
		for ( i=0; i<iVideoMacroWidth; i++ )
		{
			// 只要將每一個宏像素中的 U、V 色彩値改成 128, 就會呈現灰階畫面。
			//ptrCapture[k] = iY;
			ptrCapture[k+1] = iU;
			ptrCapture[k+3] = iV;
			k = k + 4;
		}
	}
	return (0);
}

void __fastcall TForm1::GrayMode1Click(TObject *Sender)
{
	// 設定 iFrameCounter 變數的初始值。
	iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight,
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoMacroWidth = iVideoWidth / 2;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	// 開啟預覽回呼函式FrameCallbackGrayMode。
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackGrayMode);
}
//---------------------------------------------------------------------------

LRESULT CALLBACK FrameCallbackMirror ( HWND hwndCapture, PVIDEOHDR lpvhdr)
{
	iFrameCounter ++;
	ptrCapture = lpvhdr->lpData;
	// iVideoMacroWidth: 表示一列有多少個宏像素 ( 視訊寬度的一半 )。
	// iRowBufferSize: 為一列所需的記憶體大小, 宏像素個數乘以 4 。
	// 處理鏡射問題。
	for ( j=0; j<iVideoHeight; j++)
	{
		for ( i=0; i<iVideoMacroWidth/2; i++  )
		{
			// 第 i 個 macropixel 的交換對象是第 ( iVideoWidth / 2 ) - i 。
			iPartner = iVideoMacroWidth - i - 1;
			// 先將第 i 個 marcopixel 的四個位元組的色彩值暫存起來。
			ucTempY1 = ptrCapture[j*iRowBufferSize+i*4];
			ucTempU = ptrCapture[j*iRowBufferSize+i*4+1];
			ucTempY2 = ptrCapture[j*iRowBufferSize+i*4+2];
			ucTempV = ptrCapture[j*iRowBufferSize+i*4+3];
			// 把 Partner 的四個位元組的色彩值複製過來, 但 Y1 與 Y2 要互換位置。
			ptrCapture[j*iRowBufferSize+i*4] = ptrCapture[j*iRowBufferSize+iPartner*4+2];
			ptrCapture[j*iRowBufferSize+i*4+1] = ptrCapture[j*iRowBufferSize+iPartner*4+1];
			ptrCapture[j*iRowBufferSize+i*4+2] = ptrCapture[j*iRowBufferSize+iPartner*4];
			ptrCapture[j*iRowBufferSize+i*4+ 3] = ptrCapture[j*iRowBufferSize+iPartner*4+3];
			// 將暫存的色彩值放到 Partner 的緩衝區記憶體之中, 但 Y1 與 Y2 要互換位置。
			ptrCapture[j*iRowBufferSize+iPartner*4] = ucTempY2;
			ptrCapture[j*iRowBufferSize+iPartner*4+1] = ucTempU;
			ptrCapture[j*iRowBufferSize+iPartner*4+2] = ucTempY1;
			ptrCapture[j*iRowBufferSize+iPartner*4+3] = ucTempV;
		}
	}
	return (0);
}






void __fastcall TForm1::Mirror1Click(TObject *Sender)
{
	// 設定 iFrameCounter 變數的初始值。
	iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight ,
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat(hwndVideo , &bmiBitmapInfo , iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	iVideoMacroWidth = iVideoWidth / 2;
	iRowBufferSize = iVideoMacroWidth * 4;
	// 開啟預覽回呼函式FrameCallbackMirror。
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackMirror);
}
//---------------------------------------------------------------------------

LRESULT CALLBACK FrameCallbackSingleColorObject ( HWND hwndVideo, PVIDEOHDR lpvhdr )
{
	// 取得視訊擷取視窗的緩衝區記憶體的起始位址。
	ptrCapture = lpvhdr->lpData;

	k = 0;
	for ( j = 0; j < iVideoHeight; j ++  )
	{
		for ( i = 0; i < iVideoWidth; i = i + 2 )
		{
			// 在座標 ( iObjectX1, iObjectY1 ) 與 ( iObjectX2, iObjectY2 ) 之間的區域要將色彩値改掉。
			if ( ( i > iObjectX1 ) && ( i < iObjectX2 ) && ( j > iObjectY1 ) && ( j < iObjectY2 ) )
			{
				ptrCapture [k + 0] = iY;
				ptrCapture [k + 1] = iU;
				ptrCapture [k + 2] = iY;
				ptrCapture [k + 3] = iV;
			}
			k = k + 4;
		}
	}
	Form1->lblFrameCounter->Caption = AnsiString(iFrameCounter);
	return ( 0 );
}


void __fastcall TForm1::Single1Click(TObject *Sender)
{
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight。
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize ( hwndVideo );
	capGetVideoFormat ( hwndVideo , &bmiBitmapInfo , iVideoFormatSize );
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	// 設定要填入的區域。
	iObjectX1 = 350;
	iObjectY1 = 40;
	iObjectWidth = 200;
	iObjectHeight = 50;
	iObjectX2 = iObjectX1 + iObjectWidth;
	iObjectY2 = iObjectY1 + iObjectHeight;
	// 設定要填入的色彩。
	iR = 0;
	iG = 255;
	iB = 0;
	iY = ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16;
	iU = ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128;
	iV = ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128;
	// 開啟預覽回呼函式。
	capSetCallbackOnFrame ( hwndVideo , &FrameCallbackSingleColorObject );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Color1Click(TObject *Sender)
{
	// 透過 TColorDiallog 元件來選定要填入視訊擷取視窗的顏色。
	if ( ColorDialog1->Execute( ) )
	{
		iR = GetRValue(ColorDialog1->Color);
		iG = GetGValue(ColorDialog1->Color);
		iB = GetBValue(ColorDialog1->Color);
		// 將 RGB 色彩轉換成 YUV。
		iY = ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16;
		iU = ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128;
		iV = ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128;
	}
	capSetCallbackOnFrame ( hwndVideo , &FrameCallbackSpecificColorFiltering );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SelectColor1Click(TObject *Sender)
{
	// 設定 iFrameCounter 變數的初始值。
	iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight,
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoMacroWidth = iVideoWidth / 2;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;

	// 透過 TColorDiallog 元件來選定要填入視訊擷取視窗的顏色。
	if ( Form1->ColorDialog1->Execute( ) )
	{
		iR = GetRValue(Form1->ColorDialog1->Color);
		iG = GetGValue(Form1->ColorDialog1->Color);
		iB = GetBValue(Form1->ColorDialog1->Color);
		// 將 RGB 色彩轉換成 YUV。
		iY = ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16;
		iU = ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128;
		iV = ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128;
	}

	// 開啟預覽回呼函式FrameCallbackGrayMode。
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackColorMode);

}
//---------------------------------------------------------------------------


void __fastcall TForm1::Gecko1Click(TObject *Sender)
{
	// 因為 FrameCallBackGecko 需要用到 iVideoWidth, iVideoHeight。
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoMacroWidth = iVideoWidth / 2;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	// 設定虛擬物件的各項屬性。
	voGecko.iX1 = 450;
	voGecko.iY1 = 100;
	voGecko.iWidth = 100;
	voGecko.iHeight = 100;
	voGecko.iX2 = voGecko.iX1 + voGecko.iWidth - 1;
	voGecko.iY2 = voGecko.iY1 + voGecko.iHeight - 1;
	voGecko.ucBackgroundRed = 255;
	voGecko.ucBackgroundGreen = 255;
	voGecko.ucBackgroundBlue = 255;
	// 將影像元件中的色彩資料放到陣列中
	for (j = 0; j < voGecko.iHeight; j ++)
	{
		ptrImage = (BYTE*) imGecko->Picture->Bitmap->ScanLine[j];
		for ( i = 0; i<voGecko.iWidth; i++ )
		{
			iB = ptrImage[i*3];
			iG = ptrImage[i*3+1];
			iR = ptrImage[i*3+2];
			ucGeckoRGB[j][i][0] = ( unsigned char ) iB;
			ucGeckoRGB[j][i][1] = ( unsigned char ) iG;
			ucGeckoRGB[j][i][2] = ( unsigned char ) iR;
			ucGeckoYUV[ j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
			ucGeckoYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
			ucGeckoYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
	for (j = 0; j < pause.iHeight; j++)
	{
		ptrImage1 = (BYTE*) pauseImage->Picture->Bitmap->ScanLine[j];
		for ( i = 0; i<pause.iWidth; i++ )
		{
			iB = ptrImage1[i*3];
			iG = ptrImage1[i*3+1];
			iR = ptrImage1[i*3+2];
			pauseRGB[j][i][0] = ( unsigned char ) iB;
			pauseRGB[j][i][1] = ( unsigned char ) iG;
			pauseRGB[j][i][2] = ( unsigned char ) iR;
			pauseYUV[ j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
			pauseYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
			pauseYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
	for (j = 0; j < dice.iHeight; j++)
	{
		ptrImage2 = (BYTE*) diceImage->Picture->Bitmap->ScanLine[j];
		for ( i = 0; i<dice.iWidth; i++ )
		{
			iB = ptrImage2[i*3];
			iG = ptrImage2[i*3+1];
			iR = ptrImage2[i*3+2];
			diceRGB[j][i][0] = ( unsigned char ) iB;
			diceRGB[j][i][1] = ( unsigned char ) iG;
			diceRGB[j][i][2] = ( unsigned char ) iR;
			diceYUV[ j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
			diceYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
			diceYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}

	// 開啟預覽回呼函式 FrameCallbackGecko。
	capSetCallbackOnFrame (hwndVideo , &FrameCallbackGecko);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK FrameCallbackGecko(HWND hwndVideo , PVIDEOHDR lpvhdr)
{
	// 取得視訊擷取視窗的緩衝區記憶體的起始位址。
	ptrCapture = lpvhdr->lpData;

	k = 0;
	for (j=0; j < iVideoHeight; j++)
	{
		for (i=0; i < iVideoWidth; i=i+2)
		{
			// 在座標 ( iObjectX1, iObjectY1 ) 與 ( iObjectX2, iObjectY2 ) 之間的區域要置入Gecko。
			if ( ( i >= voGecko.iX1 ) && ( i < voGecko.iX2 ) && ( j >= voGecko.iY1 ) && ( j < voGecko.iY2 ) )
			{
				// Gecko 圖片的背景區域必須忽略, 無須置入。
				if ( ( ucGeckoRGB[j-voGecko.iY1][i-voGecko.iX1][0] != voGecko.ucBackgroundBlue )
				 || ( ucGeckoRGB[j-voGecko.iY1][i-voGecko.iX1][1] != voGecko.ucBackgroundGreen )
				 || ( ucGeckoRGB[j-voGecko.iY1][i-voGecko.iX1][2] != voGecko.ucBackgroundRed ) )
				 {
					ptrCapture[k] = ucGeckoYUV[j-voGecko.iY1][i-voGecko.iX1][0];
					ptrCapture[k+1] = ucGeckoYUV[j-voGecko.iY1][i-voGecko.iX1][1];
					ptrCapture[k+2] = ucGeckoYUV[j-voGecko.iY1][i-voGecko.iX1+1][0];
					ptrCapture[k+3] = ucGeckoYUV[j-voGecko.iY1][i-voGecko.iX1][2];
				 }
			}
			k = k + 4;
		}
	}
	// 結束預覽回呼函數之執行。
	return (0);
}

// 預覽回呼函式 FrameCallbackSpecificColorFiltering
LRESULT CALLBACK FrameCallbackSpecificColorFiltering ( HWND hwndVideo, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  ptrCapture = lpvhdr->lpData;

  k = 0;
  for ( j = 0; j < iVideoHeight; j++ )
	{
	for ( i = 0; i < iVideoMacroWidth; i++ )
      {
	  if (( ptrCapture[k] > iMinY ) && ( ptrCapture[k] < iMaxY )
      && ( ptrCapture[k+1] > iMinU ) && ( ptrCapture[k + 1] < iMaxU )
      && ( ptrCapture[k+2] > iMinY ) && ( ptrCapture[k + 2] < iMaxY )
      && ( ptrCapture[k+3] > iMinV ) && ( ptrCapture[k + 3] < iMaxV ))
        {
		ptrCapture[k + 1] = iU;
        ptrCapture[k + 3] = iV;
         }
      k = k + 4;
      }
    }

  Form1->lblFrameCounter->Caption = AnsiString (iFrameCounter);
  return (0);
}

void __fastcall TForm1::miSpecificColorClick(TObject *Sender)
{
    iFrameCounter = 0;
  // 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight
  // 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
  iVideoFormatSize = capGetVideoFormatSize (hwndVideo);
  capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
  iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
  iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
  iVideoMacroWidth = iVideoWidth / 2;
  // 因為要將追蹤的顏色套上綠色的光影, 所以需要綠色的 U, V 値
  iR = 255;
  iG = 0;    // 綠色。
  iB = 0;
  iY = ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 )  + 16;
  iU = ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 )  + 128;
  iV = ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 )  + 128;
  // 開啟預覽回呼函式 ( 設定為 FrameCallbackSpecificColorFiltering ) 。
  capSetCallbackOnFrame (hwndVideo, &FrameCallbackSpecificColorFiltering);

}
//---------------------------------------------------------------------------



void __fastcall TForm1::edMinYChange(TObject *Sender)
{
	// 將Edit edMinY元件所輸入的文字轉成整數型態後, 存在整數型態變數iMinY之中。
	// 第二個整數參數0是當輸入的文字不是整數數字時, 函式的內定傳回值。
  	iMinY = StrToIntDef(edMinY->Text, 0);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMaxYChange(TObject *Sender)
{
	// 將Edit edMaxY元件所輸入的文字轉成整數型態後, 存在整數型態變數iMaxY之中。
	// 第二個整數參數255是當輸入的文字不是整數數字時, 函式的內定傳回值。
	iMaxY = StrToIntDef(edMaxY->Text, 255);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMaxUChange(TObject *Sender)
{
	// 將Edit edMaxU元件所輸入的文字轉成整數型態後, 存在整數型態變數iMaxU之中。
	// 第二個整數參數120是當輸入的文字不是整數數字時, 函式的內定傳回值。
  	iMaxU = StrToIntDef(edMaxU->Text, 120);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMinUChange(TObject *Sender)
{
	// 將Edit edMinU元件所輸入的文字轉成整數型態後, 存在整數型態變數iMinU之中。
	// 第二個整數參數90是當輸入的文字不是整數數字時, 函式的內定傳回值。
	iMinU = StrToIntDef(edMinU->Text, 90);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMinVChange(TObject *Sender)
{
	// 將Edit edMinV元件所輸入的文字轉成整數型態後, 存在整數型態變數iMinV之中。
	// 第二個整數參數90是當輸入的文字不是整數數字時, 函式的內定傳回值。
	iMinV = StrToIntDef(edMinV->Text, 90);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMaxVChange(TObject *Sender)
{
    // 將Edit edMaxV元件所輸入的文字轉成整數型態後, 存在整數型態變數iMaxV之中。
	// 第二個整數參數120是當輸入的文字不是整數數字時, 函式的內定傳回值。
	iMaxV = StrToIntDef(edMaxV->Text, 120);

}
//---------------------------------------------------------------------------

int iTemp[480][640][3] ;
unsigned char ucBackground[480][640][3] ;

int iTempYUV[480][640][3] ;
unsigned char ucBackgroundYUV[480][640][3] ;


void __fastcall TForm1::miBackgroundClick(TObject *Sender)
{
	iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight,
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	// 針對用來存放背景影像及累加影像的陣列設定初始値。
	for ( j = 0; j < iVideoHeight; j++ )
		for ( i = 0; i < iVideoWidth; i++ )
		{
		  // RGB 色彩模式背景影像。
		  ucBackground[j][i][0] = 0;
		  ucBackground[j][i][1] = 0;
		  ucBackground[j][i][2] = 0;
		  iTemp[j][i][0] = 0;
		  iTemp[j][i][1] = 0;
		  iTemp[j][i][2] = 0;
		  // YUV 色彩模式背景影像。
		  ucBackgroundYUV[j][i][0] = 0;
		  ucBackgroundYUV[j][i][1] = 0;
		  ucBackgroundYUV[j][i][2] = 0;
		  iTempYUV[j][i][0] = 0;
		  iTempYUV[j][i][1] = 0;
		  iTempYUV[j][i][2] = 0;
		}
	// 針對用來顯示背景影像的影像元件設定各項屬性 ( Property ) 値。
	imBackground->Width = iVideoWidth;
	imBackground->Height = iVideoHeight;
	imBackground->Picture->Bitmap->Width = iVideoWidth;
	imBackground->Picture->Bitmap->Height = iVideoHeight;
	imBackground->Picture->Bitmap->PixelFormat = pf24bit;
	// 開啟預覽回呼函式 ( 設定為 FrameCallbackBackgroundYUY2 ) 。
	capSetCallbackOnFrame ( hwndVideo, &FrameCallbackBackgroundYUY2 );
}
//---------------------------------------------------------------------------

LRESULT CALLBACK FrameCallbackBackgroundRGB ( HWND hwndVideo, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  ptrCapture = lpvhdr->lpData;

  if ( iFrameCounter > 30 )
  {
    // 完成累加 30 張影像, 計算平均影像。
	for ( j = 0; j < iVideoHeight; j++ )
	{
      // 取得 imBackground 影像元件第 j 個 row 的儲存色彩値的記憶體起始位址。
      ptrImage = ( BYTE * ) Form1->imBackground->Picture->Bitmap->ScanLine[j];
	  t = 0;
      for ( i=0; i<iVideoWidth; i++ )
	  {
        // 將累積陣列 iTemp 中的色彩値除以 30, 即為平均值。
        ucBackground[j][i][0] = ( unsigned char ) ( iTemp[j][i][0] / 30 );
        ucBackground[j][i][1] = ( unsigned char ) ( iTemp[j][i][1] / 30 );
        ucBackground[j][i][2] = ( unsigned char ) ( iTemp[j][i][2] / 30 );
        // 將陣列 ucBackground 的色彩資料,  放進影像元件 imBackgroung 的記憶體中, 準備顯示。
        ptrImage[t+2] = ucBackground[j][i][0];
        ptrImage[t+1] = ucBackground[j][i][1];
        ptrImage[t]  = ucBackground[j][i][2];
		t = t + 3;
	  }
	}
    // 更新影像元件 imBackgroung 的顯示內容, 使其與記憶體中內容一致。
    Form1->imBackground->Refresh();
    // 背景影像建立完成, 關閉預覽回呼函式。
    capSetCallbackOnFrame ( hwndVideo, NULL );
  }
  else   // ( iFrameCounter <= 30 )
  {
    k = 0;
    for ( j=iVideoHeight-1; j>=0; j-- )
      for ( i=0; i<iVideoWidth; i++ )
	  {
        // 將視訊擷取視窗的畫面累加到 iTemp 陣列之中。
        iTemp[j][i][0] = iTemp[j][i][0] + ptrCapture[k+2];
        iTemp[j][i][1] = iTemp[j][i][1] + ptrCapture[k+1];
        iTemp[j][i][2] = iTemp[j][i][2] + ptrCapture[k];
        k = k + 3;
	  }
  }

  return (0);
}

LRESULT CALLBACK FrameCallbackBackgroundYUY2(HWND hwndVideo, PVIDEOHDR lpvhdr)
{
  iFrameCounter++;
  ptrCapture = lpvhdr->lpData;

  if ( iFrameCounter > 30 )
  {
    // 完成累加 30 張影像, 計算平均影像。
    for ( j=0; j < iVideoHeight; j++ )
	{
      // 取得 imBackground 影像元件第 j 個 row 的儲存色彩値的記憶體起始位址。
      ptrImage = ( BYTE * ) Form1->imBackground->Picture->Bitmap->ScanLine[j];
	  t = 0;
      for ( i=0; i < iVideoWidth; i++ )
	  {
        ucBackgroundYUV[j][i][0] = ( unsigned char ) ( iTempYUV[j][i][0] / 30 );
        ucBackgroundYUV[j][i][1] = ( unsigned char ) ( iTempYUV[j][i][1] / 30 );
        ucBackgroundYUV[j][i][2] = ( unsigned char ) ( iTempYUV[j][i][2] / 30 );
        // 將 YUV 色彩轉換成 RGB 色彩後, 存到 imBackgroung 中顯示出來。
        iY = ucBackgroundYUV[j][i][0];
        iU = ucBackgroundYUV[j][i][1];
        iV = ucBackgroundYUV[j][i][2];
		iC1 = iY - 16;
        iD = iU - 128;
        iE = iV - 128;
        // 處理紅色之色彩轉換。
        iR = ( 298 * iC1    + 409 * iE + 128 ) >> 8;
        // 檢查 iR 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
        if ( iR > 255 )
          iR = 255;
        else if ( iR < 0 )
          iR = 0;
        // 處理綠色之色彩轉換
        iG = ( 298 * iC1 - 100 * iD - 208 * iE + 128 ) >> 8;
        // 檢查 iG 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
        if ( iG > 255 )
          iG = 255;
        else if ( iG < 0 )
          iG = 0;
		iB = ( 298 * iC1 + 516 * iD + 128 ) >> 8;
        // 檢查 iB 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
        if ( iB > 255 )
          iB = 255;
        else if ( iB < 0 )
          iB = 0;
		// 存到 imBackgroung 中顯示出來。
        ptrImage[t + 2] = ( unsigned char ) iR;
        ptrImage[t + 1] = ( unsigned char ) iG;
        ptrImage[t ] = ( unsigned char ) iB;
        t = t + 3;
	  }
	}
	Form1->imBackground->Refresh();
	capSetCallbackOnFrame ( hwndVideo, NULL );
  }
  else // ( iFrameCount < 30 )
  {
    k = 0;
    for ( j=0; j < iVideoHeight; j++ )
      for ( i=0; i < iVideoWidth; i=i+2 )
	  {
        iY1 = ptrCapture[k];
        iU = ptrCapture[k+1];
        iY2 = ptrCapture[k+2];
        iV = ptrCapture[k+3];
        // 累加宏像素中的第一個像素 ( Pixel 1 )
        iTempYUV[j][i][0] = iTempYUV[j][i][0] + iY1;
        iTempYUV[j][i][1] = iTempYUV[j][i][1] + iU;
		iTempYUV[j][i][2] = iTempYUV[j][i][2] + iV;
        // 累加宏像素中的第二個像素 ( Pixel 2 )
        iTempYUV[j][i+1][0] = iTempYUV[j][i+1][0] + iY2;
        iTempYUV[j][i+1][1] = iTempYUV[j][i+1][1] + iU;
        iTempYUV[j][i+1][2] = iTempYUV[j][i+1][2] + iV;
        k = k + 4;
      }
  }
  Form1->lblFrameCounter->Caption = AnsiString ( iFrameCounter );
  return 0;
}


void __fastcall TForm1::edThresholdChange(TObject *Sender)
{
	iThreshold = StrToIntDef ( edThreshold->Text, 1600 );
}
//---------------------------------------------------------------------------

LRESULT CALLBACK FrameCallbackDifferenceColorYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  // 設定 ptrCapture 指向視訊擷取視窗的緩衝區記憶體的起始位址。
  ptrCapture = lpvhdr->lpData;
  // 開始進行色彩差異之計算。
  k = 0;
  for ( j = 0; j < iVideoHeight; j++ )
  {
    for ( i = 0; i < iVideoWidth; i = i + 2 )
	{
      // 每次處理 1 個 macropixel, 共 2 個 pixels, 4 bytes 。
      iY1 = ptrCapture[k];
      iU = ptrCapture[k+1];
      iY2 = ptrCapture[k+2];
      iV = ptrCapture[k+3];
	  // 計算差異値。
	  iY1Difference = abs ( ptrCapture[k] - ucBackgroundYUV[j][i][0] );
	  iUDifference = ucBackgroundYUV[j][i][1] - ptrCapture[k + 1];
	  iY2Difference = abs ( ptrCapture[k+2] - ucBackgroundYUV[j][i+1][0] );
	  iVDifference = ucBackgroundYUV[j][i+1][2] - ptrCapture[k+3];
	  // 將差異値以彩色模式顯示在視訊擷取視窗。
      ptrCapture[k ] = iY1Difference + 16;
      ptrCapture[k+1] = iUDifference + 128;
      ptrCapture[k+2] = iY2Difference + 16;
      ptrCapture[k+3] = iVDifference + 128;
      // 更新 k 値, 準備處理下一個宏像素( macropixel )
      k = k + 4;
	}
  }
  Form1->lblFrameCounter->Caption = AnsiString(iFrameCounter);
  return 0;
}


void __fastcall TForm1::miColorModeClick(TObject *Sender)
{
   iFrameCounter = 0;
  // 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight,
  // 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
  iVideoFormatSize = capGetVideoFormatSize ( hwndVideo );
  capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
  iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
  iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
  // 開啟預覽回呼函式 ( 設定為 FrameCallbackDifferenceColorYUY2 ) 。
  capSetCallbackOnFrame(hwndVideo, &FrameCallbackDifferenceColorYUY2);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miBinaryModeClick(TObject *Sender)
{
	iFrameCounter = 0;
  // 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight，
  // 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
  iVideoFormatSize = capGetVideoFormatSize(hwndVideo );
  capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
  iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
  iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
  // 開啟預覽回呼函式 ( 設定為 FrameCallbackDifferenceBinaryYUY2 )
  capSetCallbackOnFrame(hwndVideo, &FrameCallbackDifferenceBinaryYUY2);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK FrameCallbackDifferenceBinaryYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  // 設定 ptrCapture 指向是訊擷取視窗的緩衝區記憶體的起始位址。
  ptrCapture = lpvhdr->lpData;

  k = 0;
  for (j=0; j < iVideoHeight; j++ )
  {
    for (i=0; i < iVideoWidth; i=i+2)
	{
      // 每次處理 1 個 macropixel, 共 2 個 pixels, 4 bytes。
      iY1 = ptrCapture[k];
      iU = ptrCapture[k+1];
      iY2 = ptrCapture[k+2];
      iV = ptrCapture[k+3];
      // 計算差異値 。
      iY1Difference = ptrCapture[k] - ucBackgroundYUV[j][i][0];
      iUDifference = ptrCapture[k+1] - ucBackgroundYUV[j][i][1];
      iY2Difference =ptrCapture[k+2] - ucBackgroundYUV[j][i+1][0];
      iVDifference = ptrCapture[k+3] - ucBackgroundYUV[j][i+1][2];
      // 分別計算 2 個pixel 差異値的平方和。
	  iSquareDifferenceSum1 = iY1Difference * iY1Difference + iUDifference * iUDifference + iVDifference * iVDifference;
      iSquareDifferenceSum2 = iY2Difference * iY2Difference + iUDifference * iUDifference + iVDifference * iVDifference;
      // 不管是黑或白, U, V 都是 128。
      ptrCapture[k+1] = 128;
      ptrCapture[k+3] = 128;
      // 設定 pixel 1 的 Y, 即 Y1
      if ( iSquareDifferenceSum1 > iThreshold )ptrCapture[k] = 255;
	  else ptrCapture[k] = 16;
      // 設定 pixel 2 的 Y, 即 Y2
	  if ( iSquareDifferenceSum2 > iThreshold )ptrCapture[k+2] = 255;
	  else ptrCapture[k+2] = 16;
       // 更新 k 値, 準備處理下一個 macropixel。
	  k = k + 4;
	}
  }
  Form1->lblFrameCounter->Caption = AnsiString ( iFrameCounter );
  return 0;
}

void __fastcall TForm1::miBlockModeClick(TObject *Sender)
{
  iFrameCounter = 0;
  // 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight，
  // 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
  iVideoFormatSize = capGetVideoFormatSize ( hwndVideo );
  capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
  iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
  iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
  // 開啟預覽回呼函式 ( 設定為 FrameCallbackDifferenceBlockBinaryYUY2 )
  capSetCallbackOnFrame ( hwndVideo, &FrameCallbackDifferenceBlockYUY2 );
}
//---------------------------------------------------------------------------
LRESULT CALLBACK FrameCallbackDifferenceBlockYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
	iFrameCounter++;
	ptrCapture = lpvhdr->lpData;
	// 重設區塊累加陣列。
	for (m=0; m<60; m++ )
		for (n=0; n<80; n++ )
			iBlockChangeCounter[m][n] = 0;

	k = 0;
	for ( j = 0; j < iVideoHeight; j++ )
	{
		for ( i = 0; i < iVideoWidth; i = i + 2 )
		{
			// 計算目前的 j, i 這個像素是屬於哪一個 block 的累加範圍。
			m = j / 8;
			n = i / 8;
			// 每次處理 1 個 macropixel, 共 2 個 pixels, 4 bytes。
			iY1 = ptrCapture[k];
			iU = ptrCapture[k + 1];
			iY2 = ptrCapture[k + 2];
			iV = ptrCapture[k + 3];
			// 計算差異値
			iY1Difference = abs ( ptrCapture[k] - ucBackgroundYUV[j][i][0] );
			iUDifference = abs ( ptrCapture[k + 1] - ucBackgroundYUV[j][i][1] );
			iY2Difference = abs ( ptrCapture[k + 2] - ucBackgroundYUV[j][i + 1][0] );
			iUDifference = abs ( ptrCapture[k + 3] - ucBackgroundYUV[j][i + 1][2] );
			// 分別計算 2 個pixel 差異値的平方和
			iSquareDifferenceSum1 = iY1Difference * iY1Difference + iUDifference * iUDifference + iVDifference * iVDifference;
			iSquareDifferenceSum2 = iY2Difference * iY2Difference + iUDifference * iUDifference + iVDifference * iVDifference;
			// 累加每個區塊中的差異像素
			if ( iSquareDifferenceSum1 > iThreshold )
				iBlockChangeCounter[m][n]++;
			if ( iSquareDifferenceSum2 > iThreshold )
				iBlockChangeCounter[m][n]++;
			// 更新 k 値, 準備處理下一個 macropixel
			k = k + 4;
		}
	}
	  // 顯示黑白區塊。
	  k = 0;
	  for ( j=0; j<iVideoHeight; j++ )
	  {
		for ( i=0; i<iVideoWidth; i=i+2 )
		{
		  // 計算目前的 j, i 這個像素是屬於哪一個 block 的範圍。
		  m = j / 8;
		  n = i / 8;
		  // 不管是黑或白, U, V 都是 128
		  ptrCapture[k+1] = 128;
		  ptrCapture[k+3] = 128;
		  // Block 中白點的個數超過一半就整個區塊塗成白色。
		  if ( iBlockChangeCounter[m][n] > 31 )
		  {
			ptrCapture[k] = 255;
			ptrCapture[k+2] = 255;
		  }
		  else
		  {
			ptrCapture[k] = 16;
			ptrCapture[k+2] = 16;
		  }
		  k = k + 4;
		}
	  }

	Form1->lblFrameCounter->Caption = AnsiString ( iFrameCounter );
	return 0;
}

void __fastcall TForm1::miPlayClick(TObject *Sender)
{
	iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight,
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize( hwndVideo );
	capGetVideoFormat( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	iVideoMacroWidth = iVideoWidth / 2;
	iVideoBlockWidth = iVideoWidth / 8;
	iVideoBlockHeight = iVideoHeight / 8;

  // 重設亂數產生器
  int iTime = clock();
  srand( iTime );

 // 設定虛擬play的相關資料
  voGecko.iX1 = 260;
  voGecko.iY1 = 30;
  voGecko.iWidth = 100;
  voGecko.iHeight = 100;
  voGecko.iX2 = voGecko.iX1 + voGecko.iWidth - 1;
  voGecko.iY2 = voGecko.iY1 + voGecko.iHeight - 1;
  voGecko.iBlockX1 = voGecko.iX1 / 8;
  voGecko.iBlockY1 = voGecko.iY1 / 8;
  voGecko.iBlockX2 = voGecko.iX2 / 8;
  voGecko.iBlockY2 = voGecko.iY2 / 8;
  voGecko.bVisible = true;
  voGecko.ucBackgroundRed = 255;
  voGecko.ucBackgroundGreen = 255;
  voGecko.ucBackgroundBlue = 255;

  // 設定虛擬puase的相關資料
  pause.iX1 = 260;
  pause.iY1 = 180;
  pause.iWidth = 100;
  pause.iHeight = 100;
  pause.iX2 = pause.iX1 + pause.iWidth - 1;
  pause.iY2 = pause.iY1 + pause.iHeight - 1;
  pause.iBlockX1 = pause.iX1 / 8;
  pause.iBlockY1 = pause.iY1 / 8;
  pause.iBlockX2 = pause.iX2 / 8;
  pause.iBlockY2 = pause.iY2 / 8;
  pause.bVisible = true;
  pause.ucBackgroundRed = 255;
  pause.ucBackgroundGreen = 255;
  pause.ucBackgroundBlue = 255;

  // 設定虛擬dice的相關資料
  dice.iX1 = 260;
  dice.iY1 = 330;
  dice.iWidth = 100;
  dice.iHeight = 100;
  dice.iX2 = dice.iX1 + dice.iWidth - 1;
  dice.iY2 = dice.iY1 + dice.iHeight - 1;
  dice.iBlockX1 = dice.iX1 / 8;
  dice.iBlockY1 = dice.iY1 / 8;
  dice.iBlockX2 = dice.iX2 / 8;
  dice.iBlockY2 = dice.iY2 / 8;
  dice.bVisible = true;
  dice.ucBackgroundRed = 255;
  dice.ucBackgroundGreen = 255;
  dice.ucBackgroundBlue = 255;

  // 將影像元件中的色彩資料放到陣列中
  for ( j = 0; j < voGecko.iHeight; j++ )
  {
	ptrImage = ( BYTE * )imGecko->Picture->Bitmap->ScanLine[j];
    for ( i=0; i<voGecko.iWidth; i++ )
	{
      iB = ptrImage[i*3];
      iG = ptrImage[i*3+1];
	  iR = ptrImage[i*3+2];
      ucGeckoRGB[j][i][0] = ( unsigned char ) iB;
      ucGeckoRGB[j][i][1] = ( unsigned char ) iG;
      ucGeckoRGB[j][i][2] = ( unsigned char ) iR;
      ucGeckoYUV[j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 )  >> 8 ) + 16 );
      ucGeckoYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR-74 * iG + 112 * iB + 128 )  >> 8 ) + 128 );
      ucGeckoYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 )  >> 8 ) + 128 );
	}
  }
	for (j = 0; j < pause.iHeight; j++)
	{
		ptrImage1 = (BYTE*) pauseImage->Picture->Bitmap->ScanLine[j];
		for ( i = 0; i<pause.iWidth; i++ )
		{
			iB = ptrImage1[i*3];
			iG = ptrImage1[i*3+1];
			iR = ptrImage1[i*3+2];
			pauseRGB[j][i][0] = ( unsigned char ) iB;
			pauseRGB[j][i][1] = ( unsigned char ) iG;
			pauseRGB[j][i][2] = ( unsigned char ) iR;
			pauseYUV[ j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
			pauseYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
			pauseYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
	for (j = 0; j < dice.iHeight; j++)
	{
		ptrImage2 = (BYTE*) diceImage->Picture->Bitmap->ScanLine[j];
		for ( i = 0; i<dice.iWidth; i++ )
		{
			iB = ptrImage2[i*3];
			iG = ptrImage2[i*3+1];
			iR = ptrImage2[i*3+2];
			diceRGB[j][i][0] = ( unsigned char ) iB;
			diceRGB[j][i][1] = ( unsigned char ) iG;
			diceRGB[j][i][2] = ( unsigned char ) iR;
			diceYUV[ j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
			diceYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
			diceYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}

  // 開啟預覽回呼函式 FrameCallbackPlayWithGecko。

  capSetCallbackOnFrame(hwndVideo, &FrameCallbackPlayWithGecko6A);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::miReplayClick(TObject *Sender)
{
    // 注意: 要重新讓虛擬物件出現, 只要將 voGecko.bVisible 重新設回 true 即可。
  // 如果重新出現的虛擬物件要更改位置, 只要將虛擬物件的位置相關屬性再作變更即可!
  voGecko.iX1 = rand() % 485;
  voGecko.iY1 = rand() % 355;
  voGecko.iX2 = voGecko.iX1 + voGecko.iWidth - 1;
  voGecko.iY2 = voGecko.iY1 + voGecko.iHeight - 1;
  voGecko.iBlockX1 = voGecko.iX1 / 8;
  voGecko.iBlockY1 = voGecko.iY1 / 8;
  voGecko.iBlockX2 = voGecko.iX2 / 8;
  voGecko.iBlockY2 = voGecko.iY2 / 8;
  voGecko.bVisible = true;

}
void music()
{
	if(test==2)
	{

		PlaySound(TEXT("music\\without you.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 高爾宣 OSN -【Without You】沒了妳｜Official MV";
	}
	if(test==3)
	{
		PlaySound(TEXT("music\\latingWorld.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing:【FULL】unlasting：Alicization War of Underworld By LiSA";
	}
	if(test==4)
	{
		PlaySound(TEXT("music\\wave.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 浪流連Waves Wandering-茄子蛋EggPlantEgg";
	}
	if(test==5)
	{
		PlaySound(TEXT("music\\Cartoon - On _ On (feat. Daniel Levi) _NCS Release_.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Cartoon - On_On(feat. Daniel Levi)";
	}
	if(test==6)
	{
		PlaySound(TEXT("music\\Dua Lipa - Dont Start Now (Official Music Video).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Dua Lipa - Dont Start Now (Official Music Video)";
	}
	if(test==7)
	{
		PlaySound(TEXT("music\\Ed Sheeran - Photograph (Official Music Video).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Ed Sheeran - Photograph (Official Music Video)";
	}
	if(test==8)
	{
		PlaySound(TEXT("music\\Fitz and the Tantrums - HandClap [Official Video].wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Fitz and the Tantrums - HandClap [Official Video]";
	}
	if(test==9)
	{
		PlaySound(TEXT("music\\Justin Bieber - Intentions (Official Video (Short Version)) ft. Quavo.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Justin Bieber - Intentions (Official Video (Short Version)) ft. Quavo";
	}
	if(test==10)
	{
		PlaySound(TEXT("music\\Lil Dicky - Earth (Lyrics).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Lil Dicky - Earth (Lyrics)";
	}
	if(test==11)
	{
		PlaySound(TEXT("music\\Whitney Houston - I Will Always Love You.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Whitney Houston - I Will Always Love You";
	}
	if(test==12)
	{
		PlaySound(TEXT("music\\Tones And I - Dance Monkey (Lyrics).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Tones And I - Dance Monkey (Lyrics)";
	}
	if(test==13)
	{
		PlaySound(TEXT("music\\林宥嘉 Yoga Lin [ 兜圈 ] Official Music Video (偶像劇｢必娶女人｣片尾曲).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 林宥嘉 Yoga Lin [ 兜圈 ] Official Music Video";
	}
	if(test==14)
	{
		PlaySound(TEXT("music\\郁可唯 Yisa Yu [ 路過人間 Walking by the world ] Official Music Video（電視劇《我們與惡的距離》插曲）.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 郁可唯 Yisa Yu [ 路過人間 Walking by the world ] Official Music Video";
	}
	if(test==15)
	{
		PlaySound(TEXT("music\\梁文音 那女孩對我說 歌詞.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 梁文音 那女孩對我說";
	}
	if(test==16)
	{
		PlaySound(TEXT("music\\徐佳莹《真的傻》.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 徐佳瑩-真的傻";
	}
	if(test==17)
	{
		PlaySound(TEXT("music\\陳雪凝 - 你的酒館對我打了烊【動態歌詞Lyrics】.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 陳雪凝 - 你的酒館對我打了烊";
	}
	if(test==18)
	{
		PlaySound(TEXT("music\\陳零九 Nine Chen feat. 邱鋒澤 FENG ZE【天黑請閉眼 Werewolves】Official Music Video.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: 陳零九 Nine Chen feat. 邱鋒澤 FENG ZE【天黑請閉眼 Werewolves】Official Music Video";
	}
	if(test==19)
	{
		PlaySound(TEXT("music\\Taylor Swift - ME! (Lyrics) ft. Brendon Urie of Panic! At The Disco (online-audio-converter.com).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: Taylor Swift - ME! (Lyrics) ft. Brendon Urie of Panic!";
	}
	if(test==20)
	{
		PlaySound(TEXT("music\\spark.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		Form1->playLabel->Caption= "Now playing: F.I.R.《 星火 Spark 》電影「鬥魚」主題曲 Official Music Video";
		test=1;
	}

}
//---------------------------------------------------------------------------
bool check=false;
LRESULT CALLBACK FrameCallbackPlayWithGecko6A( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  ptrCapture = lpvhdr->lpData;

  // 重設儲存區塊中, 差異像素總數的累加陣列
  for ( m = 0; m < iVideoBlockHeight; m++ )
	for ( n = 0; n < iVideoBlockWidth; n++ )
	  iBlockChangeCounter[m][n] = 0;

  // 迴圈中, 同時處理實境物件偵測 ( 差異偵測 ) 與置入虛擬物件到視訊擷取視窗
  // 先進行實境物件偵測, 再進行置入虛擬物件到視訊擷取視窗
  k = 0;
  for (j=0; j<iVideoHeight; j++)
  {
	for (i=0; i<iVideoWidth; i=i+2)
	{
	  m = j / 8;
	  n = i / 8;
	  iY1 = ptrCapture[k];
	  iU = ptrCapture[k+1];
	  iY2 = ptrCapture[k+2];
	  iV = ptrCapture[k+3];
	  iY1Difference = abs( ptrCapture[k] - ucBackgroundYUV[j][i][0] );
	  iUDifference = abs( ptrCapture[k+1] - ucBackgroundYUV[j][i][1] );
	  iY2Difference = abs( ptrCapture[k+2] - ucBackgroundYUV[j][i+1][0] );
	  iVDifference = abs( ptrCapture[k+3] - ucBackgroundYUV[j][i+1][2] );

	  iSquareDifferenceSum1 = iY1Difference * iY1Difference + iUDifference * iUDifference + iVDifference * iVDifference;
	  iSquareDifferenceSum2 = iY2Difference * iY2Difference + iUDifference * iUDifference + iVDifference * iVDifference;
	  // 累加每個區塊中的差異像素
	  if ( iSquareDifferenceSum1 > iThreshold ) iBlockChangeCounter[m][n]++;
	  if ( iSquareDifferenceSum2 > iThreshold ) iBlockChangeCounter[m][n]++;

      // 實境物件偵測完畢, 接下來, 開始置入虛擬物件到視訊擷取視窗
      // 如果 bVisible 值為 true
	  // 在座標 ( iX1, iY1 ) 與 ( iX2, iY2 ) 之間的區域要置入虛擬物件

	  if (( voGecko.bVisible ) && ( i >= voGecko.iX1 )&&
	  ( i < voGecko.iX2 ) && ( j >= voGecko.iY1 ) && ( j < voGecko.iY2 ) )
	  {
		// 虛擬物件的背景區域必須忽略, 無須置入
        if (  ( ucGeckoRGB[j - voGecko.iY1][i - voGecko.iX1][0] != voGecko.ucBackgroundBlue )
			  || ( ucGeckoRGB[j - voGecko.iY1][i - voGecko.iX1][1] != voGecko.ucBackgroundGreen )
			  || ( ucGeckoRGB[j - voGecko.iY1][i - voGecko.iX1][2] != voGecko.ucBackgroundRed ) )
		{
		  ptrCapture[k] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1][0];
		  ptrCapture[k+1] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1][1];
		  ptrCapture[k+2] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1+1][0];
		  ptrCapture[k+3] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1][2];
		}
	  }

	  if (( pause.bVisible ) && ( i >= pause.iX1 )&&
	  ( i < pause.iX2 ) && ( j >= pause.iY1 ) && ( j < pause.iY2 ) )
	  {
		// 虛擬物件的背景區域必須忽略, 無須置入
		if (  ( pauseRGB[j - pause.iY1][i - pause.iX1][0] != pause.ucBackgroundBlue )
			  || ( pauseRGB[j - pause.iY1][i - pause.iX1][1] != pause.ucBackgroundGreen )
			  || ( pauseRGB[j - pause.iY1][i - pause.iX1][2] != pause.ucBackgroundRed ) )
		{
		  ptrCapture[k] = pauseYUV[j - pause.iY1][i - pause.iX1][0];
		  ptrCapture[k+1] = pauseYUV[j - pause.iY1][i - pause.iX1][1];
		  ptrCapture[k+2] = pauseYUV[j - pause.iY1][i - pause.iX1+1][0];
		  ptrCapture[k+3] = pauseYUV[j - pause.iY1][i - pause.iX1][2];
		}
	  }

	  if (( dice.bVisible ) && ( i >= dice.iX1 )&&
	  ( i < dice.iX2 ) && ( j >= dice.iY1 ) && ( j < dice.iY2 ) )
	  {
		// 虛擬物件的背景區域必須忽略, 無須置入
		if (  ( diceRGB[j - dice.iY1][i - dice.iX1][0] != dice.ucBackgroundBlue )
			  || ( diceRGB[j - dice.iY1][i - dice.iX1][1] != dice.ucBackgroundGreen )
			  || ( diceRGB[j - dice.iY1][i - dice.iX1][2] != dice.ucBackgroundRed ) )
		{
		  ptrCapture[k] = diceYUV[j - dice.iY1][i - dice.iX1][0];
		  ptrCapture[k+1] = diceYUV[j - dice.iY1][i - dice.iX1][1];
		  ptrCapture[k+2] = diceYUV[j - dice.iY1][i - dice.iX1+1][0];
		  ptrCapture[k+3] = diceYUV[j - dice.iY1][i - dice.iX1][2];
		}
	  }



	  k = k + 4;
	}
  }

  // 實境物件的位置為 iBlockChangeCounter[m][n] > 31 的區塊,
  // 虛擬物件的位置為( iObjectBlockX1, iObjectBlockY1 ) 與 ( iObjectBlockX2, iObjectBlockY2 ) 之間,
  // 檢查實境物件與虛擬物件的位置是否重疊 ?
  bool touch = false;

  k = 0;
  for ( m = 0; m < iVideoBlockHeight; m++ )
  {
	for ( n = 0; n < iVideoBlockWidth; n++ )
	{
		if (!( ( voGecko.bVisible ) && ( iBlockChangeCounter[m][n]>31 ) &&
		 ( m > voGecko.iBlockY1 ) && ( n > voGecko.iBlockX1 ) &&
		 ( m < voGecko.iBlockY2 ) && ( n < voGecko.iBlockX2 ) ))
		 {
			 touch = true;
         }
		// 第 1 個判斷式 ( iBlockChangeCounter[m][n]>31 ) 判斷實境物件的位置,
		// 其餘 4 個判斷式是判斷虛擬物件的位置,
		// 同時成立表示兩個物件在這個 [m, n] 區塊有交集,
		if ( ( voGecko.bVisible ) && ( iBlockChangeCounter[m][n]>31 ) &&
		 ( m > voGecko.iBlockY1 ) && ( n > voGecko.iBlockX1 ) &&
		 ( m < voGecko.iBlockY2 ) && ( n < voGecko.iBlockX2 ) )
		{
			// 啟動互動處理程序。
			Form1->playLabel->Visible = true;
			if(check==false && touch == true){
				test++;
				music();
				touch = false;
			}
			else
				check=false;


			// 已經確定重疊了, 不需檢查其他區塊了! 故可以跳離 for 迴圈了。
			m = iVideoBlockHeight;
			n = iVideoBlockWidth;

		}

		if ( ( pause.bVisible ) && ( iBlockChangeCounter[m][n]>31 ) &&
		 ( m > pause.iBlockY1 ) && ( n > pause.iBlockX1 ) &&
		 ( m < pause.iBlockY2 ) && ( n < pause.iBlockX2 ) )
		{
			if(check==false)
			{
				PlaySound(NULL, NULL, 0);
				check = true;
			}

			m = iVideoBlockHeight;
			n = iVideoBlockWidth;
		}
		if ( ( dice.bVisible ) && ( iBlockChangeCounter[m][n]>31 ) &&
		 ( m > dice.iBlockY1 ) && ( n > dice.iBlockX1 ) &&
		 ( m < dice.iBlockY2 ) && ( n < dice.iBlockX2 ) )
		{
			test = rand()%20+1;
            music();
			m = iVideoBlockHeight;
			n = iVideoBlockWidth;

		}

	}
  }
  // 離開預覽回呼函式
  return 0;
}


LRESULT CALLBACK FrameCallbackPlayWithGecko6B( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  ptrCapture = lpvhdr->lpData;


  // 重設區塊累加陣列
  for ( m = 0; m < iVideoBlockHeight; m++ )
    for ( n = 0; n < iVideoBlockWidth; n++ )
		iBlockChangeCounter[m][n] = 0;

  // 迴圈中, 同時處理實境物件偵測 ( 差異偵測 ) 與置入虛擬物件到視訊擷取視窗
  // 先進行實境物件偵測, 再進行置入虛擬物件到視訊擷取視窗
  k = 0;
  for ( j=0; j < iVideoHeight; j++ )
  {
    for ( i=0; i < iVideoWidth; i=i+2 )
	{
      m = j / 8;
      n = i / 8;
	  iY1 = ptrCapture[k];
      iU = ptrCapture[k+1];
      iY2 = ptrCapture[k+2];
      iV = ptrCapture[k+3];
      iY1Difference = abs( ptrCapture[k] - ucBackgroundYUV[j][i][0] );
      iUDifference = abs( ptrCapture[k+1] - ucBackgroundYUV[j][i][1] );
      iY2Difference = abs( ptrCapture[k+2] - ucBackgroundYUV[j][i+1][0] );
      iVDifference = abs( ptrCapture[k+3] - ucBackgroundYUV[j][i+1][2] );

      iSquareDifferenceSum1 = iY1Difference * iY1Difference + iUDifference * iUDifference + iVDifference * iVDifference;
      iSquareDifferenceSum2 = iY2Difference * iY2Difference + iUDifference * iUDifference + iVDifference * iVDifference;
      // 累加每個區塊中的差異像素
	  if ( iSquareDifferenceSum1 > iThreshold )iBlockChangeCounter[m][n]++;
	  if ( iSquareDifferenceSum2 > iThreshold )iBlockChangeCounter[m][n]++;

      k = k + 4;
	}
  }

// 實境物件偵測完畢, 接下來, 開始置入虛擬物件到視訊擷取視窗
// 當使用者勾選 8 * 8 黑白區塊顯示時, 則將視訊擷取視窗內容依照差異偵測結果顯示
// 白色 YUV 色彩值的 YUV = ( 255, 128, 128 )
// 黑色 YUV 色彩值的 YUV = ( 16, 128, 128 )

  k = 0;
  for ( j = 0; j < iVideoHeight; j++ )
  {
    for ( i=0; i < iVideoWidth; i = i + 2 )
	{
		m = j / 8;
		n = i / 8;
		if ( Form1->cbBlockDisplay->Checked )
		{
			ptrCapture[k+1] = 128;
			ptrCapture[k+3] = 128;
			if ( iBlockChangeCounter[m][n] > 31 )
			{
			  ptrCapture[k] = 255;
			  ptrCapture[k+2] = 255;
			}
			else
			{
			  ptrCapture[k] = 16;
			  ptrCapture[k+2] = 16;
			}
		}

		// 如果 bVisible 值為 true,
		// 在座標 ( iX1, iY1 ) 與 ( iX2, iY2 ) 之間的區域要置入虛擬物件。
		if ( ( voGecko.bVisible ) && ( i >= voGecko.iX1 )
		&& ( i < voGecko.iX2 ) && ( j >= voGecko.iY1 ) && ( j < voGecko.iY2 ) )
		{
			// 虛擬物件的背景區域必須忽略, 無須置入。
			if (  ( ucGeckoRGB[j - voGecko.iY1][i - voGecko.iX1][0] != voGecko.ucBackgroundBlue )
			  || ( ucGeckoRGB[j - voGecko.iY1][i - voGecko.iX1][1] != voGecko.ucBackgroundGreen )
			  || ( ucGeckoRGB[j - voGecko.iY1][i - voGecko.iX1][2] != voGecko.ucBackgroundRed ) )
			  {
				  ptrCapture[k ] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1][0];
				  ptrCapture[k+1] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1][1];
				  ptrCapture[k+2] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1+1][0];
				  ptrCapture[k+3] = ucGeckoYUV[j - voGecko.iY1][i - voGecko.iX1][2];
			  }
		}
	  k = k + 4;
	}
  }

  // 實境物件的位置為 iBlockChangeCounter[m][n] > 31 的區塊,
  // 虛擬物件的位置為( iObjectBlockX1, iObjectBlockY1 ) 與 ( iObjectBlockX2, iObjectBlockY2 ) 之間,
  // 檢查實境物件與虛擬物件的位置是否重疊 ?

  k = 0;
  for ( m=0; m < iVideoBlockHeight; m++ )
	{
    for ( n=0; n < iVideoBlockWidth; n++ )
	  {
      // 第 1 個判斷式 ( iBlockChangeCounter[m][n]>31 ) 判斷實境物件的位置,
      // 其餘 4 個判斷式是判斷虛擬物件的位置,
      // 同時成立表示兩個物件在這個 [m, n] 區塊有交集 !
      if ( ( voGecko.bVisible ) && ( iBlockChangeCounter[m][n]>31 ) &&
         ( m > voGecko.iBlockY1 ) && ( n > voGecko.iBlockX1 ) &&
         ( m < voGecko.iBlockY2 ) && ( n < voGecko.iBlockX2 ) )
		 {
			// 啟動互動處理程序。
			voGecko.bVisible = false;

			// 已經確定重疊了, 不需檢查其他區塊了! 故可以跳離 for 迴圈了
			m = iVideoBlockHeight;
			n = iVideoBlockWidth;
        }
	  }
	}
  // 離開預覽回呼函式
  return 0;
}

void timer()
{
	if(Form1->Timer1->Interval%10==0)
		Form1->playLabel->Font->Color = clRed;
	else if(Form1->Timer1->Interval%3==1)
		Form1->playLabel->Font->Color = clOlive;
	else if(Form1->Timer1->Interval%3==2)
		Form1->playLabel->Font->Color = clGreen;
	else if(Form1->Timer1->Interval%3==3)
		Form1->playLabel->Font->Color = clBlue;
	else if(Form1->Timer1->Interval%3==4)
		Form1->playLabel->Font->Color = clNavy;
	else if(Form1->Timer1->Interval%3==5)
		Form1->playLabel->Font->Color = clPurple;
	else if(Form1->Timer1->Interval%3==6)
		Form1->playLabel->Font->Color = clTeal;
	else if(Form1->Timer1->Interval%3==7)
		Form1->playLabel->Font->Color = clMaroon;
}
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	Form1->playLabel->Visible = true;
	test++;
	if(test==2)
	{

		PlaySound(TEXT("music\\without you.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 高爾宣 OSN -【Without You】沒了妳｜Official MV";
	}
	if(test==3)
	{
		PlaySound(TEXT("music\\latingWorld.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing:【FULL】unlasting：Alicization War of Underworld By LiSA";
	}
	if(test==4)
	{
		PlaySound(TEXT("music\\wave.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 浪流連Waves Wandering-茄子蛋EggPlantEgg";
	}
	if(test==5)
	{
		PlaySound(TEXT("music\\Cartoon - On _ On (feat. Daniel Levi) _NCS Release_.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Cartoon - On_On(feat. Daniel Levi)";
	}
	if(test==6)
	{
		PlaySound(TEXT("music\\Dua Lipa - Dont Start Now (Official Music Video).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Dua Lipa - Dont Start Now (Official Music Video)";
	}
	if(test==7)
	{
		PlaySound(TEXT("music\\Ed Sheeran - Photograph (Official Music Video).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Ed Sheeran - Photograph (Official Music Video)";
	}
	if(test==8)
	{
		PlaySound(TEXT("music\\Fitz and the Tantrums - HandClap [Official Video].wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Fitz and the Tantrums - HandClap [Official Video]";
	}
	if(test==9)
	{
		PlaySound(TEXT("music\\Justin Bieber - Intentions (Official Video (Short Version)) ft. Quavo.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Justin Bieber - Intentions (Official Video (Short Version)) ft. Quavo";
	}
	if(test==10)
	{
		PlaySound(TEXT("music\\Lil Dicky - Earth (Lyrics).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Lil Dicky - Earth (Lyrics)";
	}
	if(test==11)
	{
		PlaySound(TEXT("music\\Whitney Houston - I Will Always Love You.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Whitney Houston - I Will Always Love You";
	}
	if(test==12)
	{
		PlaySound(TEXT("music\\Tones And I - Dance Monkey (Lyrics).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Tones And I - Dance Monkey (Lyrics)";
	}
	if(test==13)
	{
		PlaySound(TEXT("music\\林宥嘉 Yoga Lin [ 兜圈 ] Official Music Video (偶像劇｢必娶女人｣片尾曲).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 林宥嘉 Yoga Lin [ 兜圈 ] Official Music Video";
	}
	if(test==14)
	{
		PlaySound(TEXT("music\\郁可唯 Yisa Yu [ 路過人間 Walking by the world ] Official Music Video（電視劇《我們與惡的距離》插曲）.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 郁可唯 Yisa Yu [ 路過人間 Walking by the world ] Official Music Video";
	}
	if(test==15)
	{
		PlaySound(TEXT("music\\梁文音 那女孩對我說 歌詞.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 梁文音 那女孩對我說";
	}
	if(test==16)
	{
		PlaySound(TEXT("music\\徐佳莹《真的傻》.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 徐佳瑩-真的傻";
	}
	if(test==17)
	{
		PlaySound(TEXT("music\\陳雪凝 - 你的酒館對我打了烊【動態歌詞Lyrics】.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 陳雪凝 - 你的酒館對我打了烊";
	}
	if(test==18)
	{
		PlaySound(TEXT("music\\陳零九 Nine Chen feat. 邱鋒澤 FENG ZE【天黑請閉眼 Werewolves】Official Music Video.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: 陳零九 Nine Chen feat. 邱鋒澤 FENG ZE【天黑請閉眼 Werewolves】Official Music Video";
	}
	if(test==19)
	{
		PlaySound(TEXT("music\\Taylor Swift - ME! (Lyrics) ft. Brendon Urie of Panic! At The Disco (online-audio-converter.com).wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: Taylor Swift - ME! (Lyrics) ft. Brendon Urie of Panic!";
	}
	if(test==20)
	{
		PlaySound(TEXT("music\\spark.wav"),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
		playLabel->Caption= "Now playing: F.I.R.《 星火 Spark 》電影「鬥魚」主題曲 Official Music Video";
		test=1;
	}

}
//---------------------------------------------------------------------------


void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	Timer1->Interval++;
	if(Form1->Timer1->Interval%10==0)
		Form1->playLabel->Font->Color = clRed;
	else if(Form1->Timer1->Interval%10==1)
		Form1->playLabel->Font->Color = clOlive;
	else if(Form1->Timer1->Interval%10==2)
		Form1->playLabel->Font->Color = clMoneyGreen;
	else if(Form1->Timer1->Interval%10==3)
		Form1->playLabel->Font->Color = clSkyBlue;
	else if(Form1->Timer1->Interval%10==4)
		Form1->playLabel->Font->Color = clWhite;
	else if(Form1->Timer1->Interval%10==5)
		Form1->playLabel->Font->Color = clSilver;
	else if(Form1->Timer1->Interval%10==6)
		Form1->playLabel->Font->Color = clTeal;
	else if(Form1->Timer1->Interval%10==7)
		Form1->playLabel->Font->Color = clLime;
	else if(Form1->Timer1->Interval%10==8)
		Form1->playLabel->Font->Color = clAqua;
	else if(Form1->Timer1->Interval%10==9)
		Form1->playLabel->Font->Color = clYellow;
}
//---------------------------------------------------------------------------

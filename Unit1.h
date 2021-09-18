//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.MPlayer.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *WebCam1;
	TMenuItem *DriverNameVersion1;
	TMenuItem *On1;
	TMenuItem *Off1;
	TMenuItem *CaptureWindow1;
	TMenuItem *Create1;
	TMenuItem *Destroy1;
	TMenuItem *DriverCaptureWindow1;
	TMenuItem *Connect1;
	TMenuItem *Disconnect1;
	TMenuItem *DisplayModePreview1;
	TMenuItem *Enable1;
	TMenuItem *Disable1;
	TMenuItem *PreviewRate1;
	TMenuItem *N30framesec1;
	TMenuItem *N15framesec1;
	TMenuItem *PreviewScale1;
	TMenuItem *On2;
	TMenuItem *Off2;
	TMenuItem *Size1;
	TMenuItem *N3202401;
	TMenuItem *N6404801;
	TMenuItem *Autoprocessing1;
	TMenuItem *Dialog1;
	TMenuItem *miVideoSource;
	TMenuItem *miVideoFormat;
	TMenuItem *miVideoDisplay;
	TMenuItem *miVideoCompression;
	TMenuItem *Capabilities1;
	TMenuItem *Status1;
	TMenuItem *CaptureParameter1;
	TMenuItem *GetSet1;
	TSavePictureDialog *SavePictureDialog1;
	TMenuItem *Video1;
	TMenuItem *Previos1;
	TMenuItem *miFrameCounter;
	TMenuItem *N1;
	TMenuItem *Mirror1;
	TMenuItem *Object1;
	TMenuItem *Off3;
	TMenuItem *miFormat;
	TMenuItem *GrayMode1;
	TMenuItem *SelectColor1;
	TColorDialog *ColorDialog1;
	TMenuItem *Single1;
	TMenuItem *Dragonball1;
	TMenuItem *Gecko1;
	TMenuItem *N2;
	TMenuItem *Color1;
	TMenuItem *miColorFiltering;
	TMenuItem *miBackground;
	TMenuItem *miDifferenceDetection;
	TMenuItem *miSpecificColor;
	TMenuItem *miSkinColor;
	TMenuItem *miColorMode;
	TMenuItem *miBinaryMode;
	TMenuItem *miBlockMode;
	TMenuItem *miInteraction;
	TMenuItem *miVirtualSpottedGecko;
	TMenuItem *miPlay;
	TMenuItem *miReplay;
	TTimer *Timer1;
	TPageControl *pcWebcam;
	TTabSheet *tsCaptureWindow;
	TPanel *pnlDriverDescription;
	TPanel *pnlCaptureWindow;
	TTabSheet *tsInformation;
	TLabel *lblCapDriverName;
	TLabel *lblCapDriverVersion;
	TLabel *lblDriverDescription;
	TLabel *lblDriverIndex;
	TLabel *lblOverlay;
	TLabel *lblVideoSourceDialog;
	TLabel *lblVideoFormatDialog;
	TLabel *lblDriverReady;
	TLabel *lblDriverPalette;
	TLabel *lblVideoDisplayDialog;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TTabSheet *tsImage;
	TImage *imBackground;
	TTabSheet *TabSheet1;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *Label22;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TLabel *Label28;
	TLabel *Label29;
	TLabel *Label30;
	TLabel *Label31;
	TLabel *Label32;
	TLabel *Label33;
	TLabel *Label34;
	TLabel *Label35;
	TLabel *Label36;
	TLabel *Label37;
	TLabel *Label38;
	TLabel *Label39;
	TLabel *Label40;
	TLabel *Label41;
	TLabel *Label42;
	TLabel *Label43;
	TLabel *Label44;
	TTabSheet *tsVideoFormat;
	TLabel *Label45;
	TLabel *lblVideoFormatSize;
	TLabel *Label47;
	TLabel *lblSize;
	TLabel *lblHeight;
	TLabel *lblWidth;
	TLabel *lblPlanes;
	TLabel *lblBitCount;
	TLabel *lblCompression;
	TLabel *lblSizeImage;
	TLabel *lblXPelsPerMeter;
	TLabel *lblYPelsPerMeter;
	TLabel *lblColorUsed;
	TLabel *lblColorImportant;
	TImage *imGecko;
	TTabSheet *TabSheet2;
	TPanel *Panel1;
	TImage *Image3;
	TImage *pauseImage;
	TImage *diceImage;
	TPanel *Panel2;
	TEdit *edMinY;
	TEdit *edMinU;
	TEdit *edMinV;
	TLabel *Label46;
	TLabel *Label48;
	TLabel *Label49;
	TEdit *edMaxY;
	TEdit *edMaxU;
	TEdit *edMaxV;
	TButton *btnGrabFrame;
	TButton *GrabFrameNoStop;
	TButton *btnEditCopy;
	TButton *btnFileSaveDIB;
	TPanel *Panel3;
	TLabel *playLabel;
	TButton *Button1;
	TPanel *Panel5;
	TShape *Shape1;
	TShape *Shape2;
	TLabel *Label51;
	TLabel *lblFrameCounter;
	TImage *Image1;
	TImage *Image2;
	TImage *Image4;
	TImage *Image5;
	TLabel *Label52;
	TLabel *Label53;
	TLabel *Label54;
	TShape *Shape3;
	TLabel *Label50;
	TEdit *edThreshold;
	TCheckBox *cbBlockDisplay;
	TImage *Image6;
	void __fastcall On1Click(TObject *Sender);
	void __fastcall Off1Click(TObject *Sender);
	void __fastcall Create1Click(TObject *Sender);
	void __fastcall Destroy1Click(TObject *Sender);
	void __fastcall Disconnect1Click(TObject *Sender);
	void __fastcall Connect1Click(TObject *Sender);
	void __fastcall Enable1Click(TObject *Sender);
	void __fastcall Disable1Click(TObject *Sender);
	void __fastcall N30framesec1Click(TObject *Sender);
	void __fastcall N15framesec1Click(TObject *Sender);
	void __fastcall N3202401Click(TObject *Sender);
	void __fastcall N6404801Click(TObject *Sender);
	void __fastcall On2Click(TObject *Sender);
	void __fastcall Off2Click(TObject *Sender);
	void __fastcall Autoprocessing1Click(TObject *Sender);
	void __fastcall Capabilities1Click(TObject *Sender);
	void __fastcall miVideoSourceClick(TObject *Sender);
	void __fastcall miVideoFormatClick(TObject *Sender);
	void __fastcall miVideoDisplayClick(TObject *Sender);
	void __fastcall miVideoCompressionClick(TObject *Sender);
	void __fastcall btnGrabFrameClick(TObject *Sender);
	void __fastcall GrabFrameNoStopClick(TObject *Sender);
	void __fastcall btnEditCopyClick(TObject *Sender);
	void __fastcall btnFileSaveDIBClick(TObject *Sender);
	void __fastcall miFrameCounterClick(TObject *Sender);
	void __fastcall Off3Click(TObject *Sender);
	void __fastcall miFormatClick(TObject *Sender);
	void __fastcall GrayMode1Click(TObject *Sender);
	void __fastcall Mirror1Click(TObject *Sender);
	void __fastcall Single1Click(TObject *Sender);
	void __fastcall Color1Click(TObject *Sender);
	void __fastcall SelectColor1Click(TObject *Sender);
	void __fastcall Gecko1Click(TObject *Sender);
	void __fastcall miSpecificColorClick(TObject *Sender);
	void __fastcall edMinYChange(TObject *Sender);
	void __fastcall edMaxYChange(TObject *Sender);
	void __fastcall edMaxUChange(TObject *Sender);
	void __fastcall edMinUChange(TObject *Sender);
	void __fastcall edMinVChange(TObject *Sender);
	void __fastcall edMaxVChange(TObject *Sender);
	void __fastcall miBackgroundClick(TObject *Sender);
	void __fastcall edThresholdChange(TObject *Sender);
	void __fastcall miColorModeClick(TObject *Sender);
	void __fastcall miBinaryModeClick(TObject *Sender);
	void __fastcall miBlockModeClick(TObject *Sender);
	void __fastcall miPlayClick(TObject *Sender);
	void __fastcall miReplayClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);



private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

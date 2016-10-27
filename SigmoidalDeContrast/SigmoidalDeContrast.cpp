//----------------------------------------------------------------------------------
//		サンプルビデオフィルタ(フィルタプラグイン)  for AviUtl ver0.99e以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"
#include "wand/MagickWand.h"


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
#define	TRACK_N	2														//	トラックバーの数
TCHAR	*track_name[] = { "Strength", "Midtone" };	//	トラックバーの名前
int		track_default[] = { 6, 128 };	//	トラックバーの初期値
int		track_s[] = { 0, 0 };	//	トラックバーの下限値
int		track_e[] = { 30, 255 };	//	トラックバーの上限値

#define	CHECK_N	4														//	チェックボックスの数
TCHAR	*check_name[] = { "RGB colorspace", "sRGB colorspace", "LAB colorspace", "LUV colorspace" };				//	チェックボックスの名前
int		check_default[] = { 1, 0, 0, 0 };				//	チェックボックスの初期値 (値は0か1)

FILTER_DLL filter = {
	FILTER_FLAG_EX_INFORMATION | FILTER_FLAG_RADIO_BUTTON,	//	フィルタのフラグ
	//	FILTER_FLAG_ALWAYS_ACTIVE		: フィルタを常にアクティブにします
	//	FILTER_FLAG_CONFIG_POPUP		: 設定をポップアップメニューにします
	//	FILTER_FLAG_CONFIG_CHECK		: 設定をチェックボックスメニューにします
	//	FILTER_FLAG_CONFIG_RADIO		: 設定をラジオボタンメニューにします
	//	FILTER_FLAG_EX_DATA				: 拡張データを保存出来るようにします。
	//	FILTER_FLAG_PRIORITY_HIGHEST	: フィルタのプライオリティを常に最上位にします
	//	FILTER_FLAG_PRIORITY_LOWEST		: フィルタのプライオリティを常に最下位にします
	//	FILTER_FLAG_WINDOW_THICKFRAME	: サイズ変更可能なウィンドウを作ります
	//	FILTER_FLAG_WINDOW_SIZE			: 設定ウィンドウのサイズを指定出来るようにします
	//	FILTER_FLAG_DISP_FILTER			: 表示フィルタにします
	//	FILTER_FLAG_EX_INFORMATION		: フィルタの拡張情報を設定できるようにします
	//	FILTER_FLAG_NO_CONFIG			: 設定ウィンドウを表示しないようにします
	//	FILTER_FLAG_AUDIO_FILTER		: オーディオフィルタにします
	//	FILTER_FLAG_RADIO_BUTTON		: チェックボックスをラジオボタンにします
	//	FILTER_FLAG_WINDOW_HSCROLL		: 水平スクロールバーを持つウィンドウを作ります
	//	FILTER_FLAG_WINDOW_VSCROLL		: 垂直スクロールバーを持つウィンドウを作ります
	//	FILTER_FLAG_IMPORT				: インポートメニューを作ります
	//	FILTER_FLAG_EXPORT				: エクスポートメニューを作ります
	0, 0,						//	設定ウインドウのサイズ (FILTER_FLAG_WINDOW_SIZEが立っている時に有効)
	"Sigmoidal DeContrast",			//	フィルタの名前
	TRACK_N,					//	トラックバーの数 (0なら名前初期値等もNULLでよい)
	track_name,					//	トラックバーの名前郡へのポインタ
	track_default,				//	トラックバーの初期値郡へのポインタ
	track_s, track_e,			//	トラックバーの数値の下限上限 (NULLなら全て0～256)
	CHECK_N,					//	チェックボックスの数 (0なら名前初期値等もNULLでよい)
	check_name,					//	チェックボックスの名前郡へのポインタ
	check_default,				//	チェックボックスの初期値郡へのポインタ
	func_proc,					//	フィルタ処理関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	設定が変更されたときに呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	設定ウィンドウにウィンドウメッセージが来た時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL, NULL,					//	システムで使いますので使用しないでください
	NULL,						//  拡張データ領域へのポインタ (FILTER_FLAG_EX_DATAが立っている時に有効)
	NULL,						//  拡張データサイズ (FILTER_FLAG_EX_DATAが立っている時に有効)
	"Sigmoidal DeContrast by Maverick Tse",
	//  フィルタ情報へのポインタ (FILTER_FLAG_EX_INFORMATIONが立っている時に有効)
	NULL,						//	セーブが開始される直前に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	セーブが終了した直前に呼ばれる関数へのポインタ (NULLなら呼ばれません)
};


//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable(void)
{
	return &filter;
}
//	下記のようにすると1つのaufファイルで複数のフィルタ構造体を渡すことが出来ます
/*
FILTER_DLL *filter_list[] = {&filter,&filter2,NULL};
EXTERN_C FILTER_DLL __declspec(dllexport) ** __stdcall GetFilterTableList( void )
{
return (FILTER_DLL **)&filter_list;
}
*/


//---------------------------------------------------------------------
//		フィルタ処理関数
//---------------------------------------------------------------------
BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	//
	//	fp->track[n]			: トラックバーの数値
	//	fp->check[n]			: チェックボックスの数値
	//	fpip->w 				: 実際の画像の横幅
	//	fpip->h 				: 実際の画像の縦幅
	//	fpip->max_w				: 画像領域の横幅
	//	fpip->max_h				: 画像領域の縦幅
	//	fpip->ycp_edit			: 画像領域へのポインタ
	//	fpip->ycp_temp			: テンポラリ領域へのポインタ
	//	fpip->ycp_edit[n].y		: 画素(輝度    )データ (     0 ～ 4096 )
	//	fpip->ycp_edit[n].cb	: 画素(色差(青))データ ( -2048 ～ 2048 )
	//	fpip->ycp_edit[n].cr	: 画素(色差(赤))データ ( -2048 ～ 2048 )
	//
	//  画素データは範囲外に出ていることがあります。
	//  また範囲内に収めなくてもかまいません。
	//
	//	画像サイズを変えたいときは fpip->w や fpip->h を変えます。
	//
	//	テンポラリ領域に処理した画像を格納したいときは
	//	fpip->ycp_edit と fpip->ycp_temp を入れ替えます。
	//
	if (fpip->ycp_edit && fp->exfunc->is_editing(fpip->editp) && fp->exfunc->is_filter_active(fp))
	{

		//get RGB data
		int canvas_pixel_count = fpip->max_w*fpip->max_h;
		//
		PIXEL *rgbdata = new PIXEL[canvas_pixel_count];
		fp->exfunc->yc2rgb(rgbdata, fpip->ycp_edit, canvas_pixel_count);
		//IM initialization
		if (!IsMagickWandInstantiated()) MagickWandGenesis();
		//
		//Feed data into IM
		MagickWand *imdata = NewMagickWand();
		MagickConstituteImage(imdata, fpip->max_w, fpip->max_h, "BGR", StorageType::CharPixel, rgbdata);
		MagickSetFirstIterator(imdata);
		//Crop out canvas
		MagickCropImage(imdata, fpip->w, fpip->h, 0, 0);
		//Set Colorspace
		MagickSetColorspace(imdata, sRGBColorspace);
		if (fp->check[0])
		{
			MagickTransformImageColorspace(imdata, RGBColorspace);
			//MagickSetColorspace(imdata, RGBColorspace);
		}
		else if (fp->check[1])
		{
			MagickTransformImageColorspace(imdata, sRGBColorspace);
			//MagickSetColorspace(imdata, sRGBColorspace);
		}
		else if (fp->check[2])
		{
			MagickTransformImageColorspace(imdata, LabColorspace);
			//MagickSetColorspace(imdata, LabColorspace);
		}
		else if (fp->check[3])
		{
			MagickTransformImageColorspace(imdata, LuvColorspace);
			//MagickSetColorspace(imdata, LuvColorspace);
		}
		else
		{
			MagickTransformImageColorspace(imdata, RGBColorspace);
			//MagickSetColorspace(imdata, RGBColorspace);
		}
		//Apply Sigmoidal contrast
		if (!MagickSigmoidalContrastImage(imdata, MagickFalse, fp->track[0], fp->track[1]))
		{
			ExceptionType error = MagickGetExceptionType(imdata);
			TCHAR *msg = MagickGetException(imdata, &error);
			MessageBox(NULL, msg, "MagickSigmoidalContrastImage Error", MB_OK);
		}
		MagickTransformImageColorspace(imdata, sRGBColorspace);
		//Extent canvas
		//Create a black pixel wand
		PixelWand *imBlack = NewPixelWand();
		PixelSetColor(imBlack, "#000000");
		MagickSetBackgroundColor(imdata, imBlack);
		//Expand canvas
		MagickExtentImage(imdata, fpip->max_w, fpip->max_h, 0, 0);

		//Convert RGB to YC48
		MagickExportImagePixels(imdata, 0, 0, fpip->max_w, fpip->max_h, "BGR", StorageType::CharPixel, rgbdata);
		fp->exfunc->rgb2yc(fpip->ycp_temp, rgbdata, canvas_pixel_count);
		//swap temp and edit
		PIXEL_YC* buf = fpip->ycp_edit;
		fpip->ycp_edit = fpip->ycp_temp;
		fpip->ycp_temp = buf;
		buf = nullptr;

		//Cleanup
		DestroyPixelWand(imBlack);
		DestroyMagickWand(imdata);
		delete[] rgbdata;
		if (IsMagickWandInstantiated()) MagickWandTerminus();
	}

	return TRUE;
}



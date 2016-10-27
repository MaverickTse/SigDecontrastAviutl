//----------------------------------------------------------------------------------
//		�T���v���r�f�I�t�B���^(�t�B���^�v���O�C��)  for AviUtl ver0.99e�ȍ~
//----------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"
#include "wand/MagickWand.h"


//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
#define	TRACK_N	2														//	�g���b�N�o�[�̐�
TCHAR	*track_name[] = { "Strength", "Midtone" };	//	�g���b�N�o�[�̖��O
int		track_default[] = { 6, 128 };	//	�g���b�N�o�[�̏����l
int		track_s[] = { 0, 0 };	//	�g���b�N�o�[�̉����l
int		track_e[] = { 30, 255 };	//	�g���b�N�o�[�̏���l

#define	CHECK_N	4														//	�`�F�b�N�{�b�N�X�̐�
TCHAR	*check_name[] = { "RGB colorspace", "sRGB colorspace", "LAB colorspace", "LUV colorspace" };				//	�`�F�b�N�{�b�N�X�̖��O
int		check_default[] = { 1, 0, 0, 0 };				//	�`�F�b�N�{�b�N�X�̏����l (�l��0��1)

FILTER_DLL filter = {
	FILTER_FLAG_EX_INFORMATION | FILTER_FLAG_RADIO_BUTTON,	//	�t�B���^�̃t���O
	//	FILTER_FLAG_ALWAYS_ACTIVE		: �t�B���^����ɃA�N�e�B�u�ɂ��܂�
	//	FILTER_FLAG_CONFIG_POPUP		: �ݒ���|�b�v�A�b�v���j���[�ɂ��܂�
	//	FILTER_FLAG_CONFIG_CHECK		: �ݒ���`�F�b�N�{�b�N�X���j���[�ɂ��܂�
	//	FILTER_FLAG_CONFIG_RADIO		: �ݒ�����W�I�{�^�����j���[�ɂ��܂�
	//	FILTER_FLAG_EX_DATA				: �g���f�[�^��ۑ��o����悤�ɂ��܂��B
	//	FILTER_FLAG_PRIORITY_HIGHEST	: �t�B���^�̃v���C�I���e�B����ɍŏ�ʂɂ��܂�
	//	FILTER_FLAG_PRIORITY_LOWEST		: �t�B���^�̃v���C�I���e�B����ɍŉ��ʂɂ��܂�
	//	FILTER_FLAG_WINDOW_THICKFRAME	: �T�C�Y�ύX�\�ȃE�B���h�E�����܂�
	//	FILTER_FLAG_WINDOW_SIZE			: �ݒ�E�B���h�E�̃T�C�Y���w��o����悤�ɂ��܂�
	//	FILTER_FLAG_DISP_FILTER			: �\���t�B���^�ɂ��܂�
	//	FILTER_FLAG_EX_INFORMATION		: �t�B���^�̊g������ݒ�ł���悤�ɂ��܂�
	//	FILTER_FLAG_NO_CONFIG			: �ݒ�E�B���h�E��\�����Ȃ��悤�ɂ��܂�
	//	FILTER_FLAG_AUDIO_FILTER		: �I�[�f�B�I�t�B���^�ɂ��܂�
	//	FILTER_FLAG_RADIO_BUTTON		: �`�F�b�N�{�b�N�X�����W�I�{�^���ɂ��܂�
	//	FILTER_FLAG_WINDOW_HSCROLL		: �����X�N���[���o�[�����E�B���h�E�����܂�
	//	FILTER_FLAG_WINDOW_VSCROLL		: �����X�N���[���o�[�����E�B���h�E�����܂�
	//	FILTER_FLAG_IMPORT				: �C���|�[�g���j���[�����܂�
	//	FILTER_FLAG_EXPORT				: �G�N�X�|�[�g���j���[�����܂�
	0, 0,						//	�ݒ�E�C���h�E�̃T�C�Y (FILTER_FLAG_WINDOW_SIZE�������Ă��鎞�ɗL��)
	"Sigmoidal DeContrast",			//	�t�B���^�̖��O
	TRACK_N,					//	�g���b�N�o�[�̐� (0�Ȃ疼�O�����l����NULL�ł悢)
	track_name,					//	�g���b�N�o�[�̖��O�S�ւ̃|�C���^
	track_default,				//	�g���b�N�o�[�̏����l�S�ւ̃|�C���^
	track_s, track_e,			//	�g���b�N�o�[�̐��l�̉������ (NULL�Ȃ�S��0�`256)
	CHECK_N,					//	�`�F�b�N�{�b�N�X�̐� (0�Ȃ疼�O�����l����NULL�ł悢)
	check_name,					//	�`�F�b�N�{�b�N�X�̖��O�S�ւ̃|�C���^
	check_default,				//	�`�F�b�N�{�b�N�X�̏����l�S�ւ̃|�C���^
	func_proc,					//	�t�B���^�����֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�J�n���ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�I�����ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�ݒ肪�ύX���ꂽ�Ƃ��ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�ݒ�E�B���h�E�ɃE�B���h�E���b�Z�[�W���������ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL, NULL,					//	�V�X�e���Ŏg���܂��̂Ŏg�p���Ȃ��ł�������
	NULL,						//  �g���f�[�^�̈�ւ̃|�C���^ (FILTER_FLAG_EX_DATA�������Ă��鎞�ɗL��)
	NULL,						//  �g���f�[�^�T�C�Y (FILTER_FLAG_EX_DATA�������Ă��鎞�ɗL��)
	"Sigmoidal DeContrast by Maverick Tse",
	//  �t�B���^���ւ̃|�C���^ (FILTER_FLAG_EX_INFORMATION�������Ă��鎞�ɗL��)
	NULL,						//	�Z�[�u���J�n����钼�O�ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�Z�[�u���I���������O�ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
};


//---------------------------------------------------------------------
//		�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable(void)
{
	return &filter;
}
//	���L�̂悤�ɂ����1��auf�t�@�C���ŕ����̃t�B���^�\���̂�n�����Ƃ��o���܂�
/*
FILTER_DLL *filter_list[] = {&filter,&filter2,NULL};
EXTERN_C FILTER_DLL __declspec(dllexport) ** __stdcall GetFilterTableList( void )
{
return (FILTER_DLL **)&filter_list;
}
*/


//---------------------------------------------------------------------
//		�t�B���^�����֐�
//---------------------------------------------------------------------
BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	//
	//	fp->track[n]			: �g���b�N�o�[�̐��l
	//	fp->check[n]			: �`�F�b�N�{�b�N�X�̐��l
	//	fpip->w 				: ���ۂ̉摜�̉���
	//	fpip->h 				: ���ۂ̉摜�̏c��
	//	fpip->max_w				: �摜�̈�̉���
	//	fpip->max_h				: �摜�̈�̏c��
	//	fpip->ycp_edit			: �摜�̈�ւ̃|�C���^
	//	fpip->ycp_temp			: �e���|�����̈�ւ̃|�C���^
	//	fpip->ycp_edit[n].y		: ��f(�P�x    )�f�[�^ (     0 �` 4096 )
	//	fpip->ycp_edit[n].cb	: ��f(�F��(��))�f�[�^ ( -2048 �` 2048 )
	//	fpip->ycp_edit[n].cr	: ��f(�F��(��))�f�[�^ ( -2048 �` 2048 )
	//
	//  ��f�f�[�^�͔͈͊O�ɏo�Ă��邱�Ƃ�����܂��B
	//  �܂��͈͓��Ɏ��߂Ȃ��Ă����܂��܂���B
	//
	//	�摜�T�C�Y��ς������Ƃ��� fpip->w �� fpip->h ��ς��܂��B
	//
	//	�e���|�����̈�ɏ��������摜���i�[�������Ƃ���
	//	fpip->ycp_edit �� fpip->ycp_temp �����ւ��܂��B
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



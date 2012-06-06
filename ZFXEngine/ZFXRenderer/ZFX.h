#ifndef ZFX_H
#define ZFX_H

#include <windows.h>
extern bool g_bLF;
// everything went smooth
#define ZFX_OK                        S_OK

// just reports no errors
#define ZFX_CANCELED                  0x02000001

// generell error message
#define ZFX_FAIL                      0x82000001

// specific error messages

// specific error messages
#define ZFX_CREATEAPI                 0x82000002
#define ZFX_CREATEDEVICE              0x82000003
#define ZFX_CREATEBUFFER              0x82000004
#define ZFX_INVALIDPARAM              0x82000005
#define ZFX_INVALIDID                 0x82000006
#define ZFX_BUFFERSIZE                0x82000007
#define ZFX_BUFFERLOCK                0x82000008
#define ZFX_NOTCOMPATIBLE             0x82000009
#define ZFX_OUTOFMEMORY               0x8200000a
#define ZFX_FILENOTFOUND              0x8200000b
#define ZFX_INVALIDFILE               0x8200000c
#define ZFX_NOSHADERSUPPORT           0x8200000d
#define ZFX_FAILa               0x8200000c
#define ZFX_FAILb               0x8200000d
#define ZFX_FAILc               0x8200000e
#define ZFX_FAILd               0x8200000f
#define ZFX_FAIL5               0x82000006
#define ZFX_FAIL6               0x82000007
#define ZFX_FAIL7               0x82000008
#define ZFX_FAIL8               0x82000009
#define ZFX_FAIL9               0x8200000a
#define ZFX_FAIL0               0x8200000b



typedef struct ZFXCOLOR_TYPE
{
	union{
		struct{
			float fR;
			float fG;
			float fB;
			float fA;
		};
		float c[4];
	};
}ZFXCOLOR;

typedef struct ZFXMATERIAL_TYPE
{
	ZFXCOLOR cDiffuse;
	ZFXCOLOR cAmbient;
	ZFXCOLOR cSpecular;
	ZFXCOLOR cEmissive;
	float fPower;
}ZFXMATERIAL;

typedef struct ZFXTEXTURE_TYPE{
	float fAlpha;
	char *chName; //texture filename;
	void *pData;
	ZFXCOLOR* pClrKeys;  //Color key array
	DWORD dwNum;   //Number of color keys;
}ZFXTEXTURE;


#define ZFX_MAX_TEXTURE_NUM 8

typedef struct ZFXSKIN_TYPE
{
	bool bAlpha;
	UINT nMaterial;
	UINT nTexture[ZFX_MAX_TEXTURE_NUM];
}ZFXSKIN;

#endif
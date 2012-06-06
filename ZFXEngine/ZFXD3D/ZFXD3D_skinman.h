#ifndef ZFXD3DSKINMAN_H
#define ZFXD3DSKINMAN_H
#include "ZFX_skinman.h"
#include "ZFXD3D.h"

class ZFXD3DSkinManager:public ZFXSkinManager
{
	friend class ZFXD3DVCache;

	public:
		ZFXD3DSkinManager(LPDIRECT3DDEVICE9 pDevice,FILE* pLog);
		~ZFXD3DSkinManager(void);

	protected:
		LPDIRECT3DDEVICE9 m_pDevice;
		FILE *m_pLog;

		inline bool ColorEqual(const ZFXCOLOR* pCol0,const  ZFXCOLOR* pCol1);
		bool MaterialEqual(const ZFXMATERIAL* pMat0,const ZFXMATERIAL *pMat1);

		HRESULT CreateTexture(ZFXTEXTURE* pTexture,bool bAlpha);
		HRESULT SetAlphaKey(LPDIRECT3DTEXTURE9 *ppTexture, UCHAR R, UCHAR G, UCHAR B, UCHAR A);
		HRESULT SetTransparency(LPDIRECT3DTEXTURE9 *ppTexture, UCHAR Alpha);

		DWORD MakeD3DColor(UCHAR R, UCHAR G,UCHAR B, UCHAR A);

		ZFXSKIN GetSkin(UINT nSkinID);
		ZFXMATERIAL GetMaterial(UINT nMatID);
		HRESULT CheckTextureBuffer();
		const char* GetTextureName(UINT nTexID,
										   float *pfAlpha,
										   ZFXCOLOR* pAK,
										   UCHAR* pNum);

		HRESULT AddTextureHeightmapAsBump(UINT nSkinID, const char* chName);
		HRESULT AddTexture(UINT nSkinID, 
									const char* chName,
									bool bAlpha,
									float fAlpha,
									ZFXCOLOR* cColorKeys,
									DWORD dwNumColorKeys);
		HRESULT AddSkin(const ZFXCOLOR* pcAmbient,
							    const ZFXCOLOR* pcDiffuse,
								const ZFXCOLOR* pcEmissive,
								const ZFXCOLOR* pcSpecular,
								float fSpecPower,
								UINT* nSkinID);

		void Log(char*, ...);
};

#endif
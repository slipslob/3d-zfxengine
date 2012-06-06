#include "ZFXD3D_skinman.h"

#define MAX_ID 65535
#define RGB16BIT(r,g,b) ((b%32)+((g%64)<<5)+((r%32)<<11))

static ZFXMATERIAL EmptyMaterial;
static ZFXSKIN EmptySkin;

void ZFXD3DSkinManager::Log(char* chString, ...)
{
	char ch[256];
	char *pArgs;
	pArgs=(char*)&chString+sizeof(chString);
	vsprintf(ch,chString,pArgs);

	fprintf(m_pLog, "[ ZFXD3DSkinManager ]: ");
	fprintf(m_pLog,ch);
	fprintf(m_pLog,"\n");

	if(g_bLF)
		fflush(m_pLog);
}


ZFXD3DSkinManager::ZFXD3DSkinManager(LPDIRECT3DDEVICE9 pDevice,FILE* pLog){
	m_nNumMaterials=0;
	m_nNumTextures=0;
	m_nNumSkins=0;
	m_pMaterials=NULL;
	m_pTextures=NULL;
	m_pSkins=NULL;
	m_pLog=pLog;
	m_pDevice=pDevice;
	Log("online");
}

ZFXD3DSkinManager::~ZFXD3DSkinManager(){
	if(m_pTextures){
		for(UINT i=0; i<m_nNumTextures;i++){
			if(m_pTextures[i].pData){
				((LPDIRECT3DDEVICE9)(m_pTextures[i].pData))->Release();
				m_pTextures[i].pData=NULL;
			}
			if(m_pTextures[i].pClrKeys){
				delete [] m_pTextures[i].pClrKeys;
				m_pTextures[i].pClrKeys=NULL;
			}
			if(m_pTextures[i].chName){
				delete [] m_pTextures[i].chName;
				m_pTextures[i].chName=NULL;
			}
		}
		free(m_pTextures);
		m_pTextures=NULL;
	}
	if(m_pMaterials){
		free(m_pMaterials);
		m_pMaterials=NULL;
	}
	if(m_pSkins){
		free(m_pSkins);
		m_pSkins=NULL;
	}
	Log("Offline(ok)");
}

inline bool  ZFXD3DSkinManager::ColorEqual(const ZFXCOLOR* pCol0,const ZFXCOLOR* pCol1){
	if(pCol0->fA!=pCol1->fA ||
		pCol0->fR!=pCol1->fR ||
		pCol0->fG!=pCol1->fG ||
		pCol0->fB!=pCol1->fB)
		return false;
	return true;
}

bool ZFXD3DSkinManager::MaterialEqual(const ZFXMATERIAL* pMat0,const ZFXMATERIAL *pMat1){
	if(!ColorEqual(&pMat0->cAmbient,&pMat1->cAmbient) ||
		!ColorEqual(&pMat0->cDiffuse,&pMat1->cDiffuse) ||
		!ColorEqual(&pMat0->cSpecular,&pMat1->cSpecular) ||
		!ColorEqual(&pMat0->cEmissive,&pMat1->cEmissive) ||
		pMat0->fPower!=pMat1->fPower)
		return false;
	return true;
}

ZFXSKIN ZFXD3DSkinManager::GetSkin(UINT nSkinID){
	if(nSkinID<m_nNumSkins){
		return m_pSkins[nSkinID];
	}else{
		return EmptySkin;
	}
}

ZFXMATERIAL ZFXD3DSkinManager::GetMaterial(UINT nMatID){
	if(nMatID<m_nNumMaterials)
		return m_pMaterials[nMatID];
	else{
		return EmptyMaterial;
	}
}


const char* ZFXD3DSkinManager::GetTextureName(UINT nTexID,float* pfAlpha,ZFXCOLOR *pAK,UCHAR *pNum){
	if(nTexID>=m_nNumTextures) return NULL;
	if(pfAlpha) *pfAlpha=m_pTextures[nTexID].fAlpha;
	if(pNum) *pNum=m_pTextures[nTexID].dwNum;

	if(m_pTextures[nTexID].pClrKeys && pAK){
		memcpy(pAK,m_pTextures[nTexID].pClrKeys,sizeof(ZFXCOLOR)*m_pTextures[nTexID].dwNum);
	}

	return m_pTextures[nTexID].chName;
}

HRESULT ZFXD3DSkinManager::AddSkin(const ZFXCOLOR* pcAmbient,const ZFXCOLOR *pcDiffuse,const ZFXCOLOR* pcEmissive,const ZFXCOLOR* pcSpecular,
								   float fSpecPower,UINT* nSkinID){
	
	UINT nMat,n;
	bool bMat=false;

	if((m_nNumSkins%50)==0){
		n=(m_nNumSkins+50)*sizeof(ZFXSKIN);
		m_pSkins=(ZFXSKIN*)realloc(m_pSkins,n);
		if(!m_pSkins) return ZFX_OUTOFMEMORY;
	}

	ZFXMATERIAL mat;
	mat.cAmbient=*pcAmbient;
	mat.cDiffuse=*pcDiffuse;
	mat.cEmissive=*pcEmissive;
	mat.cSpecular=*pcSpecular;
	mat.fPower=fSpecPower;

	for(nMat=0;nMat<m_nNumMaterials;nMat++){
		if(MaterialEqual(&mat,&m_pMaterials[nMat])){
			bMat=true;
			break;
		}
	}

	if(bMat) m_pSkins[m_nNumSkins].nMaterial=nMat;
	else{
		m_pSkins[m_nNumSkins].nMaterial=m_nNumMaterials;
		if((m_nNumMaterials%50)==0){
			n=(m_nNumMaterials+50)*sizeof(ZFXMATERIAL);
			m_pMaterials=(ZFXMATERIAL*)realloc(m_pMaterials,n);
			if(!m_pMaterials) return ZFX_OUTOFMEMORY;
		}

		memcpy(&m_pMaterials[m_nNumMaterials],&mat,sizeof(ZFXMATERIAL));
		m_nNumMaterials++;
	}

	m_pSkins[m_nNumSkins].bAlpha=false;
	for(int i=0;i<8;i++)
		m_pSkins[m_nNumSkins].nTexture[i]=MAX_ID;

	(*nSkinID)=m_nNumSkins;
	m_nNumSkins++;

	return ZFX_OK;
}

HRESULT ZFXD3DSkinManager::CheckTextureBuffer(){
	UINT n;
	if((m_nNumTextures%50)==0){
			n=(m_nNumTextures+50)*sizeof(ZFXTEXTURE);
			m_pTextures=(ZFXTEXTURE*)realloc(m_pTextures,n);
			if(!m_pTextures) return ZFX_OUTOFMEMORY;
	}	
	return ZFX_OK;
}

HRESULT ZFXD3DSkinManager::AddTexture(UINT nSkinID, const char* chName,bool bAlpha,float fAlpha,
									ZFXCOLOR* cColorKeys,DWORD dwNumColorKeys){
				
	ZFXTEXTURE* pZFXTex=NULL;
	HRESULT hr;
	UINT nTex,n;
	bool bTex=false;

	if(nSkinID>m_nNumSkins) return ZFX_INVALIDID;

	if(m_pSkins[nSkinID].nTexture[ZFX_MAX_TEXTURE_NUM-1]!=MAX_ID){
		Log("error: AddTexture failed,all stages set");	
		return ZFX_BUFFERSIZE;
	}

	for(nTex=0;nTex<m_nNumTextures;nTex++){
		if(strcmp(chName,m_pTextures[nTex].chName)==0){
			bTex=true;
			break;
		}
	}

	if(!bTex){
		
		if(CheckTextureBuffer()==ZFX_OUTOFMEMORY)
			return ZFX_OUTOFMEMORY;
		
		if(bAlpha) m_pSkins[nSkinID].bAlpha=true;
		else m_pTextures[m_nNumTextures].fAlpha=1.0f;

		m_pTextures[m_nNumTextures].pClrKeys=NULL;

		m_pTextures[m_nNumTextures].chName=new char[strlen(chName)+1];
		memcpy(m_pTextures[m_nNumTextures].chName,chName,strlen(chName)+1);

		hr=CreateTexture(&m_pTextures[m_nNumTextures],bAlpha);
		if(FAILED(hr)) return hr;


		if(bAlpha){
			pZFXTex=&m_pTextures[m_nNumTextures];
			pZFXTex->dwNum=dwNumColorKeys;
			pZFXTex->pClrKeys=new ZFXCOLOR[dwNumColorKeys];
			memcpy(pZFXTex->pClrKeys,cColorKeys,sizeof(ZFXCOLOR)*dwNumColorKeys);
			LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)pZFXTex->pData;

			for(DWORD dw=0;dw<dwNumColorKeys;dw++){
				hr=SetAlphaKey(&pTex,UCHAR(cColorKeys[dw].fR*255),UCHAR(cColorKeys[dw].fG*255),
					UCHAR(cColorKeys[dw].fB*255),UCHAR(cColorKeys[dw].fA*255));
				if(FAILED(hr)) return hr;
			}
			
			if(fAlpha<1.0f){
				m_pTextures[m_nNumTextures].fAlpha=fAlpha;
				hr=SetTransparency(&pTex,UCHAR(fAlpha*255));
				if(FAILED(hr))return hr;
			}
		}
		nTex=m_nNumTextures;
		m_nNumTextures++;	
	}	

	for(int i=0;i<ZFX_MAX_TEXTURE_NUM;i++){
		if(m_pSkins[nSkinID].nTexture[i]==MAX_ID){
			m_pSkins[nSkinID].nTexture[i]=nTex;
			break;
		}
	}
}

HRESULT ZFXD3DSkinManager::AddTextureHeightmapAsBump(UINT nSkinID,const char* chName){
	int nTex;
	bool bTex=false;

	if(nSkinID>=m_nNumSkins) return ZFX_INVALIDID;

	if(m_pSkins[nSkinID].nTexture[ZFX_MAX_TEXTURE_NUM-1]!=MAX_ID){
		Log("error: AddTextureHeightmapAsBump failed,all stages set");	
		return ZFX_BUFFERSIZE;
	}
	
	for(nTex=0;nTex<m_nNumTextures;nTex++){
		if(strcmp(chName,m_pTextures[nTex].chName)==0){
			bTex=true;
			break;
		}
	}

	if(!bTex){
		if(CheckTextureBuffer()==ZFX_OUTOFMEMORY)
			return ZFX_OUTOFMEMORY;
		
		m_pTextures[m_nNumTextures].fAlpha=1.0f;
		m_pTextures[m_nNumTextures].pClrKeys=NULL;

		m_pTextures[m_nNumTextures].chName=new char[strlen(chName)+1];
		memcpy(m_pTextures[m_nNumTextures].chName,chName,strlen(chName)+1);

		HRESULT hr=CreateTexture(&m_pTextures[m_nNumTextures],true);
		if(FAILED(hr)){
			Log("Error: CreateTexture() failed");
			return hr;
		}

		hr=ConvertToNormalMap(&m_pTextures[m_nNumTextures]);
		if(FAILED(hr)){
			Log("Error: ConvertToNormalMap() failed");
			return hr;
		}
		
		nTex=m_nNumTextures;
		m_nNumTextures++;
	}

	for(int i=0;i<ZFX_MAX_TEXTURE_NUM;i++){
		if(m_pSkins[nSkinID].nTexture[i]==MAX_ID){
			m_pSkins[nSkinID].nTexture[i]=nTex;
			break;
		}
	}
	return ZFX_OK;
}

HRESULT ZFXD3DSkinManager::CreateTexture(ZFXTEXTURE* pTexture,bool bAlpha){
	
	D3DLOCKED_RECT d3dRect;
	D3DFORMAT	fmt;
	DIBSECTION dibS;
	HRESULT	hr;
	int LineWidth;
	void *pMemory=NULL;

	HBITMAP hBMP=(HBITMAP)LoadImage(NULL,pTexture->chName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	if(!hBMP) return ZFX_FILENOTFOUND;

	GetObject(hBMP,sizeof(DIBSECTION),&dibS);

	if(dibS.dsBmih.biBitCount!=24){
		DeleteObject(hBMP);
		return ZFX_INVALIDFILE;
	}

	if(bAlpha) fmt=D3DFMT_A8R8G8B8;
	else fmt=D3DFMT_R5G6B5;

	long lWidth=dibS.dsBmih.biWidth;
	long lHeight=dibS.dsBmih.biHeight;
	BYTE *pBMPBits=(BYTE*)dibS.dsBm.bmBits;

	hr=m_pDevice->CreateTexture(lWidth,lHeight,1,0,fmt,D3DPOOL_MANAGED,
		(LPDIRECT3DTEXTURE9*)(&(pTexture->pData)),NULL);

	if(FAILED(hr))return ZFX_FAIL;

	LPDIRECT3DTEXTURE9 pTex=((LPDIRECT3DTEXTURE9)pTexture->pData);

	if(FAILED(pTex->LockRect(0,&d3dRect,NULL,0)))
		return ZFX_BUFFERLOCK;

	if(bAlpha){
		LineWidth=d3dRect.Pitch>>2;
		pMemory=(DWORD*)d3dRect.pBits;
	}else{
		LineWidth=d3dRect.Pitch>>1;
		pMemory=(USHORT*)d3dRect.pBits;
	}

	for(int cy=0;cy<lHeight;cy++){
		for(int cx=0;cx<lWidth;cx++){
			
			if(bAlpha){
				DWORD color=0xff000000;
				int i=(cy*lWidth+cx)*3;
				memcpy(&color,&pBMPBits[i],sizeof(BYTE)*3);
				((DWORD*)pMemory)[cx+(cy*LineWidth)]=color;
			}else{
				UCHAR B=(pBMPBits[(cy*lWidth+cx)*3+0])>>3,
					G=(pBMPBits[(cy*lWidth+cx)*3+1])>>3,
					R=(pBMPBits[(cy*lWidth+cx)*3+2])>>3;

				USHORT color=RGB16BIT((int)(((float)R/255.0f)*32.0f),
					(int)(((float)G/255.0f)*64.0f),
					(int)(((float)B/255.0f)*32.0f));
				
				((USHORT*)pMemory)[cx+(cy*LineWidth)]=color;
			}
		}
	}
	pTex->UnlockRect(0);
	DeleteObject(hBMP);
	return ZFX_OK;
}
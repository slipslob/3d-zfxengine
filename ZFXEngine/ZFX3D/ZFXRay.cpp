#include "zfx3d.h"

inline void ZFXRay::Set(ZFXVector vcOrig,ZFXVector vcDir){
	this->m_vcOrig=vcOrig;
	this->m_vcDir=vcDir;
}


inline void ZFXRay::DeTransform(const ZFXMatrix &m){
	ZFXMatrix mInv;
	ZFXMatrix m=_m;
	m_vcOrig.x-=m._41;
	m_vcOrig.y-=m._42;
	m_vcOrig.z-=m._43;

	m._41=0.0f; m._42=0.0f; m._43=0.0f;

	mInv.inverseOf(&m);

	m_vcOrig=m_vcOrig*mInv;
	m_vcDir=m_vcDir*mInv;

}
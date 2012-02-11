#include "zfx3d.h"

inline void ZFXPlane::Set(const ZFXVector &vcN, const ZFXVector &vcPoint){
	m_fD= -(vcN*vcPoint);
	m_vcN=vcN;
	m_vcPoint=vcPoint;
}

inline void ZFXPlane::Set(const ZFXVector &vcN, const ZFXVector &vcPoint, float fD){
	m_fD=fD;
	m_vcN=vcN;
	m_vcPoint=vcPoint;
}

inline void ZFXPlane::Set(const ZFXVector &v0, const ZFXVector &v1, const ZFXVector &v2){
	ZFXVector edge1=v1-v0;
	ZFXVector edge2=v2-v0;

	m_vcN.Cross(edge1,edge2);
	m_fD=m_vcN*v0;
}

inline float ZFXPlane::Distance(const ZFXVector &vcPoint){
	return (_fabs(m_vcN*vcPoint)-m_fD);
}

inline int ZFXPlane::Classify(const ZFXVector &vcPoint){
	float f=((m_vcN*vcPoint)-m_fD);
	if(f>0.00001f) return ZFXFRONT;
	if(f<-0.00001f) return ZFXBACK;
	return ZFXPLANAR;
}


bool ZFXPlane::Intersects(const ZFXVector &vc0, const ZFXVector &vc1, const ZFXVector &vc2){
	int n=this->Classify(vc0);

	if ((n==this->Classify(vc1)) && (n==this->Classify(vc2)))
		return false;

	return true;
}

bool ZFXPlane::Intersects(ZFXPlane &plane, ZFXRay *pIntersection){
	ZFXVector vcCross;
	float fSqrLenght;

	//if cross product equal 0 planes are parallel
	vcCross.Cross(this->m_vcN,plane.m_vcN);
	fSqrLenght=vcCross.GetSqrLength();

	if(fSqrLenght<FLT_MIN)
		return false;

	//if we need the intersection line
	if(pIntersection){
		float fN00=this->m_vcN.GetSqrLength();
		float fN01=this->m_vcN*plane.m_vcN;
		float fN11=plane.m_vcN.GetSqrLength();
		float fDet=fN00*fN11 - fN01*fN01;

		if(_fabs(fDet)<FLT_MIN)
			return false;
		
		float fInvDet=1.0f/fDet;
		float fC0= (fN11*this->m_fD - fN01*plane.m_fD)*fInvDet;
		float fC1= (fN00*this->m_fD - fN01*this->m_fD)*fInvDet;

		(*pIntersection).m_vcDir=vcCross;
		(*pIntersection).m_vcOrig=this->m_vcN*fC0 + plane.m_vcN*fC1;

	}
	return true;
}


bool ZFXPlane::Intersects(const ZFXAabb &aabb){
	ZFXVector Vmin,Vmax;

	//x-coordinate
	if(m_vcN.x >=0.0f){
		Vmin.x=aabb.vcMin.x;
		Vmax.x=aabb.vcMax.x;
	}else{
		Vmin.x=aabb.vcMax.x;
		Vmax.x=aabb.vcMin.x;
	}
	//y coord
	if(m_vcN.y >=0.0f){
		Vmin.y=aabb.vcMin.y;
		Vmax.y=aabb.vcMax.y;
	}else{
		Vmin.y=aabb.vcMax.y;
		Vmax.y=aabb.vcMin.y;
	}

	//z coord
	if(m_vcN.z >=0.0f){
		Vmin.z=aabb.vcMin.z;
		Vmax.z=aabb.vcMax.z;
	}else{
		Vmin.z=aabb.vcMax.z;
		Vmax.z=aabb.vcMin.z;
	}

	if(((m_vcN*Vmin)+m_fD)>0.0f)
		return false;

	if(((m_vcN*Vmax)+m_fD)>=0.0f)
		return true;

	return false;
}

bool ZFXPlane::Intersects(const ZFXObb &obb){
	float fRadius=_fabs(obb.fA0*(m_vcN*obb.vcA0))+
				  _fabs(obb.fA1*(m_vcN*obb.vcA1))+
				  _fabs(obb.fA1*(m_vcN*obb.vcA2));

	float fDistance=this->Distance(obb.vcCenter);
	return (fDistance<=fRadius);
}
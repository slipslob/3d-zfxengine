
#include "zfx3d.h"

ZFXPolygon::ZFXPolygon(void){
	m_pPoints=NULL;
	m_pIndis=NULL;
	m_NumP=0;
	m_NumI=0;
	m_Flag=0;
	memset(&m_Aabb,0,sizeof(ZFXAabb));
}

ZFXPolygon::~ZFXPolygon(void){
	if(m_pPoints){
		delete [] m_pPoints;
		m_pPoints=NULL;
	}
	
	if(m_pIndis){
		delete [] m_pIndis;
		m_pIndis=NULL;
	}
}

void ZFXPolygon::Set(const ZFXVector * pPoints, int nNumP, const unsigned int * pIndis, int nNumI){
	ZFXVector vcEdge0,vcEdge1;
	bool bGotEm=false;

	if(m_pPoints) delete [] m_pPoints;
	if(m_pIndis) delete [] m_pIndis;

	m_pPoints=new ZFXVector[nNumP];
	m_pIndis=new unsigned int[nNumI];

	m_NumP=nNumP;
	m_NumI=nNumI;
	
	memcpy(m_pPoints,pPoints,sizeof(ZFXVector)*nNumP);
	memcpy(m_pIndis,pIndis,sizeof(ZFXVector)*nNumI);

	vcEdge0=m_pPoints[m_pIndis[1]]-m_pPoints[m_pIndis[0];

	for(int i=0;bGotEm==false;i++){
		if((i+1)>m_NumI) break;

		vcEdge1=m_pPoints[m_pIndis[i]]-m_pPoints[m_pIndis[0];

		vcEdge0.Normalize();
		vcEdge1.Normalize();

		if (vcEdge0.AngleWith(vcEdge1)!=0.0){
			bGotEm=true;
			break;
		}	
	}

	m_Plane.m_vcN.Cross(vcEdge0,vcEdge1);
	m_Plane.m_vcN.Normalize();
	m_Plane.m_fD= -(m_Plane.m_vcN*m_pPoints[0]);
	m_Plane.m_vcPoint=m_pPoints[0];

	CalcBoundingBox();
}

void ZFXPolygon::CalcBoundingBox(void){
	ZFXVector vcMax,vcMin;

	vcMax=vcMin=m_pPoints[0];

	for(int i=0;i<m_NumP;i++){
		if(m_pPoints[i].x>vcMax.x)
			vcMax.x=m_pPoints[i].x;
		else if (m_pPoints[i].x<vcMin.x)
			vcMin.x=m_pPoints[i].x;

		if(m_pPoints[i].y>vcMax.y)
			vcMax.y=m_pPoints[i].y;
		else if (m_pPoints[i].y<vcMin.y)
			vcMin.y=m_pPoints[i].y;

		if(m_pPoints[i].z>vcMax.z)
			vcMax.z=m_pPoints[i].z;
		else if (m_pPoints[i].z<vcMin.z)
			vcMin.z=m_pPoints[i].z;
	}

	m_Aabb.vcMin=vcMin;
	m_Aabb.vcMax=vcMax;
	m_Aabb.vcCenter=(vcMax+VcMin)/2.0f;
}

void ZFXPolygon::SwapFaces(void){
	unsigned int* pIndis=new unsigned int[m_NumI];

	for(int i=0;i<m_NumI;i++){
		pIndis[m_NumI-i-1]=m_pIndis[i];
	}

	m_Plane.m_vcN*=-1.0f;
	m_Plane.m_fD*=-1.0f;

	delete [] m_pIndis;
	m_pIndis=pIndis;
}

}


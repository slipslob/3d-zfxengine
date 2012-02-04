#include "zfx3d.h"

float _fabs(float f){ if(f<0.0f) return -f; return f;}

inline void ZFXVector::Set(float _x,float _y,float _z,float _w){
	x=_x;
	y=_y;
	z=_z;
	w=_w;
}

void ZFXVector::operator +=(const ZFXVector &v){
	x+=v.x;
	y+=v.y;
	z+=v.z;
	w+=v.w;
}

ZFXVector ZFXVector::operator+(const ZFXVector &v) const{
	return ZFXVector(x+v.x,y+v.y,z+v.z);
}

void ZFXVector::operator-=(const ZFXVector &v){
	x-=v.x;	y-=v.y;	z-=v.z;
}

ZFXVector ZFXVector::operator-(const ZFXVector &v) const{
	return ZFXVector(x-v.x,y-v.y,z-v.z);
}

void ZFXVector::operator*=(float f){
	x*=f;	y*=f;	z*=f;
}

void ZFXVector::operator/=(float f){
	x/=f;	y/=f;	z/=f;
}

ZFXVector ZFXVector::operator*(float f) const{
	return ZFXVector(x*f,y*f,z*f);
}

ZFXVector ZFXVector::operator*(const ZFXVector &v) const{
	return ZFXVector(x*v.x,y*v.y,z*v.z);
}

inline float ZFXVector::GetSqrLength(void) const{
	return(x*x+y*y+z*z);
}

inline void ZFXVector::Negate(void){
	x=-x; y=-y; z=-z;
}

inline void ZFXVector::Difference(const ZFXVector &u, const ZFXVector &v){
	x=v2.x-v1.x;
	y=v2.y-v1.y;
	z=v2.z-v1.z;
	w=1.0f;
}

inline float ZFXVector::AngleWith(ZFXVector &v){
	return (float)acos(((*this)*v)/(this->GetLength()*v.GetLength());
}

inline float ZFXVector::GetLength(void){
	float f;
	
	if(!g_bSSE){
		f=(float)sqrt(this->GetSqrLength());
	}else{
		float *pf=&f;
		w=0.0f;
		__asm{
			mov ecx,pf;		//Point to result register
			mov esi,this	//Vector u in register
			movups xmm0,[esi]	//load from memory into xmm0
			mulps xmm0,xmm0		//multiply component-wise
			movaps xmm1,xmm0
			shufps xmm1,xmm1, 4Eh //shuffle f1,f0,f3,f2 (01 00 11 10)
			addps xmm0,xmm1 
			movaps xmm1,xmm0
			shufps xxm1,xmm1, 11h //shuffle f0,f1,f0,f1 (00 01 00 01)
			addps xmm0,xmm1
			sqrtss xmm0,xmm0
			movss [ecs],xmm0
		}
		w=1.0f;
	}
	return f;
}


inline void ZFXVector::Normalize(void){
	if(!g_bSSE){
		float f=this->GetLength();
		if(f!=0.0f)
			(*this)/=f;
	}else{						//Honestly i think is better the normal way because GetLength is already SSE and i don't have to duplicate code
		w=0.0f;
		__asm{
			mov esi,this
			mov xmm0,[this]
			movaps xmm2,xmm0
			mulps xmm0,xmm0
			movaps xmm1,xmm0
			shufps xmm1,xmm1, 4Eh
			addps xmm0,xmm1
			movaps xmm1,xmm0
			shufps xmm1,xmm1, 11h
			addps xmm0,xmm1

			rsqrtps xmm0,xmm1
			mulps xmm2,xmm0
			movups [esi],xmm2
		}
		w=1.0f;
	}
}

inline void ZFXVector::Cross(const ZFXVector &u,const ZFXVector &v){
	if(!g_bSSE){
		x = v1.y*v2.z - v1.z*v2.y;
		y = v1.z*v2.x - v1.x*v2.z;
		z = v1.x*v2.y - v1.y*v2.x;
		w=1.0f;
	}else{
		w=0.0f;
		__asm{
			mov esi,v1
			mov edi,v2

			movups xmm0,[esi]
			movups xmm1,[edi]
			movaps xmm2,xmm0
			movaps xmm3,xmm1

			shufps xmm0,xmm0, 0xc9  //F3 F0 F2 F1	(11 00 10 01)
			shufps xmm1,xmm1, 0xd2	//F3 F1	F0 F2	(11 01 00 10)
			mulps xmm0,xmm1
		
			shufps xmm2,xmm2, 0xd2  //F3 F1 F0 F2	(11 01 00 10)
			shufps xmm3,xmm3, 0xc9	//F3 F0	F2 F1	 11 00 10 01) 
			mulps xmm2,xmm3		

			subps xmm0,xmm2

			mov esi,this
			movups [esi],xmm0
		}
		w=1.0f;
	}
}
		
ZFXVector ZFXVector::operator *(const ZFXMatrix &m) const{
	ZFXVector vcResult;
	if(!g_bSSE){
		vcResult.x= x*m._11 + y*m._21+ z*m._31 + m._41;
		vcResult.y= x*m._12 + y*m._22+ z*m._32 + m._42;
		vcResult.z= x*m._13 + y*m._23+ z*m._33 + m._43;
		vcResult.w= x*m._14 + y*m._24+ z*m._34 + m._44;

		vcResult/=vcResult.w;
		vcResult.w=1.0f;
	}else{
		float *ptrRet=(float*)&vcResult;
		__asm{
			mov ecx,this
			mov edx, m
			movss xmm0,[ecx]
			mov eax,ptrRet
			shufps xmm0,xmm0,0
			mulps xmm0,[edx]
			
			movss xmm1,[ecx+4]
			shufps xmm1,xmm1,0
			mulps xmm1,[edx+16]

			movss xmm2,[ecx+8]
			shufps xmm2,xmm2,0
			mulps xmm2,[edx+32]
			
			//movss xmm3,[ecx+12]
			//shufps xmm3,xmm3,0
			//multps xmm3,[edx+48]
			movups xmm3,[edx+48]

			addps xmm2,xmm3
			addps xmm0,xmm2
			movups [eax],xmm0
			mov [eax+3],1
		}
	}
}
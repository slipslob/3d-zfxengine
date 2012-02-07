#ifndef ZFX3D_H
#define ZFX3D_H


#include <math.h>
/////GLOBALS
bool g_bSSE=false;

/////

/////STRUCT
typedef struct CPUINFO_TYP{
	bool bSSE;	//Streaming SIMD Extension
	bool bSSE2; //Streaming SIMD Extension 2
	bool b3DNOW; //3DNOW!
	bool bMMX;	//MMX Support
	char name[48];	//cpu name
	bool bEXT;	//Extended feature available;
	bool bMMXEX; //MMX (AMD Specific Extensions)
	bool b3DNOWEX;  //3DNow! AMD Specific Extension
	char vendor[13];	//vendor name
}CPUINFO;

//////


//////FUNCTION DECLARATION
bool ZFX3DInitCPU()
//////END FUNCTION

/////CLASSES
class __declspec(dllexport) ZFXVector{
public:
	float x,y,z,w;

	ZFXVector(void) {x=0;y=0;z=0;w=1.0f;}
	ZFXVector(float _x,float _y,float _z)
	{x=_x;y=_y;z=_z;w=1.0f;}

	inline void Set(float _x,float _y, float _z, float_w=1.0f);
	inline float GetLength(void);
	inline float GetSqrLength(void) const;
	inline void Negate(void);
	inline void Normalize(void);
	inline float AngleWith(ZFXVector &v);
	inline void Difference(const ZFXVector &u, const ZFXVector &v);

	void operator+= (const ZFXVector &V);
	void operator -=(const ZFXVector &v);
	void operator *=(float f);
	void operator /=(float f);

	float operator *(ZFXVector &v) const;
	ZFXVector operator *(float f)const;
	ZFXVector operator *(ZFXMatrix &m)const;
	ZFXVector operator +(ZFXVector &v)const;
	ZFXVector operator -(ZFXVector &v)const;

	inline void Cross(const ZFXVector &u, const ZFXVector &v);
};

class __declspec(dllexport) ZFXMatrix{
public:
	float _11,_12,_13,_14;
	float _21,_22,_23,_24;
	float _31,_32,_33,_34;
	float _41,_42,_43,_44;

	ZFXMatrix(void){
		float *f=(float*)&(this->_11);
		memset(f,0,sizeof(ZFXMatrix));
	}
	
	inline void Identity(void);			//Set the matrix to identity;
	inline void RotaX(float angle);			//Set the matrix as a rotation matrix around axis X
	inline void RotaY(float angle);			//Set the matrix as a rotation matrix around axis Y
	inline void RotaZ(float angle);			//Set the matrix as a rotation matrix around axis Z
	inline void RotArbi(ZFXVector vcAxis, float angle);	//Rotation on arbitrary axis
	inline void Translate(float dx,float dy,float dz);	//Translation

	inline void TransposeOf(const ZFXMatrix &m);
	inline void inverseOf(const ZFXMatrix &m);

	ZFXMatrix operator * (const ZFXMatrix &m) const;
	ZFXVector operator * (const ZFXVector &vc) const;

};

class __declspec(dllexport) ZFXRay{
public:
	ZFXVector m_vcOrig,m_vcDir;

	ZFXRay(void) {}

	inline void Set(ZFXVector vcOrig,ZFXVector vcDir);
	inline void DeTransform(const ZFXMatrix &m);

	//Vector Intersection
	bool Intersects(const ZFXVector &vc0,const ZFXVector &vc1, 
					const ZFXVector &vc2,bool bCull,float *t);

	bool Intersects(const ZFXVector &vc0,const ZFXVector &vc1, 
					const ZFXVector &vc2,bool bCull,
					float fL,float *t);
	//Plane intersection
	bool Intersects(const ZFXPlane &plane,bool bCull,float *t,ZFXVector *vcHit);

	bool Intersects(const ZFXPlane &plane,bool bCull,float fL,float *t,ZFXVector *vcHit);

	//Axis aligned bounding box intersection
	bool Intersects(const ZFXAabb,ZFXVector *vcHit);

	//Object aligned bounding box intersection
	bool Intersects(const ZFXObb &obb,float *t);
	bool Intersects(const ZFXObb &obb,float fL,float* t);

};

//////END CLASSES

#endif
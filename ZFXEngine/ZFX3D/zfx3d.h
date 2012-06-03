#ifndef ZFX3D_H

////DEFINITIONS
#define ZFX3D_H
#define ZFXFRONT 0
#define ZFXBACK 1
#define ZFXPLANAR 2
#define ZFXCLIPPED 3
#define ZFXCULLED 4
#define ZFXVISIBLE 5


const double ZFXPI  =  3.14159265;
const double ZFXPI2 =  1.5707963;
const double ZFX2PI =  6.2831853;
const float  ZFXG   = -32.174f; // ft/s^2
const float  ZFXEPSILON = 0.00001f;
///////

//////FORWARD DECLARATIONS
class ZFXVector;
class ZFXMatrix;
class ZFXRay;
class ZFXPlane;
class ZFXObb;
class ZFXAabb;
class ZFXPolygon;
/////

/////INCLUDES
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <float.h>
#include <excpt.h>
#include <windows.h>
#include <winnt.h>
#include <string.h>
/////

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
float _fabs(float f);
bool ZFX3DInitCPU();
//////END FUNCTION

/////CLASSES
class __declspec(dllexport) ZFXVector{
public:
	float x,y,z,w;

	ZFXVector(void) {x=0;y=0;z=0;w=1.0f;}
	ZFXVector(float _x,float _y,float _z)
	{x=_x;y=_y;z=_z;w=1.0f;}

	inline void Set(float _x,float _y, float _z, float _w=1.0f);
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

	float operator *(const ZFXVector &v) const;
	ZFXVector operator *(float f)const;
	ZFXVector operator /(float f)const;
	ZFXVector operator *(const ZFXMatrix &m)const;
	ZFXVector operator +(const ZFXVector &v)const;
	ZFXVector operator -(const ZFXVector &v)const;

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
	inline void DeTransform(const ZFXMatrix &_m);

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
	bool Intersects(const ZFXObb &obb,float *t) const;
	bool Intersects(const ZFXObb &obb,float fL,float* t) const;

};

class __declspec(dllexport) ZFXPlane{
public:
	ZFXVector m_vcN,		//normal Vector
			  m_vcPoint;	//point on plane
	float m_fD;		//distance

	ZFXPlane(void){ }

	inline void Set(const ZFXVector &vcN,const ZFXVector &vcPoint);
	inline void Set(const ZFXVector &vcN,const ZFXVector &vcPoint,float fD);
	inline void Set(const ZFXVector &v0,const ZFXVector &v1,const ZFXVector &v2);

	//distance of a point to the plane
	inline float Distance(const ZFXVector &vcPoint);
	//Classifying a point with respect to plane
	inline int Classify(const ZFXVector &vcPoint);
	int Classify(const ZFXPolygon &poly);

	//intersection with a triangle
	inline bool Intersects(const ZFXVector &vc0,const ZFXVector &vc1,const ZFXVector &vc2);

	//Line from intersection of 2 planes
	bool Intersects(ZFXPlane &plane,ZFXRay* pIntersection);

	//Intersection with aabb, oobb
	bool Intersects(const ZFXAabb &aabb);
	bool Intersects(const ZFXObb &oobb);
};

class __declspec(dllexport) ZFXAabb{
public:
	ZFXVector vcMin,vcMax;	//extreme points
	ZFXVector vcCenter;		//Center point

	ZFXAabb(void){}
	
	void Construct(const ZFXObb &pObb);
	int Cull(const ZFXPlane *pPlanes,int nNumPlanes);
	void GetPlanes(ZFXPlane* pPlanes);
	bool Contains(const ZFXRay &ray,float fL);
	bool Intersects(const ZFXAabb &aabb);
	bool Intersects(const ZFXVector &vc);
};

class __declspec(dllexport) ZFXObb{
public:
	float fA0,fA1,fA2;
	ZFXVector vcA0,vcA1,vcA2;
	ZFXVector vcCenter;

	ZFXObb(void){}

	inline void DeTransform(const ZFXObb &obb,const ZFXMatrix &m);
	inline bool Intersects(const ZFXRay &ray,float *t);
	inline bool Intersects(const ZFXRay &ray,float fL,float *t);

	bool Intersects(const ZFXObb &Obb);
	bool Intersects(const ZFXVector &v0,const ZFXVector &v1,const ZFXVector &v2);

	int Cull(const ZFXPlane *pPlanes,int nNumPlanes);

private:
	void ObbProj(const ZFXObb &Obb,const ZFXVector &vcV,float* pfMin,float* pfMax);
	void TriProj(const ZFXVector &v0,const ZFXVector &v1,const ZFXVector &v2,const ZFXVector &vcV,
		float* pfMin,float* pfMax);

};

class __declspec(dllexport) ZFXPolygon{
	friend class ZFXPlane;

private:
	ZFXPlane m_Plane; //plane of Polygon
	int m_NumP;		//Number of points
	int m_NumI;		//Number of Indices
	ZFXAabb	m_Aabb;	//Bounding box
	unsigned int m_Flag;	
	ZFXVector* m_pPoints; //Points
	unsigned int *m_pIndis; //Indices;

	void CalcBoundingBox(void);

public:
	ZFXPolygon(void);
	~ZFXPolygon(void);

	void Set(const ZFXVector *, int, const unsigned int*,int);
	void Clip(const ZFXPlane &Plane,ZFXPolygon *pFront, ZFXPolygon *pBack);
	void Clip(const ZFXAabb &aabb);
	int Cull(const ZFXAabb &aabb);
	void CopyOf(const ZFXPolygon &Poly);

	void SwapFaces(void);
	bool Intersects(const ZFXRay&,bool,float*);
	bool Intersects(const ZFXRay&,bool,float fL,float *t);

	int GetNumPoints(void){return m_NumP;}
	int GetNumIndis(void){return m_NumI;}
	ZFXVector* GetPoints(void){return m_pPoints;}
	unsigned int* GetIndices(void){return m_pIndis;}
	ZFXPlane GetPlane(void){return m_Plane;}
	ZFXAabb GetAabb(void){return m_Aabb;}
	unsigned int GetFlag(void){ return m_Flag;}
	void SetFlag(unsigned int n){m_Flag=n;}

	void ZFXPolygon::Print(FILE *p);
};



// Our basic quaternion class
class __declspec(dllexport) ZFXQuat {
   public:
      float x, y, z, w;

      //---------------------------------------

      ZFXQuat(void) { x=0.0f, y=0.0f, z=0.0f, w=1.0f; }
      ZFXQuat(float _x, float _y, float _z, float _w)
         { x=_x; y=_y; z=_z; w=_w; }

      void  MakeFromEuler(float fPitch, float fYaw, float fRoll);
      void  Normalize();
      void  Conjugate(ZFXQuat q);
      void  GetEulers(float *fPitch, float *fYaw, float *fRoll);
      void  GetMatrix(ZFXMatrix *m);
      float GetMagnitude(void);


      void    operator /= (float f);
      ZFXQuat operator /  (float f);

      void    operator *= (float f);
      ZFXQuat operator *  (float f);

      ZFXQuat operator *  (const ZFXVector &v) const;

      ZFXQuat operator *  (const ZFXQuat &q) const;
      void    operator *= (const ZFXQuat &q);

      void    operator += (const ZFXQuat &q);
      ZFXQuat operator +  (const ZFXQuat &q) const;

      ZFXQuat operator~(void) const { return ZFXQuat(-x, -y, -z, w); }

      void Rotate(const ZFXQuat &q1, const ZFXQuat &q2);

      ZFXVector Rotate(const ZFXVector &v);

   }; // class

//////END CLASSES

#endif
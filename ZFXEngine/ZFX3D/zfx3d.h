#ifndef ZFX3D_H
#define ZFX3D_H

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


//////END CLASSES

#endif
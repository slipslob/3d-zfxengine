#include "zfx3d.h"


CPUINFO GetCPUInfo(){
	CPUINFO info;
	char* pStr=info.vendor;
	int n=1;
	int *pn=&n;

	memset(&info,0,sizeof(CPUINFO));
	//1: Vendor name,SSE2,SSE,MMX support

	__try{
		__asm{
			mov eax,0   //get Vendor name
			CPUID
			mov esi, pStr
			mov [esi],ebx	//first 4 chars
			mov [esi+4],edx	//second 4 chars
			mov [esi+8],ecx	//third 4 chars

			mov eax,1	//Get feature list
			CPUID

			test edx,04000000h	//SSE2 test
			jz	_NOSSE2			//jump if negative
			mov [info.bSSE2],1  //true

__NOSSE2:	test edx,02000000h	//SSE test
			jz	_NOSSE			//jump if negative
			mov [info.bSSE],1  //true

__NOSSE:	test edx,00800000h	//MMX test
			jz	_EXIT1			//jump if negative
			mov [info.bMMX],1  //true
__EXIT1:	//done
		}
	}__except(EXCEPTION_EXECUTE_HANDLER){
		if (_exception_code()== STATUS_ILLEGAL_INSTRUCTION)
			return info;	//CPU inactive
		return info;	//unexpected error
	}
	//Test Extended Features
	__asm{
		mov eax,80000000h
		CPUID
		cmp eax,80000000h	//eax >80000000h
		jbe _EXIT2			//jump if negative
		mov [info.bEXT],1	//true

		mov eax,80000001h
		CPUID
		test edx,80000000h	//test 3DNOW
		jz _EXIT2			//jump if negative
		mov [info.b3DNOW],1
__EXIT2:	
	}

	//Vendor Dependent 
	//INTEL: CPU id
	//AMD: CPU id, 3dnow_ex,mmx_ex
	if ((strncmp(info.vendor,"GenuineIntel",12)==0) && info.bEXT){	//intel
		__asm{
			mov eax,1
			CPUID
			mov esi,pn
			mov [esi],ebx
		}
		int m=0;
		memcpy(&m,pn,sizeof(char));
		n=m;
	}else if ((strncmp(info.vendor,"AuthenticAMD",12)==0) && info.bEXT){
		__asm{
			mov eax,1
			CPUID
			mov esi,pn
			mov [esi],eax

			mov eax,0x80000001
			CPUID
			test edx,0x40000000	//AMD Extended 3DNOW!
			jz _AMD1
			mov [info.b3DNOWEX],1
__AMD1:		test edx,0x00400000
			jz _AMD2
			mov [info.bMMXEX],1
__AMD2:		
		}
	}
	else{
		if(info.bEXT)
			;
		else
			;
	}

	info.vendor[13]='\0';
	//GetCPUName(info.name,n,info.vendor);
	return info;
}

bool OSSupportsSSE(){
	__try{
		__asm xorps xmm0,xmm0
	}
	__except(EXCEPTION_EXECUTE_HANDLER){
		if(_exception_code()==STATUS_ILLEGAL_INSTRUCTION)
			return false;	//sse not supported
		return false;	//Unexpected error
	}
	return true;
}

bool ZFX3DInitCPU(){
	CPUINFO info=GetCPUInfo();
	bool bOS=OSSupportsSSE();

	if(info.bSSE && bOS) g_bSSE=true;
	else g_bSSE=false;
	return g_bSSE;
}
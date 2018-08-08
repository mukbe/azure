#pragma once

#define WinSizeX 1280
#define WinSizeY 720

#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p){ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }

#define ONE_RAD 0.01745328f
#define IdentityMatrix {D3DXMATRIX(1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f)}
#define VertexType(t) using VertexType = t;

#define Synthesize(VarType, VarName, FuncName)	\
protected:	VarType VarName;	\
public:	inline VarType Get##FuncName(void) const { return VarName; }	\
public:	inline void Set##FuncName(VarType value) { VarName = value; }

#define Singleton(ClassName)	\
private:						\
ClassName(void);	\
~ClassName(void);	\
public:		\
static ClassName* GetInstance()	\
{			\
	static ClassName Instance;	\
	return &Instance;			\
}

#define FEPSLON		0.00001f

//a 와 b 가 같냐?
#define FLOATQUAL(a, b)     (abs((a)- (b)) <FEPSLON)

//a 가 0이냐?
#define FLOATZERO(a)		(abs(a) < FEPSLON)

//제로 벡터 이냐?
#define VECTORZERO(v)		(FLOATZERO((v).x) && FLOATZERO((v).y)&& FLOATZERO((v).z))

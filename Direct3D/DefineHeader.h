#pragma once

#define WinSizeX 1280
#define WinSizeY 720

#define SafeRelease(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SafeDelete(p){ if(p){ delete (p); (p) = NULL; } }
#define SafeDeleteArray(p){ if(p){ delete [] (p); (p) = NULL; } }

#define ONE_RAD 0.01745328f
#define IdentityMatrix D3DXMATRIX(1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f)
#define VertexType(t) using VertexType = t;

#define Synthesize(VarType, VarName, FuncName)	\
protected:	VarType VarName;	\
public:	inline VarType Get##FuncName(void) const { return VarName; }	\
public:	inline void Set##FuncName(VarType value) { VarName = value; }


#define FEPSLON		0.00001f

//a 와 b 가 같냐?
#define FLOATQUAL(a, b)     (abs((a)- (b)) <FEPSLON)

//a 가 0이냐?
#define FLOATZERO(a)		(abs(a) < FEPSLON)

//제로 벡터 이냐?
#define VECTORZERO(v)		(FLOATZERO((v).x) && FLOATZERO((v).y)&& FLOATZERO((v).z))

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HResult
#define HResult(x)                                       \
   {                                             \
      HRESULT hr = (x);                              \
      if(FAILED(hr))                                 \
      {                                          \
         LPWSTR output;                              \
         FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |            \
         FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   \
            (LPTSTR)&output, 0, NULL);                                             \
            MessageBox(NULL, output, L"Error", MB_OK);                                 \
      }                                                                     \
   }
#endif

#else
#ifndef HResult
#define HResult(x) (x)
#endif
#endif 

#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG)
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert : " #b "\n"); }} while (0)
#else
#define Assert(b)
#endif   //   DEBUG || _DEBUG
#endif

#define SingletonHeader(ClassName)	\
public:								\
	static ClassName* Get();		\
	static void Create();			\
	static void Delete();			\
private:							\
	ClassName();					\
	~ClassName();					\
	static ClassName* instance;		

#define SingletonCpp(ClassName)				\
ClassName* ClassName::instance = nullptr;	\
ClassName * ClassName::Get()				\
{											\
	assert(instance != nullptr);			\
	return instance;						\
}											\
void ClassName::Create()					\
{											\
	assert(instance == nullptr);			\
	instance = new ClassName();				\
}											\
void ClassName::Delete()					\
{											\
	SafeDelete(instance);					\
}			


#define ColorRed D3DXCOLOR(1.f,0.f,0.f,1.f)
#define ColorGreen D3DXCOLOR(0.f,1.f,0.f,1.f)
#define ColorBlue D3DXCOLOR(0.f,0.f,1.f,1.f)
#define ColorWhite D3DXCOLOR(1.f,1.f,1.f,1.f)
#define ColorGray(f) D3DXCOLOR(f,f,f,1.0f)

#define SaveJsonPath(n) { std::string("../_Scenes/Scene01/").append(n + ".json") }
#define SaveBinaryPath(n){std::string("../_Scenes/Scene01/").append(n + ".json") }
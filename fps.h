#ifndef __fpsH__
#define __fpsH__

#include "d3dfont.h"

class FPSCounter
{
public:
	FPSCounter(IDirect3DDevice9* device);
	~FPSCounter();

	bool render(D3DCOLOR color, float timeDelta);
private:
	IDirect3DDevice9* _device;
	
	CD3DFont* _font;
	DWORD     _frameCnt;
	float     _timeElapsed;
	float     _fps;
	char      _fpsString[9];
	
};
#endif // __fpsH__
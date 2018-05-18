/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __gameLogH__
#define __gameLogH__

#include "d3dfont.h"
#include <string>

const char _count = 10; // колво сохранённых ходов

class LOG
{
public:
	LOG ( IDirect3DDevice9* device );
	~LOG();
	void AddRecord( int posX, int posY, int pl );

	bool Render( D3DCOLOR color );
private:
	IDirect3DDevice9* _device;
	
	CD3DFont* _font;
	
	std::string _logString[_count];
	int _i;
	
};
#endif // __gameLogH__
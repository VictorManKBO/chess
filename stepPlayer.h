

#ifndef __stepPlayer__
#define __stepPlayer__

#include <d3dx9.h>
#include "myMethod.h"

class PLAY{
public:
	PLAY();
	VOID PLAY::Init();
	INT PLAY::GetPlayer( );
	VOID PLAY::ChangePlayer( );
	
private:
	INT _player;
};


#endif
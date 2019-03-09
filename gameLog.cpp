//////////////////////////////////////////////////////////////////////////////////////////////////

#include "gameLog.h"
#include "myMethod.h"
#include <cstdio>
#include <string.h>

LOG::LOG( IDirect3DDevice9* device )
{
	_device = device;
	
	_font = new CD3DFont("Times New Roman", 16, 0);
	_font->InitDeviceObjects( _device );
	_font->RestoreDeviceObjects();
	_i = _count -1;
}

LOG::~LOG()
{
	if( _font )
	{
		_font->InvalidateDeviceObjects();
		_font->DeleteDeviceObjects();
		delete _font;
	}
}

bool LOG::Render( D3DCOLOR color  )
{
	if( _font )
	{
		
		//if(_timeElapsed >= 1.0f)
		//{
		//	_fps = (float)_frameCnt / _timeElapsed;

		//	sprintf(_fpsString, "%f", _fps);
		//	_fpsString[8] = '\0'; // mark end of string

		//	_timeElapsed = 0.0f;
		//	_frameCnt    = 0;
		//}
		
		
		/*sprintf( _logString, "%s", "Player XXX go from " );
		char tmp[2];
		sprintf( tmp, "%c", ch ) ;
		strcat( _logString, tmp );
		sprintf( tmp, "%i", d ) ;
		strcat( _logString, tmp );
		strcat( _logString, "\0" );*/
		

		//_logString = _logString + tmp;
		//_logString += " in " +'f' + char( 5 + 48 ) + '\n'; 
		for( int i = _count - 1; ( i > _i ); i-- )
			_font->DrawText( 1000, 20 + 20*(_count - i - 1), color, _logString[i].c_str() );
		_font->DrawText( 1000, 20, color, "\u2193" );
	}
	return true;

	
}

void LOG::AddRecord( int posX, int posY, int pl ){
		char ch = 'a' + posY;
		char d = posX + 1;
		std::string player = ( pl == PLAYER1 )? "white" : "black";
		_logString[_i] = "Player " + player + " go from ";
		_logString[_i].push_back( ch );
		_logString[_i].push_back( d + 48 );
		if ( _i > 0 ) _i--;
		else for( int i = _count -1; i > 0; i-- )
			_logString[i] = _logString[i-1];
	}

#include "stepPlayer.h"

PLAY::PLAY(){
	_player = PLAYER1;
}

VOID PLAY::Init(){
	_player = PLAYER1;
}

INT PLAY::GetPlayer( ){
	return _player;
}

VOID PLAY::ChangePlayer( ){
	_player = ( _player == PLAYER1 )? PLAYER2 : PLAYER1;
}


#ifndef __stepFigureH__
#define __stepFigureH__

#include "myMethod.h"

namespace stp{
	static const int stepHorse[7][2] = {
		{2,1},
		{-2,1},
		{-2,-1},
		{1,2},
		{-1,2},
		{1,-2},
		{-1,-2}

	};
	static const int iHorse = 7;
	static const int jHorse = 2;

	static const int stepRook[4][2]={
		{1,0},
		{0,1},
		{-1,0},
		{0,-1}
	};
	static const int iRook = 4;
	static const int jRook = 2;

	static const int stepOficer[4][2]={
		{1,1},
		{-1,1},
		{1,-1},
		{-1,-1}
	};
	static const int iOficer = 4;
	static const int jOficer = 2;

	static const int stepKing[8][2]={
		//Шаг ладьи	+ Шаг офицера
		{1,0},
		{0,1},
		{-1,0},
		{0,-1},
		//
		{1,1},
		{-1,1},
		{1,-1},
		{-1,-1}
	};
	static const int iHing = 8;
	static const int jKing = 2;
};

class STEP{
public:
	STEP();
	STEP( GameField * gf );
	~STEP();
	FIGURE STEP::HitOnFigure( int i, int j );
	BOOL STEP::Go( FIGURE f );
	BOOL STEP::AllOk( );
	BOOL STEP::Rook();
	BOOL STEP::Horse();
	BOOL STEP::Oficer();
	BOOL STEP::King();
	BOOL STEP::Qeen();
	BOOL STEP::Pawn();
	VOID STEP::Move();
	FIGURE STEP::GetFigure();
	VOID STEP::ResetFigure();
	VOID STEP::Delete();
private:
	POINT _currentPos,_nextPos;
	FIGURE _figure;
	INT _player;
	BOOL _ok; // Флаг успешного окончания хода
	GameField * _gf;
};


#endif
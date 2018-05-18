
#include "stepFigure.h"
#include "myMethod.h"

//MB_ICONASTERISK	SystemAsterisk - звездочка.
//MB_ICONEXCLAMATION	SystemExclamation - восклицание.
//MB_ICONHAND	SystemHand - критическая ошибка.
//MB_ICONQUESTION	SystemQuestion - вопрос.
//MB_OK	SystemDefault - стандартный звук.

STEP::STEP(){
	_ok = false;
};

STEP::STEP( GameField * gf ){
	_gf = gf;
	_figure = EMPTY;
	_player = PLAYER1;
	_ok = false;
}
STEP::~STEP(){
};

FIGURE STEP::HitOnFigure( int i, int j ){
	
	_ok = false;
	FIGURE figure = _gf->GetFigure( i, j ); 
	if( figure != EMPTY ){ 
		_figure = figure;
		_currentPos.x = i; _currentPos.y = j;
		_player = _gf->GetPlayer( _currentPos.x, _currentPos.y );
	}
	else {
		_nextPos.x = i;
		_nextPos.y = j;
		if ( Go( _figure ) == false ) 
			::MessageBeep( MB_ICONHAND ) ;
		else _ok = true;
		ResetFigure();
	}
	return _figure;
}

BOOL STEP::AllOk( ){
	return _ok;
}

BOOL STEP::Go( FIGURE f ){
	bool flag = false ;

	switch ( f )
	{
	case LEFT_ROOK :
	
	case RIGHT_ROOK:
		flag = Rook();
		break;

	case LEFT_OFICER:
		
	case RIGHT_OFICER:
		flag = Oficer();
		break;

	case QUEEN:
		flag = Qeen();
		break;

	case KING:
		flag = King();
		break;

	case LEFT_HORSE:

	case RIGHT_HORSE:
		flag = Horse();
		break;

	case PAWN:
		flag = Pawn();
		break;

	default:
		flag = true;
		break;
	}
	if( ::GetAsyncKeyState( 'R' ) & 0x8000f ) flag = true;
	return flag;
}

BOOL STEP::Rook(){
	bool flag = false;

	if( (( _currentPos.x - _nextPos.x ) != 0 ) && ((_currentPos.y - _nextPos.y) != 0 ) )
		return false;

	bool x = true, y = true;
	
	int di, dj;
	if ( ( _nextPos.x - _currentPos.x ) >= 0 ){ 
		di = 1;
	}
	else {
		di = -1;
	}

	if ( ( _nextPos.y - _currentPos.y ) >= 0 ) dj = 1;
	else dj = -1;
	
	if( (( _currentPos.x - _nextPos.x ) != 0 ) && ((_currentPos.y - _nextPos.y) == 0 ) )
		if( di > 0 ){
			for( int i = min( _currentPos.x , _nextPos.x ) + di; i <= max( _currentPos.x , _nextPos.x ); i += di )
				if( _gf->GetFigure( i, _currentPos.y ) != EMPTY )  x = false;
		}
		else for( int i = max( _currentPos.x , _nextPos.x ) + di; i >= min( _currentPos.x , _nextPos.x ); i += di )
				if( _gf->GetFigure( i, _currentPos.y ) != EMPTY )  x = false;

	if( (( _currentPos.x - _nextPos.x ) == 0 ) && ((_currentPos.y - _nextPos.y) != 0 ) )
		if( dj > 0 ){ 
			for( int j = min( _currentPos.y , _nextPos.y ) + dj; j <= max( _currentPos.y , _nextPos.y ); j += dj )
				if( _gf->GetFigure( _currentPos.x, j ) != EMPTY ) y = false;
		}
		else for( int j = max( _currentPos.y , _nextPos.y ) + dj; j >= min( _currentPos.y , _nextPos.y ); j += dj )
				if( _gf->GetFigure( _currentPos.x, j ) != EMPTY ) y = false; 

	if( x && y ) {
		_gf->SetFigureInPos( _figure, _gf->GetPlayer( _currentPos.x, _currentPos.y ) , _nextPos.x, _nextPos.y );
		_gf->SetFigureInPos( EMPTY, EMPTY, _currentPos.x, _currentPos.y );	
		::MessageBeep( 70 );
		flag = true;
	}
	else flag = false;


	return flag;
}

BOOL STEP::Pawn(){
	bool flag = false;
	
	switch ( _player )
	{
	case PLAYER1:
		if ( _currentPos.x == PLAYER1 + 1 ) 
			if( (( _currentPos.x + 2 == _nextPos.x ) && ( _currentPos.y == _nextPos.y )) ) flag = true;
		if( (( _currentPos.x + 1 == _nextPos.x ) && ( _currentPos.y == _nextPos.y )) ) flag = true;
		break;

	case PLAYER2:
		if ( _currentPos.x == PLAYER2 - 1 ) 
			if ( (( _currentPos.x - 2 == _nextPos.x ) && ( _currentPos.y == _nextPos.y )) ) flag = true;
		if ( (( _currentPos.x - 1 == _nextPos.x ) && ( _currentPos.y == _nextPos.y )) ) flag = true;
		break;

	default:
		flag = false;
		break;
	}
	if( flag )
	{
		//::MessageBox(0, "Pawn" ,0,0) ;
		::MessageBeep( 70 );	
		flag = true;
		_gf->SetFigureInPos( _figure, _gf->GetPlayer( _currentPos.x, _currentPos.y ) , _nextPos.x, _nextPos.y );
		_gf->SetFigureInPos( EMPTY, EMPTY, _currentPos.x, _currentPos.y );	
	}
	else flag = false;
	return flag;
}

BOOL STEP::King(){
	bool flag = false;
	if( (( _currentPos.x + 1 == _nextPos.x ) && ( _currentPos.y == _nextPos.y ))
		|| (( _currentPos.x - 1 == _nextPos.x ) && ( _currentPos.y == _nextPos.y )) 
		|| (( _currentPos.y + 1 == _nextPos.y ) && ( _currentPos.x == _nextPos.x ))
		|| (( _currentPos.y - 1 == _nextPos.y ) && ( _currentPos.x == _nextPos.x ))
		
		|| (( _currentPos.x + 1 == _nextPos.x ) && ( _currentPos.y + 1 == _nextPos.y ))
		|| (( _currentPos.x + 1 == _nextPos.x ) && ( _currentPos.y - 1 == _nextPos.y ))
		|| (( _currentPos.x - 1 == _nextPos.x ) && ( _currentPos.y + 1 == _nextPos.y ))
		|| (( _currentPos.x - 1 == _nextPos.x ) && ( _currentPos.y - 1 == _nextPos.y ))  )
	{
		flag = true;
		::MessageBeep( 70 );
		_gf->SetFigureInPos( _figure, _gf->GetPlayer( _currentPos.x, _currentPos.y ) , _nextPos.x, _nextPos.y );
		_gf->SetFigureInPos( EMPTY, EMPTY, _currentPos.x, _currentPos.y );	
	}
	else flag = false;
	return flag;
}

BOOL STEP::Oficer(){
	bool flag = false;

	if( ( _currentPos.x + _currentPos.y ) == ( _nextPos.x + _nextPos.y ) ) flag = true;
	else{
		if( ( _currentPos.x - _nextPos.x ) == ( _currentPos.y - _nextPos.y ) ) flag = true;
	}
	
	int dx = 1,dy = 1;
	if ( _currentPos.x - _nextPos.x > 0 ) dx = -1;// 7,6,5,4.... иначе 1
	if ( _currentPos.y - _nextPos.y > 0 ) dy = -1;// 7,6,5,4.... иначе 1

	int i = _currentPos.x;  
	int j = _currentPos.y;
	
	while( ( min( i, _nextPos.x) < max( i, _nextPos.x) )&&( min( j, _nextPos.y) < max( j, _nextPos.y) ) ){
		i+= dx ;
		j+= dy ;
		if(  _gf->GetFigure( i, j )!= EMPTY ) flag = false;		
	}

	if( flag ) {
		_gf->SetFigureInPos( _figure, _gf->GetPlayer( _currentPos.x, _currentPos.y ) , _nextPos.x, _nextPos.y );
		_gf->SetFigureInPos( EMPTY, EMPTY, _currentPos.x, _currentPos.y );	
		::MessageBeep( 0 );
	}
	else flag = false;

	return flag;
}

BOOL STEP::Qeen(){
	return Oficer() || Rook();
}

BOOL STEP::Horse(){
	BOOL flag = false;

	if( ( abs( _nextPos.x - _currentPos.x ) + abs( _nextPos.y - _currentPos.y ) == 3 ) &&
		( ( _nextPos.x != _currentPos.x ) && (_nextPos.y != _currentPos.y) ) &&
		( _gf->GetPlayer( _nextPos.x, _nextPos.y ) == EMPTY ) )
	{
		_gf->SetFigureInPos( _figure, _gf->GetPlayer( _currentPos.x, _currentPos.y ) , _nextPos.x, _nextPos.y );
		_gf->SetFigureInPos( EMPTY, EMPTY, _currentPos.x, _currentPos.y );	
		::MessageBeep( 200 );

		flag = true;
	}
	else flag = false;

	return flag;
}

FIGURE STEP::GetFigure(){
	return _figure;
}

VOID STEP::ResetFigure(){
	_figure = EMPTY;
}

VOID STEP::Delete(){
	_gf = NULL;
}
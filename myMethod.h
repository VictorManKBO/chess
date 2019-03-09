//
//
//

#ifndef __myMethodH__
#define __myMethodH__

#include <d3dx9.h>
#include <vector>
#include <fstream>

namespace myUtil{

	//Загрузка ".Х" файла и извлечение из него информации о материале и текстурах
	VOID LoadXfile(char* nameXfile, IDirect3DDevice9* device, ID3DXMesh **outMesh, 
		std::vector<D3DMATERIAL9>* outMtrl, std::vector<IDirect3DTexture9*>* outTextures);
	//Прорисовка сетки, наложение материала и текстур
	VOID DrawObject(IDirect3DDevice9* device, ID3DXMesh **mesh, 
		std::vector<D3DMATERIAL9>* matrl, std::vector<IDirect3DTexture9*>* textures, D3DXVECTOR3 * pos, float angleRotate);
	//Считать имена из афйла
	char* ReadNameFileX();
	//Распечатать цвет
	void PrintColor( D3DCOLORVALUE *col, std::ofstream *out );
	void PrintColor( float *a, float *r, float *g, float *b, std::ofstream *out );
	
}

enum {LOW = 1, MEDIUM = 5, FAST = 10};
struct SpeedMouse{	
	SpeedMouse();
	SpeedMouse(int speed){
		_speed = speed;
	}
	int _speed;
};

enum FIGURE {LEFT_ROOK = 0, 
			LEFT_HORSE = 1, 
			LEFT_OFICER = 2, 
			QUEEN = 3, 
			KING = 4 , 
			RIGHT_OFICER = 5, 
			RIGHT_HORSE = 6, 
			RIGHT_ROOK = 7 , 
			PAWN = 8 , 
			EMPTY = 9 }; 

enum {PLAYER1 = 0, PLAYER2 = 7};

class GameField{
private:
	static const int _COUNT = 8;
	static const int _MAXPOSX = 184;//идеал
	static const int _MAXPOSZ = 183;//поправка на кривизну доски
	//|-!-|-!-|-!-|-!-|-!-|-!-|-!-|-!-|
	//-186            0				186
	static const int _SHIFTX = ( _MAXPOSX * 2 ) / 7;//идеал
	static const int _SHIFTZ = ( _MAXPOSZ + 188 ) / 7;//поправка на кривизну доски
	struct Field{
		int _inpos;
		D3DXVECTOR3 _pos;
		int _player;
	} field[8][8];

	
	
public:	
	struct ChessObject{
		ID3DXMesh *mesh;
		std::vector<D3DMATERIAL9> mtrl;
		std::vector<IDirect3DTexture9*> textures;
	} figure[9];

	GameField();
	~GameField();
	VOID GameField::InitField();
	VOID GameField::DrawFigureOnDesk( IDirect3DDevice9 * device );
	VOID GameField::LoadFigure( IDirect3DDevice9 * device );
	FIGURE GameField::GetFigure( int i, int j);
	INT GameField::GetPlayer( int i, int j );
	VOID GameField::SetFigureInPos( FIGURE figure, int player, int x, int y );
	VOID GameField::Delete();
	D3DXVECTOR3 GameField::GetPosOfField(int i, int j);
};

class ARROW{
public:
	ARROW();
	~ARROW();
	VOID ARROW::Init(GameField *gf, IDirect3DDevice9 *device);
	VOID ARROW::Show();
	VOID ARROW::Hide();
	VOID ARROW::Draw( IDirect3DDevice9 *device );
	VOID ARROW::Move( GameField *gf, int x, int y);
	VOID ARROW::ChangeColor();
	VOID ARROW::GetPos( int *x, int *y );
	VOID ARROW::Delete();
private:
	enum {GREEN_MTRL = 0 , RED_MTRL = 1 };
	D3DXVECTOR3 _pos;
	int i, j;
	BOOL show;
	char _mtrl;

	ID3DXMesh *mesh;
	std::vector<D3DMATERIAL9> mtrl;
};

#endif
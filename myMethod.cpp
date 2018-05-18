//
//
//

#include "myMethod.h"
#include "d3dUtility.h"


VOID myUtil::LoadXfile( char* nameXfile, IDirect3DDevice9* device, ID3DXMesh** outMesh, 
		std::vector<D3DMATERIAL9>* outMtrl, std::vector<IDirect3DTexture9*>* outTextures ){
	//Загрузка сетки
	
	HRESULT hr = NULL;
	ID3DXBuffer* adjBuffer = NULL;
	ID3DXBuffer* mtrljBuffer = NULL;
	DWORD NumMtrls = NULL;

	hr = D3DXLoadMeshFromX( nameXfile, D3DXMESH_MANAGED, device, &adjBuffer, &mtrljBuffer, NULL, &NumMtrls, &*outMesh ); 
	if( FAILED(hr) )	{
		::MessageBox( 0, "D3DMeshFromFileX() - Failed", 0, 0 );
	}

	//Извлечение материала и загрузка текстур
	if (mtrljBuffer !=NULL && NumMtrls != NULL){
		D3DXMATERIAL * mtrls = ( D3DXMATERIAL *)mtrljBuffer->GetBufferPointer();
		for( int i = 0; i < NumMtrls; i++ ){
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;
			//Сохранение материала
			if ( outMtrl != NULL )
				outMtrl->push_back( mtrls[i].MatD3D );
			//Проверяем, связана ли с i-тым материалом текстура
			if( mtrls[i].pTextureFilename !=NULL ){
				IDirect3DTexture9* tex = NULL;
				D3DXCreateTextureFromFile( device, mtrls[i].pTextureFilename, &tex );
				//Соxраняем текстуру, если пользователь разрешил
				if(outTextures != NULL )
					outTextures->push_back( tex );
			}
			else if(outTextures != NULL )
					outTextures->push_back( NULL );

		}
	}
	//Уничтожаем неиспользуемые переменные
	d3d::Release<ID3DXBuffer*> ( mtrljBuffer );
	d3d::Release<ID3DXBuffer*> ( adjBuffer );
}

VOID myUtil::DrawObject( IDirect3DDevice9* device, ID3DXMesh **mesh, 
		std::vector<D3DMATERIAL9>* mtrl, std::vector<IDirect3DTexture9*>* textures, D3DXVECTOR3 * pos , float angleRotate ){
	//Установка позиции, в которой будет нарисован объект
	D3DXMATRIX w;
	D3DXMatrixTranslation( &w,
				pos->x, pos->y, pos->z );
	D3DXMATRIX yRot;
	D3DXMatrixRotationY( &yRot, angleRotate );
	D3DXMatrixMultiply( &w, &yRot, &w );

	device->SetTransform( D3DTS_WORLD, &w );
	//Прорисовка объекта
	for( int k = 0; k < mtrl->size(); k++ ){
				device->SetMaterial( &mtrl->at(k) );
				if ( textures != NULL )
					device->SetTexture( NULL, textures->at(k) );
				( *mesh )->DrawSubset( k );
			}
}

char* myUtil::ReadNameFileX(){
	return NULL; 
}

void myUtil::PrintColor( D3DCOLORVALUE *col, std::ofstream *out ){
	
	*out << "	Alpha=" << col->a<< std::endl;
	*out << "	Red=" << col->r<< std::endl;
	*out << "	Green=" << col->g<< std::endl;
	*out << "	Blue=" << col->b<< std::endl;
	*out << "-------------------------------------------" <<std::endl;
	
}

void myUtil::PrintColor( float *a, float *r, float *g, float *b, std::ofstream *out ){
	
	*out << "	Alpha=" << *a<< std::endl;
	*out << "	Red=" << *r<< std::endl;
	*out << "	Green=" << *g<< std::endl;
	*out << "	Blue=" << *b<< std::endl;
	*out << "-------------------------------------------" <<std::endl;
	
}


GameField::GameField(){

}

GameField::~GameField(){

}

VOID GameField::InitField(){
	int posZ = - _MAXPOSZ;
	for( int i = 0; i < _COUNT; i++ ){
		int posX = - _MAXPOSX;
		for( int j = 0; j < _COUNT; j++ ){
			field[i][j]._inpos = EMPTY;
			field[i][j]._player = EMPTY;			
			
			field[i][j]._pos.x = posX ;
			field[i][j]._pos.y = 0 ;
			field[i][j]._pos.z = posZ ;

			posX += _SHIFTX ; 
		}
		posZ += _SHIFTZ ;
	}
		
	//расстановка фигур первого игрока
	for( int j = 0; j < _COUNT; j++ ){ field[0][j]._inpos = FIGURE( j ); field[0][j]._player = PLAYER1; }
	for( int j = 0; j < _COUNT; j++ ){ field[1][j]._inpos = FIGURE( PAWN ); field[1][j]._player = PLAYER1; }
	//расстановка фигур второго игрока
	for( int j = 0; j < _COUNT; j++ ){ field[7][j]._inpos = FIGURE( j );  field[7][j]._player = PLAYER2; }
	for( int j = 0; j < _COUNT; j++ ){ field[6][j]._inpos = FIGURE( PAWN ); field[6][j]._player = PLAYER2; }
}


VOID GameField::DrawFigureOnDesk( IDirect3DDevice9 * device ){
	for( int i = 0; i < _COUNT; i++ )
		for( int j = 0; j < _COUNT; j++ ){
			float  angleRot = 0;
			std::vector<D3DMATERIAL9> figureMtrl( figure[ field[i][j]._inpos ].mtrl.size() );
			if ( field[i][j]._player == PLAYER1 ){ 
				angleRot = 0;
				for( int k = 0; k < figureMtrl.size(); k++ )
					figureMtrl[k] = d3d::WHITE_MTRL;
			}
			else if ( field[i][j]._player == PLAYER2 ){ 
				angleRot = D3DX_PI;
				for( int k = 0; k < figureMtrl.size(); k++ )
					figureMtrl[k] = d3d::BLACK_MTRL;
			}
			
			myUtil::DrawObject( device, &figure[ field[i][j]._inpos ].mesh, 
				&figureMtrl, &figure[ field[i][j]._inpos ].textures, &field[i][j]._pos, angleRot );
		}
		
}

VOID GameField::LoadFigure( IDirect3DDevice9 * device ){
	
		myUtil::LoadXfile( "king.x", device, &figure[ KING ].mesh, &figure[ KING ].mtrl, &figure[ KING ].textures );
		myUtil::LoadXfile( "queen.x", device, &figure[ QUEEN ].mesh, &figure[ QUEEN ].mtrl, &figure[ QUEEN ].textures );
		myUtil::LoadXfile( "rook.x", device, &figure[ LEFT_ROOK ].mesh, &figure[ LEFT_ROOK ].mtrl, &figure[ LEFT_ROOK ].textures );
		myUtil::LoadXfile( "rook.x", device, &figure[ RIGHT_ROOK ].mesh, &figure[ RIGHT_ROOK ].mtrl, &figure[ RIGHT_ROOK ].textures );
		myUtil::LoadXfile( "oficer.x", device, &figure[ LEFT_OFICER ].mesh, &figure[ LEFT_OFICER ].mtrl, &figure[ LEFT_OFICER ].textures );
		myUtil::LoadXfile( "oficer.x", device, &figure[ RIGHT_OFICER ].mesh, &figure[ RIGHT_OFICER ].mtrl, &figure[ RIGHT_OFICER ].textures );
		myUtil::LoadXfile( "horse.x", device, &figure[ LEFT_HORSE ].mesh, &figure[ LEFT_HORSE ].mtrl, &figure[ LEFT_HORSE ].textures );
		myUtil::LoadXfile( "horse.x", device, &figure[ RIGHT_HORSE ].mesh, &figure[ RIGHT_HORSE ].mtrl, &figure[ RIGHT_HORSE ].textures );
		myUtil::LoadXfile( "pawn.x", device, &figure[ PAWN ].mesh, &figure[ PAWN ].mtrl, &figure[ PAWN ].textures );
		
}

FIGURE GameField::GetFigure( int i, int j ){
	return (FIGURE) field[i][j]._inpos;
}

INT GameField::GetPlayer( int i, int j ){
	return  field[i][j]._player;
}

VOID GameField::SetFigureInPos( FIGURE figure, int player, int x, int y ){
	field[x][y]._inpos = figure;
	field[x][y]._player = player;
}

VOID GameField::Delete()
{
	for( int i = 0; i < _COUNT; i++ ){
		d3d::Release<ID3DXMesh*> ( figure[i].mesh );
		for(int j = 0; j < figure[i].mtrl.size(); j++)
			d3d::Release<IDirect3DTexture9*>( figure[i].textures[j] );
		figure[i].mtrl.resize( NULL );
	}
}

D3DXVECTOR3 GameField::GetPosOfField( int i, int j ){
	return field[i][j]._pos;
}

//Arrow
ARROW::ARROW(){

}
ARROW::~ARROW(){

}
VOID ARROW::Init( GameField *gf, IDirect3DDevice9 *device ){
	myUtil::LoadXfile("arrow.x", device, &mesh, NULL, NULL);
	mtrl.push_back( d3d::GREEN_MTRL );
	i = PLAYER1;
	j = KING;
	_pos = gf->GetPosOfField( i, j ) ;
	_pos.y = 120;
}

VOID ARROW::Show(){
	show = true; 
}

VOID ARROW::Hide(){
	show = false;
}

VOID ARROW::Draw( IDirect3DDevice9 *device ){
	if (show) myUtil::DrawObject( device, &mesh, &mtrl, NULL, &_pos, 0.0f ) ;
}

VOID ARROW::Move( GameField *gf, int x, int y ){
	if(  ( j + x < 8 ) && ( j + x >= 0 ) &&
		( i + y < 8 )&&( i + y >= 0 ) )
	{_pos = gf->GetPosOfField( i+y, j+x );_pos.y = 120; j += x; i+= y;}
}

VOID ARROW::ChangeColor(){
	
	if( _mtrl == GREEN_MTRL ){
		for( int i = 0; i < mtrl.size(); i++ ){
			mtrl[ i ] = d3d::RED_MTRL;
		}
		_mtrl = RED_MTRL;
	}
	else
		if( _mtrl == RED_MTRL ){
			for( int i = 0; i < mtrl.size(); i++ ){
				mtrl[ i ] = d3d::GREEN_MTRL;
			}
			_mtrl = GREEN_MTRL;
		}
	
}

VOID ARROW::GetPos( int *x, int *y ){
	*x = i;
	*y = j;
}

VOID ARROW::Delete(){
	d3d::Release<ID3DXMesh*> ( mesh );
	mtrl.resize( NULL );
};
//Конец класса Arrow


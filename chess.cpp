//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: chess.cpp
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 610m, Windows 7 Ultra, MSVC++ 12.0 
//
// Desc: The main program file.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "myMethod.h"
#include <fstream>
#include "camera.h"
#include "fps.h"
#include "gameLog.h"
#include "stepFigure.h"
#include "stepPlayer.h"

#include <string>
#include <iostream>


//
// Globals
//

IDirect3DDevice9* Device = NULL; 

const int Width  = 1366;
const int Height = 768;

Camera TheCamera( Camera::AIRCRAFT );

FPSCounter* FPS = NULL;
LOG * Log = NULL;

ID3DXMesh* Teapot = NULL;
ID3DXMesh* Sphere = NULL;

//Шахматная доска
ID3DXMesh * desk = NULL;
std::vector<D3DMATERIAL9> deskMtrl( NULL );
std::vector<IDirect3DTexture9*> deskTextures( NULL );
D3DXVECTOR3 positionDesk;
//Король 
ID3DXMesh * king = NULL;
std::vector<D3DMATERIAL9> kingMtrl( NULL );
std::vector<IDirect3DTexture9*> kingTextures( NULL );
D3DXVECTOR3 positionKing;

GameField Field;
STEP Step( &Field );

ARROW Arrow;
PLAY Play;
INT player = PLAYER1;

d3d::Ray ray;
POINT mousePos,oldMousePos;
SpeedMouse speedMouse( LOW );

std::ofstream out( "position.txt" );

D3DXMATRIX World;
d3d::BoundingSphere BSphere;


//
// Functions
//
d3d::Ray CalcPickingRay( int x, int y )
{
	float px = 0.0f;
	float py = 0.0f;

	D3DVIEWPORT9 vp;
	Device->GetViewport( &vp );

	D3DXMATRIX proj;
	Device->GetTransform( D3DTS_PROJECTION, &proj );

	px = (((  2.0f*x) / vp.Width)  - 1.0f ) / proj( 0, 0 );
	py = ((( -2.0f*y) / vp.Height) + 1.0f ) / proj( 1, 1 );

	d3d::Ray ray;
	ray._origin    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	ray._direction = D3DXVECTOR3( px, py, 1.0f );

	return ray;
}

void TransformRay( d3d::Ray* ray, D3DXMATRIX* T )
{
	// transform the ray's origin, w = 1.
	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		T);

	// transform the ray's direction, w = 0.
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		T);

	// normalize the direction
	D3DXVec3Normalize( &ray->_direction, &ray->_direction );
}

bool RaySphereIntTest( d3d::Ray* ray, d3d::BoundingSphere* sphere )
{
	D3DXVECTOR3 v = ray->_origin - sphere->_center;

	float b = 2.0f * D3DXVec3Dot( &ray->_direction, &v );
	float c = D3DXVec3Dot( &v, &v ) - ( sphere->_radius * sphere->_radius );

	// find the discriminant
	float discriminant = ( b * b ) - ( 4.0f * c);

	// test for imaginary number
	if( discriminant < 0.0f )
		return false;

	discriminant = sqrtf( discriminant );

	float s0 = ( -b + discriminant ) / 2.0f;
	float s1 = ( -b - discriminant ) / 2.0f;

	// if a solution is >= 0, then we intersected the sphere
	if( s0 >= 0.0f || s1 >= 0.0f )
		return true;
	
	return false;
}

int sign( float a ){
	if( a > 0 ) return 1;
	else if( a < 0 ) return -1;
	else return 0;
}

void GetColor( float *a, float *r, float *g, float *b, D3DCOLORVALUE *col ){
	*a = col->a;
	*r = col->r;
	*g = col->g;
	*b = col->b;
}



void WaitUnpressKey(char a){
	do{
	}
	while ( ::GetAsyncKeyState( a ) & 0x8000f );
}
//
// Framework functions
//
bool Setup()
{

	FPS = new FPSCounter( Device );
	Log = new LOG ( Device );
	Field.InitField();
	Arrow.Init( &Field, Device );
	Play.Init();

	//Загрузка X - объекта
	myUtil::LoadXfile( "desk.x", Device, &desk , &deskMtrl, &deskTextures );
	myUtil::LoadXfile( "arrow.x", Device, &king , &kingMtrl, &kingTextures );
	Field.LoadFigure( Device );
 	//
	//
	// Compute the bounding sphere.
	//

	BYTE* v = 0;
	king->LockVertexBuffer(0, (void**)&v);

	D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		king->GetNumVertices(),
		D3DXGetFVFVertexSize(king->GetFVF()),
		&BSphere._center,
		&BSphere._radius);

	king->UnlockVertexBuffer();
	
	//
	// Build a sphere mesh that describes the teapot's bounding sphere.
	//

	D3DXCreateSphere( Device, BSphere._radius, 20, 20, &Sphere, 0 );

	//
	// Set light.
	//

	D3DXVECTOR3 dir( 0.707f, -0.0f, 0.707f );
	D3DXCOLOR col( 1.0f, 1.0f, 1.0f, 1.0f );
	D3DLIGHT9 light = d3d::InitDirectionalLight( &dir, &col );

	Device->SetLight( 0, &light );
	Device->LightEnable( 0, true );
	Device->SetRenderState( D3DRS_NORMALIZENORMALS, true );
	Device->SetRenderState( D3DRS_SPECULARENABLE, false );	

	//
	// Set view matrix.
	//

	D3DXVECTOR3 pos( 0.0f, 250.0f, -500.0f );
	
	TheCamera.setPosition( &pos );
	TheCamera.pitch( 30 * D3DX_PI / 180 );

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform( D3DTS_PROJECTION, &proj );

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>( Teapot );
	d3d::Release<ID3DXMesh*>( Sphere );
	//Удаление доски
	d3d::Release<ID3DXMesh*>( desk );
	for(int i = 0; i < deskTextures.size(); i++)
		d3d::Release<IDirect3DTexture9*>( deskTextures[i] );
	deskMtrl.resize( NULL );
	//Удаление короля
	d3d::Release<ID3DXMesh*>( king );
	for(int i = 0; i < kingTextures.size(); i++)
		d3d::Release<IDirect3DTexture9*>( kingTextures[i] );
	kingMtrl.resize( NULL );
	d3d::Delete<FPSCounter*> (FPS );
	d3d::Delete<LOG*> (Log);
	Field.Delete();
	Arrow.Delete();
	Step.Delete();
	out.close();
}

bool Display( float timeDelta )
{
	if( Device )
	{

		//
		// Update: Update the camera.
		//

		if( ::GetAsyncKeyState( 'W' ) & 0x8000f ){
			TheCamera.walk(100.0f * timeDelta);
			
		}

		if( ::GetAsyncKeyState( 'S' ) & 0x8000f ){
			TheCamera.walk(-100.0f * timeDelta);
			
		}

		if( ::GetAsyncKeyState( 'A' ) & 0x8000f ){
			TheCamera.strafe(-100.0f * timeDelta);
			
		}

		if( ::GetAsyncKeyState( 'D' ) & 0x8000f ){
			TheCamera.strafe(100.0f * timeDelta);
			
		}


		if( ::GetAsyncKeyState( 'R' ) & 0x8000f )
			TheCamera.fly(100.0f * timeDelta);

		if( ::GetAsyncKeyState( 'F' ) & 0x8000f )
			TheCamera.fly(-100.0f * timeDelta);

		if( ::GetAsyncKeyState( VK_UP ) & 0x8000f ){
			
			Arrow.Move( &Field, 0 , 1 );
			WaitUnpressKey( VK_UP );
		}

		if( ::GetAsyncKeyState( VK_DOWN ) & 0x8000f ){
			
			Arrow.Move( &Field, 0 , -1 );
			WaitUnpressKey( VK_DOWN );
		}

		if( ::GetAsyncKeyState( VK_LEFT ) & 0x8000f ){
			
			Arrow.Move( &Field, -1 , 0 );
			WaitUnpressKey( VK_LEFT );
		}
			
		if( ::GetAsyncKeyState( VK_RIGHT ) & 0x8000f ){
			
			Arrow.Move( &Field, 1 , 0 );
			WaitUnpressKey( VK_RIGHT );
		}

		if( ::GetAsyncKeyState( 'N' ) & 0x8000f )
			TheCamera.roll(1.0f * timeDelta);

		if( ::GetAsyncKeyState( 'M' ) & 0x8000f )
			TheCamera.roll(-1.0f * timeDelta);

		if( ::GetAsyncKeyState( VK_RBUTTON ) & 0x8000f ){
			GetCursorPos( &mousePos );
			
			int dx = ( mousePos.x - oldMousePos.x) ;
			int dy = ( mousePos.y - oldMousePos.y );
			
			TheCamera.yaw( sign(dx) * speedMouse._speed * timeDelta );
			TheCamera.pitch( sign(dy) * speedMouse._speed * timeDelta );
		
			oldMousePos=mousePos;
		}
		if( ::GetAsyncKeyState( VK_LBUTTON ) & 0x8000f ){
			POINT coord;
			GetCursorPos(&coord);
			positionKing.x=coord.x-Width/2 ;
			positionKing.z=Height/2-coord.y;
		}
		if( ::GetAsyncKeyState( 'C' ) & 0x8000f ){
			std::ofstream out( "color.txt" );

			out << "Ambient:" <<std::endl;
			myUtil::PrintColor( &kingMtrl[0].Ambient, &out);
		
			out << "Diffuse:" <<std::endl;
			myUtil::PrintColor( &kingMtrl[0].Diffuse, &out);
			
			out << "Emissive:" <<std::endl;
			myUtil::PrintColor( &kingMtrl[0].Emissive, &out);
			
			out << "Specular:" <<std::endl;
			myUtil::PrintColor( &kingMtrl[0].Specular, &out);
			
			float power=kingMtrl[0].Power;
			out << "Power="<<power<<std::endl;

			out.close();

			Sleep(10);
		}

		if( ::GetAsyncKeyState( VK_SPACE ) & 0x8000f ){
			FIGURE figure = Step.GetFigure();
			
			int x,y;
			Arrow.GetPos( &x, &y );
			if ( ( Field.GetPlayer( x, y ) == Play.GetPlayer() ) ||
				( Field.GetPlayer( x, y ) == EMPTY ) ) {
				if ( Step.HitOnFigure( x, y ) != EMPTY ) ;
				else if ( figure == EMPTY ) Arrow.ChangeColor();
					else if ( Step.AllOk() ) {
							Log->AddRecord( x, y, Play.GetPlayer() );
							Play.ChangePlayer();
					}
				Arrow.ChangeColor();
			}
			WaitUnpressKey( VK_SPACE );
			
		}

		if( ::GetAsyncKeyState( 'I' ) & 0x8000f ){
			
		}

		// Update the view matrix representing the cameras 
     
		
		D3DXVECTOR3 pos;
		if ( player != Play.GetPlayer() ) { 
			if( player == PLAYER1 ){
				pos = D3DXVECTOR3 ( 0.0f, 250.0f, 500.0f );
				
			}
			else {
				pos = D3DXVECTOR3 ( 0.0f, 250.0f, -500.0f );
				
			}
	
			player = Play.GetPlayer();
		}
		
	

		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);


		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, d3d::WHITE, 1.0f, 0);
		Device->BeginScene();

		
		Field.DrawFigureOnDesk( Device );
		Arrow.Show();
		Arrow.Draw( Device );
		D3DXMATRIX W;
		
		
		positionDesk.x=0.0f;
		positionDesk.y=0.0f;
		positionDesk.z=0.0f;
		D3DXMatrixTranslation( &W,
			positionDesk.x,
			positionDesk.y,
			positionDesk.z );

		Device->SetTransform( D3DTS_WORLD, &W );
		for( int i = 0; i < deskMtrl.size(); i++ ){
			Device->SetMaterial( &deskMtrl[i] );
			Device->SetTexture( NULL, deskTextures[i] );
			desk->DrawSubset( i );
		}

		//КОординаты короля
		positionKing.x=0.0f;
		positionKing.y=0.0f;
		positionKing.z=0.0f;
		D3DXMatrixTranslation(&W,
			positionKing.x,
			positionKing.y,
			positionKing.z);

		//Установка мировой матрицы на координаты короля
		Device->SetTransform(D3DTS_WORLD, &W );

		//Прорисовка короля
		for( int i = 0; i < kingMtrl.size(); i++){
			Device->SetMaterial( &kingMtrl[i] );
			Device->SetTexture( NULL, kingTextures[i] );
			king->DrawSubset( i );
		}

		if( FPS )
			FPS->render( 0xffFF4800, timeDelta );
		if( Log )
				Log->Render( 0xff004400 );
	
		
		//Установка мировой матрицы на координаты сферы короля
		D3DXMatrixTranslation(&W, BSphere._center.x, BSphere._center.y, BSphere._center.z );
		Device->SetTransform(D3DTS_WORLD, &W );		
		
		//Включение смешивания
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		
		//Установа материала и прорисовка сферы
		D3DMATERIAL9 blue = d3d::BLUE_MTRL;
		blue.Diffuse.a = 0.25f; // 25% opacity
		Device->SetMaterial(&blue);
		Sphere->DrawSubset(0);

		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);

		break;
	
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}

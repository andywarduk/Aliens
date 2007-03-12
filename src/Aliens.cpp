#include <windows.h>
#include <SDL.h>
#include <SDL_thread.h>

#define MSPERFRAME 60
#define FRAMESPERSPRITE 10

#define SPRITESRCX 13
#define SPRITESRCY 10
#define SPRITESCALE 25

#define SPRITEX (SPRITESRCX*SPRITESCALE)
#define SPRITEY (SPRITESRCY*SPRITESCALE)

// .............
// ...#.....#...
// ....#...#....
// ...#######...
// ..##.###.##..
// .###########.
// .###########.
// .#.#.....#.#.
// ....##.##....
// .............

// .............
// ...#.....#...
// .#..#...#..#.
// .#.#######.#.
// .###.###.###.
// .###########.
// ..#########..
// ...#.....#...
// ..#.......#..
// .............

int Sprite1Src[SPRITESRCY][SPRITESRCX]={
	{0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,0,0,0,0,0,1,0,0,0},
	{0,0,0,0,1,0,0,0,1,0,0,0,0},
	{0,0,0,1,1,1,1,1,1,1,0,0,0},
	{0,0,1,1,0,1,1,1,0,1,1,0,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,0,1,0,0,0,0,0,1,0,1,0},
	{0,0,0,0,1,1,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int Sprite2Src[SPRITESRCY][SPRITESRCX]={
	{0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,0,0,0,0,0,1,0,0,0},
	{0,1,0,0,1,0,0,0,1,0,0,1,0},
	{0,1,0,1,1,1,1,1,1,1,0,1,0},
	{0,1,1,1,0,1,1,1,0,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,0,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,0,1,0,0,0,0,0,1,0,0,0},
	{0,0,1,0,0,0,0,0,0,0,1,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int Sprite1[SPRITEY][SPRITEX];
int Sprite2[SPRITEY][SPRITEX];

int ScreenX=0;
int ScreenY=0;
SDL_Surface *Screen=NULL;
SDL_mutex *ScreenMutex=NULL;
int *XSin=NULL;
int *XCos=NULL;
bool ScreenInitialised=false;

void ExplodeSprites()
{
	int x1,y1,x2,y2,dx,dy;

	dy=0;
	for(y1=0;y1<SPRITESRCY;y1++){
		for(y2=0;y2<SPRITESCALE;y2++){
			dx=0;
			for(x1=0;x1<SPRITESRCX;x1++){
				for(x2=0;x2<SPRITESCALE;x2++){
					Sprite1[dy][dx]=Sprite1Src[y1][x1];
					++dx;
				}
			}
			++dy;
		}
	}

	dy=0;
	for(y1=0;y1<SPRITESRCY;y1++){
		for(y2=0;y2<SPRITESCALE;y2++){
			dx=0;
			for(x1=0;x1<SPRITESRCX;x1++){
				for(x2=0;x2<SPRITESCALE;x2++){
					Sprite2[dy][dx]=Sprite2Src[y1][x1];
					++dx;
				}
			}
			++dy;
		}
	}
}

void DestroyScreen()
{
	if(ScreenInitialised){
		delete XCos;
		delete XSin;

		ScreenInitialised=false;
	}
}

bool InitialiseScreen(int NewX,int NewY)
{
	bool Ok=false;

	DestroyScreen();

	if(SDL_LockMutex(ScreenMutex)==0){
		Screen = SDL_SetVideoMode(NewX,NewY,0,SDL_RESIZABLE|SDL_HWSURFACE);
//		Screen = SDL_SetVideoMode(NewX,NewY,0,SDL_FULLSCREEN);
		if(Screen){
			Ok=true;
			ScreenX=NewX;
			ScreenY=NewY;
			XCos=new int[ScreenX];
			XSin=new int[ScreenX];
			ScreenInitialised=true;
		}
		else{
			ScreenX=0;
			ScreenY=0;
		}
		SDL_UnlockMutex(ScreenMutex);
	}

	return Ok;
}

signed int SinTab[]={
	0,6,13,20,27,34,41,48,55,61,68,74,81,87,93,100,105,111,117,123,128,133,138,143,148,153,157,161,165,169,173,176,179,182,185,187,190,192,194,195,196,198,198,199,199,200,199,199,198,198,196,195,194,192,190,187,185,182,179,176,173,169,165,161,157,153,148,143,138,133,128,123,117,111,105,99,93,87,81,74,68,61,55,48,41,34,27,20,13,6,
	0,-6,-13,-20,-27,-34,-41,-48,-55,-61,-68,-74,-81,-87,-93,-100,-105,-111,-117,-123,-128,-133,-138,-143,-148,-153,-157,-161,-165,-169,-173,-176,-179,-182,-185,-187,-190,-192,-194,-195,-196,-198,-198,-199,-199,-200,-199,-199,-198,-198,-196,-195,-194,-192,-190,-187,-185,-182,-179,-176,-173,-169,-165,-161,-157,-153,-148,-143,-138,-133,-128,-123,-117,-111,-105,-99,-93,-87,-81,-74,-68,-61,-55,-48,-41,-34,-27,-20,-13,-6
};
signed int CosTab[]={
	200,199,199,198,198,196,195,194,192,190,187,185,182,179,176,173,169,165,161,157,153,148,143,138,133,128,123,117,111,105,99,93,87,81,74,68,61,55,48,41,34,27,20,13,6,0,-6,-13,-20,-27,-34,-41,-48,-55,-61,-68,-74,-81,-87,-93,-100,-105,-111,-117,-123,-128,-133,-138,-143,-148,-153,-157,-161,-165,-169,-173,-176,-179,-182,-185,-187,-190,-192,-194,-195,-196,-198,-198,-199,-199,
	-200,-199,-199,-198,-198,-196,-195,-194,-192,-190,-187,-185,-182,-179,-176,-173,-169,-165,-161,-157,-153,-148,-143,-138,-133,-128,-123,-117,-111,-105,-99,-93,-87,-81,-74,-68,-61,-55,-48,-41,-34,-27,-20,-13,-6,0,6,13,20,27,34,41,48,55,61,68,74,81,87,93,100,105,111,117,123,128,133,138,143,148,153,157,161,165,169,173,176,179,182,185,187,190,192,194,195,196,198,198,199,199
};

void Skew(int Matrix[][SPRITEX],int Degrees,int XOff,int YOff,int Scale)
{
	int y,x,YSin,YCos,Count;
	int SpriteX,SpriteY;
	Uint32 White;
	Uint32 Black;
	Uint32 *Colour;

	Degrees/=2;

	for(x=0;x<ScreenX;x++){
		XCos[x]=(x-XOff)*CosTab[Degrees]/Scale;
		XSin[x]=(x-XOff)*SinTab[Degrees]/Scale;
	}

	White = SDL_MapRGB(Screen->format,255,255,255);
	Black = SDL_MapRGB(Screen->format,0,0,0);

	switch(Screen->format->BytesPerPixel) {
	case 1: /* Assuming 8-bpp */
		{
		Uint8 *BufPtr;

		for(y=0;y<ScreenY;y++){
			YCos=((y-YOff)*CosTab[Degrees]/Scale);
			YSin=((y-YOff)*SinTab[Degrees]/Scale);
			BufPtr=(Uint8*) Screen->pixels + (y * (Screen->pitch / Screen->format->BytesPerPixel));
			for(x=0;x<ScreenX;x++){
				SpriteX=(XCos[x]+YSin)%SPRITEX;
				if(SpriteX<0) SpriteX=SPRITEX+SpriteX;
				SpriteY=(YCos-XSin[x])%SPRITEY;
				if(SpriteY<0) SpriteY=SPRITEY+SpriteY;
				Colour=(Matrix[SpriteY][SpriteX]?&White:&Black);
				*BufPtr=(Uint8) *Colour;
				++BufPtr;
			}
		}
		}
		break;

	case 2: /* Probably 15-bpp or 16-bpp */
		{
		Uint16 *BufPtr;

		for(y=0;y<ScreenY;y++){
			YCos=((y-YOff)*CosTab[Degrees]/Scale);
			YSin=((y-YOff)*SinTab[Degrees]/Scale);
			BufPtr=(Uint16*) Screen->pixels + (y * (Screen->pitch / Screen->format->BytesPerPixel));
			for(x=0;x<ScreenX;x++){
				SpriteX=(XCos[x]+YSin)%SPRITEX;
				if(SpriteX<0) SpriteX=SPRITEX+SpriteX;
				SpriteY=(YCos-XSin[x])%SPRITEY;
				if(SpriteY<0) SpriteY=SPRITEY+SpriteY;
				Colour=(Matrix[SpriteY][SpriteX]?&White:&Black);
				*BufPtr=(Uint16) *Colour;
				++BufPtr;
			}
		}
		}
		break;

    case 3: /* Slow 24-bpp mode, usually not used */
		{
		Uint8 *BufPtr;
		Uint8 *ColPtr;

		for(y=0;y<ScreenY;y++){
			YCos=((y-YOff)*CosTab[Degrees]/Scale);
			YSin=((y-YOff)*SinTab[Degrees]/Scale);
			BufPtr=(Uint8*) Screen->pixels + (y * Screen->pitch);
			for(x=0;x<ScreenX;x++){
				SpriteX=(XCos[x]+YSin)%SPRITEX;
				if(SpriteX<0) SpriteX=SPRITEX+SpriteX;
				SpriteY=(YCos-XSin[x])%SPRITEY;
				if(SpriteY<0) SpriteY=SPRITEY+SpriteY;
				Colour=(Matrix[SpriteY][SpriteX]?&White:&Black);
				ColPtr=(Uint8*) Colour;
				++ColPtr;
				*BufPtr=*ColPtr;
				++BufPtr;
				*BufPtr=*ColPtr;
				++BufPtr;
				*BufPtr=*ColPtr;
				++BufPtr;
			}
		}
		}
        break;

    case 4: /* Probably 32-bpp */
		{
		Uint32 *BufPtr;

		for(y=0;y<ScreenY;y++){
			YCos=((y-YOff)*CosTab[Degrees]/Scale);
			YSin=((y-YOff)*SinTab[Degrees]/Scale);
			BufPtr=(Uint32*) Screen->pixels + (y * (Screen->pitch / Screen->format->BytesPerPixel));
			for(x=0;x<ScreenX;x++){
				SpriteX=(XCos[x]+YSin)%SPRITEX;
				if(SpriteX<0) SpriteX=SPRITEX+SpriteX;
				SpriteY=(YCos-XSin[x])%SPRITEY;
				if(SpriteY<0) SpriteY=SPRITEY+SpriteY;
				Colour=(Matrix[SpriteY][SpriteX]?&White:&Black);
				*BufPtr=*Colour;
				++BufPtr;
			}
		}
		}
		break;

    }
}

int RenderLoop(void *Data)
{
	int Gap=0,GapInc=1;
	int Scale=100,SInc=4;
	int Angle=0;
	int FrameCnt=0;
	int Frame=1;
	Uint32 StartTicks;
	Uint32 ElapsedTicks;
	Uint32 Frames=0;
	bool SkipFrame=false;

	StartTicks=SDL_GetTicks();
	while(1){
		// Lock screen mutex
		if(SDL_LockMutex(ScreenMutex)==0){
			do{
				if(Screen==NULL) break;

				if(!SkipFrame){
					// Lock screen
				    if(SDL_MUSTLOCK(Screen)){
				        if(SDL_LockSurface(Screen)<0) break;
				    }

					// Draw pixels
					Skew((Frame==1?Sprite1:Sprite2),Angle,(ScreenX/2)+((50*SinTab[Angle/2])/128),(ScreenY/2)+((50*CosTab[Angle/2])/128),Scale);

					// Unlock screen
					if(SDL_MUSTLOCK(Screen)){
						SDL_UnlockSurface(Screen);
					}
				}

				Scale+=SInc;
				if(Scale>=200) SInc=-4;
				if(Scale<=40) SInc=4;
         
				Angle+=2;
				if(Angle>=360) Angle-=360;

				if(++FrameCnt==FRAMESPERSPRITE){
					if(Frame==1) Frame=2;
					else Frame=1;
					FrameCnt=0;
				}

				// Display
				SDL_UpdateRect(Screen,0,0,ScreenX,ScreenY);
			} while(0);

			// Unlock screen mutex
			SDL_UnlockMutex(ScreenMutex);
		}

		++Frames;

		ElapsedTicks=SDL_GetTicks()-StartTicks;
		if(Frames*MSPERFRAME>ElapsedTicks){
			SDL_Delay((Frames*MSPERFRAME)-ElapsedTicks);
			SkipFrame=false;
		}
		else if((Frames+1)*MSPERFRAME<ElapsedTicks){
			SkipFrame=true;
		}
	}

	return 0;
}

int WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// Explode sprites
	ExplodeSprites();

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
       // TODO MessageBox(NULL,"Unable to init SDL",MB_OK);
        exit(1);
    }
    atexit(SDL_Quit);

	// Create screen mutex
	ScreenMutex=SDL_CreateMutex();
	if(ScreenMutex==NULL){
       // TODO MessageBox
        exit(1);
	}

	// Initialise the screen
	if(InitialiseScreen(800,600)==false){
		// TODO
		exit(1);
	}

	// Start render thread
	SDL_Thread *RenderThread;
	RenderThread = SDL_CreateThread(&RenderLoop, NULL);
	if(RenderThread==NULL){
       // TODO MessageBox
        exit(1);		
	}

	// Event loop
	SDL_Event event;
	bool Finished=false;

    while(!Finished && SDL_WaitEvent(&event)){
        switch(event.type){
		case SDL_VIDEORESIZE:
			if(InitialiseScreen(event.resize.w,event.resize.h)==false){
				// TODO
				exit(1);
			}
			break;
        case SDL_QUIT:
			Finished=true;
			break;
        }
    }

	// Stop render thread
	SDL_KillThread(RenderThread);

	// Destroy screen
	DestroyScreen();

	// Destroy mutex
	SDL_DestroyMutex(ScreenMutex);
}

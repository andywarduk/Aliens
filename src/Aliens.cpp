#ifdef _WIN32
# include <windows.h>
# include "resource.h"
#endif

#include <SDL.h>
#include <SDL_thread.h>

#include "String.h"

#define MSPERFRAME 50 // 20fps
//#define MSPERFRAME 20 // 50fps
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

Uint32 Sprite1[SPRITEY][SPRITEX];
Uint32 Sprite2[SPRITEY][SPRITEX];

SDL_mutex *ScreenMutex=NULL;
int ScreenX=0;
int ScreenY=0;
SDL_Surface *Screen=NULL;
int *XSin=NULL;
int *XCos=NULL;
bool ScreenInitialised=false;

const SDL_VideoInfo *VideoInfo;

// Prototypes
void Debug(char *Message,...);

#ifdef _WIN32
HINSTANCE hGlobalInst;
# ifdef DEBUG
BOOL APIENTRY DbgDlgProc(HWND,UINT,WPARAM,LPARAM);
HWND hDbgDlg;
# endif
#endif

void ExplodeSprite(int Src[][SPRITESRCX],Uint32 Dst[][SPRITEX])
{
	int x1,y1,x2,y2,dx,dy;
	Uint32 Black;
	Uint32 White;

	White = SDL_MapRGB(Screen->format,255,255,255);
	Black = SDL_MapRGB(Screen->format,0,0,0);

	dy=0;
	for(y1=0;y1<SPRITESRCY;y1++){
		for(y2=0;y2<SPRITESCALE;y2++){
			dx=0;
			for(x1=0;x1<SPRITESRCX;x1++){
				for(x2=0;x2<SPRITESCALE;x2++){
					Dst[dy][dx] = (Src[y1][x1]==1 ? White : Black );
					++dx;
				}
			}
			++dy;
		}
	}
}

void ExplodeSprites()
{
	ExplodeSprite(Sprite1Src,Sprite1);
	ExplodeSprite(Sprite2Src,Sprite2);
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
		// Get video information
		VideoInfo=SDL_GetVideoInfo();

//		Screen = SDL_SetVideoMode(NewX,NewY,0,SDL_ANYFORMAT|SDL_RESIZABLE|SDL_SWSURFACE);
//		Screen = SDL_SetVideoMode(NewX,NewY,0,SDL_RESIZABLE|SDL_HWSURFACE|SDL_ANYFORMAT);
//		Screen = SDL_SetVideoMode(NewX,NewY,0,SDL_RESIZABLE|SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF);
		Screen = SDL_SetVideoMode(NewX,NewY,0,SDL_RESIZABLE|SDL_HWSURFACE|SDL_ANYFORMAT);
//		Screen = SDL_SetVideoMode(0,0,0,SDL_FULLSCREEN|SDL_ANYFORMAT);
		if(Screen){
			Ok=true;
			ScreenX=Screen->w;
			ScreenY=Screen->h;
			XCos=new int[ScreenX];
			XSin=new int[ScreenX];
			ScreenInitialised=true;
			
			// Explode sprites
			ExplodeSprites();
		}
		else{
			// Failed
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

void Skew(Uint32 Matrix[][SPRITEX],int Degrees,int XOff,int YOff,int Scale)
{
	int y,x,YSin,YCos;
	int SpriteX,SpriteY;
	Uint32 Colour;

	Degrees/=2;

	for(x=0;x<ScreenX;x++){
		XCos[x]=(x-XOff)*CosTab[Degrees]/Scale;
		XSin[x]=(x-XOff)*SinTab[Degrees]/Scale;
	}

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
				Colour=Matrix[SpriteY][SpriteX];
				*BufPtr=(Uint8) Colour;
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
				Colour=Matrix[SpriteY][SpriteX];
				*BufPtr=(Uint16) Colour;
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
				ColPtr=(Uint8*) &Matrix[SpriteY][SpriteX];
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
				Colour=Matrix[SpriteY][SpriteX];
				*BufPtr=Colour;
				++BufPtr;
			}
		}
		}
		break;

    }
}

int RenderLoop(void *)
{
	int Scale=100,SInc=4;
	int Angle=0;
	int FrameCnt=0;
	int Frame=1;
	Uint32 StartTicks;
	Uint32 Ticks;
#ifdef _DEBUG
	Uint32 Seconds;
	Uint32 LastSeconds=0;
	Uint32 FramesPerSecond=0;
	Uint32 SkippedPerSecond=0;
#endif
	Uint32 ElapsedTicks;
	Uint32 Frames=0;
	bool SkipFrame=false;

	StartTicks=SDL_GetTicks();
	for(;;){
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

#ifdef _DEBUG
					++FramesPerSecond;
#endif
				}
				else{
#ifdef _DEBUG
					++SkippedPerSecond;
#endif
				}

				// Unlock screen mutex
				SDL_UnlockMutex(ScreenMutex);

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
				SDL_Flip(Screen);
			} while(0);
		}

		++Frames;
		Ticks=SDL_GetTicks();
		ElapsedTicks=Ticks-StartTicks;

#ifdef _DEBUG
		Seconds=ElapsedTicks/1000;
		if(Seconds!=LastSeconds){
			Debug("Secs=%ld, FPS=%ld, Skipped=%ld",Seconds,FramesPerSecond,SkippedPerSecond);
			LastSeconds=Seconds;
			FramesPerSecond=0;
			SkippedPerSecond=0;
		}
#endif

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

void Error(char *Message,...)
{
	String MessageString;
	va_list Args;

	va_start(Args,Message);
	MessageString.VPrintf(Message,Args);
	va_end(Args);

#ifdef _WIN32
	MessageBox(GetActiveWindow(),MessageString.CStr(),TEXT("Error"),MB_OK|MB_ICONSTOP);
#else
	MessageString+="\n";
	fprintf(stderr,MessageString.CStr());
#endif
}

#ifdef DEBUG
void Debug(char *Message,...)
{
	String MessageString;
	va_list Args;
# ifdef _WIN32
	LRESULT Index;
# endif

	va_start(Args,Message);
	MessageString.VPrintf(Message,Args);
	va_end(Args);

# ifdef _WIN32
	Index=SendDlgItemMessage(hDbgDlg,IDC_DEBUG,LB_ADDSTRING,0,(LPARAM)MessageString.CStr());
	if(Index!=LB_ERR) SendDlgItemMessage(hDbgDlg,IDC_DEBUG,LB_SETCURSEL,Index,0);
# else
	MessageString+="\n";
	fprintf(stdout,MessageString.CStr());
# endif
}

# ifdef _WIN32
BOOL APIENTRY DbgDlgProc(HWND,UINT Msg,WPARAM,LPARAM)
{
	switch(Msg){
	case WM_INITDIALOG:
		return(true);
	default:
		return(false);
	}
}
# endif

#endif

bool Initialise()
{
	bool Ok=false;
#ifdef DEBUG
	char DrvName[128];
#endif

	do{
#ifdef _WIN32
# ifdef DEBUG
		// Open Debug messages dialogue
		hDbgDlg=CreateDialog(hGlobalInst,MAKEINTRESOURCE(IDD_DEBUG),GetDesktopWindow(),DbgDlgProc);
		if(hDbgDlg==NULL) Error("Error creating debug dialog (%ld)",GetLastError());
# endif
#endif

		// TODO
		//SDL_putenv("SDL_VIDEODRIVER=directx");

		// Initialise SDL
		if(SDL_Init(SDL_INIT_VIDEO) < 0){
			Error("Unable to init SDL");
			break;
		}

#ifdef DEBUG
		// Get driver name
		SDL_VideoDriverName(DrvName,128);
		Debug("Using video driver %s",DrvName); 
#endif

		// Set window caption
		SDL_WM_SetCaption("Aliens!",NULL);

		// Create screen mutex
		ScreenMutex=SDL_CreateMutex();
		if(ScreenMutex==NULL){
			Error("Unable to create Mutex");
			break;
		}

		// Create the 'screen'
		if(InitialiseScreen(800,600)==false){
			Error("Unable to initialise the screen");
			break;
		}

		// Finished
		Ok=true;
	}while(0);

	return Ok;
}

void CleanUp()
{
	// Destroy screen
	DestroyScreen();

	// Delete screen mutex
	SDL_DestroyMutex(ScreenMutex);

	// Quit SDL
	SDL_Quit();

#ifdef _WIN32
# ifdef DEBUG
	// Close Debug message window
	DestroyWindow(hDbgDlg);
# endif
#endif
}

void MainLoop()
{
	SDL_Thread *RenderThread;
	SDL_Event event;
	bool Finished=false;

	// Start render thread
	RenderThread = SDL_CreateThread(&RenderLoop, NULL);
	if(RenderThread==NULL){
		Error("Unable to create render thread");
        Finished=true;
	}

	// Event loop
    while(!Finished && SDL_WaitEvent(&event)){
        switch(event.type){
		case SDL_VIDEORESIZE:
			if(InitialiseScreen(event.resize.w,event.resize.h)==false){
				Error("Unable to re-create the screen");
				Finished=true;
			}
			break;

		case SDL_QUIT:
			Finished=true;
			break;

		}
    }

	// Stop render thread
	SDL_KillThread(RenderThread);
}

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
#else
int main(int ArgC,char **ArgV)
#endif
{
#ifdef _WIN32
	hGlobalInst=hInst;
#endif

	if(Initialise()){
		MainLoop();
	}
	CleanUp();

	return 0;
}

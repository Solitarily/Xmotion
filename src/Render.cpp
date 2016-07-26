#include <Render.h>

using namespace std;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Current displayed texture
SDL_Texture* gTexture = NULL;

color_t BackgroundColor = color_t{ 0, 0, 0, 0 }; //Default Black

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 480;

long TimeRendered = 0;

string current_activity;

bool Render_Init()
{
	#ifdef NDEBUG
		long BeginTime = micros();
	#endif
  //Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		TTF_Init();
		//Create window
		if (sim)
		{
			gWindow = SDL_CreateWindow( APPLICATION_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		}
		else
		{
			gWindow = SDL_CreateWindow( APPLICATION_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL );
		}

		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}
	#ifdef NDEBUG
		TimeRendered = micros() - BeginTime;
	#endif
	return success;
}
SDL_Texture* Render_loadImage( string path )
{
  //The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}
	return newTexture;
}
SDL_Texture* Render_RotateSurface(SDL_Surface* Surface, double Angle)
{
  return NULL;
}
SDL_Texture* Render_loadFont(string font, SDL_Color color, int size, string text)
{
	TTF_Font* Font = TTF_OpenFont(font.c_str(), size);
	if (Font == NULL)
	{
		printf("Cant open font!\n");
		return NULL;
	}
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, text.c_str(), color);
	SDL_Texture* newTexture = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);
	TTF_CloseFont(Font);
	return newTexture;
}
float file_open_scroll_offset = 0;
vector<object_t> file_tiles;
void Render_RenderStack()
{
    //Initialize renderer color
    SDL_SetRenderDrawColor( gRenderer, BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, BackgroundColor.a );

		sort( ObjectStack.begin( ), ObjectStack.end( ), [ ]( const object_t& lhs, const object_t& rhs )
		{
   		return lhs.zindex < rhs.zindex;
		});
    SDL_Rect dst;
    SDL_RenderClear( gRenderer );

		for (int x = 0; x < ObjectStack.size(); x++)
	  {
	    if (ObjectStack[x].visable == true && ObjectStack[x].activity == current_activity)
	    {
	      //Render texture to screen
	      dst.x = ObjectStack[x].position.x;
	      dst.y = ObjectStack[x].position.y;
	      //Query the texture to get its width and height to use
	      if (ObjectStack[x].size.w > 0 && ObjectStack[x].size.h > 0)
	      {
	        dst.w = ObjectStack[x].size.w;
	        dst.h = ObjectStack[x].size.h;
	      }
	      else
	      {
	        SDL_QueryTexture(ObjectStack[x].texture, NULL, NULL, &dst.w, &dst.h);
	      }
				SDL_SetTextureBlendMode( ObjectStack[x].texture, SDL_BLENDMODE_BLEND );
				SDL_SetTextureAlphaMod( ObjectStack[x].texture, ObjectStack[x].alpha );
	      SDL_RenderCopyEx( gRenderer, ObjectStack[x].texture, NULL, &dst, ObjectStack[x].angle, NULL, SDL_FLIP_NONE );
	    }
	  }

		if (current_activity == "Main")
		{
			dst.x = 60;
			dst.y = 30;
			char buffer[100];
			sprintf(buffer, "X: %0.4f", OffsetCordinates.x);
			SDL_Texture *XCordTexture = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 40, string(buffer));
			SDL_QueryTexture(XCordTexture, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( XCordTexture, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( XCordTexture, 255 );
			SDL_RenderCopyEx( gRenderer, XCordTexture, NULL, &dst, 0, NULL, SDL_FLIP_NONE );

			dst.x = 60;
			dst.y = 80;
			sprintf(buffer, "Y: %0.4f", OffsetCordinates.y);
			SDL_Texture *YCordTexture = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 40, string(buffer));
			SDL_QueryTexture(YCordTexture, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( YCordTexture, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( YCordTexture, 255 );
			SDL_RenderCopyEx( gRenderer, YCordTexture, NULL, &dst, 0, NULL, SDL_FLIP_NONE );

			dst.x = 60;
			dst.y = 130;
			sprintf(buffer, "Z: %0.4f", OffsetCordinates.z);
			SDL_Texture *ZCordTexture = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 40, string(buffer));
			SDL_QueryTexture(ZCordTexture, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( ZCordTexture, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( ZCordTexture, 255 );
			SDL_RenderCopyEx( gRenderer, ZCordTexture, NULL, &dst, 0, NULL, SDL_FLIP_NONE );

			dst.x = 300;
			dst.y = 40;
			sprintf(buffer, "DX: %0.4f", OffsetCordinates.x - GcodePointer.X);
			SDL_Texture *DX = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 20, string(buffer));
			SDL_QueryTexture(DX, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( DX, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( DX, 255 );
			SDL_RenderCopyEx( gRenderer, DX, NULL, &dst, 0, NULL, SDL_FLIP_NONE );

			dst.x = 300;
			dst.y = 75;
			sprintf(buffer, "DY: %0.4f", OffsetCordinates.y - GcodePointer.Y);
			SDL_Texture *DY = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 20, string(buffer));
			SDL_QueryTexture(DY, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( DY, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( DY, 255 );
			SDL_RenderCopyEx( gRenderer, DY, NULL, &dst, 0, NULL, SDL_FLIP_NONE );

			dst.x = 300;
			dst.y = 107;
			if (GcodePointer.G == 0)
			{
				sprintf(buffer, "F: %0.4f", RAPID_FEED);
			}
			else
			{
				sprintf(buffer, "F: %0.4f", GcodePointer.F);
			}
			SDL_Texture *FeedTexture = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 20, string(buffer));
			SDL_QueryTexture(FeedTexture, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( FeedTexture, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( FeedTexture, 255 );
			SDL_RenderCopyEx( gRenderer, FeedTexture, NULL, &dst, 0, NULL, SDL_FLIP_NONE );

			dst.x = 300;
			dst.y = 139;

			sprintf(buffer, "uCPD: %0.4f", control_signal_pulse);
			SDL_Texture *PulseTexture = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 20, string(buffer));
			SDL_QueryTexture(PulseTexture, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( PulseTexture, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( PulseTexture, 255 );
			SDL_RenderCopyEx( gRenderer, PulseTexture, NULL, &dst, 0, NULL, SDL_FLIP_NONE );

			dst.x = 22;
			dst.y = 180;
			if (GcodePointer.G == 0) sprintf(buffer, "G%0.1f X%0.4f Y%0.4f Z%0.4f", GcodePointer.G, GcodePointer.X, GcodePointer.Y, GcodePointer.Z);
			if (GcodePointer.G == 1) sprintf(buffer, "G%0.1f X%0.4f Y%0.4f Z%0.4f F%0.1f", GcodePointer.G, GcodePointer.X, GcodePointer.Y, GcodePointer.Z, GcodePointer.F);
			if (GcodePointer.G == 2 || GcodePointer.G == 3) sprintf(buffer, "G%0.1f X%0.4f Y%0.4f Z%0.4f I%0.4f J%0.4f F%0.1f", GcodePointer.G, GcodePointer.X, GcodePointer.Y, GcodePointer.Z, GcodePointer.I, GcodePointer.J, GcodePointer.F);
			SDL_Texture *LineTexture = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 20, string(buffer));
			SDL_QueryTexture(LineTexture, NULL, NULL, &dst.w, &dst.h);
			SDL_SetTextureBlendMode( LineTexture, SDL_BLENDMODE_BLEND );
			SDL_SetTextureAlphaMod( LineTexture, 255 );
			SDL_RenderCopyEx( gRenderer, LineTexture, NULL, &dst, 0, NULL, SDL_FLIP_NONE );


			if ( XCordTexture != NULL ) { SDL_DestroyTexture( XCordTexture ); }
			if ( YCordTexture != NULL ) { SDL_DestroyTexture( YCordTexture ); }
			if ( ZCordTexture != NULL ) { SDL_DestroyTexture( YCordTexture ); }

			if ( DX != NULL ) { SDL_DestroyTexture( DX ); }
			if ( DY != NULL ) { SDL_DestroyTexture( DY ); }
			if ( FeedTexture != NULL ) { SDL_DestroyTexture( FeedTexture ); }
			if ( PulseTexture != NULL ) { SDL_DestroyTexture( PulseTexture ); }
			if ( LineTexture != NULL ) { SDL_DestroyTexture( LineTexture ); }

		}
		if (current_activity == "FileOpen")
		{
			file_tiles.clear();
			point_t top_left;
			top_left.x = 250;
			top_left.y = 10 + file_open_scroll_offset;

			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir ("/media/usb")) != NULL)
			{
				  /* print all the files and directories within directory */
				  while ((ent = readdir (dir)) != NULL)
					{
						string filename = string(ent->d_name);
						if (filename != "." && filename != "..")
						{
							if (filename.find(".pgm") != std::string::npos || filename.find(".nc") != std::string::npos)
							{
								//printf ("%s\n", filename.c_str());
								if (true)
								{
									object_t tmpObject;
									tmpObject.visable = true;
									tmpObject.type = FILE;
									tmpObject.zindex = 1;
									tmpObject.tagname = "/media/usb/" + filename;
									tmpObject.position.x = top_left.x;
									tmpObject.position.y = top_left.y;
									tmpObject.size.w = 60;
									tmpObject.size.h = 60;
									tmpObject.angle = 0;
									tmpObject.visable = true;
									tmpObject.texture = Render_loadImage("Images/File.png");
									tmpObject.activity = "FileOpen";
									//printf("activity = %s\n", tmpObject.activity.c_str());
									if( tmpObject.texture == NULL )
									{
										printf("%s\tCan't open file image!\n", KRED, KBLUE ,KNORMAL);
									}
									else
									{
										file_tiles.push_back(tmpObject);
									}
									top_left.y += 75;
									char buffer[100];
									dst.x = top_left.x + 75;
									dst.y = top_left.y - 60;
									sprintf(buffer, "%s", filename.c_str());
									SDL_Texture *FilenameTexture = Render_loadFont("Sans.ttf", SDL_Color{0, 0, 0}, 30, string(buffer));
									SDL_QueryTexture(FilenameTexture, NULL, NULL, &dst.w, &dst.h);
									SDL_SetTextureBlendMode( FilenameTexture, SDL_BLENDMODE_BLEND );
									SDL_SetTextureAlphaMod( FilenameTexture, 255 );
									SDL_RenderCopyEx( gRenderer, FilenameTexture, NULL, &dst, 0, NULL, SDL_FLIP_NONE );
									if ( FilenameTexture != NULL ) { SDL_DestroyTexture( FilenameTexture ); }

									if (tmpObject.visable == true && tmpObject.activity == current_activity)
							    {
							      //Render texture to screen
							      dst.x = tmpObject.position.x;
							      dst.y = tmpObject.position.y;
							      //Query the texture to get its width and height to use
							      if (tmpObject.size.w > 0 && tmpObject.size.h > 0)
							      {
							        dst.w = tmpObject.size.w;
							        dst.h = tmpObject.size.h;
							      }
							      else
							      {
							        SDL_QueryTexture(tmpObject.texture, NULL, NULL, &dst.w, &dst.h);
							      }
										SDL_SetTextureBlendMode( tmpObject.texture, SDL_BLENDMODE_BLEND );
										SDL_SetTextureAlphaMod( tmpObject.texture, tmpObject.alpha );
							      SDL_RenderCopyEx( gRenderer, tmpObject.texture, NULL, &dst, tmpObject.angle, NULL, SDL_FLIP_NONE );
							    }

								}
							}
						}
				  }
				  closedir (dir);
			}
			else
			{
				  printf("Could not access /media/usb!\n");
			}
		}

		//Update screen
		SDL_RenderPresent( gRenderer );

}
void Render_Close()
{
  for (int x = 0; x < ObjectStack.size(); x++)
  {
    SDL_DestroyTexture( ObjectStack[x].texture );
  	ObjectStack[x].texture = NULL;
  }
  SDL_DestroyRenderer( gRenderer );
  gRenderer = NULL;
	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

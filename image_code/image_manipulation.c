#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include <SDL/SDL_rotozoom.h>
#include <math.h>

#include "detect_angle.c"

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.


void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

SDL_Surface* load_image(const char* path)
{
  SDL_Surface* temp = IMG_Load(path);
  SDL_Surface* final = SDL_ConvertSurfaceFormat(temp, \
          SDL_PIXELFORMAT_RGB888, 0);
  SDL_FreeSurface(temp);
  return final;
}


Uint32 SDL_ReadPixel(SDL_Surface* surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;

  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp)
  {
             case 1:
                  return *p;
             case 2:
                  return *(Uint16 *)p;
             case 3:
                 if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                     return p[0] << 16 | p[1] << 8 | p[2];
                 else
                     return p[0] | p[1] << 8 | p[2] << 16;
             case 4:


                  return *(Uint32 *)p;
             default:
                  return 0;
  }

}

void SDL_WritePixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel; 
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:

        *(Uint32 *)p = pixel;
        break;
    }

}

SDL_Surface* SDL_RotationCentralN(SDL_Surface* origine, float angle)
{
    float angle_radian = -angle * M_PI / 180.0;

    float tcos = cos(angle_radian);
    float tsin = sin(angle_radian);

    //calculate the size of the final image
    double largeurdest=   ceil(origine->w * fabs(tcos) + origine->h *
            fabs(tsin));
    double hauteurdest=   ceil( origine->w * fabs(tsin) + origine->h *
            fabs(tcos));

    SDL_Surface* destination = SDL_CreateRGBSurface(0, largeurdest,
            hauteurdest, origine->format->BitsPerPixel,
            origine->format->Rmask,
         origine->format->Gmask, origine->format->Bmask,
         origine->format->Amask);

    if(destination==NULL)
    {
     return NULL;
   }

 //calculate the center of the image
 int mxdest = destination->w/2.;
 int mydest = destination->h/2.;
 int mx = origine->w/2.;
 int my = origine->h/2.;

 for(int j=0;j<destination->h;j++)
   {
     for(int i=0;i<destination->w;i++)
      {
	int bx = (ceil (tcos * (i-mxdest) + tsin * (j-mydest) + mx));
	int by = (ceil (-tsin * (i-mxdest) + tcos * (j-mydest) + my));
	if (bx>=0 && bx< origine->w && by>=0 && by< origine->h)
	  {
	    Uint32 couleur = SDL_ReadPixel(origine, bx, by);
	    SDL_WritePixel(destination, i, j, couleur);
	  }
      }
   }
return destination;
}


int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: image-file");

    // - Initialize the SDL.
     if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    // - Create a window.
     SDL_Window* window = SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0,
             1000, 1000, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    // - Create a renderer.
     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
             SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a surface from the image.
    SDL_Surface* surface=load_image(argv[1]);
    //SDL_Surface* c=load_image(argv[1]);

    // - Resize the window according to the size of the image.
    SDL_SetWindowSize(window, surface->w, surface->h);

    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, surface);
    draw(renderer, texture1);


    /*gray(c,w,h);
    _medianfilter(surface,c,w,h);
    otsu(surface,w,h);*/

    float angle= find_pixels(surface);
    //int angle=35;
    printf("%f \n",angle);
    SDL_Surface* final= SDL_RotationCentralN(surface, angle);
    ///SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, final);
    //save the rotated image

    IMG_SavePNG(final, "bin_rotated.png");
    SDL_Surface* s=load_image(argv[2]);
    SDL_Surface* final2= SDL_RotationCentralN(s,angle);
    //SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, final2);
    //save the rotated image
    IMG_SavePNG(final2, "ori_rotated.png");

    /*SDL_RotationCentralN(s,-60);
    IMG_SavePNG(final2, "to_rotate.png");*/
    // - Free the surfaces.
    SDL_FreeSurface(surface);
    SDL_FreeSurface(final);
    // - Destroy the objects.
    //SDL_DestroyTexture(texture);
    //SDL_DestroyTexture(texture1);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}



/*SDL_Surface* load_image(const char* path)
{
	SDL_Surface* stemp = IMG_Load(path);
	SDL_Surface* surface = SDL_ConvertSurfaceFormat(stemp,SDL_
        PIXELFORMAT_RGB888,0);
        if(surface == NULL)
		errx(EXIT_FAILURE,"%s",SDL_GetError());
	SDL_FreeSurface(stemp);
	return surface;
}
   

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint32 average = 0.3*r + 0.59*g + 0.11*b;
    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

void surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    if(SDL_LockSurface(surface) != 0)
	errx(EXIT_FAILURE,"%s",SDL_GetError());
    for(int i = 0; i<len;i++)
    {
	    pixels[i] = pixel_to_grayscale(pixels[i],format);
    }
    SDL_UnlockSurface(surface);
}

SDL_Surface* rotate(SDL_Surface* surface,double angle)
{
	int w = surface->w;
	int h = surface->h;
	int dw = (int)(w * fabs(cos(angle)) + h*fabs(sin(angle)));
	int dh = (int)(w * fabs(sin(angle)) + h*(fabs(cos(angle))));
	size_t xc = w/2;
	size_t yc = h/2;
	SDL_Surface* dest = SDL_CreateRGBSurface(0,dw,dh,32,0,0,0,0);
	SDL_FillRect(dest,NULL, SDL_MapRGB(dest->format, 255, 255, 255));
	SDL_LockSurface(dest);
	Uint32* spix = surface->pixels;
	Uint32* dpix = dest->pixels;
	for(size_t x = 0; x < (size_t)w;x++)
	{
		for(size_t y = 0; y< (size_t)h;y++)
		{
			size_t xp = cos(angle)*(x-xc) - (sin(angle)*
                        (y-yc)) + xc;
			size_t yp = (sin(angle))*(x-xc) +cos(angle) * 
                        (y-yc) + yc;
			if(xp < (size_t)dw && yp < (size_t)dh)
			{
				dpix[yp * (size_t)dw + xp] = 
                                spix[y *(size_t)w + x];
			}
		}
	}
	SDL_UnlockSurface(dest);
	return dest;
}

double convertangle(char str[],size_t leng)
{
	int res = 0;
	size_t len = leng -1;
	int d = 1;
	for(size_t i = 0; i<len;i++)
	{
		res += (((str[len-i])%48))*d;
		d = d * 10;
	}
	return (double)res;
}


int main(int argc, char** argv)
{
    // Checks the number of arguments.
    // syntax: ./exe file angle
    if (argc < 2 || argc > 3)
        errx(EXIT_FAILURE, "Usage: image-file angle");
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
	errx(EXIT_FAILURE,"%s",SDL_GetError());
    SDL_Surface* surface  = load_image(argv[1]); //charge l'image
    surface_to_grayscale(surface); //enleve les couleur
    if(argc == 3 )
    {
	    double angle = convertangle(argv[2],(0));
	    surface = rotate(surface,(M_PI/4)); //rotozoom
                                                //Surface(surface,0,0,0); //la tourne
	    printf("%f",angle);
    }
    IMG_SavePNG(surface,"out.png");//la sauvergarde
    SDL_FreeSurface(surface);

    return EXIT_SUCCESS;
}*/

#include <err.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>


/*SDL_Surface* load_image(const char* path)
{
    SDL_Surface *img = IMG_Load(path);
    if(img == NULL)
        errx(EXIT_FAILURE,"%s",SDL_GetError());
    SDL_ConvertSurfaceFormat(img,SDL_PIXELFORMAT_RGB888,0);
    return img;
}*/

float find_lines(SDL_Surface* surface, int x , int y)
{
    int find_it = 0;

    float down_x = 0;
    float down_y = 0;

    Uint8 r, g, b;
    Uint32* pixels = surface->pixels;

    //printf("height = %i\n",surface->h);
    //printf("width = %i\n",surface->w);

    for(int i = surface->w-1; i > 0 && find_it != 1; i--)
    {
        for(int j = 0; j < surface->h && find_it != 1;j++)
        {
            SDL_GetRGB(pixels[j*(surface->w)+(size_t)i],surface->format,&r,&g,&b);
            if ( r != 0 && g!= 0 && b != 0)
            {
                find_it = 1;
                down_x = (float) i;
                down_y = (float) j;
            }
        }
    }

    float p2 = (float)down_y - (float)y;
    if(down_x < x)
    {
        float p1 = (float)x - (float)down_x;
        float res  = atanf(p2/p1)*(180/M_PI);
        return res;
    }
    else
    {
        float p1 = (float)down_x - (float)x;
        float res  = atanf(p2/p1)*(180/M_PI);
        return res;
    }
}


float find_pixels(SDL_Surface* surface)
{
    Uint8 r, g, b;
    Uint32* pixels = surface->pixels;
    SDL_PixelFormat* format = surface->format;
    if(SDL_LockSurface(surface)==0)
    {
        SDL_LockSurface(surface);
        for(int i = 0; i < surface->h; i++)
        {
            for (int j = 0; j < surface->w; j++)
            {
                SDL_GetRGB(pixels[i*(surface->w)+j],format,&r,&g,&b);
                //printf("r = %i ,b = %i, g = %i\n",r,g,b);
                if ( r != 0 && g!= 0 && b != 0)
                {
                    return find_lines(surface,j,i);
                }
            }
        }
        SDL_UnlockSurface(surface);
        return 0;
    }
    else
    {
        errx(EXIT_FAILURE, "%s",SDL_GetError());
    }
}

/*int main (int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // - Initialize the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* s = load_image(argv[1]);
    //SDL_SaveBMP(s, "test.bmp");   // if you want bmp

    printf("%f\n",find_pixels(s));

    IMG_SavePNG(s,"resolution.png");
    SDL_FreeSurface(s);

    return EXIT_SUCCESS;
}*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "solver.c"

SDL_Surface* load_image(const char* path)
{
    SDL_Surface *img = IMG_Load(path);
    if(img == NULL)
        errx(EXIT_FAILURE,"%s",SDL_GetError());
    SDL_ConvertSurfaceFormat(img,SDL_PIXELFORMAT_RGB888,0);
    return img;
}


void basic_print(char* filename, int initial[9][9],
        SDL_Surface *img)
{
    TTF_Init();
    //printf("cc0\n");

    //printf("cc1\n");
    SDL_Surface *final = SDL_ConvertSurfaceFormat(img,SDL_PIXELFORMAT_RGBA32,0);
    if(final == NULL)
        errx(EXIT_FAILURE,"%s",SDL_GetError());

    //printf("cc2\n");
    SDL_Color textColor;

    //printf("cc3\n");
    TTF_Font *font = TTF_OpenFont("Font/Pixel Powerline.ttf",80);
    int x = 0;
    int y = 0;

    //printf("cc4\n");
    for (size_t i = 0; i < 9 ; i++)
    {
        //printf("in_it\n");
        y = 0;
        for(size_t j = 0; j < 9; j++)
        {

            if (initial[i][j] != 0)
            {
                char digit[] = " ";
                digit[0] = initial[i][j] + '0';
                textColor.r = 0;
                textColor.g = 0;
                textColor.b = 0;
                textColor.a = 255;
                SDL_Surface *text = TTF_RenderText_Solid(font,digit,textColor);
                SDL_Rect rect = {50+x,30+y,115,115};
                SDL_BlitSurface(text,NULL,final,&rect);
                SDL_FreeSurface(text);
            }
            y+=114;
        }
        x+=114;
    }
    //printf("out0\n");
    char*st = strcat(filename,".result.png");
    //printf("out1\n");
    IMG_SavePNG(final,st);
    //printf("out2\n");
    //SDL_FreeSurface(text); 
    SDL_free(final);
    TTF_CloseFont( font );
    TTF_Quit();
}

void final_print(char* filename,int grid[9][9], int initial[9][9],
        SDL_Surface *img)
{
    TTF_Init();
    //printf("cc0\n");

    //printf("cc1\n");
    SDL_Surface *final = SDL_ConvertSurfaceFormat(img,SDL_PIXELFORMAT_RGBA32,0);
    if(final == NULL)
        errx(EXIT_FAILURE,"%s",SDL_GetError());

    //printf("cc2\n");
    SDL_Color textColor;

    //printf("cc3\n");
    TTF_Font *font = TTF_OpenFont("Font/Pixel Powerline.ttf",80);
    int x = 0;
    int y = 0;

    //printf("cc4\n");
    for (size_t i = 0; i < 9 ; i++)
    {
        //printf("in_it\n");
        y = 0;
        for(size_t j = 0; j < 9; j++)
        {
            char digit[] = " ";
            digit[0] = grid[i][j] + '0';

            if(initial[i][j] == grid[i][j])
            {
                textColor.r = 0;
                textColor.g = 0;
                textColor.b = 0;
                textColor.a = 255;
            }
            else
            {
                textColor.r = 1;
                textColor.g = 110;
                textColor.b = 0;
                textColor.a = 255;
            }
            SDL_Surface *text = TTF_RenderText_Solid(font,digit,textColor);
            SDL_Rect rect = {50+x,30+y,115,115};
            SDL_BlitSurface(text,NULL,final,&rect);
            SDL_FreeSurface(text);
            y+=114;
        }
        x+=114;
    }
    //printf("out0\n");
    char*st = strcat(filename,".result.png");
    //printf("out1\n");
    IMG_SavePNG(final,st);
    //printf("out2\n");
    //SDL_FreeSurface(text); 
    SDL_free(final);
    TTF_CloseFont( font );
    TTF_Quit();
}

int main (int argc, char**argv)
{
        if (argc != 4)
            errx(EXIT_FAILURE, "Usage: image-file");
	int grid[9][9];
        int initial[9][9];
	open_my_file(argv[1],grid);
	open_my_file(argv[1],initial);

        SDL_Surface *img = IMG_Load(argv[2]);
        if(img == NULL)
            errx(EXIT_FAILURE,"%s",SDL_GetError());
        if (*argv[3] == '1')
        {
            printf("basics");
            basic_print(argv[1],initial,img);
        }
        else
        {
            printf("solve");
            isValid(grid,0);
            // print_my_grid(grid);
            //printf("before\n");
            //printf("after\n");
            //printf("after if\n");
            final_print(argv[1],grid,initial,img);
            //my_final_file(argv[1],grid);
        }

        return 0;
}


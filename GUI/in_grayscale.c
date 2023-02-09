#include <err.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>

void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}


// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.

SDL_Surface* load_image(const char* path)
{
    SDL_Surface *img = IMG_Load(path);
    if(img == NULL)
        errx(EXIT_FAILURE,"%s",SDL_GetError());
    SDL_ConvertSurfaceFormat(img,SDL_PIXELFORMAT_RGB888,0);
    return img;
}
 
 

Uint32 pixel_to_black_white(Uint32 pixel_color, SDL_PixelFormat* format)
{
    // To get the RGB values of a color in the r, g, b variables.
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    // To convert a color into grayscale I compute the weighted average of the RGB values. 
    Uint8 average = 0.3*r + 0.59*g + 0.11*b;

    // Update the RGB values by using this average (r = g = b = average).
    r = average;
    g = average;
    b = average;

    if((r + b + g) / 3 > 127) // If the average of the RGB values is greater than 127, the pixel is white.
    {
        r = 0; // White.
        g = 0;
        b = 0;
    }
    else // If the average of the RGB values is less than 127, the pixel is black.
    {
        r = 255; // Black.
        g = 255;
        b = 255;
    }
    // To get a 32-bit word from RGB values.
    Uint32 color = SDL_MapRGB(format, r, g, b);
    return color;
}

Uint8 *pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
    int bpp = surf->format->BytesPerPixel;
    return (Uint8 *)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch (surface->format->BytesPerPixel)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;
    }

    return 0;
}

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch (surface->format->BytesPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
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



void init_hist(SDL_Surface *image_surface, int width, int height, float *hist)
{
    Uint8 r, g, b;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            hist[r]++;
        }
    }
}

void normalized(float *hist, int Pixels)
{
    for (int i = 0; i < 256; i++)
        hist[i] = hist[i] / Pixels;
}

Uint8 otsu_threshold(float *hist)
{
    float w0 = 0, w1 = 0, wT = 0;
    float u0 = 0, u1 = 0, uT = 0;
    float sum = 0, vk = 0, v_max = 0;
    float threshold = 0;

    for (size_t i = 0; i < 256; i++)
    {
        uT += i * hist[i];
        wT += hist[i];
    }

    for (size_t i = 0; i < 256; i++)
    {
        w0 += hist[i];
        w1 = wT - w0;

        sum += i * hist[i];
        u0 = sum / w0;
        u1 = (uT - sum) / w1;

        // Maximizing inter-class variance
        vk = w0 * w1 * (u0 - u1) * (u0 - u1);

        // Find max vk = Find threshold
        if (vk > v_max)
        {
            threshold = i;
            v_max = vk;
        }
    }

    return (Uint8)threshold;
}

void otsu(SDL_Surface *image_surface)
{
    size_t width = image_surface->w;
    size_t height = image_surface->h;

    float hist[256] = {0
    };
    init_hist(image_surface, width, height, hist);
    normalized(hist, width * height);

    Uint8 threshold = otsu_threshold(hist);
    Uint8 r, g, b;

    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < height; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            if (r > threshold)
                r = 0;
            else
                r = 255;

            pixel = SDL_MapRGB(image_surface->format, r, r, r);
            put_pixel(image_surface, i, j, pixel);
        }
   }
}

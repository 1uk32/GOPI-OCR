#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <math.h>
#include <ctype.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "in_grayscale.c"
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

GtkWidget* OCRWindow;
GtkWidget* OCRFixed;
GtkWidget* image2grid;
int gridMode = 0;
int filed = 0;
int transformed = 0;
GtkWidget* transform_solve;
GtkWidget* rotate;
GtkWidget* save;
GtkWidget* main_image;
GtkWidget* itworks;
GtkWidget* import;
GtkBuilder* builder;

int main(int argc, char *argv[])
{
  gtk_init(&argc, &argv);

  builder = gtk_builder_new_from_file("OCR_GUI.glade");

  OCRWindow = GTK_WIDGET(gtk_builder_get_object(builder, "OCRWindow"));

  g_signal_connect(OCRWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_builder_connect_signals(builder, NULL);

  OCRFixed = GTK_WIDGET(gtk_builder_get_object(builder, "OCRFixed"));
  image2grid = GTK_WIDGET(gtk_builder_get_object(builder, "image2grid"));
  transform_solve = GTK_WIDGET(gtk_builder_get_object(builder, "transform_solve"));
  rotate = GTK_WIDGET(gtk_builder_get_object(builder, "rotate"));
  save = GTK_WIDGET(gtk_builder_get_object(builder, "save"));
  main_image = GTK_WIDGET(gtk_builder_get_object(builder, "main_image"));
  itworks = GTK_WIDGET(gtk_builder_get_object(builder, "itworks"));
  import = GTK_WIDGET(gtk_builder_get_object(builder, "import"));

  gtk_widget_show(OCRWindow);
  gtk_widget_hide(save);

  gtk_main();

  return EXIT_SUCCESS;
}

void on_image2grid_state_set()
{
  if (gridMode)
    {
      gridMode = 0;
      gtk_widget_hide(save);
      gtk_widget_show(import);
      gtk_button_set_label(GTK_BUTTON(transform_solve), (const gchar* ) "Transform");
      gtk_widget_show(rotate);
      if (filed)
	{
	  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(import)), NULL);
	  GdkPixbuf* newpixbuf = gdk_pixbuf_scale_simple (pixbuf, 244, 244, GDK_INTERP_NEAREST);
	  gtk_image_set_from_pixbuf ((GtkImage*)main_image, newpixbuf);
	}
    }
  else
    {
      gridMode = 1;
      gtk_widget_show(save);
      gtk_widget_hide(import);
      gtk_button_set_label(GTK_BUTTON(transform_solve), (const gchar* ) "Solve");
      gtk_widget_hide(rotate);
      if (filed)
	{
	  char cmd[2048];
	  sprintf(cmd, "./save_grid %s %s %s", "grid_00", "empty_2.png", "1");
	  system(cmd);
	  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("grid_00.result.png", NULL);
	  GdkPixbuf* newpixbuf = gdk_pixbuf_scale_simple (pixbuf, 244, 244, GDK_INTERP_NEAREST);
	  gtk_image_set_from_pixbuf ((GtkImage*)main_image, newpixbuf);
	}
    }
  
}

void on_import_file_set(GtkFileChooserButton *f)
{
  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(f)), NULL);
  GdkPixbuf* newpixbuf = gdk_pixbuf_scale_simple (pixbuf, 244, 244, GDK_INTERP_NEAREST);
  gtk_image_set_from_pixbuf ((GtkImage*)main_image, newpixbuf);
  filed = 1;
}

void on_transform_solve_clicked()
{
  if (gridMode)
    {
      char cmd[2048];
	  sprintf(cmd, "./save_grid %s %s %s", "grid_00", "empty_2.png", "2");
	  system(cmd);
	  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("grid_00.result.png", NULL);
	  GdkPixbuf* newpixbuf = gdk_pixbuf_scale_simple (pixbuf, 244, 244, GDK_INTERP_NEAREST);
	  gtk_image_set_from_pixbuf ((GtkImage*)main_image, newpixbuf);
    }
  else
    {
      if (filed)
	{
	  if (SDL_Init(SDL_INIT_VIDEO) != 0)
	    errx(EXIT_FAILURE, "%s", SDL_GetError());
	  SDL_Window* window = SDL_CreateWindow("Window", 0, 0,10,10,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	  if (window == NULL)
	    errx(EXIT_FAILURE, "%s", SDL_GetError());
	  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	  if (renderer == NULL)
	    errx(EXIT_FAILURE, "%s", SDL_GetError());
	  SDL_Surface* s = load_image(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(import)));
	  int w = s->w;
	  int h = s->h;
	  SDL_SetWindowSize(window, w, h);
	  otsu(s);
	  IMG_SavePNG(s, "grayscale.png");
	  SDL_FreeSurface(s);
	  SDL_DestroyRenderer(renderer);
	  SDL_DestroyWindow(window);
	  SDL_Quit();

	  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("grayscale.png", NULL);
	  GdkPixbuf* newpixbuf = gdk_pixbuf_scale_simple (pixbuf, 244, 244, GDK_INTERP_NEAREST);
	  gtk_image_set_from_pixbuf ((GtkImage*)main_image, newpixbuf);
	  transformed = 1;
	}
    }
}

void on_rotate_clicked()
{
  if (filed && transformed)
    {
      char cmd[2048];
      sprintf(cmd, "./image_manipulation %s %s", "grayscale.png", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(import)));
      system(cmd);
      GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("bin_rotated.png", NULL);
      GdkPixbuf* newpixbuf = gdk_pixbuf_scale_simple (pixbuf, 244, 244, GDK_INTERP_NEAREST);
      gtk_image_set_from_pixbuf ((GtkImage*)main_image, newpixbuf);
    }
}

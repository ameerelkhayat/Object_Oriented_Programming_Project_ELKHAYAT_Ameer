#include <gtkmm/drawingarea.h>
#include <cmath>

#include "graphic_gui.h"
#include "graphic.h"

#define MAX_INTENSITY 1.0
#define MIN_INTENSITY 0.0
#define X_ORIGIN 0.0
#define Y_ORIGIN 0.0

using namespace std;

//Varibales and Internal functions prototypes

namespace
{
	const Cairo::RefPtr<Cairo::Context>* ptcr (nullptr);    //Variables
	
	WP wp_ref;
	
	int width, height;
			
	void transformer_coordonnees (double& x, double& y);    //Functions
								  
	void transformer_rayon (double& rayon);
	
	void set_couleur (int code_couleur);	
}

//External functions definitions

void space_graphic_gui::graphic_set_context (const Cairo::RefPtr<Cairo::Context>& cr)
{
	static bool init (false);
	if (!init)
	{
		ptcr = &cr;
		init = true;
	}
}

void space_graphic_gui::set_window_parameters (WP wp)
{
	wp_ref = wp;
}

void space_graphic_gui::set_width_height (const int w, const int h)
{
	width = w;
	height = h;
}	

void space_graphic_gui::color_background_white ()
{
	set_couleur (BLANC);
	(*ptcr) -> move_to (X_ORIGIN, Y_ORIGIN);
	(*ptcr) -> line_to (width, Y_ORIGIN);
	(*ptcr) -> line_to (width, height);
	(*ptcr) -> line_to (X_ORIGIN, height);
	(*ptcr) -> line_to (X_ORIGIN, Y_ORIGIN);
	(*ptcr) -> fill ();
	(*ptcr) -> stroke ();
}

void space_graphic::dessine_cercle (double x, double y, double rayon,
									int code_couleur)
{	
	transformer_coordonnees (x, y);
	transformer_rayon (rayon);
	
	set_couleur (BLANC);						//Colorie l'arriere plan du cercle
	(*ptcr) -> arc (x, y, rayon, 0, 2*M_PI);    //en blanc
	(*ptcr) -> fill ();
	
	set_couleur (code_couleur);					//Trace le cercle 
	(*ptcr) -> arc (x, y, rayon, 0, 2*M_PI);
	
	(*ptcr) -> stroke ();
}

void space_graphic::dessine_segment (double x1, double y1, double x2, double y2,
									 int code_couleur)
{
	transformer_coordonnees (x1, y1);
	transformer_coordonnees (x2, y2);
	
	set_couleur (code_couleur);		
	(*ptcr) -> move_to (x1, y1);
	(*ptcr) -> line_to (x2, y2);
	(*ptcr) -> stroke ();
}

//Internal Functions definitions

namespace
{
	void transformer_coordonnees (double& x, double& y)
	{
		x = width * (x - wp_ref.x_min)  / (wp_ref.x_max - wp_ref.x_min);
		y = height * (wp_ref.y_max - y) / (wp_ref.y_max - wp_ref.y_min);
	}
		
	void transformer_rayon (double& rayon)
	{		
		rayon = rayon * width / (wp_ref.x_max - wp_ref.x_min);
	}
		
	void set_couleur (int code_couleur)
	{
		switch (code_couleur)
		{
			case NOIR :
				(*ptcr) -> set_source_rgb(MIN_INTENSITY, MIN_INTENSITY, MIN_INTENSITY);
				break;
				
			case ROUGE :
				(*ptcr) -> set_source_rgb(MAX_INTENSITY, MIN_INTENSITY, MIN_INTENSITY);
				break;
				
			case VERT :
				(*ptcr) -> set_source_rgb(MIN_INTENSITY, MAX_INTENSITY, MIN_INTENSITY);
				break;
				
			case BLANC :
				(*ptcr) -> set_source_rgb(MAX_INTENSITY, MAX_INTENSITY, MAX_INTENSITY);
				break;
				
			default :
				(*ptcr) -> set_source_rgb(MIN_INTENSITY, MIN_INTENSITY, MIN_INTENSITY);
				break;
		}
	}
}



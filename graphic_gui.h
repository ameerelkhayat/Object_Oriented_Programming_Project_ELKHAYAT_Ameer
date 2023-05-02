/**
 * Architecture adoptée : b1
 */

#ifndef GRAPHIC_GUI_HEADER_H
#define GRAPHIC_GUI_HEADER_H

#include <gtkmm/drawingarea.h>

//~ #include "graphic.h"

struct WP //Window parameters
{
	double x_min;
    double x_max;
    double y_min;
    double y_max;
    int height;
    double asp; // aspect ratio
};

//External function prototypes used only in the gui module

namespace space_graphic_gui
{
	void graphic_set_context (const Cairo::RefPtr<Cairo::Context>& cr);
		
	void set_window_parameters (WP wp);
	
	void set_width_height (const int width, const int height);
			
	void color_background_white ();
}

#endif



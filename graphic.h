/**
 * Architecture adoptée : b1
 */

#ifndef GRAPHIC_HEADER_H
#define GRAPHIC_HEADER_H

#include "graphic_color.h"

//External functions prototypes used only in the model

namespace space_graphic
{
	void dessine_cercle (double x, double y, double rayon, int code_couleur);
						 
	void dessine_segment (double x1, double y1, double x2, double y2,
						  int code_couleur);
}

#endif



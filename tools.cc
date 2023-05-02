#include <cmath>

#include "tools.h"
#include "graphic.h"

using namespace std;


//Internal functions prototypes

namespace
{
	Segment projection_orthogonale (const Segment& v, const Segment& u);
	
	double produit_scalaire (const Segment& v1, const Segment& v2);	
}

//Method definitions

S2d Segment::get_end () const
{                               
	double x (point.x + vecteur.x);
	double y (point.y + vecteur.y);
	S2d end ( {x,y} );      
	return end;
}

double Segment::norme () const
{
	return sqrt ( pow (vecteur.x, 2) + pow (vecteur.y, 2) );
}

//External functions definitions

bool space_tools::intersection_cercle_cercle (const Cercle& c1, const Cercle& c2,
                                              double condition)
{
	Segment distance (space_tools::creer_ligne (c1.get_centre (), c2.get_centre () ));
	
	if ( distance.norme () <= ( c1.get_rayon () + c2.get_rayon () + condition ) )
	{
		return false;
	} else { return true;
	}
}

//Fonction qui crée une variable de type de Segment entre deux points de types
//S2d

Segment space_tools::creer_ligne (const S2d& p1, const S2d& p2)
{
	double x1 (p2.x - p1.x);
	double y1 (p2.y - p1.y);
	Segment ligne (p1, {x1, y1});
	
	return ligne;
}

bool space_tools::intersection_cercle_segment (const Cercle& c, const Segment& ligne,
											   double condition)
{	
	Segment vect ( space_tools::creer_ligne ( ligne.get_point (), c.get_centre () ) );
	
	Segment proj ( projection_orthogonale (ligne, vect) );
	
	double x2 ( ( vect.get_vecteur () ).x - ( proj.get_vecteur () ).x );
	double y2 ( ( vect.get_vecteur () ).y - ( proj.get_vecteur () ).y );
	
	Segment segment1 = proj;
	Segment segment2 ( space_tools::creer_ligne (ligne.get_end (), proj.get_end () ) );
	
	Segment normale ( c.get_centre (), {x2, y2} );
	
	if ( normale.norme () <= c.get_rayon () + condition )
	{
		if ( ( segment1.norme () <= ligne.norme () ) and
		     ( segment2.norme () <= ligne.norme () ) )
		{ 
			return false;
		}
	}
	return true;
}

void space_tools::dessine_cercle (const Cercle& c, int code_couleur)
{
	double x ( ( c.get_centre () ).x );
	double y ( ( c.get_centre () ).y );
	double rayon ( c.get_rayon () );
	
	space_graphic::dessine_cercle (x, y, rayon, code_couleur);	
}

void space_tools::dessine_segment (const Segment& s, int code_couleur)
{
	double x1 ( ( s.get_point () ).x );
	double y1 ( ( s.get_point () ).y );
	
	double x2 ( ( s.get_end () ).x );
	double y2 ( ( s.get_end () ).y );
	
	space_graphic::dessine_segment (x1, y1, x2, y2, code_couleur);	
}

//Internal functions definitions

namespace
{	
	Segment projection_orthogonale (const Segment& v, const Segment& u)
	{
		double coeff ( produit_scalaire (v, u) / pow ( v.norme (), 2 ) );
		double new_x ( coeff * ( v.get_vecteur () ).x );
		double new_y ( coeff * ( v.get_vecteur () ).y );
		Segment proj ( v.get_point (), { new_x, new_y } );
	
		return proj;
	}
		
	double produit_scalaire (const Segment& v1, const Segment& v2)
	{
		return ( ( v1.get_vecteur () ).x ) * ( ( v2.get_vecteur () ).x )
			  +( ( v1.get_vecteur () ).y ) * ( ( v2.get_vecteur () ).y );
	}
}


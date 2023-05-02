/**
 * Architecture adoptée : b1
 */

#ifndef TOOLS_HEADER_H
#define TOOLS_HEADER_H

#include "tools_s2d.h"
#include "graphic_color.h"

class Cercle
{
		
public :

  Cercle (S2d centre = {0., 0.}, double rayon = 0.) //constructeur
  : centre(centre), rayon (rayon) {}          
	
  S2d get_centre () const { return centre; }        //getters
  double get_rayon () const { return rayon; }       
	
private :

  S2d centre;
  double rayon;
};

class Segment
{	
public :

  Segment (S2d point = {0., 0.}, S2d vecteur = {0., 0.}) //constructeur
  : point(point), vecteur(vecteur) {}
	
  S2d get_point () const { return point; }               //getters
  S2d get_vecteur () const { return vecteur; }           
	
  S2d get_end () const;
  double norme () const;
	
private :

  S2d point;
  S2d vecteur;	
};


//External functions prototypes

namespace space_tools
{
	bool intersection_cercle_cercle (const Cercle& c1, const Cercle& c2, double val);

	Segment creer_ligne (const S2d& p1, const S2d& p2);

	bool intersection_cercle_segment (const Cercle& c, const Segment& ligne,
	                                  double condition);
	                                  
	void dessine_cercle (const Cercle& c, int code_couleur);

	void dessine_segment (const Segment& s, int code_couleur);
}

#endif
                                  

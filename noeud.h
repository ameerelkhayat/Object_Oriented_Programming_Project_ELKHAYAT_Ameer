/**
 * Architecture adoptée : b1
 */

#ifndef NOEUD_HEADER_H
#define NOEUD_HEADER_H

#include <string>
#include <vector>

#include "tools.h"

class Noeud
{
	
public :

  //Constructeur 
  Noeud (std::string type, unsigned int uid, unsigned int nbp, Cercle body);
  	    
  std::string get_type () const { return type; }     		  //getters
  unsigned int get_uid () const { return uid; }  		      
  unsigned int get_nbp () const { return nbp; } 			 
  Cercle get_body () const { return body; }     			  
  std::vector <Noeud*> get_liens () const { return liens; }   
  double get_access () const { return access; }             
  bool get_in () const { return in; }                       
  unsigned int get_parent () const { return parent; }
  std::vector <Noeud*> get_spp () const { return spp; }
  std::vector <Noeud*> get_spt () const { return spt; }	  
  
  void set_nbp (unsigned int new_nbp);						  //setters
  void set_body (Cercle cercle);							 
  void set_liens (std::vector <Noeud*> liste);				  
  void set_access (double temps);    						   
  void set_in (bool status);                                  
  void set_parent (unsigned int p);
  void set_spp (std::vector <Noeud*> liste_p);
  void set_spt (std::vector <Noeud*> liste_T);
   
  void ajouter_lien (Noeud* n);
  void supprimer_lien (size_t index);
  void supprimer_tous_liens ();		

private :

  std::string type;      				//standard attributes
  unsigned int uid;
  unsigned int nbp;
  Cercle body;
  std::vector <Noeud*> liens;
  
  double access;   						//dijkstra specific attributes
  bool in;
  unsigned int parent;
  
  std::vector <Noeud*> spp;
  std::vector <Noeud*> spt;
};

//External functions prototypes

namespace space_noeud
{	
	void decodage_ligne (std::string line, std::vector <Noeud*>& liste_noeud);		
	
	void reset_parametres_decodage ();
	
	void reset_checker ();
	
	bool give_checker (const std::vector <Noeud*>& liste_noeud);

	void dessine_logement (const Noeud& n, int code_couleur);
	
	void dessine_transport (const Noeud& n, int code_couleur);
	
	void dessine_production (const Noeud& n, int code_couleur);
	
	void add_noeud_in_file (const Noeud& n, std::ofstream& fichier);
	
	void edit_node_nbp (Noeud* n, unsigned int nbp);
	
	void edit_node_body (Noeud* n, Cercle body);
	
	void edit_node_liens (Noeud* n, std::vector <Noeud*> liens);
}

#endif

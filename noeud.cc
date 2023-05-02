#include <iostream>
#include <cmath>
#include <sstream>
#include <fstream>

#include "noeud.h"
#include "error.h"
#include "constantes.h"

#define RATIO_LONGEUR 0.75		//Utilisées pour la création du rectangle d'un noeud
#define RATIO_HAUTEUR 1./8.		//de production

using namespace std;

enum Etat_lecture {NB0, LOGEMENT,		//Differentes valeurs pour les cases du switch
				   NB1, TRANSPORT,		//de decodage_ligne
				   NB2, PRODUCTION,
				   NB3, LIENS, FIN};

//Variables and internal function prototypes

namespace
{
	int etat (NB0);  		//Variables utilisées dans decodage_ligne
	int i (0);
	int total (0); 
	
	bool checker (true); 	//Variable qui vérifie la présence d'une erreur dans
							//liste_noeud
	
	//Fonction qui rajoute des pointeurs sur des noeuds dans liste_noeud		
	
	void ajouter_noeud (string type, unsigned uid, double x, double y, unsigned nbp,
                        vector <Noeud*>& liste_noeud);
	
	//Fonctions qui vérifient la présence de l'erreur associée et appelle la fonction
	//associée du module error
	
	bool check_identical_uid (const vector <Noeud*>& liste_noeud);
	
	void check_link_vaccum (unsigned int uid_ref);
	
	bool check_max_link (const vector <Noeud*>& liste_noeud);
	
	bool check_multiple_same_link (const vector <Noeud*>& liste_noeud);
	
	bool check_node_link_superposition (const vector <Noeud*>& liste_noeud);
	
	bool check_node_node_superposition (const vector <Noeud*>& liste_noeud);
	
	bool check_reserved_uid (const vector <Noeud*>& liste_noeud);
	
	void check_self_link_node (unsigned int uid_ref);
	
	bool check_too_little_capacity (const vector <Noeud*>& liste_noeud);
	
	bool check_too_much_capacity (const vector <Noeud*>& liste_noeud);
	
	void check_errors (const std::vector <Noeud*>& liste_noeud);
	
	//Cette fonction est appelée dans link_handler et vérifie si un uid, correspondant
	//à un lien existe.
	
	void check_existence (unsigned int uid_ref, bool& uid_ref_existe,
						  size_t& index, vector <Noeud*>& liste_noeud);
	
	//Cette est fonction est appelée dans decodage_ligne. Elle vérifie la présence de
	//certaines erreurs et rajoute un lien aux noeuds associées dans le cas échéant.
	
	void link_handler (unsigned int origine, unsigned int arivee,
	                   vector <Noeud*>& liste_noeud);
}

//Constructor definition

Noeud::Noeud (std::string type, unsigned int uid, unsigned int nbp, Cercle body) :
	type (type),
	uid (uid),
	nbp (nbp),
	body (body),
	access (infinite_time),
	in (true),
    parent (no_link)
{
}

//Method definition

void Noeud::set_nbp (unsigned int new_nbp)
{
	nbp = new_nbp;
}

void Noeud::set_body (Cercle cercle)
{
	body = cercle;
}

void Noeud::set_liens (vector <Noeud*> liste)
{
	liens = liste;
}

void Noeud::set_access (double temps)
{
	access = temps;
}

void Noeud::set_in (bool status)
{
	in = status;
}

void Noeud::set_parent (unsigned int p)
{
	parent = p;
}

void Noeud::ajouter_lien (Noeud* n)
{
	liens.push_back (n);
};

void Noeud::supprimer_lien (size_t index)
{
	if (liens.size () == 1)
	{
		liens.clear ();
	} else
	{
		swap ( liens[index], liens.back () );
		liens.pop_back ();
	}
}

void Noeud::supprimer_tous_liens ()
{
	liens.clear ();
}

void Noeud::set_spp (std::vector <Noeud*> liste_p)
{
	spp = liste_p;
}

void Noeud::set_spt (std::vector <Noeud*> liste_t)
{
	spt = liste_t;
}

//External functions definitions

//Cette fonction est appelée par la fonction lecture et permet l'initialisation
//du vector liste_neoud en utilisant les fonctions ajouter_noeud et liens_function

//FONCTION ENTRE 40 ET 80 LIGNES 

void space_noeud::decodage_ligne (string line, vector <Noeud*>& liste_noeud)
{
	istringstream data (line);
					   
	string type;
	unsigned int uid (0);
	double x (0.0);
	double y (0.0);
	unsigned int nbp (0.0);
	unsigned int origine (0);
	unsigned int arrivee (0);

	switch (etat) {
		
	case NB0: 
		if (data >> total) i = 0;
		if (total == 0) {
			etat = NB1;
		} else { etat = LOGEMENT ; }  
		break;
		
	case LOGEMENT:
		if (data >> uid >> x >> y >> nbp) ++i; 
		type = "LOGEMENT";   
		ajouter_noeud (type, uid, x, y, nbp, liste_noeud); //creation et pushback d'un
		                                                   //noeud dans liste_noeud
		if (i == total) etat = NB1;
		break;
		
	case NB1 : 
		if (data >> total) i=0;
		if (total == 0) {
			etat = NB2;
		} else { etat = TRANSPORT; } 
		break;
		
	case TRANSPORT : 
		if (data >> uid >> x >> y >> nbp) ++i;
		type = "TRANSPORT";
		ajouter_noeud (type, uid, x, y, nbp, liste_noeud);
		if (i == total) etat = NB2;
		break;
		
	case NB2: 
		if (data >> total) i = 0;
		if (total == 0) {
			etat = NB3;
		} else { etat = PRODUCTION; }
		break;
		
	case PRODUCTION: 
		if (data >> uid >> x >> y >> nbp) ++i; 
		type = "PRODUCTION";
		ajouter_noeud (type, uid, x, y, nbp, liste_noeud);
		if (i == total) etat = NB3;
		break;
		
	case NB3: 
		if (data >> total) i = 0;
		if (total == 0) {
			etat = FIN;
		} else { etat = LIENS; } 
	    break;
	    
	case LIENS:
		if (data >> origine >> arrivee)   //origine et arrivee sont deux uid qui ont 
		{                                 //un lien entre eux
			link_handler (origine, arrivee, liste_noeud); //appel de link_handler
			++i;
		}
		if (i == total) etat = FIN;
		break;
		
	case FIN :
		cout << "Noeuds creees fin de decodage" <<endl;
		break;
	}
}

void space_noeud::reset_parametres_decodage ()
{
	etat = NB0;
	i = 0;
	total = 0;
}

void space_noeud::reset_checker ()
{
	checker = true;
}

bool space_noeud::give_checker (const vector <Noeud*>& liste_noeud)
{
	check_errors (liste_noeud);
	return checker;
}

void space_noeud::dessine_logement (const Noeud& n, int code_couleur)
{
	space_tools::dessine_cercle (n.get_body (), code_couleur);
}

void space_noeud::edit_node_nbp (Noeud* n, unsigned int nbp)
{
	n -> set_nbp (nbp);
}

void space_noeud::edit_node_body (Noeud* n, Cercle body)
{
	n -> set_body (body);
}

void space_noeud::edit_node_liens (Noeud* n, vector <Noeud*> liens)
{
	n -> set_liens (liens);
}
				
void space_noeud::dessine_transport (const Noeud& n, int code_couleur)	 
{
	space_tools::dessine_cercle (n.get_body (), code_couleur);
		
	double rayon ( ( n.get_body () ).get_rayon () );
	double xc ( ( (n.get_body () ).get_centre () ).x );
	double yc ( ( (n.get_body () ).get_centre () ).y );
		
	//Création de 8 points séparés de la même distance et situé sur le cercle du
	//noeud transport
		
	S2d A ( {xc, yc + rayon} );
	S2d B ( {xc + cos (M_PI/4) * rayon, yc + sin (M_PI/4) * rayon} );
	S2d C ( {xc + rayon, yc} );
	S2d D ( {xc + cos (M_PI/4) * rayon, yc - sin (M_PI/4) * rayon} );
	S2d E ( {xc, yc - rayon} );	
	S2d F ( {xc - cos (M_PI/4) * rayon, yc - sin (M_PI/4) * rayon} );
	S2d G ( {xc - rayon, yc} );
	S2d H ( {xc - cos (M_PI/4) * rayon, yc + sin (M_PI/4) * rayon} );
		
	//Dessin des quatre diamètres séparé d'un même angle de 45°
		
	space_tools::dessine_segment (space_tools::creer_ligne (A, E), code_couleur);									  
	space_tools::dessine_segment (space_tools::creer_ligne (B, F), code_couleur);									  
	space_tools::dessine_segment (space_tools::creer_ligne (C, G), code_couleur);									  
	space_tools::dessine_segment (space_tools::creer_ligne (D, H), code_couleur);		
}
			 
void space_noeud::dessine_production (const Noeud& n, int code_couleur)
{
	space_tools::dessine_cercle (n.get_body (), code_couleur);
		
	//Détermination des sommets du rectangle d'un noeud production
			
	S2d centre ( ( n.get_body () ).get_centre () );
	double diametre (2 * ( ( n.get_body () ).get_rayon () ) );
				
	S2d sommet1 ( {centre.x + (RATIO_LONGEUR / 2)*diametre,
				   centre.y + (RATIO_HAUTEUR / 2)*diametre} );
				   
	S2d sommet2 ( {sommet1.x - RATIO_LONGEUR * diametre, sommet1.y} );
	S2d sommet3 ( {sommet2.x, sommet2.y - RATIO_HAUTEUR*diametre} );
	S2d sommet4 ( {sommet3.x + RATIO_LONGEUR * diametre, sommet3.y} );
		
	//Dessin du rectangle d'un noeud production
		
	space_tools::dessine_segment (space_tools::creer_ligne (sommet1, sommet2),
				 code_couleur);
	space_tools::dessine_segment (space_tools::creer_ligne (sommet2, sommet3),
				 code_couleur);									  
	space_tools::dessine_segment (space_tools::creer_ligne (sommet3, sommet4),
				 code_couleur);
	space_tools::dessine_segment (space_tools::creer_ligne (sommet4, sommet1),
				 code_couleur);				
}

void space_noeud::add_noeud_in_file (const Noeud& n, std::ofstream& fichier)
{
	unsigned int uid ( n.get_uid () );
	unsigned int nbp ( n.get_nbp () );
	double x ( ( ( n.get_body () ).get_centre () ).x );
	double y ( ( ( n.get_body () ).get_centre () ).y );
	
	fichier << "	" << uid << " " << x << " " << y << " " << nbp <<endl;
}

//Internal functions definitions

namespace
{
	void ajouter_noeud (string type, unsigned uid, double x, double y, unsigned nbp,
						vector <Noeud*>& liste_noeud)
	{
		Cercle c ( {x, y}, sqrt (nbp) );
		
		Noeud* n ( new Noeud (type, uid, nbp, c) );
		
		liste_noeud.push_back (n); 
	}
	
	bool check_identical_uid (const vector <Noeud*>& liste_noeud)
	{
		unsigned int uid_ref;
	
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{		
			uid_ref = liste_noeud[i] -> get_uid ();
		
			for (size_t j (0); j < liste_noeud.size (); ++j) {
				if (i != j) {
					if (liste_noeud[j] -> get_uid () == uid_ref)
					{
						cout << error::identical_uid (uid_ref) <<endl;
						return false;
					}
				}
			}
		}
		return true;
	}
	
	void check_link_vaccum (unsigned int uid_ref)
	{
		cout << error::link_vacuum (uid_ref) <<endl;
	}
	
	bool check_max_link (const vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if (liste_noeud[i] -> get_type () == "LOGEMENT")
			{
				vector <Noeud*> liens ( liste_noeud[i] -> get_liens () );
				
				if (liens.size () > max_link)
				{
					cout << error::max_link ( liste_noeud[i] -> get_uid () ) <<endl;
					return false;
				}
			}
		}
		return true;
	}
	
	bool check_multiple_same_link (const vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			vector <Noeud*> liens ( liste_noeud[i] -> get_liens () );
			
			unsigned int uid_ref ( liste_noeud[i] -> get_uid () );
		
			for (size_t j (0); j < liens.size (); ++j) {				
				for (size_t k (0); k < liens.size (); ++k) {
					if (j != k) {
						if ( liens[j] -> get_uid () == liens[k] -> get_uid () )
						{
							cout << error::multiple_same_link (uid_ref, 
															   liens[j] -> get_uid ()) 
								 <<endl;
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	
	bool check_node_link_superposition (const vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			unsigned int uid1 ( liste_noeud[i] -> get_uid () );
			
			Cercle c1 ( liste_noeud[i] -> get_body () );
			
			vector <Noeud*> liens ( liste_noeud[i]->get_liens () );
			
			for (size_t j (0); j < liens.size (); ++j)
			{
				unsigned int uid2 ( liens[j] -> get_uid () );
				
				Cercle c2 ( liens[j] -> get_body () );
				
				Segment connexion ( space_tools::creer_ligne ( c1.get_centre (),
				                                               c2.get_centre () ) );
				for (size_t k (0); k < liste_noeud.size (); ++k) {
					if ( ( liste_noeud[k] -> get_uid () != uid1 )  and
						 ( liste_noeud[k] -> get_uid () != uid2 ) )
					{
						if ( ! (space_tools::intersection_cercle_segment
						                    ( liste_noeud[k] -> get_body (),
									          connexion, null ) ) ) 
						{
							cout << error::node_link_superposition
										   ( liste_noeud[k] -> get_uid () )
							     <<endl;
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	
	bool check_node_node_superposition (const vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			Cercle c1 ( liste_noeud[i] -> get_body () );
		
			for (size_t j (0); j < liste_noeud.size (); ++j) {
				if (i != j)
				{
					Cercle c2 ( liste_noeud[j] -> get_body () );
					if ( ! (space_tools::intersection_cercle_cercle
					                     ( c1, c2, null ) ) )
					{
						cout << error::node_node_superposition 
						               ( liste_noeud[i] -> get_uid (),
						                 liste_noeud[j] -> get_uid () )
						     <<endl;
						return false;
					}
				}
			}
		}
		return true;
	}
	
	bool check_reserved_uid (const vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if (liste_noeud[i] -> get_uid () == no_link)
			{
				cout << error::reserved_uid () <<endl;
				return false;
			}
		}
		return true;
	}
		
	void check_self_link_node (unsigned int uid_ref)
	{
		cout << error::self_link_node (uid_ref) <<endl;
	}

	bool check_too_little_capacity (const vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if (liste_noeud[i] -> get_nbp () < min_capacity)
			{
				cout << error::too_little_capacity ( liste_noeud[i] -> get_nbp () )
					 <<endl;
				return false;
			}
		}
		return true;
	}
	
	bool check_too_much_capacity (const vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if (liste_noeud[i] -> get_nbp () > max_capacity)
			{
				cout << error::too_much_capacity (liste_noeud[i] -> get_nbp ()) <<endl;
				return false;
			}
		}
		return true;
	}
		
	void check_existence(unsigned int uid_ref, bool& uid_ref_existe,
						 size_t& index, vector <Noeud*>& liste_noeud)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if ( uid_ref == ( liste_noeud[i] -> get_uid () ) )
			{
				uid_ref_existe = true;
				index = i;
				break;
			}
		}
	}
		
	void check_errors (const vector <Noeud*>& liste_noeud)
	{	
		if ( not ( check_identical_uid (liste_noeud) ) ) checker = false;
	
		if ( not ( check_max_link (liste_noeud) ) ) checker = false;
	
		if ( not ( check_multiple_same_link (liste_noeud) ) ) checker = false;
	
		if ( not ( check_node_link_superposition (liste_noeud) ) ) checker = false;
	
		if ( not ( check_node_node_superposition (liste_noeud) ) ) checker = false;
	
		if ( not ( check_reserved_uid (liste_noeud) ) ) checker = false;
	
		if ( not ( check_too_little_capacity (liste_noeud) ) ) checker = false;
	
		if ( not ( check_too_much_capacity (liste_noeud) ) ) checker = false;
	}
		
	void link_handler (unsigned int origine, unsigned int arrivee,
	                   vector <Noeud*>& liste_noeud)
	{
		bool origine_existe (false);
		bool arrivee_existe (false);
		size_t index1 (0);
		size_t index2 (0);
		
		if (origine == arrivee)
		{
			check_self_link_node (origine);
			checker = false;
		}
		check_existence (origine, origine_existe, index1, liste_noeud);
		check_existence (arrivee, arrivee_existe, index2, liste_noeud);
		
		if ( not (origine_existe) ) 
		{
			check_link_vaccum (origine);
			checker = false;
		}
		if ( not (arrivee_existe) )
		{
			check_link_vaccum (arrivee);
			checker = false;
		}
		liste_noeud[index1] -> ajouter_lien ( liste_noeud[index2] ); 
		liste_noeud[index2] -> ajouter_lien ( liste_noeud[index1] );
	}
}





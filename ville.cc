#include <iostream>
#include <cmath>
#include <fstream>

#include "ville.h"
#include "noeud.h"
#include "error.h"
#include "constantes.h"

using namespace std;

class Ville  //Class definition
{
	
public :
	
  vector <Noeud*> get_liste_noeud () const { return liste_noeud; } //getter
  	
  void initialisation (vector <Noeud*> liste); 
	
  void add_noeud (Noeud* n);

  void remove_noeud (size_t i);
	
  void add_lien (size_t index, Noeud* n);
  
  void supprimer_liste_noeud ();
		
private :
	
  vector <Noeud*> liste_noeud; 
};

struct LinkPair         //This structure is used to find the unique links that exist
{						//between the nodes. It is used mainly to create a file and 
	unsigned int uid1;  //save it
	unsigned int uid2;
};





//Variables and internal functions prototypes

namespace
{	
	Ville city; //Variables
	
	Noeud* clicked_node_ptr (nullptr);
		
	void dessine_tous_liens (); //Functions
	
	void dessine_tous_noeuds ();
	
	int node_type_quantity (string type);
	
	vector <LinkPair> create_all_link_pairs ();
	
	bool check_pair_duplicate (const vector <LinkPair>& tab_lp, const LinkPair& lp);
	
	void create_housing_section (ofstream& fichier);
	
	void create_transport_section (ofstream& fichier);
	
	void create_production_section (ofstream& fichier);
	
	void create_link_section (ofstream& fichier);
	
	vector <LinkPair> determine_unique_link_pairs ();
	
	void sort(vector<Noeud*> liste_noeud, vector<int>& TA, int LV);
	
	unsigned int trouver_debut ( vector<Noeud*> liste_noeud, int LV, vector<int> TA );
	
	unsigned int trouver_fin ( vector<Noeud*> liste_noeud, int LV, vector<int> TA );

	void push_cases( vector<int>& TA, int LV, unsigned int case_debut,
					 unsigned int case_fin );

	void first_sort( vector<Noeud*> liste_noeud, vector<int>& TA );

	double compute_access ( vector<Noeud*> liste_noeud, int n, Noeud* voisin );

	void first_compute (vector<Noeud*>& liste_noeud, unsigned int d);

	void fill_TA( vector<int>& TA );

	int trouver_index ( vector<int> TA, vector<Noeud*> liste_noeud, Noeud*voisin );
	
	void check_voisins ( vector<int>& TA, vector<Noeud*>liste_noeud, 
						 unsigned int n, Noeud* Vj );
	
	void reset_sp (Noeud* noeud);
	
	void fill_sp (string type, int n, vector<Noeud*> liste_noeud );
	
	void MTA_initialisation ( vector<Noeud*>& liste_noeud, vector<int>& TA, int d );					 

	double MTA_production ( unsigned int d );

	double MTA_transport ( unsigned int d );
	
	unsigned int chercher_nouveau_uid ();
	
	unsigned int chercher_noeud (double x, double y);
	
	bool node_node_superposition (Noeud* n1, Noeud* n2);
	
	bool node_link_superposition (Noeud* n1, Noeud* n2, Noeud* n_entre);
		
	void determine_size_variation (Noeud* n, Segment add_size);
	
	void show_shortest_noeuds (vector<Noeud*> sp);
	
	void show_shortest_liens (vector<Noeud*> sp);
}






//Method definitions

void Ville::initialisation (std::vector <Noeud*> liste)
{
	liste_noeud = liste;
}

void Ville::add_noeud (Noeud* n)
{
	liste_noeud.push_back (n); 
}

void Ville::remove_noeud (size_t i)
{
	if (liste_noeud.size () == 1)
	{
		delete liste_noeud[i];
		liste_noeud.pop_back ();
	} else if (liste_noeud.size () > 1)
	{	
		vector <Noeud*> liens_ref = liste_noeud[i] -> get_liens ();
		unsigned int uid_ref = liste_noeud[i] -> get_uid ();
		
		for (size_t j (0); j < liens_ref.size (); ++j)
		{
			vector <Noeud*> liens_voisin = liens_ref[j] -> get_liens ();
			
			for (size_t k (0); k < liens_voisin.size (); ++k) {
				if (liens_voisin[k] -> get_uid () == uid_ref)
				{
					liens_ref[j] -> supprimer_lien (k);
				}
			}
		}
		liens_ref.clear ();
		swap (liste_noeud[i], liste_noeud.back () );
		delete liste_noeud [liste_noeud.size () - 1];
		liste_noeud.pop_back ();
	}
}

void Ville::add_lien (size_t index, Noeud* n)
{
	liste_noeud[index] -> ajouter_lien (n);
}

void Ville::supprimer_liste_noeud ()
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		liste_noeud[i] -> supprimer_tous_liens ();
	}
	liste_noeud.clear ();
}






//External functions definitions 

void space_ville_lecture::lecture (char* nom_fichier)
{
	vector <Noeud*> liste_noeud;
	
    string line;
    ifstream fichier (nom_fichier);
    
    space_noeud::reset_parametres_decodage ();
    space_noeud::reset_checker ();
    
    if ( !fichier.fail () )
    {
        while ( getline (fichier >> ws, line) ) 
        {
			if (line[0] == '#') continue;
			
			space_noeud::decodage_ligne (line, liste_noeud);
        }
	}
	
	if ( space_noeud::give_checker (liste_noeud) )
	{
		cout << error::success () ;
		city.initialisation (liste_noeud);
	}
	
	fichier.close ();
}

void space_ville::dessine_ville ()
{
	dessine_tous_liens ();
	dessine_tous_noeuds ();
}

void space_ville::supprimer_ville ()
{
	city.supprimer_liste_noeud ();
}

void space_ville::file_creator (const char* filename)
{
	ofstream fichier (filename);
	fichier << " # file generated by projet" << "\n" << "\n" << "\n" ;
	
	create_housing_section (fichier);
	create_transport_section (fichier);
	create_production_section (fichier);
	create_link_section (fichier);
	
	fichier.close ();
}

double space_ville::calcule_ENJ ()
{
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	
	if (liste_noeud.size () == 0) return 0;
	
	double nbpL (0);
	double nbpT_P (0);
	
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		if (liste_noeud [i] -> get_type () == "LOGEMENT")
			nbpL += liste_noeud[i] -> get_nbp ();
			
		if (liste_noeud[i] -> get_type () != "LOGEMENT")
			nbpT_P += liste_noeud[i] -> get_nbp ();
	}
	
	return (nbpL - nbpT_P) / (nbpL + nbpT_P);
}

double space_ville::calcule_CI ()
{
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	vector <LinkPair> unique_lp = determine_unique_link_pairs ();
	double CI_accumulator (0);
	
	for (size_t i (0); i < unique_lp.size (); ++i)
	{
		unsigned int uid1 (unique_lp [i].uid1);
		unsigned int uid2 (unique_lp [i].uid2);
		for (size_t j (0); j < liste_noeud.size (); ++j) {
			if (liste_noeud[j] -> get_uid () == uid1)
			{
				vector <Noeud*> liens = liste_noeud[j] -> get_liens ();
				for (size_t k (0); k < liens.size (); ++k) {
					if (liens[k] -> get_uid () == uid2)
					{
						S2d origine ( ( liste_noeud[j] -> get_body () ).get_centre() );
						S2d arrivee ( ( liens[k] -> get_body () ).get_centre () );
						Segment connexion(space_tools::creer_ligne (origine, arrivee));
						
						double min_capacity (0);
						if ((liste_noeud[j] -> get_nbp ()) < (liens[k] -> get_nbp ()))
						{
							min_capacity = liste_noeud[j] -> get_nbp ();
						} else { min_capacity = liens[k] -> get_nbp (); }
						
						double speed (default_speed);
						if ( ( ( liste_noeud[j] -> get_type () ) == "TRANSPORT" ) and
							 ( ( liens[k] -> get_type () ) == "TRANSPORT" ) )
								speed = fast_speed;
						CI_accumulator += connexion.norme () * min_capacity * speed;
					}
				}
			}
		}
	}
	return CI_accumulator;
}
			
double space_ville::calcule_MTA ()
{ 
	vector<Noeud*> liste_noeud = city.get_liste_noeud ();
	double total ( null );
	double nombre_noeuds ( null );
	
	for ( size_t i(0); i < liste_noeud.size (); ++i ) { 
		if ( liste_noeud [i] -> get_type () == "LOGEMENT" )
		{ 
			total += ( MTA_transport ( i ) + MTA_production ( i ) );
			nombre_noeuds = nombre_noeuds + 1;
		}
	}
	if ( nombre_noeuds == 0 ) return null;
	
	return (total/nombre_noeuds);
}

//Verifies errors before adding node

void space_ville::add_default_node (string type, double x, double y)
{
	unsigned int uid ( chercher_nouveau_uid () );
	unsigned int nbp (min_capacity);
	Cercle body ({x, y}, sqrt (nbp));
		
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	
	for (auto& element : liste_noeud)
	{
		if ( !space_tools::intersection_cercle_cercle (element -> get_body (), body,
				   									   dist_min) )
		{
			cout << error::node_node_superposition (element -> get_uid (), uid);
			return;
		}
		
		vector <Noeud*> liens = element -> get_liens ();
		
		for (auto& voisin : liens)
		{
			Segment connexion (space_tools::creer_ligne 
									    ( ( element -> get_body () ).get_centre (),
										  ( voisin -> get_body () ).get_centre () ) );
			
			if (!space_tools::intersection_cercle_segment (body, connexion, dist_min))
			{
				cout << error::node_link_superposition (uid);
				return;
			}
		}
	}
	Noeud* n (new Noeud (type, uid, nbp, body));
	city.add_noeud (n);
}

bool space_ville::check_inside_node (double x, double y)
{
	S2d p1 ({x, y});
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	
	for (auto element : liste_noeud)
	{
		S2d p2 (  ( element -> get_body () ).get_centre () );
		double rayon ( ( element -> get_body () ).get_rayon () );
		Segment distance ( space_tools::creer_ligne (p1, p2) );
		
		if (distance.norme () < rayon) return true;
	}
	return false;
}

void space_ville::colorier_noeud_rouge (double x, double y)
{
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	unsigned int uid ( chercher_noeud (x, y) );
	
	for (size_t i (0); i < liste_noeud.size (); ++i) {
		if (liste_noeud[i] -> get_uid () == uid)
		{
			clicked_node_ptr = liste_noeud[i];
			
			if  ( ( clicked_node_ptr -> get_type () ) == "LOGEMENT" )
				space_noeud::dessine_logement (*( clicked_node_ptr ), ROUGE);
				
			if  ( ( clicked_node_ptr -> get_type () ) == "TRANSPORT" )
				space_noeud::dessine_transport (*( clicked_node_ptr ), ROUGE);
				
			if  ( ( clicked_node_ptr -> get_type () ) == "PRODUCTION" )
				space_noeud::dessine_production (*( clicked_node_ptr ), ROUGE);
		}
	}
}

void space_ville::show_shortest_path (double x, double y)
{ cout<< " start of show shortest path "<<endl;
	
	unsigned int uid ( chercher_noeud (x, y) );
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		if (liste_noeud[i] -> get_uid () == uid)
		{
			clicked_node_ptr = liste_noeud[i];
		}
	}
	
	if ( clicked_node_ptr -> get_type () == "LOGEMENT" )
	{	
	vector<Noeud*> spp = clicked_node_ptr -> get_spp ();
	vector<Noeud*> spt = clicked_node_ptr -> get_spt ();
	
	if ( spp.size() != null )
	{
		show_shortest_liens (spp);
		show_shortest_noeuds (spp);
	}
	
	if ( spt.size() != null )
	{
		show_shortest_liens (spt);
		show_shortest_noeuds (spt);
	}
	
	}cout<< "end of show shortest path "<<endl;
}
	
	
	
	
	

bool space_ville::in_selected_node (double x, double y)
{
	if (chercher_noeud (x, y) == clicked_node_ptr -> get_uid ()) return true;
	
	return false;
}

void space_ville::delete_clicked_node (double x, double y)
{
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	
	for (size_t i (0); i < liste_noeud.size (); ++i) {
		if (liste_noeud[i] -> get_uid () == clicked_node_ptr -> get_uid ())
			city.remove_noeud (i);
	}
}

//Verifies errors before changing clicked_node_ptr's position, returns true if no
//errors are found

bool space_ville::change_node_position (double x_nouveau, double y_nouveau)
{
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	double rayon = ( clicked_node_ptr -> get_body () ).get_rayon ();
	Cercle new_body ({x_nouveau, y_nouveau}, rayon);
	Noeud* temporary_node ( new Noeud (clicked_node_ptr -> get_type (),
									   clicked_node_ptr -> get_uid (),
									   clicked_node_ptr -> get_nbp (),
									   new_body) );
	
	space_noeud::edit_node_liens (temporary_node, clicked_node_ptr -> get_liens ());
	
	for (auto& element : liste_noeud) {
		if (element -> get_uid () != clicked_node_ptr -> get_uid ())
		{
			if (node_node_superposition (element, temporary_node))
				return false;
			
			vector <Noeud*> liens = element -> get_liens ();
			
			for (auto& voisin : liens) {
				if (voisin != clicked_node_ptr)
				{
					if (node_link_superposition (element, voisin, temporary_node))
						return false;
				}
			}
			for (auto& voisin : temporary_node -> get_liens ()) {
				if (element -> get_uid () != voisin -> get_uid ())
				{
					if (node_link_superposition (temporary_node, voisin, element))
						return false;
				}		
			}
		}
	}
	space_noeud::edit_node_body (clicked_node_ptr, new_body);
	return true;
}

bool space_ville::check_link_existence (double x, double y)
{
	vector <Noeud*> liens1 = clicked_node_ptr -> get_liens ();
	unsigned int uid2 ( chercher_noeud (x, y) );
		
	for (size_t j (0); j < liens1.size (); ++j) {
		if (liens1[j] -> get_uid () == uid2) return true;
	}	
	return false;
}

void space_ville::remove_old_link (double x, double y)
{
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	unsigned int uid2 ( chercher_noeud (x, y) );
	
	vector <Noeud*> liens1 = clicked_node_ptr -> get_liens ();
			
	for (size_t i (0); i < liens1.size (); ++i) {
		if (liens1[i] -> get_uid () == uid2)
			clicked_node_ptr -> supprimer_lien (i);
	}		
	
	for (size_t i (0); i < liste_noeud.size (); ++i) {
		if (liste_noeud[i] -> get_uid () == uid2)
		{
			vector <Noeud*> liens2 = liste_noeud[i] -> get_liens ();
			for (size_t j (0); j < liens2.size (); ++j) {
				if (liens2[j] -> get_uid () == clicked_node_ptr -> get_uid ())
					liste_noeud[i] -> supprimer_lien (j);
			}
		}
	}
}

bool space_ville::link_amount_possible (double x, double y)
{
	unsigned int uid (chercher_noeud (x, y));
	
	if ( (clicked_node_ptr -> get_type () == "LOGEMENT" and
		  clicked_node_ptr -> get_liens ().size () < max_link) or 
		 (clicked_node_ptr -> get_type () != "LOGEMENT") )
	{		
		for (auto& element : city.get_liste_noeud ()) {
			if (element -> get_uid () == uid)
			{
				if ( (element -> get_type () == "LOGEMENT" and
					  element -> get_liens ().size () < max_link) or 
					 (element -> get_type () != "LOGEMENT") )
				{
					return true;
				}
			}
		}
	} else
	{
		cout << error::max_link (clicked_node_ptr -> get_uid ());
		return false;
	}
	cout << error::max_link (uid) ;
	return false;
}	

//Verifies errors between new link and other existing nodes

void space_ville::create_new_link (double x, double y)
{
	vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	unsigned int uid1 ( clicked_node_ptr -> get_uid () );
	unsigned int uid2 ( chercher_noeud (x, y) );
	size_t index2 (0);
	
	for (size_t i (0); i < liste_noeud.size (); ++i) {
		if (liste_noeud[i] -> get_uid () == uid2) index2 = i;
	}
	
	Segment connexion (space_tools::creer_ligne
								 ((clicked_node_ptr -> get_body () ).get_centre(),
								  (liste_noeud[index2] -> get_body () ).get_centre()));
	for (auto& element : liste_noeud) {
		if ( (element -> get_uid () != uid1) and (element -> get_uid () != uid2) )
		{
			if (!space_tools::intersection_cercle_segment
							 (element -> get_body (), connexion, dist_min))
			{
				cout << error::node_link_superposition (element -> get_uid ());
				return;
			}
		}
	}
	
	clicked_node_ptr -> ajouter_lien ( liste_noeud[index2] );
	liste_noeud[index2] -> ajouter_lien ( clicked_node_ptr );
}

//Verifies errors between new potential node and other existing nodes/links

void space_ville::change_node_size (double x1, double y1, double x2, double y2)
{
	Segment add_size (space_tools::creer_ligne ({x1, y1}, {x2, y2}));
	Noeud* temporary_node ( new Noeud (clicked_node_ptr -> get_type (),
									   clicked_node_ptr -> get_uid (),
									   clicked_node_ptr -> get_nbp (),
									   clicked_node_ptr -> get_body () ) );
	
	determine_size_variation (temporary_node, add_size); //Changes size for temporary_
														 //node accordingly
	bool verifier (true);
	//Verification of errors with temporary_node
	for (auto& element : city.get_liste_noeud ()) {
		if (element -> get_uid () != temporary_node -> get_uid ()) {
			if (node_node_superposition (element, temporary_node))
			{
				verifier = false;
				return;
			}
		
			for (auto& voisin : element -> get_liens ()) {
				if (voisin -> get_uid () != temporary_node -> get_uid ()) {
					if (node_link_superposition (element, voisin, temporary_node))
					{
						verifier = false;
						return;
					}
				}
			}
		}
	}
	//Changes clicked_node_ptr's size if no errors have been found
	if (verifier) {
		unsigned int new_nbp= int(pow ((temporary_node->get_body ().get_rayon ()), 2));
		space_noeud::edit_node_nbp (clicked_node_ptr, new_nbp);
		space_noeud::edit_node_body (clicked_node_ptr, temporary_node -> get_body ());
	}
}







//Internal function definition

namespace
{	
	void dessine_tous_liens ()
	{
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
			
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			Noeud* n_origine ( liste_noeud [i] );
			const S2d point_origine ( ( n_origine -> get_body () ).get_centre () );
		
			vector <Noeud*> liens = n_origine -> get_liens ();
		
			for (size_t j (0); j < liens.size (); ++j)
			{
				const S2d point_arrivee ( (  liens[j] -> get_body () ).get_centre () );
				
				space_tools::dessine_segment
				             (space_tools::creer_ligne (point_origine, point_arrivee),
										   NOIR);
			}
		}
	}
		
	void dessine_tous_noeuds ()
	{
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			if  ( ( liste_noeud[i] -> get_type () ) == "LOGEMENT" )
				space_noeud::dessine_logement (*( liste_noeud[i] ), NOIR);
		
			if  ( ( liste_noeud[i] -> get_type () ) == "TRANSPORT" )
				space_noeud::dessine_transport (*( liste_noeud[i] ), NOIR);
		
			if  ( ( liste_noeud[i] -> get_type () ) == "PRODUCTION" )
				space_noeud::dessine_production (*( liste_noeud[i] ), NOIR);
		}
	}
	
	void show_shortest_noeuds (vector<Noeud*> sp)
	{cout<< " start of show shortest noeuds "<<endl;
		for (size_t i (1); i < sp.size(); ++i) // i(1) car noeud de depart reste rouge
		{
			Noeud* n ( sp [i] );
			
			if  ( ( n -> get_type () ) == "LOGEMENT" )
				space_noeud::dessine_logement (*n, VERT);
				
			if  ( ( n -> get_type () ) == "TRANSPORT" )
				space_noeud::dessine_transport (*n, VERT);
				
			if  ( ( n -> get_type () ) == "PRODUCTION" )
				space_noeud::dessine_production (*n, VERT);
		}cout<< " end of show shortest noeuds "<<endl;
	}
	
	void show_shortest_liens (vector<Noeud*> sp)
	{ cout<< " start of show shortest liens "<<endl;
		for (size_t i (0); i < (sp.size()-1); ++i)
		{
			const S2d point_origine ( (  sp[i] -> get_body () ).get_centre () );
			const S2d point_arrivee ( (  sp[ (i+1) ] -> get_body () ).get_centre () );	
			space_tools::dessine_segment
				             (space_tools::creer_ligne (point_origine, point_arrivee),
										   VERT);
		}cout<< " end of show shortest liens "<<endl;
	}	
		
	int node_type_quantity (string type)
	{
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
		int counter (0);
		
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if (liste_noeud[i] -> get_type () == type) ++counter;
		}
		
		return counter; 
	}
		
	vector <LinkPair> create_all_link_pairs ()
	{
		vector <LinkPair> all_lp;
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
		
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			unsigned int uid1 = liste_noeud[i] -> get_uid ();
			vector <Noeud*> liens = liste_noeud[i] -> get_liens ();
			
			for (size_t j (0); j < liens.size (); ++j) 
			{
				LinkPair lp ( {uid1, liens[j] -> get_uid ()} );
				all_lp.push_back (lp);
			}
		}
		return all_lp;
	}
		
	bool check_pair_duplicate (const vector <LinkPair>& tab_lp, const LinkPair& lp)
	{
		for (size_t i (0); i < tab_lp.size (); ++i)
		{
			if (tab_lp[i].uid1 == lp.uid1 and tab_lp[i].uid2 == lp.uid2) return false;
			if (tab_lp[i].uid1 == lp.uid2 and tab_lp[i].uid2 == lp.uid1) return false;
		}
		return true;
	}
		
	vector <LinkPair> determine_unique_link_pairs ()
	{
		vector <LinkPair> all_lp = create_all_link_pairs ();
		vector <LinkPair> unique_lp;
		
		for (size_t i (0); i < all_lp.size (); ++i) {
			if ( check_pair_duplicate (unique_lp, all_lp[i]) )
				unique_lp.push_back (all_lp[i]);
		}
		return unique_lp;
	}
		
	void create_housing_section (ofstream& fichier)
	{
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
		
		fichier << "# nb housing" <<endl;
		fichier << node_type_quantity ("LOGEMENT") <<endl;
		fichier << "# housing" <<endl;
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			if (liste_noeud[i] -> get_type () == "LOGEMENT")
				space_noeud::add_noeud_in_file (*( liste_noeud[i] ), fichier);
		}
		
		fichier << "\n" ;
	}
		
	void create_transport_section (ofstream& fichier)
	{
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
		
		fichier << "# nb transport" <<endl;
		fichier << node_type_quantity ("TRANSPORT") <<endl;
		fichier << "# transport" <<endl;
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if (liste_noeud[i] -> get_type () == "TRANSPORT")
				space_noeud::add_noeud_in_file (*( liste_noeud[i] ), fichier);
		}
		
		fichier << "\n" ;
	}
		
	void create_production_section (ofstream& fichier)
	{
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
		
		fichier << "# nb production" <<endl;
		fichier << node_type_quantity ("PRODUCTION") <<endl;
		fichier << "# production" <<endl;
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if (liste_noeud[i] -> get_type () == "PRODUCTION")
				space_noeud::add_noeud_in_file (*( liste_noeud[i] ), fichier);
		}
		
		fichier << "\n" ;
	}
		
	void create_link_section (ofstream& fichier)
	{
		vector <LinkPair> unique_lp = determine_unique_link_pairs ();	
		fichier << "# nb link" <<endl;
		fichier << unique_lp.size () <<endl;
		for (size_t i (0); i < unique_lp.size (); ++i)
		{
			fichier << "	" << unique_lp[i].uid1 << " " << unique_lp[i].uid2 <<endl;
		}
	}
	
	
	
	void sort(vector<Noeud*> liste_noeud, vector<int>& TA, int LV)
	{ 
		unsigned int case_debut ( 0 );
		unsigned int case_fin ( 0 );
	
		case_debut = trouver_debut (liste_noeud, LV, TA);
		case_fin = trouver_fin (liste_noeud, LV, TA);
	
		push_cases ( TA, LV, case_debut, case_fin );
	}

	unsigned int trouver_debut (vector<Noeud*> liste_noeud, int LV, vector<int> TA)
	{
		for (size_t i (0); i < liste_noeud.size(); ++i) {
			if ( liste_noeud [LV] -> get_access () <= 
				 liste_noeud [TA [i]] -> get_access () )
			{
				return i;
			}
		}
		return 0;
	}

	unsigned int trouver_fin (vector<Noeud*> liste_noeud, int LV, vector<int> TA)
	{
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			if ( liste_noeud [ LV ] -> get_uid () == 
				 liste_noeud [ TA [i] ] -> get_uid() )
			{
				return i; 
			}
		}
		return 0;
	}
	
	void push_cases (vector<int>& TA, int LV, unsigned int case_debut,
					 unsigned int case_fin)
	{ 
		int temp (LV);
		int un (1);
	
		for (size_t i (0); i < (case_fin - case_debut); ++i)
		{
			TA [case_fin - i] = TA [case_fin - i - un];
		}
	
		TA [case_debut] = temp;
	}

	void first_sort (vector<Noeud*> liste_noeud, vector<int>& TA)
	{ 
		int temp (0);
		
		for (size_t i (0); i < liste_noeud.size (); ++i) {
			for (size_t j (i); j < liste_noeud.size(); ++j) {
				if ( liste_noeud[ TA[i] ] -> get_access () > 
				     liste_noeud[ TA[j] ] -> get_access () )
				{
					temp = TA [i];
					TA [i] = TA [j];
					TA [j] = temp;
				}
			}
		}
	}

	double compute_access (vector<Noeud*> liste_noeud, int n, Noeud* voisin)  
	{
		S2d centre1 = ( liste_noeud [n] -> get_body () ).get_centre ();
		
		S2d centre2 = ( voisin -> get_body () ).get_centre ();
		
		
		Segment ligne = space_tools::creer_ligne (centre1 , centre2);
	
		if ( liste_noeud [n] -> get_type () == "TRANSPORT" and
			 voisin -> get_type () == "TRANSPORT" )
		{  
			return  ligne.norme () / fast_speed ;
		}
	
		return  ligne.norme () / default_speed ;
	
	}

	void first_compute (vector<Noeud*>& liste_noeud, unsigned int d)
	{
		vector <Noeud*> liens = liste_noeud [d] -> get_liens () ;
		
		for (size_t i (0); i < liens.size(); ++i)
		{ 
			liens [i] -> set_access ( compute_access ( liste_noeud, d, liens [i] ) );
			liens [i] -> set_parent ( d );
		}
	
	}

	void fill_TA (vector<int>& TA)
	{ 
		for (size_t i (0); i < TA.size (); ++i)
		{
			TA [i] = i;
		}
	}

	int trouver_index (vector<int> TA, vector<Noeud*> liste_noeud, Noeud*voisin)
	{
		for (size_t i (1); i < liste_noeud.size (); ++i) {
			if ( liste_noeud[ TA [i] ] -> get_uid () == voisin -> get_uid () )
			{
				return TA [i];
			}
		}
		return null;
	}

	void reset_liste ( vector<Noeud*>& liste_noeud )
	{
		for (size_t i (0); i < liste_noeud.size (); ++i)
		{
			liste_noeud [i] -> set_access (infinite_time);
			liste_noeud [i] -> set_in (true);
			liste_noeud [i] -> set_parent (no_link);
		}
	}
	 
	void check_voisins (vector<int>& TA, vector<Noeud*>liste_noeud, unsigned int n,
	Noeud* Vj)
	{
		double alt;
		alt = liste_noeud [n] -> get_access () + compute_access (liste_noeud, n, Vj);
		
		if (Vj -> get_access () > alt)
		{
			Vj -> set_access (alt);
			Vj -> set_parent (n);
			sort ( liste_noeud, TA, trouver_index ( TA, liste_noeud, Vj ) );
		}
	}
	
	void reset_sp (Noeud* noeud)
	{
		vector<Noeud*> spp_temp ( noeud -> get_spp () );
		vector<Noeud*> spt_temp ( noeud -> get_spt () );
		
		for (unsigned int i (0); i < ( spp_temp.size () ); ++i)
		{
			spp_temp.pop_back ();
		}
		for (unsigned int i (0); i < ( spt_temp.size () ); ++i)
		{
			spt_temp.pop_back ();
		}
		noeud -> set_spp ( spp_temp );
		noeud -> set_spt ( spt_temp );
	}
	
	void fill_sp (string type, int n, vector<Noeud*> liste_noeud )
	{ cout<< " start of fill_sp "<<endl;
		Noeud* noeud = liste_noeud [n];
		reset_sp (noeud);
		vector<Noeud*> liste;
		unsigned int compteur (0); 
		
		for (unsigned int i (0); i == compteur; ++i)
		{
			liste.insert ( liste.begin(), noeud );
			
			if (not (noeud -> get_parent() == no_link) )
			{
				noeud = liste_noeud [ noeud -> get_parent() ];
				++compteur;
			}
		}
		// for testing
		if (type == "p")
		{ cout<< "production path is"<<endl;
			for ( unsigned int i (0); i< liste.size(); ++i ) {
				cout<< liste[i] -> get_uid() <<endl;
				}
			
			noeud -> set_spp (liste);
		}
		if (type == "t")
		{cout<< "transport path is"<<endl;
			for ( unsigned int i (0); i< liste.size(); ++i ) {cout<< liste[i] -> get_uid()<<endl;}
			
			noeud -> set_spt (liste);
		}cout<< " end of fill_sp "<<endl;
		//
	}
		
	void MTA_initialisation (vector<Noeud*>& liste_noeud, vector<int>& TA, int d)
	{
		fill_TA (TA);
		liste_noeud [d] -> set_access (null);
	
		first_compute (liste_noeud, d);
		first_sort (liste_noeud, TA);
	}
	 
	double MTA_production ( unsigned int d )
	{
		unsigned int n ( 0 );
	
		vector<Noeud*> liste_noeud = city.get_liste_noeud ();
		vector<int> TA (liste_noeud.size (), null);
		MTA_initialisation (liste_noeud, TA, d);
	
		for (size_t i (1); i < liste_noeud.size (); ++i) { // 1 car en ignore d
			if( liste_noeud [ TA[i] ] -> get_in () == true )
			{
				n = TA [i];
			
				if (liste_noeud [n] -> get_type () == "PRODUCTION")
				{
					fill_sp ("p", n, liste_noeud);
					double temps = liste_noeud [n] -> get_access ();
					reset_liste (liste_noeud);
					return temps;
				}
			
				liste_noeud [n] -> set_in ( false );
			
				vector<Noeud*> V = liste_noeud [ n ] -> get_liens ();
			
				for ( size_t j ( 0 ); j < V.size(); ++j ) {
					if ( V [j] -> get_in () == true and 
						 V [j] -> get_uid () != liste_noeud [d] -> get_uid () ) 
					{
						check_voisins ( TA, liste_noeud, n, V [j] );
						
					}
				}
			}
		}
		reset_liste ( liste_noeud );
		return infinite_time;
	} 
		
	double MTA_transport ( unsigned int d )
	{
		unsigned int n (0);
		vector<Noeud*> liste_noeud = city.get_liste_noeud ();
		vector<int> TA (liste_noeud.size (), null);
		MTA_initialisation (liste_noeud, TA, d);
	
		for (size_t i (1); i < liste_noeud.size (); ++i) { // 1 car en ignore d
			if (liste_noeud [ TA[i] ] -> get_in () == true)
			{
				n = TA [i];
				
				if (liste_noeud [n] -> get_type () == "TRANSPORT")
				{
					fill_sp ("t", n, liste_noeud);
					double temps = liste_noeud [n] -> get_access ();
					reset_liste (liste_noeud);
					return temps;
				}
			
				liste_noeud [n] -> set_in (false);
			
				if (liste_noeud [n] -> get_type () != "PRODUCTION")
				{
					vector<Noeud*> V = liste_noeud [ TA[i] ] -> get_liens ();
				
					for (size_t j (0); j < V.size (); ++j) {
						if ( V [j] -> get_in () == true and 
							 V [j] -> get_uid () != liste_noeud [d] -> get_uid () ) 
						{
							check_voisins ( TA, liste_noeud, n, V [j] );
						}
					}
				}
			}
		}
		reset_liste ( liste_noeud );
		return infinite_time;
	}
	
	unsigned int chercher_nouveau_uid ()
	{
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
		unsigned int nouveau_uid (1);
		bool unique (false);
		
		do
		{
			unique = true;
						
			for (size_t i (0); i < liste_noeud.size (); ++i)
			{
				if (liste_noeud[i] -> get_uid () == nouveau_uid)
				{
					unique = false;
					++nouveau_uid;
				}
			}
		} while (!unique);
		
		return nouveau_uid;
	}
	
	unsigned int chercher_noeud (double x, double y)
	{
		S2d p1 ({x, y});
		vector <Noeud*> liste_noeud = city.get_liste_noeud ();
	
		for (auto element : liste_noeud)
		{
			unsigned int uid ( element -> get_uid () );
			S2d p2 (  ( element -> get_body () ).get_centre () );
			double rayon ( ( element -> get_body () ).get_rayon () );
			Segment distance ( space_tools::creer_ligne (p1, p2) );
		
			if (distance.norme () < rayon) return uid;
		}
		return chercher_nouveau_uid (); //Si le point (x, y) n'appartient pas à city,
	                                    //on renvoie un uid qui correspond à aucun des
	}									//des noeuds existants
	
	bool node_node_superposition (Noeud* n1, Noeud* n2)
	{
		if (!space_tools::intersection_cercle_cercle (n1 -> get_body (),
													  n2 -> get_body (), dist_min))
		{
			cout << error::node_node_superposition (n1 -> get_uid (),n2 -> get_uid ());
			return true;
		}
		return false;
	}
	
	bool node_link_superposition (Noeud* n1, Noeud* n2, Noeud* n_entre)
	{
		Segment connexion = space_tools::creer_ligne (n1 -> get_body ().get_centre (),
													  n2 -> get_body ().get_centre ());
													  
		if (!space_tools::intersection_cercle_segment (n_entre -> get_body (),
													   connexion, dist_min))
		{
			cout << error::node_link_superposition (n_entre -> get_uid ());
			return true;
		}
		return false;
	}
	
	void determine_size_variation (Noeud* n, Segment add_size)
	{
		S2d centre = ( n -> get_body () ).get_centre ();
		S2d p1 = add_size.get_point ();
		S2d p2 = add_size.get_end ();
		
		Segment dist1 = space_tools::creer_ligne (centre, p1);
		Segment dist2 = space_tools::creer_ligne (centre, p2);
		
		if (dist1.norme () < dist2.norme ())
		{
			double new_rayon = (n -> get_body () ).get_rayon () + add_size.norme ()/2;
			if (new_rayon > sqrt (max_capacity))
			{
				Cercle new_body ({ (n -> get_body ()).get_centre ().x,
								   (n -> get_body ()).get_centre ().y},
								    sqrt (max_capacity));
				space_noeud::edit_node_body (n, new_body);
			} else {
				Cercle new_body ({ (n -> get_body ()).get_centre ().x,
								   (n -> get_body ()).get_centre ().y},
								    new_rayon);
				space_noeud::edit_node_body (n, new_body);
			}
		} else {
			double new_rayon = (n -> get_body () ).get_rayon () - add_size.norme ()/2;
			if (new_rayon < sqrt (min_capacity))
			{
				Cercle new_body ({ (n -> get_body ()).get_centre ().x,
								   (n -> get_body ()).get_centre ().y},
								    sqrt (min_capacity));
				space_noeud::edit_node_body (n, new_body);
			} else {
				Cercle new_body ({ (n -> get_body ()).get_centre ().x,
								   (n -> get_body ()).get_centre ().y},
								    new_rayon);
				space_noeud::edit_node_body (n, new_body);
			}
		}
	}
}






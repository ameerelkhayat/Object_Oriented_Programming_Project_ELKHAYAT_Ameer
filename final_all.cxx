#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>         

//prototype tools
Segment creer_ligne (const S2d& p1, const S2d& p2);
bool intersection_cercle_cercle (const Cercle& c1, const Cercle& c2, const double& val);
double produit_scalaire (const Segment& v1, const Segment& v2);
Segment projection_orthogonale (const Segment& v, const Segment& u);
Segment creer_ligne (const S2d& p1, const S2d& p2);
bool intersection_cercle_segment (const Cercle& c, const Segment& ligne, double val);

struct S2d
{
	double x;
	double y;
};

class Cercle
{
		
public :

	Cercle (S2d centre = {0., 0.}, double rayon = 0.)
	: centre(centre), rayon (rayon) {}
	S2d get_centre () const { return centre; }
	double get_rayon () const { return rayon; }
	
private :

	S2d centre;
	double rayon;
	
};

class Segment
{
	
public :

	Segment (S2d point = {0., 0.}, S2d vecteur = {0., 0.})
	: point(point), vecteur(vecteur) {}
	double norme () const { return sqrt (pow(vecteur.x, 2) +pow(vecteur.y, 2)); }
	S2d get_point () const { return point; };
	S2d get_vecteur () const { return vecteur; };
	
private :

	S2d point;
	S2d vecteur;
	
};

//prototype noeud

class Noeud
{
	
public :

	Noeud (string type, unsigned int uid, unsigned int nbp, Cercle body) //Constructeur 
	       :type(type), uid (uid), nbp(nbp), body(body) {}; 
	string get_type () const { return type; }; 
	unsigned int get_uid () const { return uid; }; //getter
	unsigned int get_nbp () const { return nbp; }; //
	Cercle get_body () const { return body; };     //
	vector <Noeud*> get_liens () const { return liens; };
	void ajouter_lien (Noeud* n) { liens.push_back (n); };
	
private :

	string type;
	unsigned int uid;
	unsigned int nbp;
	Cercle body;
	vector <Noeud*> liens;

};


//prototype ville
class Ville
{
	
public :
	
	void ajouter_noeud (const Noeud& n)  { liste_noeud.push_back (n); };
	void enlever_noeud (unsigned int i)
	{
		swap (liste_noeud[i], liste_noeud.back ());
		liste_noeud.pop_back ();
	};
	vector <Noeud> get_liste_noeud () const { return liste_noeud; };
	void add_lien (size_t index, Noeud* n) { liste_noeuds[index].ajouter_lien(n); };
		
private :
	
	vector <Noeud> liste_noeud;
};


//prototype error
bool check_identical_uid (const vector <Noeud>& liste_noeud);
bool check_link_vaccum (const vector <Noeud>& liste_noeud);
bool check_max_link (const vector <Noeud>& liste_noeud);
bool check_multiple_same_link (const vector <Noeud>& liste_noeud);
bool check_node_link_superposition (const vector <Noeud>& liste_noeud);
bool check_node_node_superposition (const vector <Noeud>& liste_noeud);
bool check_reserved_uid (const vector <Noeud>& liste_noeud);
bool check_self_link_node (const vector <Noeud>& liste_noeud);
bool check_too_little_capacity (const vector <Noeud>& liste_noeud);
bool check_too_much_capacity (const vector <Noeud>& liste_noeud);
void check_errors();

//prototype automate de lecture
void lecture(char * nom_fichier);
void decodage_ligne(string line);

//prototype error

namespace error
{
	// Two nodes have the same uid that is supposed to be unique.
	std::string identical_uid(unsigned int uid);
	
	// One of the nodes indicated for a link does not exist.
	std::string link_vacuum(unsigned int uid);
	
	// A housing node number of links exceeds the allowed maximum number 
	std::string max_link(unsigned int uid);
	
	// The same link is defined several times
	std::string multiple_same_link(unsigned int uid1, unsigned int uid2);
	
	// A node overlap a link 
	std::string node_link_superposition(unsigned int uid);
	
	// Two nodes overlap 
	std::string node_node_superposition(unsigned int uid1, unsigned int uid2);
	
	// A node is using the reserved uid
	std::string reserved_uid();
	
	// Everything went well => file reading and all validation checks
	std::string success();
	
	// A link refers twice to the same node
	std::string self_link_node(unsigned int uid);
	
	// A node that has too little capacity
	std::string too_little_capacity(unsigned int capacity);
	
	// A node that has too much capacity
	std::string too_much_capacity(unsigned int capacity);
}








using namespace std;
int main(int argc, char **argv)
{
	
	return 0;
}











//tools functions
bool intersection_cercle_cercle (const Cercle& c1, const Cercle& c2, const double& val)
{
	Segment distance (creer_ligne (c1.get_centre (), c2.get_centre ()));
	
	if (distance.norme () <= (c1.get_rayon () + c2.get_rayon() + val))
	{
		return false;
	} else { return true;
	}
};

/////////////////////////////////////////////////////////////////////

double produit_scalaire (const Segment& v1, const Segment& v2)
{
	return ((v1.get_vecteur ()).x)*((v2.get_vecteur ()).x)
	      +((v1.get_vecteur ()).y)*((v2.get_vecteur ()).y);
};

/////////////////////////////////////////////////////////////////////

Segment projection_orthogonale (const Segment& v, const Segment& u)
{
	double coeff (produit_scalaire(v, u)/pow(v.norme (), 2));
	double new_x (coeff*(v.get_vecteur ()).x);
	double new_y (coeff*(v.get_vecteur ()).y);
	Segment proj (v.get_point(), {new_x, new_y});
	
	return proj;
};

/////////////////////////////////////////////////////////////////////

Segment creer_ligne (const S2d& p1, const S2d& p2)
{
	double x1 (p2.x - p1.x);
	double y1 (p2.y - p1.y);
	Segment ligne (p1, {x1, y1});
	
	return ligne;
};

/////////////////////////////////////////////////////////////////////

bool intersection_cercle_segment (const Cercle& c, const Segment& ligne, double val)
{	
	Segment vect (creer_ligne(ligne.get_point (), c.get_centre ()));
	
	Segment proj (projection_orthogonale (ligne, vect));
	
	double x2 ((vect.get_vecteur ()).x - (proj.get_vecteur ()).x);
	double y2 ((vect.get_vecteur ()).y - (proj.get_vecteur ()).y);
	
	Segment normale (c.get_centre (), {x2, y2});
	
	if (normale.norme () <= c.get_rayon () + val)
	{
		return false;
	} else { return true;
	}
};


//error functions
//mega function
void check_errors(){
	if(!check_identical_uid(city.get_liste_noeud())){
		exit(0);
	};
	
	if(!check_link_vaccum(city.get_liste_noeud())){
		exit(0);
	};
		if(!check_max_link(city.get_liste_noeud())){
		exit(0);
	};
	
		if(!check_multiple_same_link(city.get_liste_noeud())){
		exit(0);
	};
	
		if(!check_node_link_superposition(city.get_liste_noeud())){
		exit(0);
	};
	
		if(!check_node_node_superposition(city.get_liste_noeud())){
		exit(0);
	};
	
		if(!check_reserved_uid(city.get_liste_noeud())){
		exit(0);
	};
	
		if(!check_self_link_node(city.get_liste_noeud())){
		exit(0);
	};
	
		if(!check_too_little_capacity(city.get_liste_noeud())){
		exit(0);
	};
	
		if(!check_too_much_capacity(city.get_liste_noeud())){
		exit(0);
	};
	
	cout<<error::success;
}
	
//aly's errors
bool check_identical_uid (const vector <Noeud>& liste_noeud)
{
	unsigned int uid_ref;
	//~ bool a (0);
	
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{		
		uid_ref = liste_noeud[i].get_uid ();
		
		for (size_t j (0); j < liste_noeud.size (); ++j)
		{
			if (i != j)
			{
				if (liste_noeud[j].get_uid () == uid_ref)
				{
					cout << error::identical_uid (uid_ref) <<endl;
					//~ a = true;
					//~ break;
					return false;
				}
			}
			//~ if (a);
			//~ break;
		};
	};
	
	return true;
};

bool check_link_vaccum (const vector <Noeud>& liste_noeud)
{
	bool verifier;
	
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		Noeud noeud_ref (liste_noeud[i]);
		vector <Noeud*> liens (noeud_ref.get_liens ());
		
		verifier = false;
		
		for (size_t j (0); j < liens.size (); ++j)
		{
			unsigned int uid_ref (liens[j] -> get_uid ());
			for (size_t k (0); k < liste_noeud.size (); ++k)
			{
				if (i != k)
				{
					if (uid_ref == liste_noeud[k].get_uid ())
					verifier = true;
					break;
				}
			};
			if (!verifier)
			{
				cout << error::link_vacuum (uid_ref) <<endl;
				return false;
			}
		};
	};
	return true;
};

bool check_max_link (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		vector <Noeud*> liens (liste_noeud[i].get_liens ());
		if (liens.size () > max_link)
		{
			cout << error::max_link (liste_noeud[i].get_uid ()) <<endl;
			return false;
		}

	};
	return true;
};

bool check_multiple_same_link (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		vector <Noeud*> liens (liste_noeud[i].get_liens ());
		unsigned int uid_ref (liste_noeud[i].get_uid ());
		
		for (size_t j (0); j < liens.size (); ++j)
		{			
			for (size_t k (0); k < liens.size (); ++k)
			{
				if (j != k)
				{
					if (liens[j] -> get_uid () == liens[k] -> get_uid ())
					{
						cout << error::multiple_same_link (uid_ref, 
						                                   liens[j] -> get_uid ()) 
						                                   <<endl;
						return false;
					}
				}
			};
		};
	};
	return true;
};

bool check_node_link_superposition (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		unsigned int uid1 (liste_noeud[i].get_uid ());
		Cercle c1 (liste_noeud[i].get_body ());
		vector <Noeud*> liens (liste_noeud[i].get_liens ());
		
		for (size_t j (0); j < liens.size (); ++j)
		{
			unsigned int uid2 (liens[j] -> get_uid ());
			Cercle c2 (liens[j] -> get_body ());
			Segment connexion (creer_ligne (c1.get_centre (), c2.get_centre ()));
			
			for (size_t k (0); k < liste_noeud.size (); ++k)
			{
				if ((liste_noeud[k].get_uid () != uid1) and (liste_noeud[k].get_uid () != uid2))
				{
					if (!(intersection_cercle_segment (liste_noeud[k].get_body (),
					                                   connexion, dist_min)))
					{
						cout << error::node_link_superposition (liste_noeud[k].get_uid ()) <<endl;
						return false;
					}
				}
			};
		};
	};
	return true;
};

bool check_node_node_superposition (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		Cercle c1 (liste_noeud[i].get_body ());
		
		for (size_t j (0); j < liste_noeud.size (); ++j)
		{
			Cercle c2 (liste_noeud[j].get_body ());
			if (i != j)
			{
				if (!(intersection_cercle_cercle (c1, c2, dist_min)))
				{
					cout << error::node_node_superposition (liste_noeud[i].get_uid (),
					                                        liste_noeud[j].get_uid ())
					                                        <<endl;
					return false;
				}
			}
		};
	};
	return true;
};

bool check_reserved_uid (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		if (liste_noeud[i].get_uid () == no_link)
		{
			cout << error::reserved_uid () <<endl;
			return false;
		}
	};
	return true;
};

bool check_self_link_node (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		vector <Noeud*> liens (liste_noeud[i].get_liens ());
		unsigned int uid_ref (liste_noeud[i].get_uid ());
		
		for (size_t j (0); j < liens.size (); ++j)
		{
			if (liens[j] -> get_uid () == uid_ref)
			{
				cout << error::self_link_node (uid_ref) <<endl;
				return false;
			}
		};
	};
	return true;
};

bool check_too_little_capacity (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		if (liste_noeud[i].get_nbp () < min_capacity)
		{
			cout << error::too_little_capacity (liste_noeud[i].get_nbp ()) <<endl;
			return false;
		}
	};
	return true;
};

bool check_too_much_capacity (const vector <Noeud>& liste_noeud)
{
	for (size_t i (0); i < liste_noeud.size (); ++i)
	{
		if (liste_noeud[i].get_nbp () > max_capacity)
		{
			cout << error::too_much_capacity (liste_noeud[i].get_nbp ()) <<endl;
			return false;
		}
	};
	return true;
};


//automate de lecture functions
void lecture( char * nom_fichier)
{
    string line;
    ifstream fichier(nom_fichier); 
    if(!fichier.fail()) 
    {
        // l’appel de getline filtre aussi les séparateurs
        while(getline(fichier >> ws,line)) 
        {
			// ligne de commentaire à ignorer, on passe à la suivante
			if(line[0]=='#')  continue;  
       
			decodage_ligne(line);
        }
      
	}
	fichier.close();
}


void decodage_ligne(string line, ville city){
	istringstream data(line);
  
	// états de l'automate de lecture					 
	enum Etat_lecture {NB0,LOGEMENT,NB1,TRANSPORT,NB2,PRODUCTION,NB3,lIENS,FIN};
  
	static int etat(NB0); // état initial
	static int i(0), total(0);
	string T;
	unsigned int u(0);
	double x(0.0);
	double y(0.0);
	int pop(0.0);
	unsigned int origine(0);
	unsigned int arrivee(0);
	cercle c({0., 0.}, 0);

	switch(etat) {
	case NB0: 
		(data >> total);
		i=0 ;
		if(total==0) etat=NB1; else etat=LOGEMENT ;  
	    break;

	case LOGEMENT: //creates logement noeud
		(data >> u >> x >> y >> pop); 
		T="LOGEMENT";
		c({x,y},sqrt(pop));
		noeud N(T,u,pop,c);
		city.ajouter_noeud(N);
		++i;
		if(i == total) etat=NB1 ;
	    break;

	case NB1: 
		(data >> total);
		i=0 ;
		if(total==0) etat=NB2; else etat=TRANSPORT ; 
	    break;

	case TRANSPORT: 
		(data >> u >> x >> y >> pop); 
		T="TRANSPORT";
		c({x,y},sqrt(pop));
		noeud N(T,u,pop,c);
		city.ajouter_noeud(N); 
		++i;
		if(i == total) etat=NB2 ;
	     break;

	case NB2: 
		(data >> total);
		i=0;
		if(total==0) etat=NB3; else etat=PRODUCTION ; 
	     break;

	case PRODUCTION: 
		(data >> u >> x >> y >> pop); 
		T="PRODUCTION";
		c({x,y},sqrt(pop));
		noeud N(T,u,pop,c);
		city.ajouter_noeud(N);
		++i;
		if(i == total) etat=NB3 ;
	    break;
	    
	case NB3: 
		(data >> total);
		i=0;
		if(total==0) etat=FIN; else etat=LIENS ; 
	     break;
	     
	case LIENS:
	//put liens in the formerly initaialised noeuds in the other cases
		(data>>origine>>arrivee);
		for(size_t a(0); a< city.get_liste_noeud().size(); ++a){
			for(size_t b(0); b< city.get_liste_noeud().size(); ++b){	
			if(origine==((city.get_liste_noeud())[a]).get_uid() and //gets both the noeuds to whom belongs the origine and the arivee uid
			   arrivee==((city.get_liste_noeud())[b]).get_uid()){
				   city.add_lien(a,&city.get_liste_noeud[b]);//adds the noeud(b) arrivee adress to the liens liste in noud(a) origine
				   city.add_lien(b,&city.get_liste_noeud[a]);//does same thing but to the noued(b) and with the address of noeud(a) origine
			   };
		   };
	   };
	   ++i;
	   if(i == total) etat=FIN;
	   break;

	case FIN: //fin du decodage
	cout<<"noeuds creer fin de decodage";
	break;
	};	
};


//source error
namespace
{
	void sort_uid(unsigned int & uid1, unsigned int & uid2);
}

namespace
{
	void sort_uid(unsigned int & uid1, unsigned int & uid2)
	{
		if(uid1 > uid2)
		{
			unsigned int uid_tmp(uid2);
			uid2 = uid1;
			uid1 = uid_tmp;
		}
	}
}


string error::identical_uid(unsigned int uid)
{
	return string("Impossible to have two identical uids: ") + to_string(uid)
		   + string("\n");
}

string error::link_vacuum(unsigned int uid)
{
	return string("No node correspond for this link. Node not found: ")
		   + to_string(uid) + string("\n");
}

string error::max_link(unsigned int uid)
{
	return string("Too many connections for node: ") + to_string(uid) + string("\n");
}

string error::multiple_same_link(unsigned int uid1, unsigned int uid2)
{
	sort_uid(uid1, uid2);
	return string("Link already exists inbetween nodes ") + to_string(uid1)
		   + string(" and ") + to_string(uid2) + string("\n");
}

string error::node_link_superposition(unsigned int uid)
{
	return string("Impossible to have superposition between a link and a node. Node "
		   "is ") + to_string(uid) + string("\n");
}

string error::node_node_superposition(unsigned int uid1, unsigned int uid2)
{
	sort_uid(uid1, uid2);
	return string("Impossible to have superposition between two nodes: ")
		   + to_string(uid1) + string(" with ") + to_string(uid2) + string("\n");
}

string error::reserved_uid()
{
	return string("Impossible to have the reserved uid\n");
}

string error::success()
{
	return string("Correct file\n");
}

string error::self_link_node(unsigned int uid)
{
	return string("Impossible to self-link a node: ") + to_string(uid)
		   + string(" <-> ") + to_string(uid) + string("\n");
}

string error::too_little_capacity(unsigned int capacity)
{
	return string("Impossible to have a too little capacity: ") + to_string(capacity)
		   + string(" < ") + to_string(min_capacity) + string("\n");
}

string error::too_much_capacity(unsigned int capacity)
{
	return string("Impossible to have a too much capacity: ") + to_string(capacity)
		   + string(" > ") + to_string(max_capacity) + string("\n");
}







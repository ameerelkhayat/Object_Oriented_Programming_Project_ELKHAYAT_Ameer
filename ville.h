/**
 * Architecture adoptée : b1
 */

#ifndef VILLE_HEADER_H
#define VILLE_HEADER_H

#include <string>
#include <vector>

#include "ville_lecture.h"

//External function prototypes

namespace space_ville
{	
	void dessine_ville ();
	
	void supprimer_ville ();
	
	void file_creator (const char* filename);
	
	double calcule_ENJ ();
	
	double calcule_CI ();
	
	double calcule_MTA ();
	
	void add_default_node (std::string type, double x, double y);
	
	bool check_inside_node (double x, double y);
	
	void colorier_noeud_rouge (double x, double y); //Change aussi
													//clicked_node_ptr!
	
	bool in_selected_node (double x, double y);   //Verifies if the clicked node has											
												  //been clicked again
	void delete_clicked_node (double x, double y);
	
	bool change_node_position (double x_nouveau, double y_nouveau);
							   
	bool check_link_existence (double x, double y);
							   
	bool link_amount_possible (double x, double y);
							   
	void create_new_link (double x, double y);
						  
	void remove_old_link (double x, double y);
	
	void change_node_size (double x1, double y1, double x2, double y2);
	
	void show_shortest_path (double x, double y);
}

#endif


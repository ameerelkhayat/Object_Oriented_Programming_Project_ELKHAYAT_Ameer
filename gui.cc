#include <gtkmm.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "gui.h"
#include "ville.h"
#include "constantes.h"
#include "tools_s2d.h"
#include "graphic_gui.h"

#define BORDER_WIDTH 10
#define INFORMATION_PRECISION 4
#define ZOOM_PRECISION 1
#define LINE_WIDTH 3
#define LEFT_BUTTON 1
#define RIGHT_BUTTON 3
#define DEFAULT_ZOOM 1.0
#define SEPARATION_1 1
#define SEPARATION_2 2

using namespace std;

class Canvas : public Gtk::DrawingArea
{
public :
  Canvas ();
  virtual ~Canvas ();
    
  WP get_wp_update () const { return wp_update; }					//getters
  S2d get_point_clicked () const { return point_clicked; }
  S2d get_size_variation_point () const {return size_variation_point; }
  bool get_node_clicked () const { return node_clicked; } 
  bool get_allow_size_change () const { return allow_size_change; }
  bool get_shortest_path_clicked () const { return shortest_path_clicked; }
  
  void set_wp (WP wp);											    //setter
  void set_point_clicked (S2d point);
  void set_size_variation_point (S2d point);
  void set_node_clicked (bool click);
  void set_allow_size_change (bool allow);
  void set_shortest_path_clicked (bool clicked);
  
  void refresh ();

protected :
  bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr) override;
  
private :
  void adjust_window_parameters (int width, int height);

  S2d point_clicked;
  S2d size_variation_point;
  WP wp_ref;
  WP wp_update;
  bool node_clicked;
  bool allow_size_change;
  bool shortest_path_clicked;
};

class Archipelago : public Gtk::Window
{
public:
  Archipelago();
  virtual ~Archipelago();
  
protected:
  void set_labels_informations (); 		   //Function that names the labels and is
										   //called evertime the labels need to be
										   //reset
  
  void set_label_zoom ();				   //Equivalent but to the zoom label

  void create_frame_General ();			   //These functions allow the constructor of
  void create_frame_Display ();			   //this class to be smaller and more
  void create_frame_Editor ();			   //organized, they contain call of pack_start
  void create_frame_Informations ();       //methods, frame naming...
  
  void on_Button_clicked_exit ();		   //Signal handlers
  void on_Button_clicked_new ();
  void on_Button_clicked_open ();
  void on_Button_clicked_save ();
  void on_Button_clicked_shortest_path ();
  void on_Button_clicked_zoom_in ();
  void on_Button_clicked_zoom_out ();
  void on_Button_clicked_zoom_reset ();
  void on_Button_clicked_edit_link ();
  bool on_button_press_event(GdkEventButton * event);
  bool on_button_release_event(GdkEventButton * event);
  bool on_key_press_event(GdkEventKey * key_event);
  
  void make_new_node (double x, double y);	//Used in left/right click signal handler
  void select_node (double x, double y);
  void delete_node (double x, double y);
  void delete_link (double x, double y);
  void make_link (double x, double y);
  void right_button_click_handler (double x, double y);
  
  void left_button_release_handler (double x, double y); //Used in left button release
														 //signal handler
    
  void connect_signal_handlers ();		   //Connects all the signal handlers to their
										   //respective buttons
										   
  void transformer_coordonnees (double& xf, double& yf); //Converts from windown to the
														 //to the model
  
  Gtk::Box m_Box, m_Box_Left, m_Box_Right; //Main Boxes (m_Box is the most general)
  
  Gtk::Box m_Box_general;				   //Sub Boxes (Sub Boxes will contain buttons,
  Gtk::Box m_Box_display;                  //radio buttons, labels...)
  Gtk::Box m_Box_editor;
  Gtk::Box m_Box_informations;
  
  Canvas              m_Canvas;			   //Drawing Area
  
  Gtk::Frame frame_General;				   //Frames will contain Sub Boxes of buttons
  Gtk::Button		  m_Button_exit;
  Gtk::Button		  m_Button_new;
  Gtk::Button 		  m_Button_open;
  Gtk::Button 		  m_Button_save; 
  
  Gtk::Frame frame_Display;
  Gtk::ToggleButton   m_Button_shortest_path;
  Gtk::Button		  m_Button_zoom_in;
  Gtk::Button		  m_Button_zoom_out;
  Gtk::Button		  m_Button_zoom_reset;
  Gtk::Label		  m_Label_zoom;
  
  Gtk::Frame frame_Editor;
  Gtk::ToggleButton	  m_Button_edit_link;
  Gtk::RadioButton    m_radio_housing;
  Gtk::RadioButton    m_radio_transport;
  Gtk::RadioButton    m_radio_production;
  
  Gtk::Frame frame_Informations;
  Gtk::Label		  m_Label_ENJ;
  Gtk::Label		  m_Label_CI;
  Gtk::Label		  m_Label_MTA;
};





namespace
{
	double asp_default ( ( dim_max - (-dim_max) ) /
						 ( dim_max - (-dim_max) ) );
	WP wp_default ({ -dim_max, dim_max,
					 -dim_max, dim_max,
					  default_drawing_size , asp_default });
	
	double zoom_factor (DEFAULT_ZOOM);
}

Canvas::Canvas () //Constructeur
{
	node_clicked = false;
	allow_size_change = false;
	shortest_path_clicked = false;
}

Canvas::~Canvas () //Destructeur
{
}

void Canvas::set_wp (WP wp)
{
	wp_ref = wp;
	wp_update = wp_ref;
}

void Canvas::set_point_clicked (S2d point)
{
	point_clicked = point;
}

void Canvas::set_size_variation_point (S2d point)
{
	size_variation_point = point;
}

void Canvas::set_node_clicked (bool click)
{
	node_clicked = click;
}

void Canvas::set_allow_size_change (bool allow)
{
	allow_size_change = allow;
}

void Canvas::set_shortest_path_clicked (bool clicked)
{
	shortest_path_clicked = clicked;
}
	
void Canvas::refresh ()  //Creates a signal to recall signal handler on_draw
{
	auto win = get_window ();
	if (win)
	{
		Gdk::Rectangle r
		(null, null, get_allocation ().get_width (), get_allocation ().get_height () );
		   
	    win -> invalidate_rect (r, false);
    }
}


void Canvas::adjust_window_parameters (int width, int height)
{
	double new_aspect_ratio((double)width/height);
	if( new_aspect_ratio > wp_ref.asp)
    { // keep y_max and y_min. Adjust y_max and x_min
		wp_update.y_max = wp_ref.y_max ;
	    wp_update.y_min = wp_ref.y_min ;	
	  
	    double delta(wp_ref.x_max - wp_ref.x_min);
	    double mid((wp_ref.x_max + wp_ref.x_min)/2);

	    wp_update.x_max = mid + 0.5*(new_aspect_ratio/wp_ref.asp)*delta ;
	    wp_update.x_min = mid - 0.5*(new_aspect_ratio/wp_ref.asp)*delta ;		  	  
    }
    else
    { // keep x_max and x_min. Adjust y_max and y_min
	    wp_update.x_max = wp_ref.x_max ;
	    wp_update.x_min = wp_ref.x_min ;
	  	  
 	    double delta(wp_ref.y_max - wp_ref.y_min);
	    double mid((wp_ref.y_max + wp_ref.y_min)/2);

	    wp_update.y_max = mid + 0.5*(wp_ref.asp/new_aspect_ratio)*delta ;
	    wp_update.y_min = mid - 0.5*(wp_ref.asp/new_aspect_ratio)*delta ;		  	  
    }
}




//on_draw met en jeu l'ensemble des fonctions de dessin du modèle ainsi que celles du
//module graphique.

bool Canvas::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{	
	Gtk::Allocation allocation ( get_allocation () );
	const int width = allocation.get_width();
    const int height = allocation.get_height();
	
	adjust_window_parameters (width, height);
	
    space_graphic_gui::set_width_height (width, height);
    space_graphic_gui::set_window_parameters (wp_update);
	space_graphic_gui::graphic_set_context (cr);
	
	cr -> set_line_width (LINE_WIDTH);
	space_graphic_gui::color_background_white ();
	space_ville::dessine_ville ();
	
	if (node_clicked)
	{
		if (shortest_path_clicked)
		{
			space_ville::show_shortest_path (point_clicked.x, point_clicked.y);
		} 
		space_ville::colorier_noeud_rouge (point_clicked.x, point_clicked.y);
	}
	return true;
}





//Constructeur d'Archipelago qui définit la fenetre, l'interface de l'utilisateur,
//positionnement des différents éléments...

Archipelago::Archipelago () :
	m_Box (Gtk::ORIENTATION_HORIZONTAL, SEPARATION_2),
	m_Box_Left (Gtk::ORIENTATION_VERTICAL, SEPARATION_1),
	m_Box_Right (Gtk::ORIENTATION_VERTICAL),
	m_Box_general (Gtk::ORIENTATION_VERTICAL, SEPARATION_2),
	m_Box_display (Gtk::ORIENTATION_VERTICAL, SEPARATION_2),
	m_Box_editor (Gtk::ORIENTATION_VERTICAL, SEPARATION_2),
	m_Box_informations (Gtk::ORIENTATION_VERTICAL, SEPARATION_1),
	m_Button_exit ("exit"), m_Button_new ("new"), m_Button_open ("open"),
	m_Button_save ("save"),
	m_Button_shortest_path ("shortest path"), m_Button_zoom_in ("zoom in"),
	m_Button_zoom_out ("zoom out"), m_Button_zoom_reset ("zoom reset"),
	m_Label_zoom ("zoom: x1.0"),
	m_Button_edit_link ("edit_link"), m_radio_housing ("housing"),
	m_radio_transport ("transport"), m_radio_production ("production"),
	m_Label_ENJ ("ENJ: 0.0000"), m_Label_CI ("CI: 0"), m_Label_MTA ("MTA: 0")
{
	set_title ("Projet Archipelago"); //Set the caracteristics of the main window
	set_border_width (BORDER_WIDTH);
	add (m_Box);

	m_Box.pack_start (m_Box_Left, false, false); //Left Box contains the user interface
	m_Box.pack_start (m_Box_Right);              //Right Box contains the drawing area 

	m_Canvas.set_wp (wp_default);
	m_Canvas.set_size_request (default_drawing_size, default_drawing_size);
	m_Box_Right.pack_start (m_Canvas);
  
	create_frame_General ();		 //Creating all the frames as mentioned
	create_frame_Display ();
	create_frame_Editor ();
	create_frame_Informations ();
  
	m_Box_Left.pack_start (frame_General, false, false);	//Adding all of the frames
	m_Box_Left.pack_start (frame_Display, false, false);	//into the left box
	m_Box_Left.pack_start (frame_Editor, false, false);
	m_Box_Left.pack_start (frame_Informations, false, false);
	
	connect_signal_handlers ();								//Call of signal handlers
	show_all_children ();
}

//Destructeur

Archipelago::~Archipelago ()
{
}

void Archipelago::set_labels_informations ()
{	
	ostringstream ENJ_val;
	ENJ_val <<setprecision (INFORMATION_PRECISION);
	ENJ_val <<fixed;
	
	ostringstream CI_val;
	
	ostringstream MTA_val;
		
	//Creating ENJ Label
	
	ENJ_val << space_ville::calcule_ENJ () ;
	m_Label_ENJ.set_text ( "ENJ: " + ENJ_val.str () );
	
	//Creating CI Label
	
	CI_val << space_ville::calcule_CI () ;
	m_Label_CI.set_text ( "CI: " + CI_val.str () );
	
	//Creating MTA Label
	
	MTA_val << space_ville::calcule_MTA () ;
	m_Label_MTA.set_text ( "MTA: " + MTA_val.str () );
}

void Archipelago::set_label_zoom ()
{
	ostringstream zoom_val;
	zoom_val <<setprecision (ZOOM_PRECISION);
	zoom_val <<fixed;
	zoom_val <<zoom_factor ;
	
	m_Label_zoom.set_text ( "zoom: " + zoom_val.str () );
}

//Frame creators

void Archipelago::create_frame_General ()
{
	m_Box_general.pack_start (m_Button_exit, false, false);
    m_Box_general.pack_start (m_Button_new, false, false);
    m_Box_general.pack_start (m_Button_open, false, false);
    m_Box_general.pack_start (m_Button_save, false, false);

    frame_General.set_label ("General");
    frame_General.add (m_Box_general);
}

void Archipelago::create_frame_Display ()
{
    m_Box_display.pack_start (m_Button_shortest_path, false, false);
    m_Box_display.pack_start (m_Button_zoom_in, false, false);
    m_Box_display.pack_start (m_Button_zoom_out, false, false);
    m_Box_display.pack_start (m_Button_zoom_reset, false, false);
    m_Box_display.pack_start (m_Label_zoom, false, false);
  
    frame_Display.set_label ("Display");
    frame_Display.add (m_Box_display);
}

void Archipelago::create_frame_Editor ()
{
	m_radio_transport.join_group (m_radio_housing);
    m_radio_production.join_group (m_radio_housing);
  
    m_Box_editor.pack_start (m_Button_edit_link, false, false);
    m_Box_editor.pack_start (m_radio_housing, false, false);
    m_Box_editor.pack_start (m_radio_transport, false, false);
    m_Box_editor.pack_start (m_radio_production, false, false);
  
    frame_Editor.set_label ("Editor");
    frame_Editor.add (m_Box_editor);
}

void Archipelago::create_frame_Informations ()
{
	set_labels_informations ();
	m_Box_informations.pack_start (m_Label_ENJ, false, false);
    m_Box_informations.pack_start (m_Label_CI, false, false);
    m_Box_informations.pack_start (m_Label_MTA, false, false);
  
    frame_Informations.set_label ("Informations");
    frame_Informations.add (m_Box_informations);
}

//Signal handlers definitions

void Archipelago::on_Button_clicked_exit ()
{
	exit (0);
}

void Archipelago::on_Button_clicked_new ()
{
	space_ville::supprimer_ville ();
	m_Canvas.refresh ();
	set_labels_informations ();
}

void Archipelago::on_Button_clicked_open ()
{
	Gtk::FileChooserDialog dialog ("Please choose a file",
								   Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for (*this);

    dialog.add_button ("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button ("_Open", Gtk::RESPONSE_OK);
    
    int result = dialog.run ();

    switch (result)
    {
		case (Gtk::RESPONSE_OK) :
		{
			string filename = dialog.get_filename ();
			space_ville::supprimer_ville ();
			char* arg = new char[filename.size ()];
			filename.copy (arg, filename.size () + 1);
			arg[filename.size ()] = '\0';
			space_ville_lecture::lecture (arg);
			m_Canvas.refresh ();
			set_labels_informations ();
			break;
		}
		case (Gtk::RESPONSE_CANCEL) : break;
		default: break;
	}
}

void Archipelago::on_Button_clicked_save ()
{
	Gtk::FileChooserDialog dialog ("Please save file",
								   Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for (*this);

    dialog.add_button ("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button ("_Save", Gtk::RESPONSE_OK);
        
    int result = dialog.run ();

    switch (result)
    {
		case (Gtk::RESPONSE_OK) :
		{
			string filename = dialog.get_filename ();
			space_ville::file_creator ( filename.c_str () );
			break;
		}
		case (Gtk::RESPONSE_CANCEL) : break;
		default : break;
	}
}

void Archipelago::on_Button_clicked_shortest_path ()
{
	if (m_Button_shortest_path.get_active ())
	{
		m_Canvas.set_shortest_path_clicked (true);
		m_Canvas.refresh();
	}
	else
	{
		m_Canvas.set_shortest_path_clicked (false);
		m_Canvas.refresh();
	}
}

void Archipelago::on_Button_clicked_zoom_in ()
{	
	if (zoom_factor < max_zoom)
	{
		zoom_factor += delta_zoom;
		set_label_zoom ();
		
		WP wp_new ({ -dim_max*(1/zoom_factor),
					  dim_max*(1/zoom_factor),
				     -dim_max*(1/zoom_factor),
				      dim_max*(1/zoom_factor),
					  wp_default.height, wp_default.asp });
		m_Canvas.set_wp (wp_new);
		m_Canvas.refresh ();
	}
}

void Archipelago::on_Button_clicked_zoom_out ()
{	
	if (zoom_factor > min_zoom)
	{
		zoom_factor -= delta_zoom;
		set_label_zoom ();
				
		WP wp_new ({ -dim_max*(1/zoom_factor),
					  dim_max*(1/zoom_factor),
				     -dim_max*(1/zoom_factor),
				      dim_max*(1/zoom_factor),
					  wp_default.height, wp_default.asp });
		m_Canvas.set_wp (wp_new);
		m_Canvas.refresh ();
	}
}

void Archipelago::on_Button_clicked_zoom_reset ()
{
	zoom_factor = DEFAULT_ZOOM;
	set_label_zoom ();
	m_Canvas.set_wp (wp_default);
	m_Canvas.refresh ();
}

void Archipelago::on_Button_clicked_edit_link ()
{
	if (!m_Canvas.get_node_clicked ())
	{
		cout << "Must select a node first!" <<endl;
		m_Button_edit_link.set_active (false);
	} else {
		if (m_Button_edit_link.get_active ()) m_Button_edit_link.set_active ();
	}	
}

//FONCTION ENTRE 40 ET 80 LIGNES

bool Archipelago::on_button_press_event(GdkEventButton * event)
{
	if (event -> type == GDK_BUTTON_PRESS)
	{
		double clic_x = event -> x;
		double clic_y = event -> y;
		
		double origin_x = m_Canvas.get_allocation().get_x();
		double origin_y = m_Canvas.get_allocation().get_y();
				
		double width = m_Canvas.get_allocation().get_width();
		double height= m_Canvas.get_allocation().get_height();
				
		if (clic_x >= origin_x && clic_x <= origin_x + width &&
		    clic_y >= origin_y && clic_y <= origin_y + height)
		{
			double x = clic_x - origin_x;
			double y = clic_y - origin_y;
			transformer_coordonnees (x, y);
			
			if (event -> button == LEFT_BUTTON)
			{				
				if ( not space_ville::check_inside_node (x, y) )
				{
					if (m_Button_edit_link.get_active ())
						m_Canvas.refresh ();
					else
					{
						if ( m_Canvas.get_node_clicked () )
						{
							m_Canvas.set_size_variation_point ({x, y});
							m_Canvas.set_allow_size_change (true);
							m_Canvas.refresh ();
							return true;
						} else make_new_node (x, y);
					}
				}
				else
				{
					if ( not m_Canvas.get_node_clicked () )
						select_node (x, y);
				    else
				    {
						if (space_ville::in_selected_node (x, y))
							
							delete_node (x, y);
						else
						{
							if (m_Button_edit_link.get_active ())
							{
								if (space_ville::check_link_existence (x, y))
									delete_link (x, y);
								else make_link (x, y);
							}
							else
							{
								m_Canvas.set_point_clicked ({x, y});
								m_Canvas.refresh ();
								set_labels_informations ();
							}
						}
					}
				}
			} 
			else if (event -> button == RIGHT_BUTTON) 
				     right_button_click_handler (x, y);
		}
	}
	return true;
}

bool Archipelago::on_button_release_event(GdkEventButton * event)
{
	if(event->type == GDK_BUTTON_RELEASE)
	{
		double clic_x = event->x ;
		double clic_y = event->y ;
		
		double origin_x = m_Canvas.get_allocation().get_x();
		double origin_y = m_Canvas.get_allocation().get_y();
				
		double width = m_Canvas.get_allocation().get_width();
		double height= m_Canvas.get_allocation().get_height();
				
		if(clic_x >= origin_x && clic_x <= origin_x + width &&
		   clic_y >= origin_y && clic_y <= origin_y + height)
		{
			double x = clic_x - origin_x;
			double y = clic_y - origin_y;
			transformer_coordonnees (x, y); 
			
			if (event -> button == LEFT_BUTTON)
				left_button_release_handler (x, y);
		}
	}
	return true;
}
 
bool Archipelago::on_key_press_event (GdkEventKey * key_event)
{
	if(key_event->type == GDK_KEY_PRESS)
	{
		switch(gdk_keyval_to_unicode (key_event->keyval))
		{
			case 'i':
				on_Button_clicked_zoom_in ();
				break;
			case 'n' :
				on_Button_clicked_new ();
				break;
			case 'o':
				on_Button_clicked_zoom_out ();
				break;
			case 'q':
				exit (0);
				break;
		}
	}
	
	return Gtk::Window::on_key_press_event (key_event);
}

void Archipelago::make_new_node (double x, double y)
{
	if (m_radio_housing.get_active ())
	{
		space_ville::add_default_node ("LOGEMENT", x, y);
		m_Canvas.refresh ();
		set_labels_informations ();
	}
	
	if (m_radio_transport.get_active ())
	{
		space_ville::add_default_node ("TRANSPORT", x, y);
		m_Canvas.refresh ();
		set_labels_informations ();
	}
	
	if (m_radio_production.get_active ())
	{
		space_ville::add_default_node ("PRODUCTION", x, y);
		m_Canvas.refresh ();
		set_labels_informations ();
	}
}

void Archipelago::select_node (double x, double y)
{
	m_Canvas.set_point_clicked ({x, y});
	m_Canvas.set_node_clicked (true);
	m_Canvas.refresh ();
	set_labels_informations ();
}

void Archipelago::delete_node (double x, double y)
{
	space_ville::delete_clicked_node (x, y);
	m_Canvas.set_node_clicked (false);
	m_Canvas.set_allow_size_change (false);
	m_Canvas.refresh ();
	set_labels_informations ();
}

void Archipelago::delete_link (double x, double y)
{
	space_ville::remove_old_link (x, y);
	set_labels_informations ();
	m_Canvas.refresh ();
}

void Archipelago::make_link (double x, double y)
{
	if (space_ville::link_amount_possible (x, y))
	{
		space_ville::create_new_link (x, y);
		m_Canvas.refresh ();
		set_labels_informations ();
	}
}

void Archipelago::right_button_click_handler (double x, double y)
{
	if ( m_Canvas.get_node_clicked () )
	{
		if (space_ville::change_node_position (x, y))
		{
			m_Canvas.set_point_clicked ({x, y});
			m_Canvas.refresh ();
			set_labels_informations ();
		}
	}
}

void Archipelago::left_button_release_handler (double x, double y)
{
	if ( (not m_Button_edit_link.get_active ()) and
		m_Canvas.get_node_clicked ()  and
		m_Canvas.get_allow_size_change ())
	{
		if (x != m_Canvas.get_size_variation_point ().x or
			y != m_Canvas.get_size_variation_point ().y)
		{
			space_ville::change_node_size
						(m_Canvas.get_size_variation_point ().x,
						 m_Canvas.get_size_variation_point ().y, x, y);
						 set_labels_informations ();
						 m_Canvas.refresh ();
						 m_Canvas.set_allow_size_change (false);
		}
		else
		{
			m_Canvas.set_node_clicked (false);
			m_Canvas.set_allow_size_change (false);
			m_Canvas.refresh ();
		}	
	}
}
	
//Function that connects all the signal handlers to their respective buttons

void Archipelago::connect_signal_handlers ()
{
	m_Button_exit.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_exit) );
                  
    m_Button_new.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_new) );
                  
    m_Button_open.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_open) );   
                  
    m_Button_save.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_save) );
                  
    m_Button_shortest_path.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_shortest_path) );
                  
    m_Button_zoom_in.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_zoom_in) );
                  
    m_Button_zoom_out.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_zoom_out) );       
                     
    m_Button_zoom_reset.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_zoom_reset) );
                  
    m_Button_edit_link.signal_clicked ().connect ( sigc::mem_fun (*this,
                  &Archipelago::on_Button_clicked_edit_link) );
}

void Archipelago::transformer_coordonnees (double& xf, double& yf)
{
	const int width = m_Canvas.get_allocation().get_width();
	const int height= m_Canvas.get_allocation().get_height();
	
	WP wp = m_Canvas.get_wp_update ();
	
	xf = (xf/width) * (wp.x_max - wp.x_min) + wp.x_min;
	yf = wp.y_max - (yf/height) * (wp.y_max - wp.y_min);
}

//External Function definition

int space_gui::launch_app (char **argv)
{
	int argc_dummy (1);	
	auto app = Gtk::Application::create (argc_dummy, argv, "org.gtkmm.example");

    Archipelago ArchipelagoWindow;
    ArchipelagoWindow.set_resizable (true);
	
	return app -> run (ArchipelagoWindow);
}



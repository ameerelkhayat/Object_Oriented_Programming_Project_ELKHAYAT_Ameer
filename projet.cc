/**
 * Architecture adoptée : b1
 */

#include "gui.h"
#include "ville_lecture.h"

int main (int argc, char **argv)
{
	if (argc == 2) space_ville_lecture::lecture (argv[1]);
	
	return space_gui::launch_app (argv);
}

# Object_Oriented_Programming_Project_ELKHAYAT_Ameer
Repository containing the code for my Object Oriented Programming class project.

#Summary of the project
The goal of this project is to develop a tool to edit the map of a city by placing different
types of neighborhoods in 2D space. The focus is on the efficiency of travel while looking for a certain balance between the
balance between the different types of neighborhoods and estimating infrastructure costs.
Two abstractions are used to model and evaluate the floating city:

- Geometry: to test the correct distribution of neighborhoods and connections.
The city is defined in a world coordinate system with origin (0,0); X is the horizontal axis, oriented
X is the horizontal axis, positively oriented to the right, and Y is the vertical axis, positively oriented upwards. The unit is the m.
The initial space is delimited by [-dim_max, dim_max] according to X and Y but nothing prevents then to
change the size of this space with the mouse and the keyboard.
The geometrical abstraction is the way to check the non-overlapping rules between two
two neighborhoods and between a connection and a neighborhood. For this purpose we adopt this approach:
o A neighborhood is represented by a circle whose radius depends on the number of people
o A connection between two neighborhoods is represented by a line segment.

- Graph: to determine the effectiveness of the proposed plan
o A neighborhood is a node in the graph
o A connection between two neighborhoods is a link between two nodes of the graph.
Using Dijkstra's graph traversal algorithm, we can determine the shortest time to travel from one neighborhood to another
from one neighborhood to another.

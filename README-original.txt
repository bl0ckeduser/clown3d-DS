Readme for original PC version from which this port was forked:

----------------------- about -------------------------------------

This is an extremely simple 3D platformer demo, featuring AABB 
collisions (via homegrown collision code based on simplistic 
vector arithmetic), bullets which disappear ("explode") when they hit 
objects, and variable FPS physics (via "delta time" calculations and 
constant acceleration formulas).

This is my first [ongoing] attempt at a truly 3D game. It is 
written in the C programming language using the GL/glu and SDL APIs.

It is mostly written from scratch, however...

- Thanks to the many great OpenGL tutorials available on the
  Web (notably NeHe's), as well as the freely available
  "OpenGL Red Book", and, finally to the great OpenGL man pages.

  NeHe OpenGL tutorials: http://nehe.gamedev.net/
  "OpenGL Red Book": http://www.opengl.org/documentation/red_book/

- Thanks to the great OBJ file format documentation available
  at [http://www.royriggs.com/obj.html]. The current program
  uses a custom OBJ-like 3D model file format.

- The OOP-style object code structure was heavily influenced by 
  the structure of the source code for the C++ game "Grall 2" 
  by nikki93 (http://nikhilesh.info/grall2.html).

- Variable FPS physics formulas taken from my grade 11 physics course

----------------------- controls ----------------------------------

movement: cursor keys (aka up/down/left/right arrow keys)
	   (Note that the left and right keys rotate the
	   player, while up and down keys translate the
	   player)
jumping: Z key
shooting bullets: S key
resetting the game: R key

keypad 1: switch to third-person view (default)
keypad 2: switch to first-person view

----------------------- arguments --------------------------------

 - using "-f" as the *last* argument runs the game full-screen
 - alternate levels can be specified by typing in e.g.
	./demo worlds/maze	(Unix)
	demo worlds/maze	(Windows)




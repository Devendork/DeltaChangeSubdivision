Laura Devendorf
This is a final project for EECS 294 - Computer Aided Geometric Desgin at UC Berkeley in the Fall of 2012.

The code implements Charles Loop's subdivison algorithm based on a control mesh. The novel feature of this UI is the ability to specify "delta changes" at each vertex at any step in the subdivision. These delta changes determine how far from the original subdivison position a vertex should be moved and allow for fine manipulation on smooth meshes. 

TO RUN:
./delta --filename (path to .obj file)
note: if no input file is specified, a default cube is loaded 

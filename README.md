# scenegraph


## plan:

- responsabilities of geometry system
	- hold all vaos
	- heap vaos built in loader given to system
	- 


# what does the renderer have?
- list of entities

# what does the renderer need?
- transform
	- where'd it come from?
		- resolved through the scene graph 
		- node resolves with parents (inverse traversal)
	- where is it?
		- Entitys got it, owned by node
	- how do we get it?
		- entity resolves transform

- geometry
	- where'd it come from?
		- loaded from the mesh loader
	- where is it?
		- stored in system, entity has a ptr
	- how do we get it?
		- from entity

- material
	- where'd it come from?
		- built from R
	- where is it?
		- R has it, entity has ptr
	- textures
		- where'd it come from?
			- loaded from R
		- where is it?
			stored in R, mat has a ref
	- parameters
		- where'd it come from?
			- loaded from R
		- where is it?
			- here
	- shaders
		- where'd it come from?
			- loaded from R
		- where is it?
			- R has it, mat has ptr
		
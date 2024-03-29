
//*****************************************************************************************//
//                                      Includes                                           //
//*****************************************************************************************//

#include "includes.all"

//*****************************************************************************************//
//                                       World                                             //
//*****************************************************************************************//
World::World () { 
	environment = NULL; terrain = NULL; objects = NULL; objectsSize = 0;
	//WILF PhysX...
	physicsTerrain = NULL; physicsWorldMesh = NULL; physicsPlane = NULL;
}

World::~World () {

	// Get rid of the objects...
	for (long i = 0; i < objectsSize; i++) {
		delete objects [i];
	}

	delete [] objects;

	//WILF PhysX...
	if (physicsTerrain != NULL) physicsTerrain->release (); 
	if (physicsWorldMesh != NULL) physicsWorldMesh->release ();
	if (physicsPlane != NULL) physicsPlane->release ();
	deleteObjectCollectionEntries (dynamicObjects);

	Shader::activeShader = NULL;
};

void World::setup () {
	ShaderManager::setup ();
}

void World::wrapup () {
	ShaderManager::wrapup ();
}

void World::tick () {
	// Tick Enviroment if we have one
	if (environment != NULL) {environment->tick ();}
	if (shaderManager != NULL) {shaderManager->tick ();}

	//Tick all the objects in the world such as doors, sun, weather...
	for (long objectIndex = 0; objectIndex < objectsSize; objectIndex++) {
		objects [objectIndex]->tick ();		
	}
	
	//Also, the PhysX dynamic objects...
	loopVector (dynamicObjectIndex, dynamicObjects)
		dynamicObjects [dynamicObjectIndex]->tick ();
	endloop
}

void World::draw () {
	Shader::reset ();

	//Draw the environment if we have one.
	if (environment != NULL) {environment->draw ();}
	if (environment != NULL) {halt ("\nThere is an environment object");} //Doesn't halt so there is no environment object (causes black sky).

	//Draw all the objects in the world such as terrain, buildings, etc.
	for (long objectIndex = 0; objectIndex < objectsSize; objectIndex++) {
		objects [objectIndex]->draw ();		
	}
	
	//Also, the PhysX dynamic objects...
	loopVector (dynamicObjectIndex, dynamicObjects)
		dynamicObjects [dynamicObjectIndex]->draw ();
	endloop
}

void World::log () {
	//For debugging, it might be useful to output the entire world into the log file.

	//The simple stuff first.
	::log ("\n\nWorld...\nStart position [%3.2f,%3.2f,%3.2f]", startPosition.x, startPosition.y, startPosition.z);

	//The objects: A version WITH loop macros.
	for (long objectIndex = 0; objectIndex < objectsSize; objectIndex++) {
		objects [objectIndex]->log ();		
	}
}

World *World::read () {
	//Prompt for a ".wrl" file (THIS IS YOUR OWN PERSONAL FORMAT) and reads it into a brand new world.
 
	char *filename = promptForWorldRead ();
	if (filename == NULL) return NULL;

	World *world = new World;

	::ifstream input; input.open (filename); 
	if (input.bad ()) {prompt ("Unable to open input file \"%s\".", filename); return NULL;}

	//Import into this empty world...
	world->import (input);

	input.close ();
	return world;
}

Object *World::privateImportObject (::ifstream &input) {
	char line [256]; //Working variable...

	//Input the header.
	SKIP_TO_COLON;
	SKIP_TO_SEMICOLON; long objectIndex = atoi (line); //Only useful for debugging or browsing.
	CLEAR_THE_LINE;

	//Get the type which we need in order to create an object of the correct type...
	SKIP_TO_ENDLINE;
	// Getting the type
	char key [256]; char type [256]; type [0] = '\0';
	sscanf (line, " \"%[^\"]\" => \"%[^\"]\"", key, type);

	Object *object = NULL;
	
	if (stricmp (type, "Environment") == 0) {
		object = new EnvironmentObject (type);	
		::log ("\nCreate environment object...");
	} else if (stricmp (type, "static geometry") == 0) {
		object = new Object (type);
	} else if (stricmp (type, "Terrain") == 0) {
		object = new Terrain (type);
	} else {
		prompt ("\nIgnored unexpected object of type \"%s\"...", type);
		return NULL;
	}

	// Import the rest of the object
	object->import (input, this);
	return object;
}

void World::import (::ifstream &input) {
	char line [256]; //Working variable...
	
	//Input the header.
	CLEAR_THE_LINE;

	//Grab the start position.
	SKIP_TO_COLON;
		SKIP_TO_COMMA; startPosition.x = atof (line);
		SKIP_TO_COMMA; startPosition.y = atof (line);
		SKIP_TO_SEMICOLON; startPosition.z = atof (line);

	//Input the textures
	textureManager.import (input, this);
	
	//Input the shaders
	shaderManager->import (input, this);

	//Input the objects
	SKIP_TO_COLON;
	SKIP_TO_SEMICOLON; objectsSize = atoi (line);
	objects = new Object * [objectsSize];

	for (long objectIndex = 0; objectIndex < objectsSize; objectIndex++) {
		//Import objects using the polymorphic import that matches its type...
		Object *object = privateImportObject (input);
		if (object == NULL) continue; //It was discarded since it was unexpected...
		
		const char *type = object->type; //A few objects need to be globally referenced...
		if (stricmp (type, "Environment") == 0) {
			environment = (EnvironmentObject *) object;
			::log ("\nThere was an environment object...");
		} else if (stricmp (type, "Terrain") == 0) {
			terrain = (Terrain *) object;
		}
		objects [objectIndex] = object;					
		
	}

	finalize ();
}

void World::finalize () {
	//This runs immediately after a world is imported... 
	//WILF PhysX...

	//Reset for the next one (which deals with discarding and recreating the scene).
	physicsManager->reset (); 

	//Add new physics objects for the new world...
	const bool useTerrain = true;
	if (useTerrain) {
		physicsTerrain = physicsManager->physicsTerrain (terrain); physicsManager->add (physicsTerrain);
		physicsWorldMesh = physicsManager->physicsMesh (this); physicsManager->add (physicsWorldMesh);
	} else {
		physicsPlane = physicsManager->physicsPlane (Point (0.0, 0.0, 0.0), Point (0.0, 1.0, 0.0)); //I want my plane pointing up (which is down for physX).
		physicsManager->add (physicsPlane);
	}

	//for (int i = 0; i < 5; i++)
	//{
	//	for (int j = 0; j < 30; j++)
	//	{
	//		for (int k = 0; k < 5; k++)
	//		{
	//			Cube *cube = new Cube (Point(k*0.5,j*0.5 - 5,i*0.5), Vector(0,0,0), 0.5, 0.5, 0.5, 1.0); //width, height, depth, mass
	//			cube->physicsCube->putToSleep();
	//			physicsManager->add (cube->physicsCube);
	//			addDynamicObject (cube); //The world now owns the cube and will tick, draw, and delete it.
	//		}
	//	}
	//}

	//The rest has nothing to do with PhysX...
	camera->reset (); player->reset (startPosition);
}
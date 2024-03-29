//*****************************************************************************************//
//                                        Object                                           //
//*****************************************************************************************//

#ifndef environmentObjectModule
#define environmentObjectModule 

class World; //Forward reference for import method...
class Texture;

class EnvironmentObject : public Object {
protected:
	enum skySides {UP, DOWN, BACK, LEFT, RIGHT, FRONT};
public:	
	char* skyname;
	Texture* skyTextures [6]; 

	EnvironmentObject (const char* typeName);
	EnvironmentObject ();
	~EnvironmentObject ();

	virtual void tick ();	
	virtual void draw ();
	virtual void log ();

	void drawSkyBox ();
	void drawSide ();

	virtual void import (::ifstream &input, World *world);
};

#endif
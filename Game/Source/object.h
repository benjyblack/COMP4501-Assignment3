//*****************************************************************************************//
//                                        Object                                           //
//*****************************************************************************************//

#ifndef objectModule
#define objectModule 

class World; //Forward reference for import method...
class Shader; //Forward reference for instance variable...

class Object {	
public:
	
	char* type;
	DualTransformation transformation;
	Face** faces; long facesSize;
	
	Shader *shader;

	Object ();
	Object (const char* type);
	virtual ~Object ();

	virtual void tick ();
	virtual void draw ();
	virtual void log ();

	virtual void import (::ifstream &input, World *world);
};

declareCollection (Object);

#endif //objectModule
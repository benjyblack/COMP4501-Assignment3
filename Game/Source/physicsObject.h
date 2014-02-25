
//*****************************************************************************************//
//                                     Physics Object                                      //
//*****************************************************************************************//

#ifndef physicsObjectModule
#define physicsObjectModule 

class Cube : public Object {
public:
	PxRigidDynamic *physicsCube; float width; float height; float depth;

	Cube (Point &position, Vector velocity, float width, float height, float depth, float mass) {
		::log ("\nCreate cube %x...", this);
		this->width = width; this->height = height; this->depth = depth;
		physicsCube = physicsManager->physicsBox (position, velocity, width, height, depth, mass);
	}
	virtual ~Cube () {::log ("\nDelete cube %x...", this); physicsCube->release ();}

	Point position () {return asPoint (physicsCube->getGlobalPose ().p);}
	void position (Point &point) {physicsCube->setGlobalPose (transformTranslatedTo (physicsCube->getGlobalPose (), point));}
	Transformation transformation () {return asTransformation (physicsCube->getGlobalPose ());}
	
	static void Cube::playerThrowCube (); //Special demo code...

	void tick () {} //Runs on its own... so there is nothing to do...
	void draw ();
};

class Sphere : public Object {
public:
	PxRigidDynamic *physicsSphere; float width; float height; float depth;

	Sphere (Point &position, Vector velocity, float width, float height, float depth, float mass) {
		::log ("\nCreate Sphere %x...", this);
		this->width = width; this->height = height; this->depth = depth;
		physicsSphere = physicsManager->physicsBox (position, velocity, width, height, depth, mass);
	}
	virtual ~Sphere () {::log ("\nDelete Sphere %x...", this); physicsSphere->release ();}

	Point position () {return asPoint (physicsSphere->getGlobalPose ().p);}
	void position (Point &point) {physicsSphere->setGlobalPose (transformTranslatedTo (physicsSphere->getGlobalPose (), point));}
	Transformation transformation () {return asTransformation (physicsSphere->getGlobalPose ());}
	
	static void Sphere::playerThrowSphere (); //Special demo code...

	void tick () {} //Runs on its own... so there is nothing to do...
	void draw ();
};

#endif //physicsObjectModule
//*****************************************************************************************//
//                                        Terrain                                          //
//*****************************************************************************************//

#ifndef terrainObjectModule
#define terrainObjectModule 

class Texture;
class Point;
class BoundingBox;
class FaceGroup;
class World; //Forward reference for import method...

class Terrain : public Object {
	//static GLfloat defaultColor [];
	//long pointsSize;
public:
	long divisions;  // Used for culling terrain
	BoundingBox* boundingBoxes;

	Texture* colorTexture;
	Texture* heightTexture;
	float* heightMap;
	long heightMapWidth; long heightMapHeight;
	double xScale, yScale; // meters per pixel
	Point* offset; // used to move terrain
	float seaFloor;
	float mountainTop;
	GamePoint* points;
	unsigned long *indices;
	FaceGroup* faceGroup;

	// When using subgroups
	GamePoint** pointGroups;
	FaceGroup** faceGroups;
	unsigned long** indexGroups;

	Terrain (char * typeName);
	~Terrain ();

	// Constructor-time
	void initialize ();
	float heightFromTextureAt (long x, long y);
	float &heightAt (long x, long y);

	// User for face buidling
	Point pointForXY (long x, long y);
	GamePoint gamePointForXY (long x, long y);

	// Collision Detection
	void XYForPoint (Point &point, long &x, long &y);
	double heightAtPoint (Point &point) {
		long x, z; XYForPoint (point, x, z); //Note: x, z could be out of bounds so we clamp to the border...
		return offset->y + heightAt (clamp (x, 0, heightMapWidth - 1), clamp (z, 0, heightMapWidth - 1)); 
	}
		
	void tick ();
	void draw ();
	void log ();

	void import (::ifstream &input, World *world);

	//PhysX facilities...
	void physicsAttributes (float &xScaler, float &yScaler, float &zScaler, Point &position) {//WILF PhysX...
		//Set terrain-wide attributes needed for building a physics terrain...
		xScaler = xScale;
		yScaler = (mountainTop - seaFloor) / 255.0; //Terrain heights range from 0 to 255...
		zScaler =  yScale; //Texture x/y is terrain x/z.
		position = *offset + Vector (0.0, seaFloor, -zScaler * heightMapHeight); //As we go up the height map from the bottom left corner, z is getting more negative...
	}

	long physicsCoordinateFor (long x, long y) {//where [x,y] are height map coordinate... //WILF PhysX...
		//With the bottom left as the origin of the terrain, y increases going up... In PhysX, the top left is the origin with y increasing going down..
		//So y increasing from 0, 1, 2, 3, ... in the terrain requires a matching y in PhysX that goes 1024, 1023, 1022, ...
		//Also, physX uses a column based representation where increasing x by 1 jumps by one whole cobuilumn (one height). 
		//The terrain by contrast uses a row based representation where jumping y by 1 causes a jump by one whole row (one width)
		return (heightMapHeight - 1 - y) + x * heightMapHeight;
	}

	long physicsHeightFor (long x, long y) {//where [x,y] are height map coordinate...
		Point &fromPoint = pointForXY (x, y); //Accessed via x + y * heightMapWidth
		float yScaler = (mountainTop - seaFloor) / 255.0; //Terrain heights range from 0 to 255...
		return (long) ((fromPoint.y - offset->y - seaFloor) / yScaler); 
	}
};

#endif //terrainObjectModule
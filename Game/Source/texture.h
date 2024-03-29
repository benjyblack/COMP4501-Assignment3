//*****************************************************************************************//
//                                      Texture                                            //
//*****************************************************************************************//

#ifndef textureModule
#define textureModule 

enum TextureType {RGBAType, RGBType, FloatRGBAType};

struct Pixel {unsigned char r, g, b, a;};
struct FloatPixel {float r, g, b, a;};

class Texture {
public:
	TextureType type;
	long width, height;
	GLuint textureHandle;
	bool textureLoaded;
	char *textureName;
	long *bytes;

	Texture (long width, long height, TextureType type);
	~Texture ();

	void activate (); //Do this before drawing a polygon.
	void load (bool mipmapping = true, bool forceClamp = false, long filter = GL_LINEAR); //Give it to the game card (use defaults).
	void unload (); //Let the game card lose it.
	
	Pixel* pixelAt (long x, long y);
	long colorAt (long x, long y);

	static Texture *readTexture (char *fileName);
	static bool readTextureExtent (char *fileName, long &width, long &height);
	static Texture *readUnknownTexture (char *shortTextureName, const bool haltIfNotFound = true);
};

declareCollection (Texture);

#endif //textureModule
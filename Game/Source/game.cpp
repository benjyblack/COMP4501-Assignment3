
//*****************************************************************************************//
//                                      Includes                                           //
//*****************************************************************************************//

#include "includes.all"

//*****************************************************************************************//
//                                        Game                                             //
//*****************************************************************************************//

Game *game = NULL;
double DT; 

HDC Game::deviceContext; GLuint Game::fontBase; 
const bool useLights = false;

void setupOpenGL () {
	glEnable (GL_CULL_FACE); glEnable (GL_DEPTH_TEST); glEnable (GL_TEXTURE_2D);
	glLineWidth (3.0);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (40.0, 1.0, 1.0, 100.0); //See resizeWindow for parameter explanation.
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	GLfloat lightColor [] = {1.0f, 1.0f, 1.0f, 1.0f}; //white
	glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
	glEnable (GL_LIGHT0); glEnable (GL_LIGHTING);
	glDisable (GL_LIGHTING); //We can deal with our own lighting.
	glEnable (GL_COLOR_MATERIAL); //Track color.

	glClearColor (0.0, 0.0, 0.0, 1.0); //black
	//glClearColor (0.0, 0.0, 0.0, 0.0); //transparent black
	//glClearColor (1.0, 0.0, 1.0, 1.0); //purple
	glClearDepth (1.0); //Far
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawBuffer (GL_BACK); glReadBuffer (GL_BACK);
	glEnable (GL_DEPTH_TEST); glDepthFunc (GL_LEQUAL); glDepthMask (GL_TRUE); 
	glShadeModel (GL_SMOOTH);	

	glFrontFace (GL_CCW); glCullFace (GL_BACK); glEnable (GL_CULL_FACE);

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
	glDisable (GL_ALPHA_TEST);
	glPolygonOffset (0.0, -3.0);

	//Setup materials.
	GLfloat	frontMaterialDiffuse [4] = {0.2f, 0.2f, 0.2f, 1.0f};
	glMaterialfv (GL_FRONT, GL_DIFFUSE, frontMaterialDiffuse);
	GLfloat	frontMaterialAmbient [4] = {0.8f, 0.8f, 0.8f, 1.0f};
	glMaterialfv (GL_FRONT, GL_AMBIENT, frontMaterialAmbient);
	GLfloat frontMaterialSpecular [4] = {0.1f, 0.1f, 0.1f, 1.0f};
	glMaterialfv (GL_FRONT, GL_SPECULAR, frontMaterialSpecular);
	GLfloat	frontMaterialShininess [] = {70.0f};
	glMaterialfv (GL_FRONT, GL_SHININESS, frontMaterialShininess);
	GLfloat	frontMaterialEmission [4] = {0.0f, 0.0f, 0.0f, 1.0f};
	glMaterialfv (GL_FRONT, GL_EMISSION, frontMaterialEmission);

	GLfloat	backMaterialDiffuse [4] = {0.0f, 0.0f, 0.0f, 1.0f};
	glMaterialfv (GL_BACK, GL_DIFFUSE, backMaterialDiffuse);
	GLfloat	backMaterialAmbient [4] = {0.0f, 0.0f, 0.0f, 1.0f};
	glMaterialfv (GL_BACK, GL_AMBIENT, backMaterialAmbient);
	GLfloat	backMaterialSpecular [4] = {0.0f, 0.0f, 0.0f, 1.0f};
	glMaterialfv (GL_BACK, GL_SPECULAR, backMaterialSpecular);
	GLfloat	backMaterialShininess [] = {0.0f};
	glMaterialfv (GL_BACK, GL_SHININESS, backMaterialShininess);
	GLfloat	backMaterialEmission [4] = {0.0f, 0.0f, 0.0f, 1.0f};
	glMaterialfv (GL_BACK, GL_EMISSION, backMaterialEmission);

	glColorMaterial (GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//Use a global default texture environment mode.
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if (useLights) glEnable (GL_LIGHTING); else glDisable (GL_LIGHTING);
}

void Game::setup () {
	setupOpenGL (); setupOpenGLExtensions ();
	glewInit ();
	setupFont ();
	physicsManager = new PhysicsManager (); 
	game = new Game ();
	Player::setup ();
	Camera::setup ();
	InputManager::setup ();
	World::setup ();
	FaceGroup::setup ();
	Shader::setup ();
	setupColoredLights();
}

void Game::wrapup () {
	wrapupColoredLights();
	Shader::wrapup ();
	FaceGroup::wrapup ();
	//if (game == NULL) return; //Wrapping up a second time (shouldn't happen).
	if (game->world != NULL) {delete game->world; game->world = NULL;}
	wrapupFont ();
	delete game; game = NULL;
	Player::wrapup ();
	Camera::wrapup ();
	InputManager::wrapup ();
	World::wrapup ();
	delete physicsManager;
	::log ("\nEnding game...\n\n");
}

void Game::tick () {
	inputManager->tick ();
	camera->tick ();
	player->tick ();
	if (world != NULL) world->tick ();
	tickColoredLights();
}

void Game::drawTeapots () {
	//Draw 4 teapots around a circle... It's too hard to find just one if the mouse rotates too fast.
	Point locations [] = {Point (2.0, 0.0, 0.0), Point (-2.0, 0.0, 0.0), Point (0.0, 0.0, 2.0),
		Point (0.0, 0.0, -2.0)};
	glColor4d (1.0, 1.0, 0.0, 1.0); //Yellow...

	glDisable (GL_TEXTURE_2D);
	for (long index = 0; index < 4; index++) {
		glPushMatrix ();
			Point &location = locations [index];
			glTranslated (location.x, location.y, location.z); 
			glutSolidTeapot (0.5);
		glPopMatrix ();
	}
	glColor4d (1.0, 1.0, 1.0, 1.0); //White...
}

bool disableShaders = false;
void Game::draw () {
	//If there is no world, draw a teapot; otherwise, draw the world...
	//Neither the input manager nor the camera draws itself...
	
	camera->beginCamera ();
		if (world == NULL) { 
			drawTeapots ();
		} else {
			switch (drawingChoice) {
				case UseNormalDrawing: 
					world->draw (); 
					if (drawFuzzBallsInNormalDraw) {drawAllLightFuzzBallsInNormalWorld ();} 
					else if (drawLightSpheresInNormalDraw) {drawAllLightSpheresInNormalWorld ();}
					break;
				case UseIndexedLights: 
					drawColoredLights (world); 
					break;
			}
		}
		player->draw ();
	camera->endCamera ();
	drawFrameRate ();
	drawHelp ();
}

void Game::setupFont () {
	deviceContext = GetDC (NULL);
	HFONT font; //Windows font ID...
	fontBase = glGenLists (96); //Storage for 96 characters
	font = CreateFont (	
		-24,							//Height of font
		0,								//Width of font
		0,								//Angle of escapement
		0,								//Orientation angle
		FW_BOLD,						//Font weight
		FALSE,							//Italic
		FALSE,							//Underline
		FALSE,							//Strikeout
		ANSI_CHARSET,					//Character set identifier
		OUT_TT_PRECIS,					//Output precision
		CLIP_DEFAULT_PRECIS,			//Clipping precision
		ANTIALIASED_QUALITY,			//Output quality
		FF_DONTCARE | DEFAULT_PITCH,	//Family and pitch
		"Arial");						//Font name

	SelectObject (deviceContext, font);			//Selects The Font We Want
	wglUseFontBitmaps (deviceContext, 32, 96, fontBase); //Builds 96 characters starting at character 32
}

void Game::wrapupFont () {
	ReleaseDC (NULL, deviceContext);
	glDeleteLists (fontBase, 96); //Delete all 96 characters
}

void Game::privateDrawString (const char *text) {
	glPushAttrib (GL_LIST_BIT);	//Pushes the display list bits
	glListBase (fontBase - 32); //Sets the base character to 32
	glCallLists (strlen (text), GL_UNSIGNED_BYTE, text); //Draws the display list text
	glPopAttrib (); //Pops the display list bits
}

extern long screenWidth, screenHeight;

void Game::begin2DDrawing () {
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
		glLoadIdentity ();
		glOrtho (0.0, (GLfloat) screenWidth, 0.0, (GLfloat) screenHeight, -100.0f, 100.0f);
		glMatrixMode (GL_MODELVIEW);
		glPushMatrix ();
			glLoadIdentity ();
}

void Game::end2DDrawing () {
			glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

void Game::drawMessage (long x, long y, const char *message, ...) {
	char text [1000]; va_list parameters;									
	if (message == NULL) return;

	//Extract variable length parameters and copy into text as in printf.
	va_start (parameters, message);					
	    vsprintf (text, message, parameters);		
	va_end (parameters);

	//Determine the end of the string and convert each occurrence of '\n' to '\0'.
	char *end = text + strlen (text);
	for (char *next = text; *next != '\0'; next++) {
		if (*next == '\n') *next = '\0';
	}

	//Draw the multi-line message...
	glUseProgram (0); //Use no shader...
	begin2DDrawing ();
		glDepthFunc (GL_ALWAYS);
		glDisable (GL_TEXTURE_2D);
		glDisable (GL_LIGHTING);
		glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
		long yOffset = y;
		for (char *line = text; line < end; line += strlen (line) + 1) {
			glRasterPos2i (x, yOffset); yOffset -= 32;
			privateDrawString (line);
		}
		glDepthFunc (GL_LEQUAL);
		if (useLights) glEnable (GL_LIGHTING);
	end2DDrawing ();
}

void Game::drawFrameRate () {
	//Draw the frame rate avoiding extreme fluctuations (since all you see is flickering).
	double frameRate = 1.0 / DT; //Frames/sec = 1/(seconds per frame).
	static double stableRate = frameRate; //This initializes only the first time...
	static double oldFrameRate = frameRate;
	//If it changed by more than 2 per cent of the stable value, use the new value; otherwise use the stable one...
	if (absolute (frameRate - stableRate) > 4.0) stableRate  = frameRate; 

	Point position = player != NULL ? player->position () : Zero; 
	char *positionString = player != NULL ? asString (" at [%3.2f,%3.2f,%3.2f]", position.x, position.y, position.z) : "";
	//drawMessage (1, screenHeight-20, "angle %3.1f FPS: %3.1f%s %s", shaderManager->xAngle, stableRate, positionString, 
	//	game == NULL || game->world == NULL ? "" : (game->flyModeOn ? "FLYING" : "COLLISION DETECTION"));
	drawMessage (1, screenHeight-20, "FPS: %3.1f%s %s", stableRate, positionString, 
		game == NULL || game->world == NULL ? "" : (game->flyModeOn ? "FLYING" : "COLLISION DETECTION"));
}

void Game::drawNote (const char *message, ...) {
	char text [1000]; va_list parameters;									
	if (message == NULL) return;

	//Extract variable length parameters and copy into text as in printf.
	va_start (parameters, message);					
	    vsprintf (text, message, parameters);		
	va_end (parameters);

	drawMessage (1, screenHeight-52, "%s", text);
}

void Game::drawHelp () {
	if (helpOn) {
		drawMessage (1, screenHeight-50, "%s", "W - move forward");
		drawMessage (1, screenHeight-80, "%s", "S - move back");
		drawMessage (1, screenHeight-110, "%s", "A - move left");
		drawMessage (1, screenHeight-140, "%s", "D - move right");
		drawMessage (1, screenHeight-170, "%s", "Q - move up");
		drawMessage (1, screenHeight-200, "%s", "E - move down");
		drawMessage (1, screenHeight-230, "%s", "f - toggle flymode");
		drawMessage (1, screenHeight-260, "%s", "t - toggle wireframe");
		drawMessage (1, screenHeight-290, "%s", "1 - throw cube");
		drawMessage (1, screenHeight-320, "%s", "2 - throw sphere");
		drawMessage (1, screenHeight-350, "%s", "? - toggle help");
	}
}
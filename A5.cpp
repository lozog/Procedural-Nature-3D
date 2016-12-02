#include "A5.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>								// rand, srand

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

// Simple OpenGL Image Library
// http://www.lonesock.net/soil.html
#include "SOIL.h"

#include "lsystem.hpp"

using namespace glm;
using namespace std;

// terrain needs to be square or terrain map vertices get all hecked up
static const size_t TERRAIN_WIDTH = 100;
static const size_t TERRAIN_LENGTH = TERRAIN_WIDTH;
static size_t WATER_HEIGHT = 9; // 17
static const unsigned int NUM_OCTAVES = 7; // # of octaves for terrain generation
static double REDIST = 0.8f; // 1.05f;
static const unsigned int PLANT_DENSITY = 2000; // density of foliage (lower->denser)
static bool drawShadowDebugQuad = false;

//----------------------------------------------------------------------------------------
// Constructor
A5::A5()
	: theTerrain(TERRAIN_WIDTH, TERRAIN_LENGTH, NUM_OCTAVES, REDIST),
	theWater(TERRAIN_WIDTH, TERRAIN_LENGTH),
	theSkybox(),
	mouseSensitivity(0.05f),
	forwardPress(false),
	backwardPress(false),
	leftPress(false),
	rightPress(false),
	upPress(false),
	downPress(false)
{
	treeMap = new bool*[TERRAIN_LENGTH];
	grassMap = new bool*[TERRAIN_LENGTH];
	for(int i = 0; i < TERRAIN_LENGTH; i += 1) {
		treeMap[i] = new bool[TERRAIN_WIDTH];
		grassMap[i] = new bool[TERRAIN_WIDTH];
	} // for

	reset();
}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{
	// delete foliage
	theTrees.clear();
	theGrass.clear();

	// delete maps
	for(int i = 0; i < TERRAIN_LENGTH; i += 1) {
		delete [] treeMap[i];
		delete [] grassMap[i];
	} // for
	delete [] treeMap;
	delete [] grassMap;
}

//----------------------------------------------------------------------------------------
// Reset things
void A5::reset() {
	resetCamera();
	resetLight();
	resetFoliage();

	cout << "controls:" << endl;
	cout << "U/Y: raise/lower x position of the Sun" << endl;
	cout << "O/I: raise/lower # of octaves in terrain" << endl;
	cout << "H/G: raise/lower water level" << endl;
	cout << "K/J: raise/lower sun intensity" << endl;
	cout << "L: toggle noise function implementation (Simplex vs. my Perlin)" << endl;
	cout << "M/N: raise/lower distribution power" << endl;
	cout << "B: toggle shadow map debug quad" << endl;
}

//----------------------------------------------------------------------------------------
// Reset camera position
void A5::resetCamera() {
	cameraPos 		= glm::vec3( 0.0f, 0.0f, 0.0f );
	cameraFront 	= glm::vec3( 1.0f, 0.0f, 0.0f );
	cameraRight = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront);
	cameraUp 		= glm::vec3( 0.0f, 1.0f, 0.0f );
	firstMouseMove 	= false;
	pitch 			= 0.0f;
	yaw 			= 0.0f;
	cameraSpeed 	= 1.0f;
	REDIST 			= 1.05f;

	switch( TERRAIN_LENGTH ) {
		case 512:
			// position camera to have a view of 512x512 grid by default
			cout << "grid is 512x512" << endl;
			cameraPos 		= glm::vec3( -286.0f, 74.0f, -284.0f );
			cameraFront 	= glm::vec3( 0.589f, -0.262f, 0.764f );
			pitch = -16.5f;
			yaw = 52.42f;
		break;
		case 200:
			cout << "grid is 200x200" << endl;
			// position camera to have a view of 200x200 grid by default
			cameraPos 		= glm::vec3( -121.0f, 57.0f, -74.0f );
			cameraFront 	= glm::vec3( 0.800f, -0.157f, 0.579f );
			pitch = -27.0f;
			yaw = 10.1f;
			cameraSpeed = 1.5f;
		break;
		case 100:
			cout << "grid is 100x100" << endl;
			// position camera to have a view of 100x100 grid by default
			cameraPos 		= glm::vec3( -88.3f, 66.0f, -34.8f );
			cameraFront 	= glm::vec3( 0.870f, -0.403f, 0.285f );

			// cameraPos 		= glm::vec3( 134.0f, 45.0f, 20.0f );
			// cameraFront 	= glm::vec3( -0.962, -0.176, -0.208f );

			pitch = -23.77f;
			yaw = 18.13f;
			cameraSpeed = 1.5f;
			REDIST = 0.95f;
		break;
	} // switch
}

void A5::resetFoliage() {
	for( int x = 0; x < TERRAIN_LENGTH; x += 1 ) {
		for( int z = 0; z < TERRAIN_WIDTH; z += 1 ) {
			treeMap[x][z] = false;
			grassMap[x][z] = false;
		} // for
	} // for

	theTrees.clear();
	theGrass.clear();
}

//----------------------------------------------------------------------------------------
// Reset lights
void A5::resetLight() {
	m_theSunColour = glm::vec3(1.0f, 0.7f, 0.0f);
	m_theSunDir = glm::vec3(0.1f, 0.0f, -0.5f);
	m_theSunIntensity = 0.7f;
	m_globalAmbientLight = glm::vec3(0.3f, 0.3f, 0.3f);
}

//----------------------------------------------------------------------------------------
// Load object texture with RGB
void A5::loadTexture( const char* filename, GLuint* texture ) {

	// generate texture
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	// load texture image with SOIL
	int width, height;
	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
	
	// bind image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// colours seem more saturated (better) WITHOUT this
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// free image and unbind texture
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Load object texture with RGBA
void A5::loadTextureAlpha( const char* filename, GLuint* texture ) {

	// generate texture
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	// load texture image with SOIL
	int width, height;
	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);

	// bind image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// colours seem more saturated (better) WITHOUT this
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// free image and unbind texture
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Load skybox textures
void A5::loadSkybox( const std::vector<std::string> filenames, GLuint* texture ) {

	// generate texture
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *texture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// load texture image with SOIL
	int width, height;
	unsigned char* image;

	unsigned int numFaces = filenames.size();
	for ( GLuint i = 0; i < numFaces; i += 1 ) {

		const char* filename = filenames.at(i).c_str();
		image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
		// cout << filename << " " << width << " " << height << endl;
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);

	} // for
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free image and unbind texture
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor( 0.1, 0.1, 0.1, 1.0 );

	// TODO: loading screen

	//----------------------------------------------------------------------------------------
	/*
	 * Set up vertex shader
	 */

	// Build the vertex shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up vertex uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	theSunColour_uni		= m_shader.getUniformLocation( "theSunColour" );
	theSunDir_uni 			= m_shader.getUniformLocation( "theSunDir" );
	theSunIntensity_uni 	= m_shader.getUniformLocation( "theSunIntensity" );
	globalAmbientLight_uni 	= m_shader.getUniformLocation( "globalAmbientLight" );
	eye_uni 				= m_shader.getUniformLocation( "eye" );
	P_lightspace_uni = m_shader.getUniformLocation( "lightProj" );
	V_lightspace_uni = m_shader.getUniformLocation( "lightView" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up skybox shader
	 */

	// Build the skybox shader
	m_skybox_shader.generateProgramObject();
	m_skybox_shader.attachVertexShader(
		getAssetFilePath( "skyboxVertexShader.vs" ).c_str() );
	m_skybox_shader.attachFragmentShader(
		getAssetFilePath( "skyboxFragmentShader.fs" ).c_str() );
	m_skybox_shader.link();

	// Set up skybox uniforms
	P_skybox_uni = m_skybox_shader.getUniformLocation( "P" );
	V_skybox_uni = m_skybox_shader.getUniformLocation( "V" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up billboard shader
	 */

	// Build the billboard shader
	m_billboard_shader.generateProgramObject();
	m_billboard_shader.attachVertexShader(
		getAssetFilePath( "billboardVertexShader.vs" ).c_str() );
	m_billboard_shader.attachFragmentShader(
		getAssetFilePath( "billboardFragmentShader.fs" ).c_str() );
	m_billboard_shader.link();

	// Set up billboard uniforms
	grass_position_uni 	= m_billboard_shader.getUniformLocation( "grass_position" );
	cameraUp_uni 		= m_billboard_shader.getUniformLocation( "cameraUp" );
	cameraRight_uni 	= m_billboard_shader.getUniformLocation( "cameraRight" );
	P_billboard_uni 	= m_billboard_shader.getUniformLocation( "P" );
	V_billboard_uni 	= m_billboard_shader.getUniformLocation( "V" );
	M_billboard_uni 	= m_billboard_shader.getUniformLocation( "M" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up shadowmap shader
	 */

	// Build the shadowmap shader
	m_shadow_shader.generateProgramObject();
	m_shadow_shader.attachVertexShader(
		getAssetFilePath( "shadowVertexShader.vs" ).c_str() );
	m_shadow_shader.attachFragmentShader(
		getAssetFilePath( "shadowFragmentShader.fs" ).c_str() );
	m_shadow_shader.link();

	// Set up vertex uniforms
	P_shadow_uni = m_shadow_shader.getUniformLocation( "P" );
	V_shadow_uni = m_shadow_shader.getUniformLocation( "V" );
	M_shadow_uni = m_shadow_shader.getUniformLocation( "M" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up debugquad shader
	 */

	// Build the debugquad shader
	m_debugquad_shader.generateProgramObject();
	m_debugquad_shader.attachVertexShader(
		getAssetFilePath( "debugquadVertexShader.vs" ).c_str() );
	m_debugquad_shader.attachFragmentShader(
		getAssetFilePath( "debugquadFragmentShader.fs" ).c_str() );
	m_debugquad_shader.link();

	//----------------------------------------------------------------------------------------
	/*
	 * Load textures from images into OpenGL
	 */

	// load model textures
	loadTexture("res/grass.png", &m_ground_texture);
	loadTexture("res/water.png", &m_water_texture);
	loadTexture("res/bark.png", &m_tree_texture);
	loadTextureAlpha("res/sgrass5-1.png", &m_grass_texture);
	loadTextureAlpha("res/grass-screendoor.png", &m_screendoor_texture);

	// load skybox texture - CAUTION: order matters!
	const std::vector<std::string> skyboxTextureFiles {
		/*"res/skybox/rt.png",
		"res/skybox/lf.png",
		"res/skybox/up.png",
		"res/skybox/dn.png",
		"res/skybox/bk.png",
		"res/skybox/ft.png"*/
		/*"res/skybox/arid2/arid2_rt.jpg",
		"res/skybox/arid2/arid2_lf.jpg",
		"res/skybox/arid2/arid2_up.jpg",
		"res/skybox/arid2/arid2_dn.jpg",
		"res/skybox/arid2/arid2_bk.jpg",
		"res/skybox/arid2/arid2_ft.jpg"*/
		"res/skybox/desertdawn/desertdawn_ft.jpg",
		"res/skybox/desertdawn/desertdawn_bk.jpg",
		"res/skybox/desertdawn/desertdawn_up.jpg",
		"res/skybox/desertdawn/desertdawn_dn.jpg",
		"res/skybox/desertdawn/desertdawn_rt.jpg",
		"res/skybox/desertdawn/desertdawn_lf.jpg"
	};
	loadSkybox( skyboxTextureFiles, &m_skybox_texture );

	//----------------------------------------------------------------------------------------
	/*
	 * Finish initialization - init the environment, set up OpenGL, etc.
	 */

	// initialize environment
	initEnvironment();

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );

	// grab and hide the cursor for first-person mode
	glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

	// specify the blending function to be used (in this application, used for billboards)
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void A5::initEnvironment() {
	initShadowMap( &m_shadow_texture, &shadowMap_FBO );

	theTerrain.init( m_shader, m_ground_texture );
	theWater.init( m_shader, m_water_texture, WATER_HEIGHT );

	resetFoliage();
	initFoliage();

	grass.init( m_billboard_shader, m_grass_texture, m_screendoor_texture );

	theSkybox.init( m_skybox_shader, m_skybox_texture );
}

void A5::initShadowMap( GLuint* texture, GLuint* fbo ) {
	// GLuint shadowMap_frameBuffer = 0;
	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
	// cout << "shadow: " << *texture << endl;

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
		m_framebufferWidth, m_framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *texture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		cout << "error rendering shadow map to framebuffer" << endl;
		return;
	}
}

void A5::initFoliage() {

	// define some L-Systems
	// each system guides the generation of different "species"
	Rule sys1rule1("F", "FF/[/F&&F\\F]\\[\\F^F^F]");
	Rules sys1rules;
	sys1rules.push_back(&sys1rule1);

	Rule sys2rule1("F", "F[&F[//F^+F]]+F[\\F]");
	Rules sys2rules;
	sys2rules.push_back(&sys2rule1);

	Rule sys3rule1("F", "FF[F//F][&\\F]");
	Rules sys3rules;
	sys3rules.push_back(&sys3rule1);

	vector<Rules> treeLSystems;
	treeLSystems.push_back(sys1rules);
	treeLSystems.push_back(sys2rules);
	treeLSystems.push_back(sys3rules);

	const unsigned int numTreeLSystems = treeLSystems.size();

	// TODO: more complicated rules might not all have the same axiom
	string axiom = "F";

	unsigned int numTrees = 0;
	double** heightMap = theTerrain.getHeightMap();
	srand(1995); // good year
	for (int x = 1; x < TERRAIN_LENGTH-1; x += 1) {
		for (int z = 1; z < TERRAIN_WIDTH-1; z += 1) {
			double scale = 0.5f;
			double gridX = ((double)x / ((double)TERRAIN_LENGTH * scale)) - 0.5f;
			double gridY = ((double)z / ((double)TERRAIN_WIDTH * scale)) - 0.5f;

			// randomly pick spots to plant trees
			// in the future, could use a random perturbation scatter to pick spots
			int random = rand() % PLANT_DENSITY;
			bool plantTree = ( random < 3 || random > PLANT_DENSITY - 2 ? true : false );
			bool plantGrass = ( random > 3 && random <= 1000 ? true : false );

			if( plantTree ) {
				// decide if chosen position is suitable for tree
				// not underwater, slope not too steep, not "too close" to another tree

				// make sure above water
				if (heightMap[x][z] <= WATER_HEIGHT + 1.0f) continue;
				// TODO: check normal of terrain
				
				// make sure tree has breathing room (ha!)
				if (treeMap[x-1][z] ||
					treeMap[x][z-1] ||
					treeMap[x-1][z-1] ) {
					// cout << "skipping tree at " << x << ", " << z << endl;
					continue;
				}

				treeMap[x][z] = true;

				// generate seed for this tree (get it? seed?!)
				int treetype = random % numTreeLSystems;
				Rules treeRules = treeLSystems.at(treetype);
				string seed = LSystem::generateExpr(axiom, treeRules, 3);		

				glm::vec3 position = glm::vec3((float)x, heightMap[x][z]-0.25f, (float)z);
				LTree* tree = new LTree();

				// TODO: encapsulate randomness of angles, etc. with treetype prods in treetype object
				tree->init(
					glm::vec3(1.0f, 0.0f, 0.0f),						// heading vector
					glm::vec3(0.0f, -1.0f, 0.0f),						// down vector (direction of gravity)
					position,						// tree position
					seed,												// L-system expression
					0.75f,												// contraction ratio
					18.0f,												// divergence angle
					10.0f,												// range of divergence
					0.9f,												// length (width?) decrease ratio,
					m_shader, m_tree_texture
				);
				theTrees.push_back(tree);

			} else if( plantGrass ) {
				if ( x == 1 || z == 1 || x == TERRAIN_LENGTH - 1 || z == TERRAIN_WIDTH - 1)
					continue;
				// decide if chosen position is suitable for grass
				// not *in* tree, not underwater, slope not too steep

				// make sure above underwater
				if (heightMap[x][z] <= WATER_HEIGHT + 1.0f) continue;
				// TODO: check normal of terrain
				if ( treeMap[x][z] ) continue;

				grassMap[x][z] = true;

				glm::vec3* position = new glm::vec3((float)x, heightMap[x][z]+0.4f, (float)z);
				theGrass.push_back(position);
			} // if
		} // for
	} // for
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
	// calculate viewMatrix
	if ( forwardPress ) moveCameraForward();
	if ( backwardPress ) moveCameraBackward();
	if ( leftPress ) moveCameraLeft();
	if ( rightPress ) moveCameraRight();
	if ( upPress ) moveCameraUp();
	if ( downPress ) moveCameraDown();

	// cout << cameraPos + cameraFront << endl;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	#if 0
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A5 running simultaneously, this would break; you'd want to make
	// this into instance fields of A5.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_ENABLED );


	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	// ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "##Colour", colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 0 ) ) {
			// Select this colour.
		}
		ImGui::PopID();

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}

	glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

	#endif
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
// FROM: http://learnopengl.com/code_viewer.php?code=advanced-lighting/shadow_mapping_depth_map
void RenderQuad()
{
	GLuint quadVAO = 0;
	GLuint quadVBO;
 
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void A5::drawShadowMap( glm::mat4* W, glm::mat4* lightProj, glm::mat4* lightView ) {
	// render depth of scene from light's perspective

	glClear(GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv( P_shadow_uni, 1, GL_FALSE, value_ptr( *lightProj ) );
	glUniformMatrix4fv( V_shadow_uni, 1, GL_FALSE, value_ptr( *lightView ) );
	glUniformMatrix4fv( M_shadow_uni, 1, GL_FALSE, value_ptr( *W ) );

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap_FBO);

	/*theTerrain.draw();
	// theWater.draw();
	for( LTree* tree : theTrees ) {
		tree->draw();
	} // for
	for( glm::vec3* grassPosition : theGrass ) {
		glUniform3fv( grass_position_uni, 1, value_ptr( *grassPosition ) );
		grass.draw();
	} // for*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void A5::drawSkybox() {
	m_skybox_shader.enable();

		// glUniform1i(m_skybox_shader.getUniformLocation("billboard"), 0);

		// set skybox matrix uniforms
		glm::mat4 rotateOnlyView = glm::mat4(glm::mat3( view ));
		glUniformMatrix4fv( P_skybox_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_skybox_uni, 1, GL_FALSE, value_ptr( rotateOnlyView ) );

		theSkybox.draw(); 			// draw skybox first

	m_skybox_shader.disable();
}

void A5::drawObjects( glm::mat4* W, glm::mat4* lightProj, glm::mat4* lightView ) {
	m_shader.enable();

		// set matrix uniforms
		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( *W ) );

		glUniformMatrix4fv( P_lightspace_uni, 1, GL_FALSE, value_ptr( *lightProj ) );
		glUniformMatrix4fv( V_lightspace_uni, 1, GL_FALSE, value_ptr( *lightView ) );

		// set uniforms for theSun
		glUniform3fv( theSunColour_uni, 1, value_ptr( m_theSunColour ) );
		glUniform3fv( theSunDir_uni, 1, value_ptr( m_theSunDir ) );
		glUniform1f( theSunIntensity_uni, m_theSunIntensity );

		// ambient light uniform
		glUniform3fv( globalAmbientLight_uni, 1, value_ptr( m_globalAmbientLight ) );

		glUniform3fv( eye_uni, 1, value_ptr( cameraPos ) );

		// draw environment
		theTerrain.draw();
		theWater.draw();

		for( LTree* tree : theTrees ) {
			tree->draw();
		} // for

	m_shader.disable();
}

void A5::drawBillboards( glm::mat4* W ) {
	m_billboard_shader.enable();
	// glEnable( GL_BLEND );

		// set matrix uniforms
		glUniformMatrix4fv( P_billboard_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_billboard_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_billboard_uni, 1, GL_FALSE, value_ptr( *W ) );

		// pass in camera vectors for billboard calculations
		glUniform3fv( cameraUp_uni, 1, value_ptr( cameraUp ) );
		glUniform3fv( cameraRight_uni, 1, value_ptr( cameraRight ) );

		glUniform1i(m_billboard_shader.getUniformLocation("billboard"), 0);
		glUniform1i(m_billboard_shader.getUniformLocation("screendoor"), 1);

		for( glm::vec3* grassPosition : theGrass ) {
			glUniform3fv( grass_position_uni, 1, value_ptr( *grassPosition ) );
			grass.draw();
		} // for

	// glDisable( GL_BLEND );
	m_billboard_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(TERRAIN_WIDTH)/2.0f, 0, -float(TERRAIN_WIDTH)/2.0f ) );

	// calculate ortho projection matrix for light's POV
	glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f);
	glm::mat4 lightView = glm::lookAt(
									  // glm::vec3(-10.0f, 4.0f, 5.0f),
									  -m_theSunDir,
									  glm::vec3(20.0f, 0.0f, 20.0f),
									  glm::vec3(0.0f, 1.0f, 0.0f)
									 );

	glEnable( GL_DEPTH_TEST );

	m_shadow_shader.enable();

		drawShadowMap( &W, &lightProj, &lightView );

	m_shadow_shader.disable();

	// glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;							// DEBUG
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( drawShadowDebugQuad ) {
		m_debugquad_shader.enable();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_shadow_texture);
		RenderQuad();
		m_debugquad_shader.disable();
	} else {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_shadow_texture);
		drawSkybox();
		drawObjects( &W, &lightProj, &lightView );
		drawBillboards( &W );
	}
	
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A5::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if ( !firstMouseMove ) {
			// prevent jumping on first mouse movement
			// cout << "firstMouseMove" << endl;
			xPosPrev = xPos;
			yPosPrev = yPos;
			firstMouseMove = true;
		} // if

		double xOffset = xPos - xPosPrev;
		double yOffset = yPosPrev - yPos;

		// cout << xPos << " from " << xPosPrev << " becomes " << xOffset << endl;

		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw		+= xOffset;
		pitch 	+= yOffset;

		// limit range of up/down motion
		if( pitch > 89.0f )  pitch = 89.0f;
		if( pitch < -89.0f ) pitch = -89.0f;

		// calculate new direction vector for camera
		glm::vec3 front;
	    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);

		// calculate new up & right vectors for camera
		cameraRight = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront);
		cameraUp = glm::cross(cameraFront, cameraRight);

		xPosPrev = xPos;
		yPosPrev = yPos;
	} // if

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A5::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A5::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A5::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		// Respond to some key events.

		// UI controls
		if (key == GLFW_KEY_ESCAPE) {
			// quit application
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_R) {
			// reset
			reset();
			eventHandled = true;
		} // if

		// Camera controls
		if (key == GLFW_KEY_W) {
			// toggle forward movement
			forwardPress = true;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_S) {
			// toggle backward movement
			backwardPress = true;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_A) {
			// toggle left movement
			leftPress = true;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_D) {
			// toggle right movement
			rightPress = true;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_Q) {
			// toggle up movement
			downPress = true;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_E) {
			// toggle down movement
			upPress = true;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_Z) {
			cameraSpeedDown();
			eventHandled = true;
		}
		if (key == GLFW_KEY_X) {
			cameraSpeedUp();
			eventHandled = true;
		}
		if (key == GLFW_KEY_P) {
			// print debug info
			cout << cameraPos << endl;
			cout << cameraFront << endl;
			cout << pitch << " " << yaw << endl;
			eventHandled = true;
		}

		// Debug controls
		if (key == GLFW_KEY_O) {
			theTerrain.numOctaves += 1;
			cout << theTerrain.numOctaves << " octaves" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_I) {
			if ( theTerrain.numOctaves > 0)
				theTerrain.numOctaves -= 1;
			cout << theTerrain.numOctaves << " octaves" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_L) {
			theTerrain.mode += 1;
			theTerrain.mode %= theTerrain.numModes; // cycle through terrain modes
			cout << "mode: " << theTerrain.mode << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_K) {
			m_theSunIntensity += 0.1f;;
			cout << m_theSunIntensity << " intensity" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_J) {
			if ( m_theSunIntensity > 0.1f)
				m_theSunIntensity -= 0.1f;
			cout << m_theSunIntensity << " intensity" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_M) {
			theTerrain.redist += 0.05f;
			cout << theTerrain.redist << " redist power" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_N) {
			if ( theTerrain.redist > 0.01f)
				theTerrain.redist -= 0.05f;
			cout << theTerrain.redist << " redist power" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_U) {
			m_theSunDir += glm::vec3(0.5f, 0.0f, 0.0f);
			cout << m_theSunDir << " sun direction" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_Y) {
			if ( m_theSunDir.x > 0.0f)
				m_theSunDir -= glm::vec3(0.5f, 0.0f, 0.0f);
			cout << m_theSunDir << " sun direction" << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_H) {
			WATER_HEIGHT += 1;
			cout << "water height: " << WATER_HEIGHT << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_G) {
			WATER_HEIGHT -= 1;
			cout << "water height: " << WATER_HEIGHT << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_B) {
			drawShadowDebugQuad = !drawShadowDebugQuad;
			initEnvironment();
			eventHandled = true;
		}
	} // if

	if ( action == GLFW_RELEASE ) {

		// Camera Controls
		if ( key == GLFW_KEY_W ) {
			forwardPress = false;
			eventHandled = true;
		} // if
		if ( key == GLFW_KEY_S ) {
			backwardPress = false;
			eventHandled = true;
		} // if
		if ( key == GLFW_KEY_A ) {
			leftPress = false;
			eventHandled = true;
		} // if
		if ( key == GLFW_KEY_D ) {
			rightPress = false;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_Q) {
			downPress = false;
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_E) {
			upPress = false;
			eventHandled = true;
		} // if
	} // if

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Camera control functions
 */

#define DEBUG_CAMERAMOVEMENT 0
void A5::moveCameraForward() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "forward" << endl;
	#endif
	cameraPos += cameraSpeed * cameraFront;
}

void A5::moveCameraBackward() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "backward" << endl;
	#endif
	cameraPos -= cameraSpeed * cameraFront;
}

void A5::moveCameraLeft() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "left" << endl;
	#endif
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void A5::moveCameraRight() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "right" << endl;
	#endif
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void A5::moveCameraUp() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "up" << endl;
	#endif
	cameraPos += glm::normalize(cameraUp) * cameraSpeed;
}

void A5::moveCameraDown() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "down" << endl;
	#endif
	cameraPos -= glm::normalize(cameraUp) * cameraSpeed;
}

void A5::cameraSpeedUp() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "camera speed up" << endl;
	#endif
	cameraSpeed += 0.1f;
}

void A5::cameraSpeedDown() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "camera speed down" << endl;
	#endif
	cameraSpeed -= 0.1f;
	if (cameraSpeed < 0.1f) cameraSpeed = 0.1f;
}
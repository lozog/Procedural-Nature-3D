#include "A5.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "inputparams.hpp"

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

using namespace glm;
using namespace std;

//----------------------------------------------------------------------------------------
// Constructor
A5::A5( int argc, char **argv )
	: theSkybox(),
	mouseSensitivity(0.05f),
	forwardPress(false),
	backwardPress(false),
	leftPress(false),
	rightPress(false),
	upPress(false),
	downPress(false)
{
	switch( argc ) {
		case 2:
			readInputParams( argv[1] );
		break;
		default:
			readInputParams( "inparams/inparams1" );
	} // switch

	// cout << TERRAIN_LENGTH << " " << TERRAIN_WIDTH << " " << NUM_OCTAVES << " " << REDIST << endl;

	theTerrain.create(TERRAIN_WIDTH, TERRAIN_LENGTH, NUM_OCTAVES, REDIST);
	theWater.create(TERRAIN_WIDTH, TERRAIN_LENGTH);

	treeMap = new bool*[TERRAIN_LENGTH];
	grassMap = new bool*[TERRAIN_LENGTH];
	leafColourMap = new glm::vec4*[TERRAIN_LENGTH];
	// grassColourMap = new glm::vec4*[TERRAIN_LENGTH];
	for(int i = 0; i < TERRAIN_LENGTH; i += 1) {
		treeMap[i] = new bool[TERRAIN_WIDTH];
		grassMap[i] = new bool[TERRAIN_WIDTH];
		leafColourMap[i] = new glm::vec4[TERRAIN_WIDTH];
		// grassColourMap[i] = new glm::vec4[TERRAIN_WIDTH];
	} // for

	reset();
}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{
	// delete foliage
	resetFoliage();

	// delete LSystems
	for( Rules* rules : treeLSystems ) {
		for( Rule* rule : *rules ) {
			delete rule;
		} // for
		delete rules;
	} // for
	treeLSystems.clear();

	// delete maps
	for(int i = 0; i < TERRAIN_LENGTH; i += 1) {
		delete [] treeMap[i];
		delete [] grassMap[i];
		delete [] leafColourMap[i];
		// delete [] grassColourMap[i];
	} // for
	delete [] treeMap;
	delete [] grassMap;
	delete [] leafColourMap;
	// delete [] grassColourMap;
}

//----------------------------------------------------------------------------------------
// Reset things
void A5::reset() {
	resetCamera();
	resetLight();
	resetFoliage();

	cout << "3D Nature Environment with Procedurally Generated Terrain and Trees" << endl;
	cout << "By Liam Ozog" << endl;
	cout << "CS488 Fall 2016" << endl;
	cout << endl;

	cout << "Controls:" << endl;
	cout << "1/2: change sunlight X direction" << endl;
	cout << "3/4: change sunlight Y direction" << endl;
	cout << "5/6: change sunlight Z direction" << endl;
	cout << endl;

	cout << "B: toggle shadow map display" << endl;
	cout << endl;

	cout << "W/A/S/D: Camera movement" << endl;
	cout << "Q/E: Raise/lower camera" << endl;
	cout << "Z/X: Decrease/increase camera speed" << endl;
	cout << endl;

	cout << "O/I: raise/lower # of octaves in terrain" << endl;
	cout << "H/G: raise/lower water level" << endl;
	cout << "M/N: raise/lower distribution power" << endl;
}

//----------------------------------------------------------------------------------------
// Reset camera position
void A5::resetCamera() {
	cameraPos 		= CAMERA_POS;
	cameraFront 	= CAMERA_FRONT;
	cameraRight 	= glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront);
	cameraUp 		= glm::vec3( 0.0f, 1.0f, 0.0f );
	firstMouseMove 	= false;
	pitch 			= CAMERA_PITCH;
	yaw 			= CAMERA_YAW;
	cameraSpeed 	= CAMERA_SPEED;
}

//----------------------------------------------------------------------------------------
// clears away all trees and grass
void A5::resetFoliage() {
	for( int x = 0; x < TERRAIN_LENGTH; x += 1 ) {
		for( int z = 0; z < TERRAIN_WIDTH; z += 1 ) {
			treeMap[x][z] = false;
			grassMap[x][z] = false;
		} // for
	} // for

	for( LTree* tree : theTrees ) {
		delete tree;
	} // for
	theTrees.clear();
	for( glm::vec3* grass : theGrass ) {
		delete grass;
	} // for
	theGrass.clear();
}

//----------------------------------------------------------------------------------------
// Reset lights to defaults
void A5::resetLight() {
	m_theSunDir 		 = SUN_DIRECTION;
	m_theSunColour 		 = SUN_COLOUR;
	m_theSunIntensity 	 = SUN_INTENSITY;

	m_globalAmbientLight = AMBIENT_COLOUR;
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

	glGenerateMipmap(GL_TEXTURE_2D); // this fails on VMware if 3D acceleration is on
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// colours seem more saturated (better) WITHOUT the following line
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
void A5::loadSkybox( string skyboxName, GLuint* texture ) {

	// skybox is a cubemap, which has 6 faces
	const unsigned int numFaces = 6;

	// load skybox texture - CAUTION: faces need to load in this order
	const char* skyboxFaces[numFaces] =
	{   "ft",
		"bk",
		"up",
		"dn",
		"rt",
		"lf"
	};
	// generate texture
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *texture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int width, height;
	unsigned char* image;

	for ( GLuint i = 0; i < numFaces; i += 1 ) {

		string filename = "res/skybox/"
						  + skyboxName + "/"
						  + skyboxName + "_"
						  + skyboxFaces[i] + ".jpg";

		// load texture from image with SOIL
		image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		// pass texture to OpenGL
		glTexImage2D( 	GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
					);

	} // for

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free image and unbind texture
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void A5::buildShader( ShaderProgram& shader,
				const char* vertexShaderName,
				const char* fragmentShaderName ) {
	shader.generateProgramObject();
	shader.attachVertexShader(
		getAssetFilePath( vertexShaderName ).c_str() );
	shader.attachFragmentShader(
		getAssetFilePath( fragmentShaderName ).c_str() );
	shader.link();
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor( 0.1, 0.1, 0.1, 1.0 );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up vertex shader
	 */

	buildShader( m_shader,
				"VertexShader.vs",
				"FragmentShader.fs" );

	// Set up vertex uniforms
	P_uni 					= m_shader.getUniformLocation( "P" );
	V_uni 					= m_shader.getUniformLocation( "V" );
	M_uni 					= m_shader.getUniformLocation( "M" );
	theSunColour_uni		= m_shader.getUniformLocation( "theSunColour" );
	theSunDir_uni 			= m_shader.getUniformLocation( "theSunDir" );
	theSunIntensity_uni 	= m_shader.getUniformLocation( "theSunIntensity" );
	globalAmbientLight_uni 	= m_shader.getUniformLocation( "globalAmbientLight" );
	eye_uni 				= m_shader.getUniformLocation( "eye" );
	P_lightspace_uni 		= m_shader.getUniformLocation( "lightProj" );
	V_lightspace_uni 		= m_shader.getUniformLocation( "lightView" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up water shader
	 */

	// Build the water shader
	buildShader( m_water_shader,
				"waterVertexShader.vs",
				"waterFragmentShader.fs" );

	// Set up water uniforms
	P_water_uni 					= m_water_shader.getUniformLocation( "P" );
	V_water_uni 					= m_water_shader.getUniformLocation( "V" );
	M_water_uni 					= m_water_shader.getUniformLocation( "M" );
	theSunColour_water_uni		 	= m_water_shader.getUniformLocation( "theSunColour" );
	theSunDir_water_uni 		 	= m_water_shader.getUniformLocation( "theSunDir" );
	theSunIntensity_water_uni 	 	= m_water_shader.getUniformLocation( "theSunIntensity" );
	globalAmbientLight_water_uni 	= m_water_shader.getUniformLocation( "globalAmbientLight" );
	eye_water_uni 				 	= m_water_shader.getUniformLocation( "eye" );
	P_water_lightspace_uni 			= m_water_shader.getUniformLocation( "lightProj" );
	V_water_lightspace_uni 			= m_water_shader.getUniformLocation( "lightView" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up skybox shader
	 */

	// Build the skybox shader
	buildShader( m_skybox_shader,
				"skyboxVertexShader.vs",
				"skyboxFragmentShader.fs" );

	// Set up skybox uniforms
	P_skybox_uni = m_skybox_shader.getUniformLocation( "P" );
	V_skybox_uni = m_skybox_shader.getUniformLocation( "V" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up billboard shader
	 */

	// Build the billboard shader
	buildShader( m_billboard_shader,
				"billboardVertexShader.vs",
				"billboardFragmentShader.fs" );

	// Set up billboard uniforms
	billboard_position_uni 	= m_billboard_shader.getUniformLocation( "billboard_position" );
	billboard_colour_uni 	= m_billboard_shader.getUniformLocation( "billboard_colour" );
	cameraUp_uni 			= m_billboard_shader.getUniformLocation( "cameraUp" );
	cameraRight_uni 		= m_billboard_shader.getUniformLocation( "cameraRight" );
	P_billboard_uni 		= m_billboard_shader.getUniformLocation( "P" );
	V_billboard_uni 		= m_billboard_shader.getUniformLocation( "V" );
	M_billboard_uni 		= m_billboard_shader.getUniformLocation( "M" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up shadowmap shader
	 */

	// Build the shadowmap shader
	buildShader( m_shadow_shader,
				"shadowVertexShader.vs",
				"shadowFragmentShader.fs" );

	// Set up vertex uniforms
	P_shadow_uni = m_shadow_shader.getUniformLocation( "P" );
	V_shadow_uni = m_shadow_shader.getUniformLocation( "V" );
	M_shadow_uni = m_shadow_shader.getUniformLocation( "M" );

	//----------------------------------------------------------------------------------------
	/*
	 * Set up debugquad shader
	 */

	// Build the debugquad shader
	buildShader( m_debugquad_shader,
				"debugquadVertexShader.vs",
				"debugquadFragmentShader.fs" );

	//----------------------------------------------------------------------------------------
	/*
	 * Load textures from images into OpenGL
	 */

	// load model textures
	loadTexture(GROUND_TEXTURE.c_str(), &m_ground_texture);
	loadTexture("res/stone.png", &m_cliff_texture);
	loadTexture("res/water.png", &m_water_texture);
	loadTexture("res/bark.png", &m_tree_texture);
	loadTextureAlpha("res/sgrass5-1.png", &m_grass_texture);
	loadTextureAlpha("res/leaf.png", &m_leaf_texture);
	loadTextureAlpha("res/grass-screendoor.png", &m_grass_screendoor_texture);
	loadTextureAlpha("res/leaf-screendoor.png", &m_leaf_screendoor_texture);

	loadSkybox( SKYBOX_NAME, &m_skybox_texture );

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

	theTerrain.init( m_shader, m_ground_texture, m_cliff_texture );
	theWater.init( m_water_shader, m_water_texture, WATER_HEIGHT );

	resetFoliage();
	initFoliage();

	grass.init( m_billboard_shader, m_grass_texture, m_grass_screendoor_texture );
	leaf.init( m_billboard_shader, m_leaf_texture, m_leaf_screendoor_texture );

	theSkybox.init( m_skybox_shader, m_skybox_texture );
}

void A5::initShadowMap( GLuint* texture, GLuint* fbo ) {
	// GLuint shadowMap_frameBuffer = 0;
	glGenFramebuffers(1, fbo);

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		m_framebufferWidth, m_framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		cout << "error rendering shadow map to framebuffer" << endl;
		return;
	} // if
}

void A5::initFoliage() {
	const unsigned int numTreeLSystems = treeLSystems.size();

	string axiom = "S";

	unsigned int numTrees = 0;
	double** heightMap = theTerrain.getHeightMap();
	glm::vec3** normalMap = theTerrain.getNormalMap();
	srand(1995); // good year
	for (int x = 1; x < TERRAIN_LENGTH-1; x += 1) {
		for (int z = 1; z < TERRAIN_WIDTH-1; z += 1) {
			// randomly pick spots to plant trees
			// in the future, could use a random perturbation scatter to pick spots
			int randomTree = rand() % TREE_DENSITY;
			bool plantTree = ( randomTree < 3 || randomTree > TREE_DENSITY - 2 ? true : false );
			int randomGrass = rand() % GRASS_DENSITY;
			bool plantGrass = ( randomGrass <= 300 ? true : false );

			if( plantTree ) {
				// decide if chosen position is suitable for tree
				// not underwater, slope not too steep, not "too close" to another tree

				// make sure above water
				if (heightMap[x][z] <= WATER_HEIGHT + 1.0f) continue;

				// make sure ground isn't too steep
				glm::vec3 normal = normalMap[x][z];
				float angleOfGround = acos(dot(normal, vec3(0.0f, 1.0f, 0.0f)));
				angleOfGround -= 0.225f;
				if ( angleOfGround > 0.25f ) continue;
				
				// make sure tree has breathing room (ha!)
				if (treeMap[x-1][z] ||
					treeMap[x][z-1] ||
					treeMap[x-1][z-1] ) {
					// cout << "skipping tree at " << x << ", " << z << endl;
					continue;
				}

				treeMap[x][z] = true;

				// generate seed for this tree (get it? seed?!)
				int treetype = randomTree % numTreeLSystems;
				Rules* treeRules = treeLSystems.at(treetype);
				string seed = LSystem::generateExpr(axiom, *treeRules, 7);		

				glm::vec3 position = glm::vec3((float)x, heightMap[x][z]-0.25f, (float)z);
				LTree* tree = new LTree();

				// TODO: encapsulate randomness of angles, etc. with treetype prods in treetype object
				tree->init(
					glm::vec3(1.0f, 0.0f, 0.0f),			// heading vector
					glm::vec3(0.0f, -1.0f, 0.0f),			// down vector (direction of gravity)
					position,								// tree position
					seed,									// L-system expression
					0.75f,									// contraction ratio
					18.0f,									// divergence angle
					10.0f,									// range of divergence
					0.9f,									// length (width?) decrease ratio,
					m_shader, m_tree_texture
				);
				theTrees.push_back(tree);

				// vary leaf colour per tree
				glm::vec4 leafColour = LEAF_COLOUR;
				leafColour.x = (float)(rand() % x) / (float)TERRAIN_LENGTH;
				leafColourMap[x][z] = leafColour;
				// cout << leafColourMap[x][z] << endl;

			} else if( plantGrass ) {
				if ( x == 1 || z == 1 || x == TERRAIN_LENGTH - 1 || z == TERRAIN_WIDTH - 1)
					continue;
				// decide if chosen position is suitable for grass
				// not *in* tree, not underwater, slope not too steep

				// make sure above underwater
				if (heightMap[x][z] <= WATER_HEIGHT + 1.0f) continue;

				// make sure ground isn't too steep
				glm::vec3 normal = normalMap[x][z];
				float angleOfGround = acos(dot(normal, vec3(0.0f, 1.0f, 0.0f)));
				angleOfGround -= 0.225f;
				if ( angleOfGround > 0.25f ) continue;

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
	// calculate view matrix
	if ( forwardPress )	 moveCameraForward();
	if ( backwardPress ) moveCameraBackward();
	if ( leftPress )	 moveCameraLeft();
	if ( rightPress )	 moveCameraRight();
	if ( upPress )		 moveCameraUp();
	if ( downPress )	 moveCameraDown();

	// cout << cameraPos + cameraFront << endl;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	// no gui.
}

//----------------------------------------------------------------------------------------
/*
 * Rendering routines
 */

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

	theTerrain.draw();
	// theWater.draw();
	for( LTree* tree : theTrees ) {
		tree->draw();
	} // for
	/*for( glm::vec3* grassPosition : theGrass ) {
		glUniform3fv( billboard_position_uni, 1, value_ptr( *grassPosition ) );
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

void A5::drawObjects( glm::mat4* W, glm::mat4* lightProj, glm::mat4* lightView, GLuint* shadowmapTexture ) {
	m_shader.enable();

		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, m_cliff_texture);
		glUniform1i(m_shader.getUniformLocation("cliffTexture"), 1);

		glActiveTexture(GL_TEXTURE0+2);
		glBindTexture(GL_TEXTURE_2D, *shadowmapTexture);
		glUniform1i(m_shader.getUniformLocation("shadowMap"), 2);

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

		// draw terrain
		theTerrain.draw();

		// draw trees
		for( LTree* tree : theTrees ) {
			tree->draw();
		} // for

	m_shader.disable();
}

void A5::drawWater( glm::mat4* W, glm::mat4* lightProj, glm::mat4* lightView, GLuint* shadowmapTexture ) {
	m_water_shader.enable();

		// bind shadowmap texture
		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, *shadowmapTexture);
		glUniform1i(m_water_shader.getUniformLocation("shadowMap"), 1);

		// bind skybox cubemap texture (for reflections)
		glActiveTexture(GL_TEXTURE0+2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture);
		glUniform1i(m_water_shader.getUniformLocation("skybox"), 2);

		// set matrix uniforms
		glUniformMatrix4fv( P_water_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_water_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_water_uni, 1, GL_FALSE, value_ptr( *W ) );

		// matrices needed for shadowmap calculations
		glUniformMatrix4fv( P_water_lightspace_uni, 1, GL_FALSE, value_ptr( *lightProj ) );
		glUniformMatrix4fv( V_water_lightspace_uni, 1, GL_FALSE, value_ptr( *lightView ) );

		// set uniforms for the Sun
		glUniform3fv( theSunColour_water_uni, 1, value_ptr( m_theSunColour ) );
		glUniform3fv( theSunDir_water_uni, 1, value_ptr( m_theSunDir ) );
		glUniform1f( theSunIntensity_water_uni, m_theSunIntensity );

		// ambient light uniform
		glUniform3fv( globalAmbientLight_water_uni, 1, value_ptr( m_globalAmbientLight ) );

		glUniform3fv( eye_water_uni, 1, value_ptr( cameraPos ) );

		theWater.draw();

		#if 0
		glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);
			// draw the water
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilMask(0xFF);
			glDepthMask(GL_FALSE);
			glClear(GL_STENCIL_BUFFER_BIT);
			theWater.draw();
			// draw reflections
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDepthMask(GL_TRUE);

			glm::mat4 reflectModel = glm::scale(
							glm::translate(*W, glm::vec3(0.0f, -1.0f, 0.0f)),
							glm::vec3(1.0f, 1.0f, 1.0f)
			);
			glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( reflectModel ) );
			// theTerrain.draw();
			for( LTree* tree : theTrees ) {
				tree->draw();
			} // for

		glDisable(GL_STENCIL_TEST);
		#endif

	m_water_shader.disable();
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

		// draw the grass
		for( glm::vec3* grassPosition : theGrass ) {
			// glm::vec4 grassColour = grassColourMap[(int)grassPosition->x][(int)grassPosition->z];
			glm::vec4 grassColour = GRASS_COLOUR;
			glUniform4fv( billboard_colour_uni, 1, value_ptr( grassColour ) );

			glUniform3fv( billboard_position_uni, 1, value_ptr( *grassPosition ) );

			grass.draw();
		} // for

		#if 1
		// draw every tree's leaves
		for( LTree* tree : theTrees ) {
			vector<glm::vec3*> leaves = tree->getLeafPositions();
			glm::vec3 origin = tree->getOrigin();
			glm::vec4 leafColour = leafColourMap[(int)origin.x][(int)origin.z];
			glUniform4fv( billboard_colour_uni, 1, value_ptr( leafColour ) );

			for( glm::vec3* leafPosition : leaves ) {
				glUniform3fv( billboard_position_uni, 1, value_ptr( *leafPosition ) );
				leaf.draw();
			} // for
		} // for
		#endif

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
	float orthoSize = (float)TERRAIN_WIDTH / 1.5f;
	float nearPlane = -40.0f;
	float farPlane = (float)TERRAIN_WIDTH + 15.0f;
	float centerPoint = (float)TERRAIN_WIDTH / 20.0f;
	glm::mat4 lightProj = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(
									  m_theSunDir,
									  glm::vec3(20.0f, 15.0f, 20.0f),
									  glm::vec3(0.0f, 1.0f, 0.0f)
									 );

	glEnable( GL_DEPTH_TEST );

	m_shadow_shader.enable();

		drawShadowMap( &W, &lightProj, &lightView );

	m_shadow_shader.disable();

	// glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;							// DEBUG
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( drawShadowDebugQuad ) {
		// display the shadow map
		m_debugquad_shader.enable();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_shadow_texture);
		RenderQuad();
		m_debugquad_shader.disable();
	} else {
		// draw scene normally
		drawSkybox();
		drawObjects( &W, &lightProj, &lightView, &m_shadow_texture );
		drawWater( &W, &lightProj, &lightView, &m_shadow_texture );
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
			initEnvironment();
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
		if (key == GLFW_KEY_1) {
			m_theSunDir.x += 1.0f;
			cout << "m_theSunDir.x: " << m_theSunDir.x << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_2) {
			m_theSunDir.x -= 1.0f;
			cout << "m_theSunDir.x: " << m_theSunDir.x << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_3) {
			m_theSunDir.y += 1.0f;
			cout << "m_theSunDir.y: " << m_theSunDir.y << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_4) {
			m_theSunDir.y -= 1.0f;
			cout << "m_theSunDir.y: " << m_theSunDir.y << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_5) {
			m_theSunDir.z += 1.0f;
			cout << "m_theSunDir.z: " << m_theSunDir.z << endl;
			initEnvironment();
			eventHandled = true;
		}
		if (key == GLFW_KEY_6) {
			m_theSunDir.z -= 1.0f;
			cout << "m_theSunDir.z: " << m_theSunDir.z << endl;
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
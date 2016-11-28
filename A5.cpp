#include "A5.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

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

// terrain needs to be square or terrain map vertices get all hecked up
static const size_t TERRAIN_WIDTH = 200;
static const size_t TERRAIN_LENGTH = TERRAIN_WIDTH;
static size_t WATER_HEIGHT = 17;
static const unsigned int NUM_OCTAVES = 7; // # of octaves for terrain generation
static const double REDIST = 1.05f;

//----------------------------------------------------------------------------------------
// Constructor
A5::A5()
	: current_col( 0 ),
	theTerrain(TERRAIN_WIDTH, TERRAIN_LENGTH, NUM_OCTAVES, REDIST),
	theWater(TERRAIN_WIDTH, TERRAIN_LENGTH),
	theSkybox(),
	tree(2.0f, 1.0f, 5.0f, glm::vec3(25.0f, 30.0f, 25.0f), 2, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
	// tree2(1.0f, 0.5f, 5.0f, glm::vec3(25.0f, 35.0f, 25.0f), 2),
	mouseSensitivity(0.05f),
	forwardPress(false),
	backwardPress(false),
	leftPress(false),
	rightPress(false),
	upPress(false),
	downPress(false)
{
	reset();
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{}

//----------------------------------------------------------------------------------------
// Load object texture
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

	#if 1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#endif

	// colours seem more saturated WITHOUT this
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
// Reset things
void A5::reset() {
	resetCamera();
	resetLight();
	cout << "controls:" << endl;
	
	cout << "U/Y: raise/lower x position of the Sun" << endl;
	cout << "O/I: raise/lower # of octaves in terrain" << endl;
	cout << "H/G: raise/lower water level" << endl;
	cout << "K/J: raise/lower sun intensity" << endl;
	cout << "L: toggle noise function implementation (Simplex vs. my Perlin)" << endl;
	cout << "M/N: raise/lower distribution power" << endl;
}

//----------------------------------------------------------------------------------------
// Reset camera position
void A5::resetCamera() {
	cameraPos 		= glm::vec3( 0.0f, 0.0f, 0.0f );
	cameraFront 	= glm::vec3( 1.0f, 0.0f, 0.0f );
	cameraUp 		= glm::vec3( 0.0f, 1.0f, 0.0f );
	firstMouseMove = false;
	pitch = 0.0f;
	yaw = 0.0f;
	cameraSpeed = 0.3f;
	#if 0
	// position camera to have a view of 100x100 grid by default
	cameraPos 		= glm::vec3( -54.0f, 20.0f, -64.0f );
	cameraFront 	= glm::vec3( 0.589f, -0.262f, 0.764f );
	pitch = -15.77f;
	yaw = 51.72f;
	#endif
	#if 1
	// position camera to have a view of 200x200 grid by default
	cameraPos 		= glm::vec3( -121.0f, 57.0f, -74.0f );
	cameraFront 	= glm::vec3( 0.800f, -0.157f, 0.579f );
	pitch = -27.0f;
	yaw = 10.1f;
	cameraSpeed = 1.5f;
	#endif
	#if 0
	// position camera to have a view of 512x512 grid by default
	cameraPos 		= glm::vec3( -286.0f, 74.0f, -284.0f );
	cameraFront 	= glm::vec3( 0.589f, -0.262f, 0.764f );
	pitch = -16.5f;
	yaw = 52.42f;
	#endif
}

//----------------------------------------------------------------------------------------
// Reset lights
void A5::resetLight() {
	m_theSunColour = glm::vec3(1.0f, 1.0f, 0.0f);
	m_theSunDir = glm::vec3(0.5f, -0.5f, 0.0f);
	m_theSunIntensity = 0.8f;
	m_globalAmbientLight = glm::vec3(0.3f, 0.5f, 0.9f);
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor( 0.1, 0.1, 0.1, 1.0 );

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
	// M_skybox_uni = m_skybox_shader.getUniformLocation( "M" );

	// load textures
	loadTexture("res/grass.png", &m_ground_texture);
	loadTexture("res/water.png", &m_water_texture);
	loadTexture("res/tree.png", &m_tree_texture);
	const std::vector<std::string> skyboxTextureFiles {
		// "res/grass.png",
		"res/skybox/rt.png",
		"res/skybox/lf.png",
		"res/skybox/up.png",
		"res/skybox/dn.png",
		"res/skybox/bk.png",
		"res/skybox/ft.png"
	};
	loadSkybox( skyboxTextureFiles, &m_skybox_texture );

	// initialize environment
	initEnvironment();

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );

	// grab and hide the cursor for first-person mode
	glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
}

void A5::initEnvironment() {
	theTerrain.init( m_shader, m_ground_texture );
	theWater.init( m_shader, m_water_texture, WATER_HEIGHT );

	// TODO: program crashes when you call tree.init multiple times
	tree.init(m_shader, m_tree_texture );
	// tree2.init(m_shader, m_tree_texture );

	theSkybox.init(m_skybox_shader, m_skybox_texture );
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
	// view = glm::lookAt( cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 1.0f, 0.0f ) );
	// cout << cameraPos + cameraFront << endl;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// cout << glfwGetInputMode( m_window, GLFW_CURSOR ) << endl;
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

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(TERRAIN_WIDTH)/2.0f, 0, -float(TERRAIN_WIDTH)/2.0f ) );

	glEnable( GL_DEPTH_TEST );

	// glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;							// DEBUG
	m_skybox_shader.enable();
		// set skybox matrix uniforms
		glm::mat4 translateOnlyView = glm::mat4(glm::mat3(view));
		glUniformMatrix4fv( P_skybox_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_skybox_uni, 1, GL_FALSE, value_ptr( translateOnlyView ) );
		// glUniformMatrix4fv( M_skybox_uni, 1, GL_FALSE, value_ptr( W ) );

		theSkybox.draw(); 			// skybox first

	m_skybox_shader.disable();
	
	m_shader.enable();


		// set matrix uniforms
		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

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
		tree.draw();
		// tree2.draw();

	m_shader.disable();

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
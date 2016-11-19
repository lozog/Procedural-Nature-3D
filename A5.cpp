#include "A5.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 10;

//----------------------------------------------------------------------------------------
// Constructor
A5::A5()
	: current_col( 0 ),
	theTerrain(10, 10, DIM),
	cameraSpeed(0.05f),
	mouseSensitivity(0.05f),
	forwardPress(false),
	backwardPress(false),
	leftPress(false),
	rightPress(false),
	firstMouseMove(false)
{
	// cameraPos 		= glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 );
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
// Reset things
void A5::reset() {
	resetCamera();
}

//----------------------------------------------------------------------------------------
// Reset camera position
void A5::resetCamera() {
	cameraPos 		= glm::vec3( 0.0f, 1.0f, 0.0f );
	cameraFront 	= glm::vec3( 0.0f, 0.0f, -1.0f );
	cameraUp 		= glm::vec3( 0.0f, 1.0f, 0.0f );
	pitch = 0.0f;
	yaw = 0.0f;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	theTerrain.init( m_shader );

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	// cout << float(DIM)*2.0*M_SQRT1_2 << " " << float(DIM)*2.0*M_SQRT1_2 << endl;

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );

	// grab and hide the cursor
	glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	// glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
}

//----------------------------------------------------------------------------------------
/*
 * Camera control functions
 */

// TODO: implement first-person camera controls
// see http://learnopengl.com/#!Getting-started/Camera
/*
 GLfloat cameraSpeed = 0.05f;
     if(key == GLFW_KEY_W)
         cameraPos += cameraSpeed * cameraFront;
     if(key == GLFW_KEY_S)
         cameraPos -= cameraSpeed * cameraFront;
     if(key == GLFW_KEY_A)
         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
     if(key == GLFW_KEY_D)
         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
*/

#define DEBUG_CAMERAMOVEMENT 0
void A5::moveCameraForward() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "forward" << endl;
	#endif
	// eye += glm::vec3(0.0f, 0.0f, -1.0f);
	// view = glm::lookAt( eye, center, up );
	// view = glm::translate(view, glm::vec3(0.0f, 0.0f, 1.0f));
	cameraPos += cameraSpeed * cameraFront;
}

void A5::moveCameraBackward() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "backward" << endl;
	#endif
	// view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
	cameraPos -= cameraSpeed * cameraFront;
}

void A5::moveCameraLeft() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "left" << endl;
	#endif
	// view = glm::translate(view, glm::vec3(-1.0f, 0.0f, 0.0f));
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void A5::moveCameraRight() {
	#if DEBUG_CAMERAMOVEMENT
	cout << "right" << endl;
	#endif
	// view = glm::translate(view, glm::vec3(1.0f, 0.0f, 0.0f));
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;							// DEBUG
	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the terrain for now.
		theTerrain.draw( col_uni );

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
			xPosPrev = xPos;
			yPosPrev = yPos;
			firstMouseMove = true;
		}
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
		if (key == GLFW_KEY_Q) {
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
			// moveCameraForward();
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_S) {
			// toggle backward movement
			backwardPress = true;
			// moveCameraBackward();
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_A) {
			// toggle left movement
			leftPress = true;
			// moveCameraLeft();
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_D) {
			// toggle right movement
			rightPress = true;
			// moveCameraRight();
			eventHandled = true;
		} // if
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
	} // if

	return eventHandled;
}

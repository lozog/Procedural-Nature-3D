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
	cameraSpeed(0.05f)
{
	// cameraPos 		= glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 );
	cameraPos 		= glm::vec3( 0.0f, 0.0f, 0.0f );
	cameraFront 	= glm::vec3( 0.0f, 0.0f, -1.0f );
	cameraUp 		= glm::vec3( 0.0f, 1.0f, 0.0f );
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{}

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
void A5::moveCameraForward() {
	cout << "forward" << endl;
	// eye += glm::vec3(0.0f, 0.0f, -1.0f);
	// view = glm::lookAt( eye, center, up );
	// view = glm::translate(view, glm::vec3(0.0f, 0.0f, 1.0f));
	cameraPos += cameraSpeed * cameraFront;
}

void A5::moveCameraBackward() {
	cout << "backward" << endl;
	// view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
	cameraPos -= cameraSpeed * cameraFront;
}

void A5::moveCameraLeft() {
	cout << "left" << endl;
	// view = glm::translate(view, glm::vec3(-1.0f, 0.0f, 0.0f));
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void A5::moveCameraRight() {
	cout << "right" << endl;
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
	// view = glm::lookAt( cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 1.0f, 0.0f ) );
	// cout << cameraPos + cameraFront << endl;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A5 running simultaneously, this would break; you'd want to make
	// this into instance fields of A5.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

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
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
	}

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

		// Camera controls
		if (key == GLFW_KEY_W) {
			// move forward
			moveCameraForward();
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_S) {
			// move forward
			moveCameraBackward();
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_A) {
			// move forward
			moveCameraLeft();
			eventHandled = true;
		} // if
		if (key == GLFW_KEY_D) {
			// move forward
			moveCameraRight();
			eventHandled = true;
		} // if
	} // if

	if ( action == GLFW_RELEASE ) {
		if ( key == GLFW_KEY_W ) {
			// do stuff
		} // if
	} // if

	return eventHandled;
}

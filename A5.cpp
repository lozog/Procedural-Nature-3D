#include "A5.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <cmath>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM 	  	= 16;
static const size_t BUFFER_FACE = 3 * 3 * 2; // 3D coords for 3 vertices for 2 triangles
static const size_t ZOOM_MAX 	= 35;
static const size_t ZOOM_MIN 	= 10;

//----------------------------------------------------------------------------------------
// Constructor

/*
default colours from http://colrd.com/palette/23651/
rgb(170,187,136)
rgb(51,26,0)
rgb(219,14,0)
rgb(255,231,160)
rgb(68,186,151)
rgb(255,153,119)
rgb(110,119,0)
rgb(8,38,47)
*/
A5::A5()
	: current_col( 0 ),
	  theGrid(DIM)
{
	A5::reset();
}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{}

//----------------------------------------------------------------------------------------
// helper functions
void A5::reset() {
	// default colours
	colour[0][0] = 0.667f;
	colour[0][1] = 0.733f;
	colour[0][2] = 0.533f;
	colour[1][0] = 0.200f;
	colour[1][1] = 0.102f;
	colour[1][2] = 0.000f;
	colour[2][0] = 0.859f;
	colour[2][1] = 0.055f;
	colour[2][2] = 0.000f;
	colour[3][0] = 1.000f;
	colour[3][1] = 0.906f;
	colour[3][2] = 0.627f;
	colour[4][0] = 0.267f;
	colour[4][1] = 0.729f;
	colour[4][2] = 0.592f;
	colour[5][0] = 1.000f;
	colour[5][1] = 0.600f;
	colour[5][2] = 0.467f;
	colour[6][0] = 0.431f;
	colour[6][1] = 0.467f;
	colour[6][2] = 0.000f;
	colour[7][0] = 0.031f;
	colour[7][1] = 0.149f;
	colour[7][2] = 0.184f;

	// active cell
	act_x = 0;
	act_y = 0;
	act_z = 0;

	rotate_x = 0.0f;
	zoom = DIM;

	theGrid.reset();
}


//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

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

	initGrid();
	initBars();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A5::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);
	
	float *verts = new float[ sz ];
	size_t ct = 0;
	// define the grid - GL_LINES
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the grid vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A5::initBars() {
	for (int x = 0; x < DIM; ++x) {
		for (int z = 0; z < DIM; ++z) {
			// init each buffer
			GLuint* bar_vbo = theGrid.getBarVBO(x, z);
			glGenBuffers( 1, bar_vbo );

		}
	}
}

void A5::writeBarsToBuffer() {

	for (int x = 0; x < DIM; ++x) {
		for (int z = 0; z < DIM; ++z) {
			int height = theGrid.getHeight(x, z);
			// cout << "init " << height << " cubes at " << act_x << "," << act_z << endl; // DEBUG print

			if (height == 0) continue;

			// make space for cubes:
			// BUFFER_FACE * 6 faces
			// per cube for a bar of (height) cubes
			size_t sz = BUFFER_FACE * 6 * (height);

			float *verts = new float[ sz ];
			size_t ct = 0;

			// define a cube - GL_TRIANGLES
			for( int y = 0; y < height; ++y ) {
				verts[ ct ]   = x;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z;
				verts[ ct+3 ] = x;
				verts[ ct+4 ] = y+1;
				verts[ ct+5 ] = z;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y;
				verts[ ct+8 ] = z;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y+1;
				verts[ ct+2 ] = z;
				verts[ ct+3 ] = x+1;
				verts[ ct+4 ] = y;
				verts[ ct+5 ] = z;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z;
				ct += 9;

				verts[ ct ]   = x+1;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z;
				verts[ ct+3 ] = x+1;
				verts[ ct+4 ] = y;
				verts[ ct+5 ] = z+1;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z;
				ct += 9;

				verts[ ct ]   = x+1;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z+1;
				verts[ ct+3 ] = x+1;
				verts[ ct+4 ] = y+1;
				verts[ ct+5 ] = z;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z+1;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y+1;
				verts[ ct+2 ] = z+1;
				verts[ ct+3 ] = x+1;
				verts[ ct+4 ] = y;
				verts[ ct+5 ] = z+1;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z+1;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z+1;
				verts[ ct+3 ] = x;
				verts[ ct+4 ] = y+1;
				verts[ ct+5 ] = z+1;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y;
				verts[ ct+8 ] = z+1;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z;
				verts[ ct+3 ] = x;
				verts[ ct+4 ] = y;
				verts[ ct+5 ] = z+1;
				verts[ ct+6 ] = x;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z+1;
				verts[ ct+3 ] = x;
				verts[ ct+4 ] = y+1;
				verts[ ct+5 ] = z;
				verts[ ct+6 ] = x;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z+1;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y+1;
				verts[ ct+2 ] = z;
				verts[ ct+3 ] = x;
				verts[ ct+4 ] = y+1;
				verts[ ct+5 ] = z+1;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y+1;
				verts[ ct+2 ] = z+1;
				verts[ ct+3 ] = x+1;
				verts[ ct+4 ] = y+1;
				verts[ ct+5 ] = z;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y+1;
				verts[ ct+8 ] = z+1;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z;
				verts[ ct+3 ] = x;
				verts[ ct+4 ] = y;
				verts[ ct+5 ] = z+1;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y;
				verts[ ct+8 ] = z;
				ct += 9;

				verts[ ct ]   = x;
				verts[ ct+1 ] = y;
				verts[ ct+2 ] = z+1;
				verts[ ct+3 ] = x+1;
				verts[ ct+4 ] = y;
				verts[ ct+5 ] = z;
				verts[ ct+6 ] = x+1;
				verts[ ct+7 ] = y;
				verts[ ct+8 ] = z+1;
				ct += 9;

			}

			// Create the bar vertex array to record buffer assignments.
			GLuint* bar_vao = theGrid.getBarVAO(x, z);
			glGenVertexArrays( 1, bar_vao );
			glBindVertexArray( *bar_vao );

			// bind to the bar vertex buffer
			GLuint* bar_vbo = theGrid.getBarVBO(x, z);
			glBindBuffer( GL_ARRAY_BUFFER, *bar_vbo ); // must rebind anytime contents change
			glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
				verts, GL_STATIC_DRAW );

			// Specify the means of extracting the position values properly.
			GLint posAttrib = m_shader.getAttribLocation( "position" );
			glEnableVertexAttribArray( posAttrib );
			glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

			// Reset state to prevent rogue code from messing with *my*
			// stuff!
			glBindVertexArray( 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			// OpenGL has the buffer now, there's no need for us to keep a copy.
			delete [] verts;

			CHECK_GL_ERRORS;
		}
	}
}

void A5::initCursor(int x, int z, int height) {

	// make space for cursor:
	// (2 triangles for the square, 1 each for each indicator)
	size_t sz = BUFFER_FACE*2;

	float *verts = new float[ sz ];
	size_t ct = 0;

	// add a visually negligable amount of height
	// so the indicator gets drawn over the bar
	// (i.e. passes the depth test)
	float y = height + .001f;

	// define a square - GL_TRIANGLES
	verts[ ct ]   = x;
	verts[ ct+1 ] = y;
	verts[ ct+2 ] = z;
	verts[ ct+3 ] = x;
	verts[ ct+4 ] = y;
	verts[ ct+5 ] = z+1;
	verts[ ct+6 ] = x+1;
	verts[ ct+7 ] = y;
	verts[ ct+8 ] = z;
	ct += 9;

	verts[ ct ]   = x;
	verts[ ct+1 ] = y;
	verts[ ct+2 ] = z+1;
	verts[ ct+3 ] = x+1;
	verts[ ct+4 ] = y;
	verts[ ct+5 ] = z;
	verts[ ct+6 ] = x+1;
	verts[ ct+7 ] = y;
	verts[ ct+8 ] = z+1;
	ct += 9;

	verts[ ct ]   = -1;
	verts[ ct+1 ] = 0;
	verts[ ct+2 ] = z;
	verts[ ct+3 ] = -0.5f;
	verts[ ct+4 ] = 0;
	verts[ ct+5 ] = z+0.5f;
	verts[ ct+6 ] = -1;
	verts[ ct+7 ] = 0;
	verts[ ct+8 ] = z+1;
	ct += 9;

	verts[ ct ]   = x;
	verts[ ct+1 ] = 0;
	verts[ ct+2 ] = DIM+1;
	verts[ ct+3 ] = x+0.5f;
	verts[ ct+4 ] = 0;
	verts[ ct+5 ] = DIM+0.5f;
	verts[ ct+6 ] = x+1;
	verts[ ct+7 ] = 0;
	verts[ ct+8 ] = DIM+1;
	ct += 9;

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_cursor_vao );
	glBindVertexArray( m_cursor_vao );

	// Create the cursor vertex buffer
	glGenBuffers( 1, &m_cursor_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_cursor_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glDeleteBuffers(1, &m_cursor_vbo);


	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
	// Place per frame, application logic here ...

	writeBarsToBuffer();

	// initialize the cursor
	initCursor(act_x, act_z, theGrid.getHeight(act_x, act_z));


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
		if( ImGui::Button( "Reset" ) ) {
			A5::reset();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		for (int i = 0; i < NUM_COLS; ++i) {
			ImGui::PushID( i );
			ImGui::ColorEdit3( "##Colour", colour[i] );
			ImGui::SameLine();
			if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
				// Select this colour.
			}
			ImGui::PopID();
		}

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

	// apply any rotation to the grid (before centering it)
	W = glm::rotate(W, glm::radians( rotate_x ), vec3(0.0f, 1.0f, 0.0f));

	// centre the grid
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	// apply zoom
	view = glm::lookAt(
		glm::vec3( 0.0f, float(zoom)*2.0*M_SQRT1_2, float(zoom)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );


	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// draw the grid
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4);

		// draw the cubes
		// int numCubes = 0; // DEBUG variable
		for (int x = 0; x < DIM; ++x) {
			for (int z = 0; z < DIM; ++z) {

				// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // DEBUG

				int height = theGrid.getHeight(x, z);
				if (height == 0) continue;
				// numCubes += height; // DEBUG

				int barColour = theGrid.getColour(x, z);

				GLuint* bar_vao = theGrid.getBarVAO(x, z);
				glBindVertexArray( *bar_vao );
				glUniform3f( col_uni, colour[barColour][0], colour[barColour][1], colour[barColour][2] ); // change colour of cube here
				glDrawArrays( GL_TRIANGLES, 0, BUFFER_FACE*6*(height) );

				// glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); // DEBUG
			}
		}
		// cout << "gonna draw " << numCubes << " cubes" << endl; // DEBUG print

		// Highlight the active square.
		glBindVertexArray( m_cursor_vao );
		glUniform3f( col_uni, 1, 0, 0 );
		glDrawArrays( GL_TRIANGLES, 0, BUFFER_FACE*2);

	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

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
		if (leftMousePress) {
			// rotate
			if (!hasInitialRotPos) {
				initialRotateXPos = xPos;
				initialRotateYPos = yPos;
				hasInitialRotPos = true;
			} else {
				float moveDistX = 0.5f * (xPos - initialRotateXPos);
				rotate_x = moveDistX;
			}
		}
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

		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
			leftMousePress = true;
			eventHandled = true;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_RELEASE) {
			leftMousePress = false;
			hasInitialRotPos = false;
			eventHandled = true;
		}
		// cout << "mousepress: " << leftMousePress << endl;
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
	// cout << "xOffSet: " << xOffSet << ", yOffSet: " << yOffSet << endl; // DEBUG
	bool zoomOut = yOffSet > 0; // if yOffSet==1, we're zooming out

	if (zoomOut) {
		if (zoom > ZOOM_MIN) {
			zoom -= 1.0f;
		}
	} else {
		if (zoom < ZOOM_MAX) {
			zoom += 1.0f;
		}
	}
	// cout << "zoom: " << zoom << endl;

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

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.

		if (key == GLFW_KEY_Q) {
			// quit application
			glfwSetWindowShouldClose(m_window, GL_TRUE);

			eventHandled = true;
		}

		if (key == GLFW_KEY_R) {
			// reset to defaults
			A5::reset();

			eventHandled = true;
		}

		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			shiftPressed = true;
			eventHandled = true;
		}

		if (key == GLFW_KEY_UP) {
			if (act_z > 0) {
				if (shiftPressed) {
					theGrid.copyBar(act_x, act_z, act_x, act_z-1);
				}
				act_z -= 1;
				if (theGrid.getHeight(act_x, act_z) > 0) {
					current_col = theGrid.getColour(act_x, act_z);
				}
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_LEFT) {
			if (act_x > 0) {
				if (shiftPressed) {
					theGrid.copyBar(act_x, act_z, act_x-1, act_z);
				}
				act_x -= 1;
				if (theGrid.getHeight(act_x, act_z) > 0) {
					current_col = theGrid.getColour(act_x, act_z);
				}
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_DOWN) {
			if (act_z < DIM-1) {
				if (shiftPressed) {
					theGrid.copyBar(act_x, act_z, act_x, act_z+1);
				}
				act_z += 1;
				if (theGrid.getHeight(act_x, act_z) > 0) {
					current_col = theGrid.getColour(act_x, act_z);
				}
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_RIGHT) {
			if (act_x < DIM-1) {
				if (shiftPressed) {
					theGrid.copyBar(act_x, act_z, act_x+1, act_z);
				}
				act_x += 1;
				if (theGrid.getHeight(act_x, act_z) > 0) {
					current_col = theGrid.getColour(act_x, act_z);
				}
			}
			eventHandled = true;
		}
		// print coord info
		// cout << "x: " << act_x << ",y: " << act_y << ",z: " << act_z << endl;

		if (key == GLFW_KEY_SPACE) {
			int height = theGrid.getHeight(act_x, act_z);
			if (height == 0) {
				theGrid.setColour(act_x, act_z, current_col);
			}
			theGrid.incHeight(act_x, act_z);
			eventHandled = true;
		}
		if (key == GLFW_KEY_BACKSPACE) {
			theGrid.decHeight(act_x, act_z);
			eventHandled = true;
		}
		// cout << "height at " << act_x << "," << act_z << ": " << theGrid.getHeight(act_x, act_z) << endl;

		if (key == GLFW_KEY_Z) {
			current_col = (current_col-1)%NUM_COLS;
			eventHandled = true;
		}
		if (key == GLFW_KEY_X) {
			current_col = (current_col+1)%NUM_COLS;
			eventHandled = true;
		}
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			shiftPressed = false;
			eventHandled = true;
		}
	}

	return eventHandled;
}

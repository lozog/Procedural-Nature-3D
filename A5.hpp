#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"

static const size_t NUM_COLS	= 8;

class A5 : public CS488Window {
public:
	A5();
	virtual ~A5();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initGrid();
	void initBars();
	void initCursor(int x, int z, int height);

	void writeBarsToBuffer();

	void reset();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to active cell indicator geometry.
	GLuint m_cursor_vao; // Vertex Array Object
	GLuint m_cursor_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;
	// glm::mat4 W; // model matrix

	// colour related fields
	float colour[NUM_COLS][3];
	int current_col;

	// zoom related fields
	float zoom;

	// rotation and related fields
	float rotate_x; // angle of rotation in degrees
	bool hasInitialRotPos = false;
	double initialRotateXPos;
	double initialRotateYPos; // we don't actually use yPos

	// Grid & related fields
	Grid theGrid;
	int act_x, act_y, act_z; // coords of current selected cell

	// input related fields
	bool shiftPressed   = false;
	bool leftMousePress = false;
};

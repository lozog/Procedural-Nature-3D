#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "terrain.hpp"

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
	// Camera controls
	void moveCameraForward();
	void moveCameraBackward();
	void moveCameraLeft();
	void moveCameraRight();

	// camera-related fields
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraRight;

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; 		// Uniform location for Projection matrix.
	GLint V_uni; 		// Uniform location for View matrix.
	GLint M_uni; 		// Uniform location for Model matrix.
	GLint col_uni;   	// Uniform location for cube colour.

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	Terrain theTerrain;

	float colour[3];
	int current_col;
};

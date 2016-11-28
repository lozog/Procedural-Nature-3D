#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "terrain.hpp"
#include "water.hpp"
#include "skybox.hpp"
#include "branch.hpp"

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
	void loadTexture( const char* filename, GLuint* texture );
	void loadSkybox( const std::vector<std::string> filenames, GLuint* texture );
	void initEnvironment();

	void reset();
	void resetCamera();
	void resetLight();

	// Camera controls
	void moveCameraForward();
	void moveCameraBackward();
	void moveCameraLeft();
	void moveCameraRight();
	void moveCameraUp();
	void moveCameraDown();
	void cameraSpeedUp();
	void cameraSpeedDown();

	// camera-related fields
	glm::vec3 cameraPos;						// position of camera
	glm::vec3 cameraFront;						// point camera is looking at
	glm::vec3 cameraUp;							// "up" relative to cameraFront
	float cameraSpeed;							// movement speed of camera
	float mouseSensitivity;						// sensitivity of mouse control of pitch/yaw

	// input-related fields
	bool forwardPress,							// keyboard movement
		 backwardPress,
		 leftPress,
		 rightPress,
		 upPress,
		 downPress;
	double xPosPrev, yPosPrev;					// mousePos of previous frame
	bool firstMouseMove;						// prevents jump on first mouse movement
	double pitch, yaw;

	// Fields related to the object vertex shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; 								// Uniform location for Projection matrix.
	GLint V_uni; 								// Uniform location for View matrix.
	GLint M_uni; 								// Uniform location for Model matrix.
	GLint theSunColour_uni;   					// Uniform location for the Sun colour
	GLint theSunDir_uni;   						// Uniform location for the Sun direction
	GLint theSunIntensity_uni;   				// Uniform location for the Sun intensity
	GLint globalAmbientLight_uni;   			// Uniform location for global ambient light
	GLint eye_uni;   							// Uniform location for eye position

	// Fields related to the skybox vertex shader and uniforms.
	ShaderProgram m_skybox_shader;
	GLint P_skybox_uni; 						// Uniform location for Projection matrix.
	GLint V_skybox_uni; 						// Uniform location for View matrix.
	GLint M_skybox_uni; 						// Uniform location for Model matrix.

	// Lighting
	glm::vec3 m_theSunColour;
	glm::vec3 m_theSunDir;
	float m_theSunIntensity;
	glm::vec3 m_globalAmbientLight;

	// Texture
	GLuint m_ground_texture;
	GLuint m_water_texture;
	GLuint m_skybox_texture;
	GLuint m_tree_texture;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	// models
	Terrain theTerrain;
	Water theWater;
	Skybox theSkybox;
	Branch tree;
	// Branch tree2;

	float colour[3];
	int current_col;
};

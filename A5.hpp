#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "terrain.hpp"
#include "water.hpp"
#include "skybox.hpp"
#include "branchnode.hpp"
#include "ltree.hpp"
#include "billboard.hpp"
#include "lsystem.hpp"

class A5 : public CS488Window {
public:
	A5( int argc, char **argv );
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
	// data loading functions
	void readInputParams( const char* paramFile );
	void loadTexture( const char* filename, GLuint* texture );
	void loadTextureAlpha( const char* filename, GLuint* texture );
	void loadSkybox( const std::string skyboxName, GLuint* texture );

	// initializes model data
	void initEnvironment();
	void initFoliage();
	void initShadowMap( GLuint* texture, GLuint* fbo  );

	// reset to default functions
	void reset();
	void resetCamera();
	void resetLight();
	void resetFoliage();

	// drawing functions
	void drawShadowMap( glm::mat4* W, glm::mat4* lightProj, glm::mat4* lightView );
	void drawSkybox();
	void drawObjects( glm::mat4* W, glm::mat4* lightProj, glm::mat4* lightView, GLuint* shadowmapTexture );
	void drawWater( glm::mat4* W, glm::mat4* lightProj, glm::mat4* lightView, GLuint* shadowmapTexture );
	void drawBillboards( glm::mat4* W );

	// Camera controls
	void moveCameraForward();
	void moveCameraBackward();
	void moveCameraLeft();
	void moveCameraRight();
	void moveCameraUp();
	void moveCameraDown();
	void cameraSpeedUp();
	void cameraSpeedDown();

	void buildShader( ShaderProgram& shader,
				const char* vertexShaderName,
				const char* fragmentShaderName );

	// camera-related fields
	glm::vec3 cameraPos;						// position of camera
	glm::vec3 cameraFront;						// point camera is looking at
	glm::vec3 cameraUp;							// "up" relative to cameraFront
	glm::vec3 cameraRight;						// "right" relative to cameraFront
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
	GLint P_lightspace_uni;
	GLint V_lightspace_uni;

	// Fields related to the water vertex shader and uniforms.
	ShaderProgram m_water_shader;
	GLint P_water_uni; 							// Uniform location for Projection matrix.
	GLint V_water_uni; 							// Uniform location for View matrix.
	GLint M_water_uni; 							// Uniform location for Model matrix.
	GLint theSunColour_water_uni;   			// Uniform location for the Sun colour
	GLint theSunDir_water_uni;   				// Uniform location for the Sun direction
	GLint theSunIntensity_water_uni;   			// Uniform location for the Sun intensity
	GLint globalAmbientLight_water_uni;   		// Uniform location for global ambient light
	GLint eye_water_uni;   						// Uniform location for eye position
	GLint P_water_lightspace_uni;
	GLint V_water_lightspace_uni;

	// Fields related to the skybox vertex shader and uniforms.
	ShaderProgram m_skybox_shader;
	GLint P_skybox_uni; 						// Uniform location for Projection matrix.
	GLint V_skybox_uni; 						// Uniform location for View matrix.

	// Fields related to the billboard vertex shader and uniforms.
	ShaderProgram m_billboard_shader;
	GLint billboard_position_uni;   			// Uniform location for grass position
	GLint P_billboard_uni; 						// Uniform location for Projection matrix.
	GLint V_billboard_uni; 						// Uniform location for View matrix.
	GLint M_billboard_uni; 						// Uniform location for Model matrix.
	GLint cameraUp_uni; 						// Uniform location for camera up vector
	GLint cameraRight_uni; 						// Uniform location for camera right vector

	// Fields related to the shadow map vertex shader and uniforms.
	ShaderProgram m_shadow_shader;
	GLint P_shadow_uni; 						// Uniform location for Projection matrix.
	GLint V_shadow_uni; 						// Uniform location for View matrix.
	GLint M_shadow_uni; 						// Uniform location for Model matrix.
	GLuint shadowMap_FBO;

	ShaderProgram m_debugquad_shader;

	// Lighting
	glm::vec3 m_theSunColour;
	glm::vec3 m_theSunDir;
	float m_theSunIntensity;
	glm::vec3 m_globalAmbientLight;

	// Textures
	GLuint m_ground_texture;
	GLuint m_cliff_texture;
	GLuint m_water_texture;
	GLuint m_skybox_texture;
	GLuint m_tree_texture;
	GLuint m_grass_texture;
	GLuint m_grass_screendoor_texture;
	GLuint m_leaf_texture;
	GLuint m_leaf_screendoor_texture;
	GLuint m_shadow_texture;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	std::vector<Rules*> treeLSystems;

	// models
	Terrain theTerrain;
	Water theWater;
	Skybox theSkybox;
	std::vector<LTree*> theTrees;
	std::vector<glm::vec3*> theGrass;
	Billboard grass;
	Billboard leaf;

	// useful maps that keep track of object placement
	bool** treeMap;
	bool** grassMap;

};

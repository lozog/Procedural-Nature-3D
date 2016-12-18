#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "lsystem.hpp"

// Default input parameters

// terrain needs to be square or terrain map vertices get all hecked up
extern size_t TERRAIN_WIDTH;
extern size_t TERRAIN_LENGTH;
extern size_t WATER_HEIGHT;
extern unsigned int NUM_OCTAVES; // # of octaves for terrain generation
extern double REDIST; // 1.05f;
extern unsigned int TREE_DENSITY; // density of trees (lower->denser)
extern unsigned int GRASS_DENSITY; // density of grass (lower->denser)
extern bool drawShadowDebugQuad;
extern std::string SKYBOX_NAME;
extern glm::vec3 SUN_DIRECTION;
extern glm::vec3 SUN_COLOUR;
extern float SUN_INTENSITY;
extern glm::vec3 AMBIENT_COLOUR;
extern glm::vec3 CAMERA_POS;
extern glm::vec3 CAMERA_FRONT;
extern float CAMERA_PITCH;
extern float CAMERA_YAW;
extern float CAMERA_SPEED;
extern glm::vec4 GRASS_COLOUR;
extern glm::vec4 LEAF_COLOUR;
extern std::string GROUND_TEXTURE;
extern std::vector<Rules*> treeLSystems;

//----------------------------------------------------------------------------------------
// Reads input parameters from file
void readInputParams( const char* paramFile );
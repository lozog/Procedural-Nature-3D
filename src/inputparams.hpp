#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "lsystem.hpp"

// Default input parameters

// terrain needs to be square or terrain map vertices get all hecked up
static size_t TERRAIN_WIDTH = 100;
static size_t TERRAIN_LENGTH = TERRAIN_WIDTH;
static size_t WATER_HEIGHT = 9; // 17
static unsigned int NUM_OCTAVES = 7; // # of octaves for terrain generation
static double REDIST = 0.8f; // 1.05f;
static unsigned int TREE_DENSITY = 2000; // density of trees (lower->denser)
static unsigned int GRASS_DENSITY = 2000; // density of grass (lower->denser)
static bool drawShadowDebugQuad = false;
static std::string SKYBOX_NAME = "heather";
static glm::vec3 SUN_DIRECTION = glm::vec3(48.0f, 35.0f, 29.0f);
static glm::vec3 SUN_COLOUR = glm::vec3(1.0f, 0.7f, 0.0f);
static float SUN_INTENSITY = 0.5f;
static glm::vec3 AMBIENT_COLOUR = glm::vec3(0.3f, 0.3f, 0.3f);
static glm::vec3 CAMERA_POS = glm::vec3( 0.0f, 0.0f, 0.0f );
static glm::vec3 CAMERA_FRONT = glm::vec3( 1.0f, 0.0f, 0.0f );
static float CAMERA_PITCH = 0.0f;
static float CAMERA_YAW = 0.0f;
static float CAMERA_SPEED = 1.0f;
static glm::vec4 GRASS_COLOUR = glm::vec4(0.4f, 0.3f, 0.0f, 1.0f);
static glm::vec4 LEAF_COLOUR = glm::vec4(0.9f, 0.1f, 0.1f, 1.0f);
static std::string GROUND_TEXTURE = "res/grass.png";
static std::vector<Rules*> treeLSystems;

//----------------------------------------------------------------------------------------
// Reads input parameters from file
void readInputParams( const char* paramFile );
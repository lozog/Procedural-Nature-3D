#include "inputparams.hpp"


#include <iostream>
#include <fstream>								// ifstream
#include <sstream>								// stringstream

using namespace std;

void readInputParams( const char* paramFile ) {
	istream* in;
	ifstream infile;
	infile.open( paramFile );
	if (infile) {
		if ( infile.is_open() ) {
			in = &infile;
		} // if
		for ( ;; ) {									// my CS 343 is showing
			string inputline;
			getline(*in, inputline);					// read one line

			if ( in->fail() ) break;					// end of param file

            stringstream inputLineStream(inputline);

			string paramName;
			inputLineStream >> paramName;
			// cout << paramName << endl;

			if ( paramName.at(0) == '#' ) continue; 	// skip comments

			if ( paramName == "TERRAIN_SIZE" ) {
				size_t in;
				inputLineStream >> in;
				TERRAIN_WIDTH = in;
				TERRAIN_LENGTH = TERRAIN_WIDTH;
			} else if ( paramName == "NUM_OCTAVES" ) {
				unsigned int in;
				inputLineStream >> in;
				NUM_OCTAVES = in;
			} else if ( paramName == "WATER_HEIGHT" ) {
				size_t in;
				inputLineStream >> in;
				WATER_HEIGHT = in;
			} else if ( paramName == "REDIST" ) {
				double in;
				inputLineStream >> in;
				REDIST = in;
			} else if ( paramName == "GROUND_TEXTURE" ) {
				string in;
				inputLineStream >> in;
				GROUND_TEXTURE = "res/" + in;
			} else if ( paramName == "TREE_DENSITY" ) {
				unsigned int in;
				inputLineStream >> in;
				TREE_DENSITY = in;
			} else if ( paramName == "GRASS_DENSITY" ) {
				unsigned int in;
				inputLineStream >> in;
				GRASS_DENSITY = in;
			} else if ( paramName == "GRASS_COLOUR" ) {
				float in;
				glm::vec3 vec3f;
				inputLineStream >> in;
				vec3f.x = in;
				inputLineStream >> in;
				vec3f.y = in;
				inputLineStream >> in;
				vec3f.z = in;
				GRASS_COLOUR = glm::vec4(vec3f, 1.0f);
			} else if ( paramName == "LEAF_COLOUR" ) {
				float in;
				glm::vec3 vec3f;
				inputLineStream >> in;
				vec3f.x = in;
				inputLineStream >> in;
				vec3f.y = in;
				inputLineStream >> in;
				vec3f.z = in;
				LEAF_COLOUR = glm::vec4(vec3f, 1.0f);
			} else if ( paramName == "SKYBOX_NAME" ) {
				string in;
				inputLineStream >> in;
				SKYBOX_NAME = in;
			} else if ( paramName == "SUN_DIRECTION" ) {
				float in;
				glm::vec3 vec3f;
				inputLineStream >> in;
				vec3f.x = in;
				inputLineStream >> in;
				vec3f.y = in;
				inputLineStream >> in;
				vec3f.z = in;
				SUN_DIRECTION = vec3f;
			} else if ( paramName == "SUN_COLOUR" ) {
				float in;
				glm::vec3 vec3f;
				inputLineStream >> in;
				vec3f.x = in;
				inputLineStream >> in;
				vec3f.y = in;
				inputLineStream >> in;
				vec3f.z = in;
				SUN_COLOUR = vec3f;
			} else if ( paramName == "SUN_INTENSITY" ) {
				float in;
				inputLineStream >> in;
				SUN_INTENSITY = in;
			} else if ( paramName == "AMBIENT_COLOUR" ) {
				float in;
				glm::vec3 vec3f;
				inputLineStream >> in;
				vec3f.x = in;
				inputLineStream >> in;
				vec3f.y = in;
				inputLineStream >> in;
				vec3f.z = in;
				AMBIENT_COLOUR = vec3f;
			} else if ( paramName == "CAMERA_POS" ) {
				float in;
				glm::vec3 vec3f;
				inputLineStream >> in;
				vec3f.x = in;
				inputLineStream >> in;
				vec3f.y = in;
				inputLineStream >> in;
				vec3f.z = in;
				CAMERA_POS = vec3f;
			} else if ( paramName == "CAMERA_FRONT" ) {
				float in;
				glm::vec3 vec3f;
				inputLineStream >> in;
				vec3f.x = in;
				inputLineStream >> in;
				vec3f.y = in;
				inputLineStream >> in;
				vec3f.z = in;
				CAMERA_FRONT = vec3f;
			} else if ( paramName == "CAMERA_PITCH" ) {
				float in;
				inputLineStream >> in;
				CAMERA_PITCH = in;
			} else if ( paramName == "CAMERA_YAW" ) {
				float in;
				inputLineStream >> in;
				CAMERA_YAW = in;
			} else if ( paramName == "CAMERA_SPEED" ) {
				float in;
				inputLineStream >> in;
				CAMERA_SPEED = in;
			} else if ( paramName == "LSYSTEM" ) {
				int numRules;
				inputLineStream >> numRules;
				Rules* sysRules = new Rules();
				for( unsigned int i = 0; i < numRules; i += 1 ) {

					string prodrule;
					getline(*in, prodrule);					// read one line
					if ( prodrule.at(0) == '#' ) { 			// skip comments
						i -= 1;
						continue;
					} // if
					stringstream prodLineStream(prodrule);
					// cout << prodrule << endl;

					string LHS, RHS;
					prodLineStream >> LHS;
					prodLineStream >> RHS;

					Rule* rule = new Rule(LHS, RHS);
					sysRules->push_back(rule);
				} // for
				treeLSystems.push_back(sysRules);
			} else {
				cout << "skipping unrecognized parameter: " << paramName << endl;
			} // if
		} // for
	} else {
		cout << "Error opening input parameter file." << endl;
		exit(1);
	} // if

	infile.close();
}

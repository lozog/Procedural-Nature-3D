#include "ObjFileDecoder.hpp"
using namespace glm;

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
using namespace std;

#include "cs488-framework/Exception.hpp"


//---------------------------------------------------------------------------------------
void ObjFileDecoder::decode(
		const char * objFilePath,
		std::string & objectName,
        std::vector<vec3> & positions,
        std::vector<vec3> & normals,
        std::vector<vec2> & uvCoords
) {

	// Empty containers, and start fresh before inserting data from .obj file
	positions.clear();
	normals.clear();
	uvCoords.clear();

    ifstream in(objFilePath, std::ios::in);
    in.exceptions(std::ifstream::badbit);

    if (!in) {
        stringstream errorMessage;
        errorMessage << "Unable to open .obj file " << objFilePath
            << " within method ObjFileDecoder::decode" << endl;

        throw Exception(errorMessage.str().c_str());
    }

    string currentLine;
    int positionIndexA, positionIndexB, positionIndexC;
    int normalIndexA, normalIndexB, normalIndexC;
    int uvCoordIndexA, uvCoordIndexB, uvCoordIndexC;
    vector<vec3> temp_positions;
    vector<vec3> temp_normals;
    vector<vec2> temp_uvCoords;

	objectName = "";

    while (!in.eof()) {
        try {
            getline(in, currentLine);
        } catch (const ifstream::failure &e) {
            in.close();
            stringstream errorMessage;
            errorMessage << "Error calling getline() -- " << e.what() << endl;
            throw Exception(errorMessage.str());
        }
	    if (currentLine.substr(0, 2) == "o ") {
		    // Get entire line excluding first 2 chars.
		    istringstream s(currentLine.substr(2));
		    s >> objectName;


	    } else if (currentLine.substr(0, 2) == "v ") {
            // Vertex data on this line.
            // Get entire line excluding first 2 chars.
            istringstream s(currentLine.substr(2));
            glm::vec3 vertex;
            s >> vertex.x;
            s >> vertex.y;
            s >> vertex.z;
            temp_positions.push_back(vertex);

        } else if (currentLine.substr(0, 3) == "vn ") {
            // Normal data on this line.
            // Get entire line excluding first 2 chars.
            istringstream s(currentLine.substr(2));
            vec3 normal;
            s >> normal.x;
            s >> normal.y;
            s >> normal.z;
            temp_normals.push_back(normal);

        } else if (currentLine.substr(0, 3) == "vt ") {
            // Texture coordinate data on this line.
            // Get entire line excluding first 2 chars.
            istringstream s(currentLine.substr(2));
            vec2 textureCoord;
            s >> textureCoord.s;
            s >> textureCoord.t;
            temp_uvCoords.push_back(textureCoord);

        } else if (currentLine.substr(0, 2) == "f ") {
            // Face index data on this line.

            int index;

            // sscanf will return the number of matched index values it found
            // from the pattern.
            int numberOfIndexMatches = sscanf(currentLine.c_str(), "f %d/%d/%d",
                                              &index, &index, &index);

            if (numberOfIndexMatches == 3) {
                // Line contains indices of the pattern vertex/uv-cord/normal.
                sscanf(currentLine.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &positionIndexA, &uvCoordIndexA, &normalIndexA,
                       &positionIndexB, &uvCoordIndexB, &normalIndexB,
                       &positionIndexC, &uvCoordIndexC, &normalIndexC);

                // .obj file uses indices that start at 1, so subtract 1 so they start at 0.
                uvCoordIndexA--;
                uvCoordIndexB--;
                uvCoordIndexC--;

                uvCoords.push_back(temp_uvCoords[uvCoordIndexA]);
                uvCoords.push_back(temp_uvCoords[uvCoordIndexB]);
                uvCoords.push_back(temp_uvCoords[uvCoordIndexC]);

            } else {
                // Line contains indices of the pattern vertex//normal.
                sscanf(currentLine.c_str(), "f %d//%d %d//%d %d//%d",
		               &positionIndexA, &normalIndexA,
                       &positionIndexB, &normalIndexB,
                       &positionIndexC, &normalIndexC);
            }

            positionIndexA--;
            positionIndexB--;
            positionIndexC--;
            normalIndexA--;
            normalIndexB--;
            normalIndexC--;

            positions.push_back(temp_positions[positionIndexA]);
            positions.push_back(temp_positions[positionIndexB]);
            positions.push_back(temp_positions[positionIndexC]);

            normals.push_back(temp_normals[normalIndexA]);
            normals.push_back(temp_normals[normalIndexB]);
            normals.push_back(temp_normals[normalIndexC]);
        }
    }

    in.close();

	if (objectName.compare("") == 0) {
		// No 'o' object name tag defined in .obj file, so use the file name
		// minus the '.obj' ending as the objectName.
		const char * ptr = strrchr(objFilePath, '/');
		objectName.assign(ptr+1);
		size_t pos = objectName.find('.');
		objectName.resize(pos);
	}
}

//---------------------------------------------------------------------------------------
void ObjFileDecoder::decode(
		const char * objFilePath,
		std::string & objectName,
        std::vector<vec3> & positions,
        std::vector<vec3> & normals
) {
    std::vector<vec2> uvCoords;
    decode(objFilePath, objectName, positions, normals, uvCoords);
}

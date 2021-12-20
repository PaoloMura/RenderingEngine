#pragma once

#include <fstream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Constants.h"
#include "ModelTriangle.h"
#include "Parser.h"
#include "TextureMap.h"
#include "TexturePoint.h"
#include "Utils.h"

struct VertexNormal {
    glm::vec3 normalSum;
    int adjacentFaces;
};

void parseOBJFile(const char *filename, std::vector<ModelTriangle> &triangles, std::vector<glm::vec3> &vertices,
                  TextureMap &tm, array<float, 3> scaleFactor);

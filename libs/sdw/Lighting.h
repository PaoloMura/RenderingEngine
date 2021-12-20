#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include "Colour.h"
#include "Constants.h"
#include "ModelTriangle.h"
#include "RayTriangleIntersection.h"

using namespace std;

Colour applyLight(RayTriangleIntersection rti, glm::vec3 tuv, glm::vec3 lightPoint, vector<ModelTriangle> &triangles,
                  glm::vec3 cameraPosition, int shadow, bool lightModes[], int shading, bool debug, Constants consts);

#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <thread>
#include "Constants.h"
#include "Draw.h"
#include "Lighting.h"
#include "ModelTriangle.h"
#include "RayTriangleIntersection.h"

using namespace std;

RayTriangleIntersection getClosestIntersection(glm::vec3 solution,
                                               glm::vec3 cameraPosition,
                                               ModelTriangle triangle,
                                               int triangleIndex);

void testGetClosestIntersection(vector<ModelTriangle> &triangles);

void drawRayTracedRender(DrawingWindow &window, vector<ModelTriangle> &modelTriangles,
                         glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
                         glm::vec3 light, int shadow, bool lightModes[], int shading, bool debug, Constants consts);

void drawRayTracedRender(DrawingWindow &window, ModelTriangle modelTriangle,
                         glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
                         glm::vec3 light, int shadow, bool lightModes[], int shading, bool debug, Constants consts);

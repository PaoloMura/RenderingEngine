#pragma once

#include <assert.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include "Colour.h"
#include "Constants.h"
#include "Draw.h"
#include "DrawingWindow.h"
#include "ModelTriangle.h"
#include "TextureMap.h"
#include "TexturePoint.h"

CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, glm::vec3 vertexPosition,
                                       float focalLength, float multiplier);

void drawPointCloud(DrawingWindow &window, std::vector<glm::vec3> &vertices,
                    glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength);

void drawWireFrame(DrawingWindow &window, std::vector<ModelTriangle> modelTriangles,
                   glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, bool debug);

void drawRasterisedRender(DrawingWindow &window, std::vector<ModelTriangle> modelTriangles, float depthMap[][WIDTH],
                          TextureMap &tm, glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
                          bool debug);

void drawRasterisedRender(DrawingWindow &window, ModelTriangle mt, float depthMap[][WIDTH], TextureMap &tm,
                          glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, bool debug);

void drawLightSource(DrawingWindow &window, glm::vec3 lightPosition, float depthMap[][WIDTH],
                     glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength);

void testTextureMapping(DrawingWindow &window, float depthMap[][WIDTH], TextureMap &tm, bool debug);
#pragma once

#include <assert.h>
#include <chrono>
#include <unordered_map>
#include <string>
#include <thread>
#include <vector>
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include "Colour.h"
#include "Constants.h"
#include "DrawingWindow.h"
#include "TextureMap.h"
#include "TexturePoint.h"
#include "Utils.h"

void setPixel(DrawingWindow &window, CanvasPoint point, Colour colour);

void fill(DrawingWindow &window, Colour colour);

void drawPoint(DrawingWindow &window, float depthMap[][WIDTH], CanvasPoint point, Colour colour);

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour);

void drawLine(DrawingWindow &window, float depthMap[][WIDTH], CanvasPoint from, CanvasPoint to, Colour colour);

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle ct, Colour colour);

void drawStrokedTriangles(DrawingWindow &window, std::vector<CanvasTriangle> triangles, std::vector<Colour> colours);

void insertRakes(std::unordered_map<int, std::vector<float>> &umap, CanvasPoint from, CanvasPoint to, bool left, bool debug);

std::unordered_map<int, std::vector<float>> createTriangleMap(CanvasTriangle &ct, bool debug);

void drawFilledTriangle(DrawingWindow &window, float depthMap[][WIDTH], CanvasTriangle ct, Colour colour, bool debug);

void drawTextureMappedTriangle(DrawingWindow &window, float depthMap[][WIDTH], CanvasTriangle ct, TextureMap &tm, bool debug);

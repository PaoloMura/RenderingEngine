#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include "Colour.h"
#include "Constants.h"
#include "DrawingWindow.h"

void translate(glm::vec3 &point, int axis, float distance);

void rotate(glm::vec3 &point, int axis, float theta);

void panOrTilt(glm::mat3 &cameraOrientation, int axis, float theta);

void lookAt(glm::vec3 cameraPosition, glm::vec3 focusPoint, glm::mat3 &cameraOrientation, glm::mat3 &cameraOrientationRay);

void orbit(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, glm::mat3 &cameraOrientationRay, int axis, float theta);

void animate(glm::vec3 &cameraPosition);

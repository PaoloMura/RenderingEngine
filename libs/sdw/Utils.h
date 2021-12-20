#pragma once

#include <string>
#include <vector>
#include "CanvasPoint.h"

std::vector<std::string> split(const std::string &line, char delimiter);

bool compare(CanvasPoint p1, CanvasPoint p2);

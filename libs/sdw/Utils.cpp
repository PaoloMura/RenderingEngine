#include <algorithm>
#include <sstream>
#include "Utils.h"

std::vector<std::string> split(const std::string &line, char delimiter) {
	auto haystack = line;
	std::vector<std::string> tokens;
	size_t pos;
	while ((pos = haystack.find(delimiter)) != std::string::npos) {
		tokens.push_back(haystack.substr(0, pos));
		haystack.erase(0, pos + 1);
	}
	// Push the remaining chars onto the vector
	tokens.push_back(haystack);
	return tokens;
}

bool compare(CanvasPoint p1, CanvasPoint p2) {
    if (p1.y < p2.y) return true;
    else if (p1.y == p2.y and p1.x <= p2.x) return true;
    else return false;
}

#pragma once

#include <array>
#include <glm/glm.hpp>
#include <cmath>

using namespace std;

const int WIDTH = 500;
const int HEIGHT = 500;
const int MULTIPLIER = 7000;
const float TRAN_OFFSET = 0.02;
const float ROT_OFFSET = 0.01;
const float MAX_DEPTH = 0.8;
const float DEPTH_MAP[HEIGHT][WIDTH]{};
const int THREADS = 8;
const int LIGHT_RADIUS = 5;
//float LIGHT_SPACING = 0.01;

// either 1.0 or -1.0 depending on whether the vector points in the OBJ file are listed in clockwise or anticlockwise
// winding order
const float NORMAL_DIRECTION = -1.0;

// a value of 0.0 sets light intensity to 1
// higher values give a more dramatic radius-intensity curve
//const float INTENSITY_FACTOR = 0.2;

// a value of 0.0 causes a division by zero error!
// higher values increase the effect of angle of incidence
// best to stay in the range 0.0 < I_F <= 1.0
//const float INCIDENCE_FACTOR = 0.2;

// higher values make the specular light smaller and more focused
//const int SPECULAR_POWER = 25;

// this minimum intensity should be between 0.0 and 1.0
// float AMBIENT_THRESHOLD = 0.2;

enum viewingMode {
    POINT_CLOUD,
    WIREFRAME,
    RASTERISED,
    RAY_TRACED
};

// shadow modes
const int NO_SHADOW = 0;
const int HARD_SHADOW = 1;
const int SOFT_SHADOW = 2;

// lighting modes
const int PROXIMITY = 0;
const int INCIDENCE = 1;
const int SPECULAR = 2;
const int AMBIENT = 3;

// shading modes
const int FLAT = 0;
const int GOURAUD = 1;
const int PHONG = 2;


// TODO: remove this?
//float LIGHT_SPACING;
//float INTENSITY_FACTOR;
//float INCIDENCE_FACTOR;
//int SPECULAR_POWER;
//float AMBIENT_THRESHOLD;

//struct Constants {
//    float LIGHT_SPACING = 0.05;
//    float INTENSITY_FACTOR = 0.15;
////    float INCIDENCE_FACTOR = 0.2;
//    int SPECULAR_POWER = 45;
//    float AMBIENT_THRESHOLD = 0.15;
//};

struct Constants {
    float LIGHT_SPACING = 0.05;
    float INTENSITY_FACTOR = 0.05;
    int SPECULAR_POWER = 45;
    float AMBIENT_THRESHOLD = 0.15;
};

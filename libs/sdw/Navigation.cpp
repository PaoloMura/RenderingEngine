#include "Navigation.h"

using namespace std;

void translate(glm::vec3 &point, int axis, float distance) {
    glm::vec3 translationVector = glm::vec3(0,0,0);
    translationVector[axis] = distance;
    point = point + translationVector;
}

glm::mat3 xRotation(float theta) {
    return glm::mat3(1, 0, 0,
                     0, cos(theta), -sin(theta),
                     0, sin(theta), cos(theta));
}

glm::mat3 yRotation(float theta) {
    return glm::mat3(cos(theta), 0, sin(theta),
                     0, 1, 0,
                     -sin(theta), 0, cos(theta));
}

glm::mat3 zRotation(float theta) {
    return glm::mat3(cos(theta), -sin(theta), 0,
                     sin(theta), cos(theta), 0,
                     0, 0, 1);
}

void rotate(glm::vec3 &point, int axis, float theta) {
    glm::mat3 rotationMatrix;
    switch (axis) {
        case 0:
            rotationMatrix = xRotation(theta);
            break;
        case 1:
            rotationMatrix = yRotation(theta);
            break;
        case 2:
            rotationMatrix = zRotation(theta);
    }
    point = rotationMatrix * point;
}

void panOrTilt(glm::mat3 &cameraOrientation, int axis, float theta) {
    glm::mat3 rotationMatrix;
    switch (axis) {
        case 0:
            rotationMatrix = xRotation(theta);
            break;
        case 1:
            rotationMatrix = yRotation(theta);
            break;
        case 2:
            rotationMatrix = zRotation(theta);
    }
    cameraOrientation = rotationMatrix * cameraOrientation;
}

//void translate(glm::vec3 &point, glm::vec3 matrix) {
//    point = point + matrix;
//}
//
//void rotate(glm::vec3 &point, glm::mat3 rotationMatrix) {
//    point = rotationMatrix * point;
//}
//
//void panOrTilt(glm::mat3 &cameraOrientation, glm::mat3 rotationMatrix) {
//    cameraOrientation = rotationMatrix * cameraOrientation;
//}

void lookAt(glm::vec3 cameraPosition, glm::vec3 focusPoint, glm::mat3 &cameraOrientation, glm::mat3 &cameraOrientationRay) {
    // update the rasterised camera's orientation matrix
    glm::vec3 forward = glm::normalize(cameraPosition - focusPoint);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,-1,0)));
    glm::vec3 up = glm::normalize(glm::cross(forward, right));
    cameraOrientation = glm::mat3(right, up, forward);

    // update the ray-traced camera's orientation matrix
    glm::vec3 forwardRay = glm::normalize(glm::vec3(-forward[0], -forward[1], forward[2]));
    glm::vec3 rightRay = glm::normalize(glm::cross(forwardRay, glm::vec3(0,-1,0)));
    glm::vec3 upRay = glm::normalize(glm::cross(forwardRay, rightRay));
    cameraOrientationRay = glm::mat3(rightRay, upRay, forwardRay);
}

void orbit(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, glm::mat3 &cameraOrientationRay, int axis, float theta) {
    rotate(cameraPosition, axis, theta);
    lookAt(cameraPosition, glm::vec3(0,0,0), cameraOrientation, cameraOrientationRay);
}

void animate(glm::vec3 &cameraPosition) {
    rotate(cameraPosition, 1, -ROT_OFFSET);
}

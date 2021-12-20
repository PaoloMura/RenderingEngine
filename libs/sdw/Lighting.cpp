#include "Lighting.h"

using namespace std;

float hardShadow(glm::vec3 startPoint, glm::vec3 endPoint, vector<ModelTriangle> &triangles, int triangleIndex) {
    glm::vec3 rayDirection = glm::normalize(endPoint - startPoint);
    float rayDistance = glm::distance(endPoint, startPoint);
    int i = 0;
    bool blocked = false;
    while (!blocked && i < triangles.size()) {
        ModelTriangle triangle = triangles[i];
        glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        glm::vec3 SPVector = startPoint - triangle.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;
        if (i != triangleIndex
            && possibleSolution[0] > 0.0 && possibleSolution[0] < rayDistance
            && possibleSolution[1] >= 0.0 && possibleSolution[2] >= 0.0
            && possibleSolution[1] + possibleSolution[2] <= 1)
            blocked = true;
        i++;
    }
    if (blocked) return 0.0;
    else return 1.0;
}

float softShadow(glm::vec3 startPoint, glm::vec3 lightPoint, vector<ModelTriangle> &triangles, int triangleIndex, Constants consts) {
    float hits = 0;
    for (int i=-LIGHT_RADIUS; i <= LIGHT_RADIUS; i++) {
        for (int j=-LIGHT_RADIUS; j <= LIGHT_RADIUS; j++) {
            glm::vec3 endPoint = lightPoint;
            endPoint[0] += i * consts.LIGHT_SPACING;
            endPoint[2] += j * consts.LIGHT_SPACING;
            hits += hardShadow(startPoint, endPoint, triangles, triangleIndex);
        }
    }
    float totalLights = pow((2 * LIGHT_RADIUS + 1), 2);
    return hits / totalLights;
}

float proximityLight(glm::vec3 modelPoint, glm::vec3 lightPoint, bool debug, Constants consts) {
    float intensity;
    float radius = glm::length(modelPoint - lightPoint);
    float offset = sqrt(1 / (4 * M_PI));
    float r = consts.INTENSITY_FACTOR * radius + offset;
    if (r == 0) intensity = 1;
    else intensity = 1 / (4 * M_PI * r * r);
    return intensity;
}

float incidenceLight(glm::vec3 modelPoint, glm::vec3 lightPoint, glm::vec3 normal, bool debug) {
    float intensity = 1.0;
    glm::vec3 pointToLight = glm::normalize(lightPoint - modelPoint);
    float dotProduct = glm::dot(normal, pointToLight);
    if (dotProduct <= 0) intensity = 0.0;
    else intensity = dotProduct;
//    else {
//        intensity = 1 + INCIDENCE_FACTOR * log(dotProduct + exp(-1/INCIDENCE_FACTOR));
//        if (intensity > 1) intensity = 1.0;
//    }
    return intensity;
}

float specularLight(glm::vec3 modelPoint, glm::vec3 lightPoint, glm::vec3 normal, glm::vec3 cameraPosition, bool debug, Constants consts) {
    glm::vec3 incidenceRay = glm::normalize(modelPoint - lightPoint);
    glm::vec3 reflectedRay = glm::normalize(incidenceRay - 2.0f * normal * glm::dot(incidenceRay, normal));
    glm::vec3 pointToCamera = glm::normalize(cameraPosition - modelPoint);
    return pow(glm::dot(pointToCamera, reflectedRay), consts.SPECULAR_POWER);
}

float interpolateIntensity(glm::vec3 tuv, float intensities[3], bool debug) {
    float u = tuv[1];
    float v = tuv[2];
    float w = 1 - u - v;

    float i0 = intensities[0];
    float i1 = intensities[1];
    float i2 = intensities[2];

    return (w * i0) + (u * i1) + (v * i2);
}

glm::vec3 interpolateNormal(ModelTriangle triangle, glm::vec3 tuv, bool debug) {
    float u = tuv[1];
    float v = tuv[2];
    float w = 1 - u - v;

    glm::vec3 n0 = triangle.vertexNormals[0];
    glm::vec3 n1 = triangle.vertexNormals[1];
    glm::vec3 n2 = triangle.vertexNormals[2];

    return glm::normalize((w * n0) + (u * n1) + (v * n2));
}

Colour applyLight(RayTriangleIntersection rti, glm::vec3 tuv, glm::vec3 lightPoint, vector<ModelTriangle> &triangles,
                  glm::vec3 cameraPosition, int shadow, bool lightModes[], int shading, bool debug, Constants consts) {
    float intensity = 1.0;
    glm::vec3 normal = (shading == PHONG) ? interpolateNormal(rti.intersectedTriangle, tuv, debug) : rti.intersectedTriangle.normal;

    if (shadow == HARD_SHADOW) intensity *= hardShadow(rti.intersectionPoint, lightPoint, triangles, rti.triangleIndex);
    else if (shadow == SOFT_SHADOW) intensity *= softShadow(rti.intersectionPoint, lightPoint, triangles, rti.triangleIndex, consts);

    if (lightModes[PROXIMITY]) intensity *= proximityLight(rti.intersectionPoint, lightPoint, debug, consts);
    if (lightModes[SPECULAR]) {
        float specularIntensity;
        if (shading == GOURAUD) {
            float specularIntensities[3];
            for (int i=0; i < 3; i++) {
                specularIntensities[i] = specularLight(rti.intersectedTriangle.vertices[i], lightPoint,
                                                       rti.intersectedTriangle.vertexNormals[i], cameraPosition, debug, consts);
            }
            specularIntensity = interpolateIntensity(tuv, specularIntensities, debug);
        }
        else specularIntensity = specularLight(rti.intersectionPoint, lightPoint, normal, cameraPosition, debug, consts);
        intensity = max(intensity, specularIntensity);
    }
    if (lightModes[INCIDENCE]) {
        float incidenceIntensity;
        if (shading == GOURAUD) {
            float incidenceIntensities[3];
            for (int i=0; i < 3; i++) {
                incidenceIntensities[i] = incidenceLight(rti.intersectedTriangle.vertices[i], lightPoint,
                                                         rti.intersectedTriangle.vertexNormals[i], debug);
            }
            incidenceIntensity = interpolateIntensity(tuv, incidenceIntensities, debug);
        }
        else incidenceIntensity = incidenceLight(rti.intersectionPoint, lightPoint, normal, debug);
        intensity = (intensity + incidenceIntensity) / 2.0f;
    }
    if (lightModes[AMBIENT]) intensity = max(intensity, consts.AMBIENT_THRESHOLD);
    Colour c = rti.intersectedTriangle.colour;
    return Colour(c.red * intensity, c.green * intensity, c.blue * intensity);
}

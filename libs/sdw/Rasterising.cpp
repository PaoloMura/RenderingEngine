#include "Rasterising.h"

using namespace std;

// isolates the x and z components of the camera's position and forward direction
// returns true if the angle between them is less than π/3 (i.e. they're facing the same direction)
bool facingModel(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, bool debug) {
    glm::vec2 position = glm::normalize(glm::vec2(cameraPosition.x, cameraPosition.z));
    glm::vec2 orientation = glm::normalize(glm::vec2(cameraOrientation[2].x, cameraOrientation[2].z));
    float cos_theta =  glm::dot(position, orientation);
    return cos_theta > 0.5;
}

CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, glm::vec3 vertexPosition,
                                       float focalLength, float multiplier) {
    glm::vec3 camToVertex = vertexPosition - cameraPosition;
    camToVertex = camToVertex * cameraOrientation;
    assert(camToVertex.z != 0.0);
    float u = (camToVertex.x * focalLength / camToVertex.z) * multiplier + (WIDTH / 2);
    float v = (camToVertex.y * focalLength / camToVertex.z) * multiplier + (HEIGHT / 2);
    return CanvasPoint(u, v, -1/camToVertex.z);
}

void drawPointCloud(DrawingWindow &window, vector<glm::vec3> &vertices,
                    glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength) {
    for (int i=0; i < vertices.size(); i++) {
        CanvasPoint point = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, vertices[i], focalLength, MULTIPLIER);
        setPixel(window, point, Colour(255, 255, 255));
    }
}

void drawWireFrame(DrawingWindow &window, vector<ModelTriangle> modelTriangles,
                   glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, bool debug) {
    if (facingModel(cameraPosition, cameraOrientation, debug)) {
        for (int i = 0; i < modelTriangles.size(); i++) {
            ModelTriangle mt = modelTriangles[i];
            CanvasPoint p1 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[0], focalLength,
                                                        MULTIPLIER);
            CanvasPoint p2 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[1], focalLength,
                                                        MULTIPLIER);
            CanvasPoint p3 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[2], focalLength,
                                                        MULTIPLIER);
//            if (p1.depth < MAX_DEPTH and p2.depth < MAX_DEPTH and p3.depth < MAX_DEPTH) {
//                CanvasTriangle ct = CanvasTriangle(p1, p2, p3);
//                drawStrokedTriangle(window, ct, mt.colour);
//            }
            CanvasTriangle ct = CanvasTriangle(p1, p2, p3);
            drawStrokedTriangle(window, ct, mt.colour);
        }
    }
}

void drawRasterisedRender(DrawingWindow &window, vector<ModelTriangle> modelTriangles, float depthMap[][WIDTH],
                          TextureMap &tm, glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
                          bool debug) {
    if (facingModel(cameraPosition, cameraOrientation, debug)) {
        for (int i = 0; i < modelTriangles.size(); i++) {
            ModelTriangle mt = modelTriangles[i];
            CanvasPoint p1 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[0], focalLength,
                                                        MULTIPLIER);
            CanvasPoint p2 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[1], focalLength,
                                                        MULTIPLIER);
            CanvasPoint p3 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[2], focalLength,
                                                        MULTIPLIER);
//            if (p1.depth < MAX_DEPTH && p2.depth < MAX_DEPTH && p3.depth < MAX_DEPTH) {
//                CanvasTriangle ct = CanvasTriangle(p1, p2, p3);
//                if (mt.texturePoints[0].x == -1) {
//                    drawFilledTriangle(window, depthMap, ct, mt.colour, false);
//                }
//                else {
//                    ct.v0().texturePoint = mt.texturePoints[0];
//                    ct.v1().texturePoint = mt.texturePoints[1];
//                    ct.v2().texturePoint = mt.texturePoints[2];
//                    drawTextureMappedTriangle(window, depthMap, ct, tm, debug);
//                }
//            }
            CanvasTriangle ct = CanvasTriangle(p1, p2, p3);
            if (mt.texturePoints[0].x == -1) {
                drawFilledTriangle(window, depthMap, ct, mt.colour, false);
            }
            else {
                ct.v0().texturePoint = mt.texturePoints[0];
                ct.v1().texturePoint = mt.texturePoints[1];
                ct.v2().texturePoint = mt.texturePoints[2];
                drawTextureMappedTriangle(window, depthMap, ct, tm, debug);
            }
        }
    }
}

void drawRasterisedRender(DrawingWindow &window, ModelTriangle mt, float depthMap[][WIDTH], TextureMap &tm,
                          glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, bool debug) {
    if (facingModel(cameraPosition, cameraOrientation, debug)) {
        CanvasPoint p1 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[0], focalLength,
                                                    MULTIPLIER);
        CanvasPoint p2 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[1], focalLength,
                                                    MULTIPLIER);
        CanvasPoint p3 = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, mt.vertices[2], focalLength,
                                                    MULTIPLIER);
        if (debug) cout << mt.vertices[0].y << ',' << mt.vertices[1].y << ',' << mt.vertices[2].y << endl;
        if (p1.depth < MAX_DEPTH and p2.depth < MAX_DEPTH and p3.depth < MAX_DEPTH) {
            CanvasTriangle ct = CanvasTriangle(p1, p2, p3);
            if (mt.texturePoints[0].x == -1) {
                drawFilledTriangle(window, depthMap, ct, mt.colour, debug);
            }
            else {
                ct.v0().texturePoint = mt.texturePoints[0];
                ct.v1().texturePoint = mt.texturePoints[1];
                ct.v2().texturePoint = mt.texturePoints[2];
                drawTextureMappedTriangle(window, depthMap, ct, tm, debug);
            }
        }
    }
}

void drawLightSource(DrawingWindow &window, glm::vec3 lightPosition, float depthMap[][WIDTH],
                     glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength) {
    CanvasPoint point = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, lightPosition, focalLength, MULTIPLIER);
    drawPoint(window, depthMap, point, Colour(255, 255, 255));
}

void testTextureMapping(DrawingWindow &window, float depthMap[][WIDTH], TextureMap &tm, bool debug) {
    fill(window, Colour(0, 0, 0));
    CanvasPoint p1 = CanvasPoint(160, 10, 1);
    CanvasPoint p2 = CanvasPoint(300, 230, 1);
    CanvasPoint p3 = CanvasPoint(10, 150, 1);
//    p1.texturePoint = TexturePoint(195, 5);
//    p2.texturePoint = TexturePoint(395, 380);
//    p3.texturePoint = TexturePoint(65, 330);
    p1.texturePoint = TexturePoint(395, 380);
    p2.texturePoint = TexturePoint(195, 5);
    p3.texturePoint = TexturePoint(65, 330);
    CanvasTriangle ct = CanvasTriangle(p1, p2, p3);
    drawTextureMappedTriangle(window, depthMap, ct, tm, debug);
}

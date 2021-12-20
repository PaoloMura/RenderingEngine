#include "RayTracing.h"

using namespace std;

glm::vec4 solveTUV(glm::vec3 startPosition, glm::vec3 rayDirection, vector<ModelTriangle> &triangles, int myIndex, bool debug) {
    // the solution vector holds (t, u, v, i) where i is the triangle index
    glm::vec3 solution = glm::vec3(INT32_MAX, 0, 0);
    int triangleIndex = -1;
    for (int i=0; i < triangles.size(); i++) {
        if (i != myIndex) {
            ModelTriangle triangle = triangles[i];
            glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
            glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
            glm::vec3 SPVector = startPosition - triangle.vertices[0];
            glm::mat3 DEMatrix(-rayDirection, e0, e1);
            glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;
            if (possibleSolution[0] > 0.0 && possibleSolution[0] < solution[0]
                && possibleSolution[1] >= 0.0 && possibleSolution[2] >= 0.0
                && possibleSolution[1] + possibleSolution[2] <= 1) {
                solution = possibleSolution;
                triangleIndex = i;
            }
        }
    }
    return glm::vec4(solution, triangleIndex);
}

RayTriangleIntersection getClosestIntersection(glm::vec3 solution, glm::vec3 startPosition,
                                               ModelTriangle triangle, int triangleIndex, bool debug) {
    glm::vec3 point = triangle.vertices[0] + solution[1] * (triangle.vertices[1] - triangle.vertices[0]) +
                      solution[2] * (triangle.vertices[2] - triangle.vertices[0]);
    float distance = abs(point[2] - startPosition[2]);
    return RayTriangleIntersection(point, distance, triangle, triangleIndex);
}

void castRay(glm::vec3 startPosition, glm::vec3 rayDirection, vector<ModelTriangle> &triangles,
             glm::vec4 &tuvi, RayTriangleIntersection &rti, bool debug) {
    tuvi = solveTUV(startPosition, glm::normalize(rayDirection), triangles, tuvi[3], debug);
    int triangleIndex = tuvi[3];
    if (triangleIndex != -1) {
        glm::vec3 tuv = glm::vec3(tuvi);
        ModelTriangle triangle = triangles[triangleIndex];
        rti = getClosestIntersection(tuv, startPosition, triangle, triangleIndex, debug);
    }
}

void drawRayTracedRender(DrawingWindow &window, vector<ModelTriangle> &triangles,
                         glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
                         glm::vec3 light, int shadow, bool lightModes[], int shading, bool debug, Constants consts) {
    for (int j=0; j < HEIGHT; j++) {
        for (int i=0; i < WIDTH; i++) {
            float scaleFactor = -MULTIPLIER;
            float x = cameraPosition[0] + (i - WIDTH/2) / scaleFactor;
            float y = cameraPosition[1] + (j - HEIGHT/2) / scaleFactor;
            float z = cameraPosition[2] - focalLength;
            glm::vec3 pixelPosition = glm::vec3(x, y, z);
            glm::vec3 rayDirection = glm::normalize(pixelPosition - cameraPosition);
            rayDirection = rayDirection * cameraOrientation;

            glm::vec4 tuvi = glm::vec4(0, 0, 0, -1);
            RayTriangleIntersection rti;
            castRay(cameraPosition, rayDirection, triangles, tuvi, rti, debug);

            if (tuvi[3] != -1) {
                if (rti.intersectedTriangle.reflective) {
                    glm::vec3 normal = glm::normalize(rti.intersectedTriangle.normal);
                    rayDirection = rayDirection - 2 * normal * glm::dot(rayDirection, normal);
                    castRay(rti.intersectionPoint, glm::normalize(rayDirection), triangles, tuvi, rti, debug);
                }
                if (tuvi[3] != -1) {
                    Colour colour = applyLight(rti, glm::vec3(tuvi), light, triangles, cameraPosition, shadow, lightModes, shading, debug, consts);
                    setPixel(window, CanvasPoint(i, j), colour);
                }
            }

//            glm::vec4 tuvi = solveTUV(cameraPosition, glm::normalize(rayDirection), triangles);
//            if (tuvi[3] != -1) {
//                glm::vec3 tuv = glm::vec3(tuvi);
//                int triangleIndex = tuvi[3];
//                ModelTriangle triangle = triangles[triangleIndex];
//                RayTriangleIntersection rti = getClosestIntersection(tuv, cameraPosition, triangle, triangleIndex);
//                if (triangle.reflective) {
//                    glm::vec3 startPosition = rti.intersectionPoint;
//                    glm::vec3 reflectedRayDirection = rayDirection - 2 * triangle.normal * glm::dot(rayDirection, triangle.normal);
//                    tuvi = solveTUV(startPosition, glm::normalize(reflectedRayDirection), triangles);
//                    if (tuvi[3] != -1) {
//                        tuv = glm::vec3(tuvi);
//                        int newTriangleIndex = tuvi[3];
//                        ModelTriangle triangle = triangles[newTriangleIndex];
//                        if (newTriangleIndex != triangleIndex) {
//                            rti = getClosestIntersection(tuv, startPosition, triangle, newTriangleIndex);
//                            Colour colour = applyLight(rti, tuv, light, triangles, cameraPosition, shadow, lightModes, shading, debug);
//                            setPixel(window, CanvasPoint(i, j), colour);
//                        }
//                    }
//                }
//                else {
//                    Colour colour = applyLight(rti, tuv, light, triangles, cameraPosition, shadow, lightModes, shading, debug);
//                    setPixel(window, CanvasPoint(i, j), colour);
//                }
//            }
        }
    }
}

void drawRayTracedRender(DrawingWindow &window, ModelTriangle triangle,
                         glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
                         glm::vec3 light, int shadow, bool lightModes[], int shading, bool debug, Constants consts) {
    for (int j=0; j < HEIGHT; j++) {
        for (int i=0; i < WIDTH; i++) {
            float scaleFactor = -MULTIPLIER;
            float x = cameraPosition[0] + (i - WIDTH/2) / scaleFactor;
            float y = cameraPosition[1] + (j - HEIGHT/2) / scaleFactor;
            float z = cameraPosition[2] - focalLength;
            glm::vec3 pixelPosition = glm::vec3(x, y, z);
            glm::vec3 rayDirection = pixelPosition - cameraPosition;
            rayDirection = rayDirection * cameraOrientation;
            vector<ModelTriangle> triangles{triangle};
            glm::vec4 tuvi = solveTUV(cameraPosition, glm::normalize(rayDirection), triangles, -1, debug);
            if (tuvi[3] == -1) setPixel(window, CanvasPoint(i, j), Colour(0, 0, 0));
            else {
                glm::vec3 tuv = glm::vec3(tuvi);
                int triangleIndex = tuvi[3];
                RayTriangleIntersection rti = getClosestIntersection(tuv, cameraPosition, triangles[triangleIndex],
                                                                     triangleIndex, debug);
                Colour colour = applyLight(rti, tuv, light, triangles, cameraPosition, shadow, lightModes, shading, debug, consts);
                setPixel(window, CanvasPoint(i, j), colour);
            }
        }
    }
}
















//void threadRayTrace(DrawingWindow &window, vector<ModelTriangle> &modelTriangles,
//                    glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
//                    glm::vec3 light, bool lightModes[], int startRow, int endRow, bool debug) {
//    for (int j=startRow; j < endRow; j++) {
//        for (int i=0; i < WIDTH; i++) {
//            float scaleFactor = -MULTIPLIER;
//            float x = cameraPosition[0] + (i - WIDTH/2) / scaleFactor;
//            float y = cameraPosition[1] + (j - HEIGHT/2) / scaleFactor;
//            float z = cameraPosition[2] - focalLength;
//            glm::vec3 pixelPosition = glm::vec3(x, y, z);
//            glm::vec3 rayDirection = pixelPosition - cameraPosition;
//            rayDirection = rayDirection * cameraOrientation;
//            glm::vec4 tuvi = solveTUV(cameraPosition, glm::normalize(rayDirection), modelTriangles);
//            if (tuvi[3] == -1) setPixel(window, CanvasPoint(i, j), Colour(0, 0, 0));
//            else {
//                glm::vec3 tuv = glm::vec3(tuvi);
//                int triangleIndex = tuvi[3];
//                RayTriangleIntersection rti = getClosestIntersection(tuv, cameraPosition, modelTriangles[triangleIndex],
//                                                                     triangleIndex);
//                Colour colour = applyLight(rti, light, modelTriangles, cameraPosition, shadow, lightModes, debug);
//                setPixel(window, CanvasPoint(i, j), colour);
//            }
//        }
//    }
//}
//
//void drawRayTracedRender(DrawingWindow &window, vector<ModelTriangle> &modelTriangles,
//                         glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength,
//                         glm::vec3 light, bool lightModes[], bool debug) {
//    vector<thread> threads(THREADS);
//    int rows = HEIGHT / THREADS;
//    for (int i=0; i < THREADS; i++) {
//        thread t = thread(threadRayTrace, window, modelTriangles, cameraPosition, cameraOrientation, focalLength, light, lightModes,
//                          i * rows, (i+1) * rows, debug);
//        threads.push_back(t);
//    }
//    for (auto &t : threads) {
//        t.join();
//    }
//    thread t1 = thread(threadRayTrace, window, modelTriangles, cameraPosition, cameraOrientation, focalLength, light, lightModes,
//              0, HEIGHT/2, debug);
//    thread t2 = thread(threadRayTrace, window, modelTriangles, cameraPosition, cameraOrientation, focalLength, light, lightModes,
//              HEIGHT/2, HEIGHT, debug);
//    t1.join();
//    t2.join();
//}




//void myActualFunction(int param1, int param2, int etc, int startRow, int endRow) {
//    for (int i=startRow; i < endRow; i++) {
//        cout << "section to be processed" << endl;
//    }
//}
//
//void myThreadingFunction(int param1, int param2, int etc) {
//    vector<thread> myThreads;
//    for (int i=0; i < NUM_OF_THREADS; i++) {
//        int startRow = i * rows;
//        int endRow = (i+1) * rows;
//        thread t(myActualFunction, param1, param2, etc, startRow, endRow);
//        myThreads.push_back(t);
//    }
//    for (int i=0; i < NUM_OF_THREADS; i++) {
//        myThreads[i].join();
//    }
//}







#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iterator>
#include <SDL.h>
#include <unordered_map>
#include <vector>
#include <Colour.h>
#include <Constants.h>
#include <Draw.h>
#include <DrawingWindow.h>
#include <ModelTriangle.h>
#include <Navigation.h>
#include <Parser.h>
#include <Rasterising.h>
#include <RayTracing.h>
#include <RayTriangleIntersection.h>
#include <TextureMap.h>

using namespace std;

class Session {
private:
    // general attributes
    bool running = true;
    SDL_Event event;
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    float depthMap[HEIGHT][WIDTH];
    int currentTriangle = 0;
    viewingMode vm;
    bool animation = false;
    bool singleTriangle = false;
    Constants consts;

    // camera attributes
    float focalLength = 0.2;
//    glm::vec3 cameraPosition = {0.0, 0.0, 4.0};
//    glm::vec3 cameraPosition = {0.0, 0.0, 1.62};

    // model attributes
    vector<ModelTriangle> modelTriangles;
    vector<glm::vec3> vertices;
    array<float, 3> scaleFactor = {-0.17, 0.17, 0.17};
    int numTriangles;
    TextureMap tm;

    // test attributes
    bool debug = false;

    // private methods
    void draw();
    void changeCurrentTriangle(int offset);
    void displayTitle();
    void handleEvent();
public:
    // camera attributes
    glm::vec3 cameraPosition = glm::vec3(0, 0, 4);
    glm::mat3 cameraOrientation = glm::mat3(1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1);
    glm::mat3 cameraOrientationRay = glm::mat3(1, 0, 0,
                                               0, 1, 0,
                                               0, 0, 1);

    // light attributes
    glm::vec3 lightPosition = glm::vec3(0, 0.4, 0);
    int shadow = NO_SHADOW;
    bool lightModes[6] = {false, false, false, false}; // this is indexed by the constant lightingModes
    int shading = FLAT;

//    glm::vec3 lightPosition = {0.0, 0.0, 0.4};
//    glm::vec3 lightPosition = {0.0, 0.02, 0.25};

    Session(const char *objFilename, viewingMode view, Constants constants) {
        parseOBJFile(objFilename, modelTriangles, vertices, tm, scaleFactor);
        numTriangles = modelTriangles.size();
        vm = view;
        consts = constants;
    }
    bool update();
    void save(string filename);
};

void Session::draw() {
    fill(window, Colour(0, 0, 0));
    switch (vm) {
        case POINT_CLOUD:
            drawPointCloud(window, vertices, cameraPosition, cameraOrientation, focalLength);
            break;
        case WIREFRAME:
            drawWireFrame(window, modelTriangles, cameraPosition, cameraOrientation, focalLength, debug);
            break;
        case RASTERISED:
            if (singleTriangle) {
                drawWireFrame(window, modelTriangles, cameraPosition, cameraOrientation, focalLength, false);
                drawRasterisedRender(window, modelTriangles[currentTriangle], depthMap, tm,
                                     cameraPosition, cameraOrientation, focalLength, debug);
            }
            else drawRasterisedRender(window, modelTriangles, depthMap, tm,
                                      cameraPosition, cameraOrientation, focalLength, debug);
//            drawLightSource(window, lightPosition, depthMap, cameraPosition, cameraOrientationRay, focalLength);
            break;
        case RAY_TRACED:
            if (singleTriangle) {
                drawWireFrame(window, modelTriangles, cameraPosition, cameraOrientation, focalLength, false);
                drawRayTracedRender(window, modelTriangles[currentTriangle],
                                    cameraPosition, cameraOrientationRay, focalLength,
                                    lightPosition, shadow, lightModes, shading, debug, consts);
            }
            else drawRayTracedRender(window, modelTriangles, cameraPosition, cameraOrientationRay, focalLength,
                                     lightPosition, shadow, lightModes, shading, debug, consts);
//            drawLightSource(window, lightPosition, depthMap, cameraPosition, cameraOrientationRay, focalLength);
            break;
    }
}

void Session::changeCurrentTriangle(int offset) {
    currentTriangle += offset;
    if (currentTriangle < 0) currentTriangle = numTriangles - 1;
    else if (currentTriangle >= numTriangles) currentTriangle = 0;
}

void Session::handleEvent() {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            panOrTilt(cameraOrientation, 1, -ROT_OFFSET);
            panOrTilt(cameraOrientationRay, 1, ROT_OFFSET);
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            panOrTilt(cameraOrientation, 1, ROT_OFFSET);
            panOrTilt(cameraOrientationRay, 1, -ROT_OFFSET);
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            panOrTilt(cameraOrientation, 0, ROT_OFFSET);
            panOrTilt(cameraOrientationRay, 0, -ROT_OFFSET);
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            panOrTilt(cameraOrientation, 0, -ROT_OFFSET);
            panOrTilt(cameraOrientationRay, 0, ROT_OFFSET);
        }
        else if (event.key.keysym.sym == SDLK_q) translate(cameraPosition, 2, TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_w) translate(cameraPosition, 1, -TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_e) translate(cameraPosition, 2, -TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_a) translate(cameraPosition, 0, -TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_s) translate(cameraPosition, 1, TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_d) translate(cameraPosition, 0, TRAN_OFFSET);
//        else if (event.key.keysym.sym == SDLK_u) rotate(cameraPosition, ROT_UP);
//        else if (event.key.keysym.sym == SDLK_h) rotate(cameraPosition, ROT_LEFT);
//        else if (event.key.keysym.sym == SDLK_j) rotate(cameraPosition, ROT_DOWN);
//        else if (event.key.keysym.sym == SDLK_k) rotate(cameraPosition, ROT_RIGHT);
        else if (event.key.keysym.sym == SDLK_u) translate(lightPosition, 1, TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_h) translate(lightPosition, 0, TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_j) translate(lightPosition, 1, -TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_k) translate(lightPosition, 0, -TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_y) translate(lightPosition, 2, -TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_i) translate(lightPosition, 2, TRAN_OFFSET);
        else if (event.key.keysym.sym == SDLK_TAB) changeCurrentTriangle(1);
        else if (event.key.keysym.sym == SDLK_BACKSPACE) changeCurrentTriangle(-1);
        else if (event.key.keysym.sym == SDLK_1) vm = POINT_CLOUD;
        else if (event.key.keysym.sym == SDLK_2) vm = WIREFRAME;
        else if (event.key.keysym.sym == SDLK_3) vm = RASTERISED;
        else if (event.key.keysym.sym == SDLK_4) vm = RAY_TRACED;
        else if (event.key.keysym.sym == SDLK_5) shadow = (shadow + 1) % 3;
        else if (event.key.keysym.sym == SDLK_6) lightModes[PROXIMITY] = not lightModes[PROXIMITY];
        else if (event.key.keysym.sym == SDLK_7) lightModes[INCIDENCE] = not lightModes[INCIDENCE];
        else if (event.key.keysym.sym == SDLK_8) lightModes[SPECULAR] = not lightModes[SPECULAR];
        else if (event.key.keysym.sym == SDLK_9) lightModes[AMBIENT] = not lightModes[AMBIENT];
        else if (event.key.keysym.sym == SDLK_0) shading = (shading + 1) % 3;
        else if (event.key.keysym.sym == SDLK_COMMA) debug = true;
        else if (event.key.keysym.sym == SDLK_PERIOD) animation = not animation;
        else if (event.key.keysym.sym == SDLK_SLASH) singleTriangle = not singleTriangle;
        else if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

void Session::displayTitle() {
    string shadowStr;
    switch (shadow) {
        case NO_SHADOW:
            shadowStr = "NONE";
            break;
        case HARD_SHADOW:
            shadowStr = "HARD";
            break;
        case SOFT_SHADOW:
            shadowStr = "SOFT";
            break;
    }

    string prox = (lightModes[0]) ? "1" : "0";
    string inci = (lightModes[1]) ? "1" : "0";
    string spec = (lightModes[2]) ? "1" : "0";
    string ambi = (lightModes[3]) ? "1" : "0";

    string shadeStr;
    switch (shading) {
        case FLAT:
            shadeStr = "FLAT";
            break;
        case GOURAUD:
            shadeStr = "GOUR";
            break;
        case PHONG:
            shadeStr = "PHON";
            break;
    }

    string title = "SHDO(" + shadowStr + "), PROX(" + prox + "), INCI(" + inci +
                   "), SPEC(" + spec + "), AMBI(" + ambi + "), SHDE(" + shadeStr + ")";
    window.setTitle(title.c_str());
}

bool Session::update() {
    // reset the depth map
    memset(depthMap, 0, sizeof(depthMap));
    if (window.pollForInputEvents(event)) handleEvent();
    draw();

    if (debug) {
        cout << "camera position:" << endl;
        cout << to_string(cameraPosition) << endl;
        cout << "camera orientation" << endl;
        cout << to_string(cameraOrientation) << endl;
        cout << "light position" << endl;
        cout << to_string(lightPosition) << endl;
    }

    displayTitle();
    window.renderFrame();
    debug = false;
    return running;
}

void Session::save(string filename) {
    window.savePPM(filename);
}

string paddedNum(int num) {
    if (num > 99) return to_string(num);
    else if (num > 9) return "0" + to_string(num);
    else return "00" + to_string(num);
}

int animateWireframe(Session session, int imageNum) {
    // 1. initial setup
    session.cameraPosition = glm::vec3(0, 0, 6);
    panOrTilt(session.cameraOrientation, 0, M_PI * 0.1);
    session.save("output/animation/wireframe/00" + paddedNum(imageNum) + ".ppm");
    imageNum++;

    // 2. pan down to see model
    for (float i=0; i < 15; i++) {
        float theta = 0.1f / 15;
        panOrTilt(session.cameraOrientation, 0, M_PI * -theta);
        session.update();
        if (imageNum % 10)
        session.save("output/animation/wireframe/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

    // 3. move from position 0 to 1
    glm::vec3 cp0 = session.cameraPosition;
    glm::mat3 co0 = session.cameraOrientation;

    glm::vec3 cp1 = glm::vec3(1.5, 0, 4.5);
    glm::mat3 co1 = glm::mat3(0.976, 0, -0.218,
                              0, 1, 0,
                              0.218, 0, 0.976);

    glm::vec3 cpStep = (cp1 - cp0) / 15;
    glm::mat3 coStep = (co1 - co0) / 15;

    for (int i=0; i < 15; i++) {
        session.cameraPosition += cpStep;
        session.cameraOrientation += coStep;
        session.update();
        session.save("output/animation/wireframe/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

    // 4. move from position 1 to 2
    glm::vec3 cp2 = glm::vec3(-1.5, 0, 3);
    glm::mat3 co2 = glm::mat3(0.917, 0, 0.399,
                              0, 1, 0,
                              -0.399, 0, 0.917);

    cpStep = (cp2 - cp1) / 15;
    coStep = (co2 - co1) / 15;

    for (int i=0; i < 15; i++) {
        session.cameraPosition += cpStep;
        session.cameraOrientation += coStep;
        session.update();
        session.save("output/animation/wireframe/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

    // 5. move from position 2 to 3
    glm::vec3 cp3 = glm::vec3(0.5, 0, 1.5);
    glm::mat3 co3 = glm::mat3(0.949, -0.005, -0.314,
                              -0.033, 0.993, -0.115,
                              0.312, 0.12, 0.942);

    cpStep = (cp3 - cp2) / 15;
    coStep = (co3 - co2) / 15;

    for (int i=0; i < 15; i++) {
        session.cameraPosition += cpStep;
        session.cameraOrientation += coStep;
        session.update();
        session.save("output/animation/wireframe/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

    // 6. pan down, away from model
    for (float i=0; i < 15; i++) {
        float theta = 0.2f / 15;
        panOrTilt(session.cameraOrientation, 0, M_PI * theta);
        session.update();
        session.save("output/animation/wireframe/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

//    // hand over control to the user
//    bool running = true;
//    while (running) {
//        running = session.update();
//    }
    return imageNum;
}

int animateRasterised(Session session, int imageNum) {
    // 1. initial setup
    session.cameraPosition = glm::vec3(0, 0, 4);
    panOrTilt(session.cameraOrientation, 0, M_PI * -0.1);
    session.save("output/animation/rasterised/" + paddedNum(imageNum) + ".ppm");
    imageNum++;

    // 2. pan down to see model
    for (float i=0; i < 15; i++) {
        float theta = 0.1f / 15;
        panOrTilt(session.cameraOrientation, 0, M_PI * theta);
        session.update();
        session.save("output/animation/rasterised/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

    // 3. small rotation in one direction
    float theta = 0.0f;
    for (int i=0; i < 4; i++) {
        orbit(session.cameraPosition, session.cameraOrientation, session.cameraOrientationRay, 1, theta);
        session.update();
        session.save("output/animation/rasterised/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
        theta += 0.03;
    }
    for (int i=0; i < 4; i++) {
        orbit(session.cameraPosition, session.cameraOrientation, session.cameraOrientationRay, 1, theta);
        session.update();
        session.save("output/animation/rasterised/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
        theta -= 0.03;
    }

    // 4. big rotation in other direction
    for (int i=0; i < 29; i++) {
        orbit(session.cameraPosition, session.cameraOrientation, session.cameraOrientationRay, 1, theta);
        session.update();
        session.save("output/animation/rasterised/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
        if (theta > -0.45) theta -= 0.05;
    }
    for (int i=0; i < 30; i++) {
        orbit(session.cameraPosition, session.cameraOrientation, session.cameraOrientationRay, 1, theta);
        session.update();
        session.save("output/animation/rasterised/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
        if (theta < -0.1) theta += 0.1;
    }

    // 5. pan down away from model
    for (float i=0; i < 15; i++) {
        float theta = 0.2f / 15;
        panOrTilt(session.cameraOrientation, 0, M_PI * theta);
        session.update();
        session.save("output/animation/rasterised/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

//    // hand over control to the user
//    bool running = true;
//    while (running) {
//        running = session.update();
//    }
    return imageNum;
}

void animateRayTraced(Session session) {
    // 1. initial setup
    session.cameraPosition = glm::vec3(0, 0, 3.5);
    session.lightPosition = glm::vec3(0, 0.4, 0.1);
    session.lightModes[PROXIMITY] = true;
    session.lightModes[AMBIENT] = true;
    panOrTilt(session.cameraOrientationRay, 0, M_PI * -0.1);
    int imageNum = 0;
    session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
    imageNum++;

    // 2. pan down to see model
    for (float i=0; i < 15; i++) {
        float theta = 0.1f / 15;
        panOrTilt(session.cameraOrientationRay, 0, M_PI * theta);
        session.update();
        session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
        imageNum++;
    }

    // 3. push in and rotate the light round with proximity lighting
    for (int i=0; i < 10; i++) {
        translate(session.cameraPosition, 2, -0.01);
        rotate(session.lightPosition, 1, M_PI / 5);
        session.update();
        session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
        imageNum++;
    }

    // 4. push in and rotate the light round with incident lighting
    session.lightModes[INCIDENCE] = true;
    for (int i=0; i < 10; i++) {
        translate(session.cameraPosition, 2, -0.01);
        rotate(session.lightPosition, 1, M_PI / 5);
        session.update();
        session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
        imageNum++;
    }

    // 5. push in and rotate the light round with specular lighting
    session.lightModes[SPECULAR] = true;
    for (int i=0; i < 10; i++) {
        translate(session.cameraPosition, 2, -0.01);
        rotate(session.lightPosition, 1, M_PI / 5);
        session.update();
        session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
        imageNum++;
    }

    // 6. push in and rotate the light round with hard shadows
    session.shadow = HARD_SHADOW;
    for (int i=0; i < 10; i++) {
        translate(session.cameraPosition, 2, -0.01);
        rotate(session.lightPosition, 1, M_PI / 5);
        session.update();
        session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
        imageNum++;
    }

    // 7. push in and rotate the light round with soft shadows
    session.shadow = SOFT_SHADOW;
    for (int i=0; i < 10; i++) {
        translate(session.cameraPosition, 2, -0.01);
        rotate(session.lightPosition, 1, M_PI / 5);
        session.update();
        session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
        imageNum++;
    }

    // 8. pan down away from the model
    for (float i=0; i < 15; i++) {
        float theta = 0.2f / 15;
        panOrTilt(session.cameraOrientationRay, 0, M_PI * theta);
        session.update();
        session.save("output/animation/raytraced/" + to_string(imageNum) + ".jpg");
        imageNum++;
    }

//    // hand over control to the user
//    bool running = true;
//    while (running) {
//        running = session.update();
//    }
}

void animateSphere(Session session, int imageNum) {
    // 1. initial setup
    session.cameraPosition = glm::vec3(0, 0, 1.62);
    session.lightPosition = glm::vec3(0, 0.02, 0.15);
    session.lightModes[PROXIMITY] = true;
    session.lightModes[INCIDENCE] = true;
    session.shading = GOURAUD;
    session.save("output/animation/sphere/" + paddedNum(imageNum) + ".ppm");
    imageNum++;

    // 2. translate light forward and back with gouraud shading
    for (int i=0; i < 10; i++) {
        translate(session.lightPosition, 2, 0.01);
        session.update();
        session.save("output/animation/sphere/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }
    for (int i=0; i < 10; i++) {
        translate(session.lightPosition, 2, -0.01);
        session.update();
        session.save("output/animation/sphere/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }

    // 3. translate light forward and back with phong shading
    session.shading = PHONG;
    for (int i=0; i < 10; i++) {
        translate(session.lightPosition, 2, 0.01);
        session.update();
        session.save("output/animation/sphere/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }
    for (int i=0; i < 10; i++) {
        translate(session.lightPosition, 2, -0.01);
        session.update();
        session.save("output/animation/sphere/" + paddedNum(imageNum) + ".ppm");
        imageNum++;
    }
}

int test() {
//    Session session("models/textured-cornell-box.obj");
//    Session session("models/new-sphere.obj");
//    Session session("models/sphere2.obj");


    // INTENSITY test
//    for (int i=1; i < 20; i++) {
//        session.consts.INTENSITY_FACTOR = i * 0.05f;
//        session.update();
//        string filename = "output/Int=" + to_string(session.consts.INTENSITY_FACTOR) + ".ppm";
//        session.save(filename);
//        cout << "completed " << i << endl;
//    }

    // Light Position test
//    for (int i=0; i < 15; i++) {
//        session.lightPosition[2] = 0.15 + 0.01 * i;
//        bool running = session.update();
//        string filename = "output/LP=" + to_string(session.lightPosition[2]) + ".ppm";
//        session.save(filename);
//        cout << "completed " << i << endl;
//        if (!running) return 0;
//    }

//    // Light Rotation test
//    for (int i=0; i < 20; i++) {
//        rotate(session.lightPosition, 1, 0.1 * M_PI);
//        bool running = session.update();
//        string filename = "output/LR=" + to_string(i) + ".ppm";
//        session.save(filename);
//        cout << "completed " << i << endl;
//        if (!running) return 0;
//    }

    // SPECULAR test
//    for (int i=1; i < 10; i++) {
//        session.consts.SPECULAR_POWER = i * 5;
//        bool running = session.update();
//        string filename = "output/SPEC=" + to_string(session.consts.SPECULAR_POWER) + ".ppm";
//        session.save(filename);
//        cout << "completed " << i << endl;
//        if (!running) return 0;
//    }
//    for (int i=10; i < 25; i++) {
//        session.consts.SPECULAR_POWER = i * 10;
//        bool running = session.update();
//        string filename = "output/SPEC=" + to_string(session.consts.SPECULAR_POWER) + ".ppm";
//        session.save(filename);
//        cout << "completed " << i << endl;
//        if (!running) return 0;
//    }
//
//    // AMBIENT test
//    for (int i=1; i < 20; i++) {
//        session.consts.AMBIENT_THRESHOLD = i * 0.05;
//        bool running = session.update();
//        string filename = "output/AMBI=" + to_string(session.consts.AMBIENT_THRESHOLD) + ".ppm";
//        session.save(filename);
//        cout << "completed " << i << endl;
//        if (!running) return 0;
//    }
//
//    // SPACING test
//    float spaces[] = {0.001, 0.005, 0.01, 0.05, 0.1, 0.5};
//    for (int i=0; i < 6; i++) {
//        session.consts.LIGHT_SPACING = spaces[i];
//        bool running = session.update();
//        string filename = "output/SPACE=" + to_string(session.consts.LIGHT_SPACING) + ".ppm";
//        session.save(filename);
//        cout << "completed " << i << endl;
//        if (!running) return 0;
//    }
    return 0;
}

int main(int argc, char *argv[]) {
//    Session session("models/textured-cornell-box.obj", WIREFRAME, cs);
//    Session session("models/new-sphere.obj");
//    Session session("models/sphere2.obj");

    Constants cs;
//    Session sessionWireframe("models/textured-cornell-box.obj", WIREFRAME, cs);
//    int imageNum = animateWireframe(sessionWireframe, 0);
//
    Session sessionRasterised("models/textured-cornell-box.obj", RASTERISED, cs);
//    imageNum = animateRasterised(sessionRasterised, imageNum);
//
//    Constants cs;
//    cs.LIGHT_SPACING = 0.05;
//    cs.INTENSITY_FACTOR = 0.15;
//    cs.SPECULAR_POWER = 45;
//    cs.AMBIENT_THRESHOLD = 0.15;
//    Session sessionCornell("models/textured-cornell-box.obj", RAY_TRACED, cs);
//    animateRayTraced(sessionCornell);
//
//    Constants cs;
//    cs.LIGHT_SPACING = 0.05;
//    cs.INTENSITY_FACTOR = 0.01;
//    cs.SPECULAR_POWER = 45;
//    cs.AMBIENT_THRESHOLD = 0.15;
//    Session sessionSphere("models/sphere2.obj", RAY_TRACED, cs);
//    animateSphere(sessionSphere, 250);


    bool running = true;
    while (running) {
        running = sessionRasterised.update();
    }
    return 0;
}


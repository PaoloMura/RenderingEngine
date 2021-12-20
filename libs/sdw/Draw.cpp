#include "Draw.h"

using namespace std;

void setPixel(DrawingWindow &window, CanvasPoint point, Colour colour) {
    int x = point.x;
    int y = point.y;
    int red = colour.red;
    int green = colour.green;
    int blue = colour.blue;
    uint32_t byteColour = (255 << 24) + (red << 16) + (green << 8) + blue;
    if (x >= 0 and x < WIDTH and y >= 0 and y < HEIGHT) {
        window.setPixelColour(x, y, byteColour);
    }
}

void setPixel(DrawingWindow &window, CanvasPoint point, Colour colour, bool debug) {
//    if (debug) cout << "set pixel" << endl;
    int x = point.x;
    int y = point.y;
    int red = colour.red;
    int green = colour.green;
    int blue = colour.blue;
    uint32_t byteColour = (255 << 24) + (red << 16) + (green << 8) + blue;
    if (x >= 0 and x < WIDTH and y >= 0 and y < HEIGHT) {
        window.setPixelColour(x, y, byteColour);
    }
}

void fill(DrawingWindow &window, Colour colour) {
    window.clearPixels();
    for (int j=0; j < HEIGHT; j++) {
        for (int i=0; i < WIDTH; i++)
            setPixel(window, CanvasPoint(i, j), colour);
    }
}

void drawPoint(DrawingWindow &window, float depthMap[][WIDTH], CanvasPoint point, Colour colour) {
    int x = point.x;
    int y = point.y;
    if (x >= 0 and x < WIDTH and y >= 0 and y < HEIGHT) {
        if (point.depth > depthMap[y][x]) {
            setPixel(window, point, colour);
            depthMap[y][x] = point.depth;
        }
    }
}

void drawTexturePoint(DrawingWindow &window, float depthMap[][WIDTH], CanvasPoint point, TextureMap &tm) {
    int x = point.x;
    int y = point.y;
    if (x >= 0 and x < WIDTH and y >= 0 and y < HEIGHT) {
        if (point.depth > depthMap[y][x]) {
            int pixel = (int) (point.texturePoint.y * tm.width + point.texturePoint.x);
            if (pixel < tm.pixels.size()) {
                uint32_t colour = tm.pixels[pixel];
                window.setPixelColour(x, y, colour);
                depthMap[y][x] = point.depth;
            }
        }
    }
}

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;
    for (int i = 0; i < numberOfSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = round(from.y + (yStepSize * i));
        if (i == 0) {
            x = floor(x);
        } else if (i == numberOfSteps - 1) {
            x = floor(x);
        } else {
            x = floor(x);
        }
        CanvasPoint point = CanvasPoint(x, y);
        setPixel(window, point, colour);
    }
}

void drawLine(DrawingWindow &window, float depthMap[][WIDTH], CanvasPoint from, CanvasPoint to, Colour colour, bool debug) {
    // if from and to have the same x-y coordinates, draw a point at whichever one has the closest depth
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float dDiff = to.depth - from.depth;
    if (xDiff == 0 and yDiff == 0) {
        if (from.depth >= to.depth) drawPoint(window, depthMap, from, colour);
        else drawPoint(window, depthMap, to, colour);
    }
    else {
        // interpolate between the two points
        float steps = max(abs(xDiff), abs(yDiff));
        float xStep = xDiff / steps;
        float yStep = yDiff / steps;
        float dStep = dDiff / steps;
        for (int i = 0; i < steps; i++) {
            // round to avoid gaps at the end of rakes
            float x = from.x + (xStep * i);
            float x2 = ceil(x);
            x = floor(x);
            float y = round(from.y + (yStep * i));
            float d = from.depth + (dStep * i);
            drawPoint(window, depthMap, CanvasPoint(x, y, d), colour);
            if (i == steps - 1) {
                drawPoint(window, depthMap, CanvasPoint(x2, y, d), colour);
            }
        }
    }
}

void drawTextureRake(DrawingWindow &window, float depthMap[][WIDTH], CanvasPoint from, CanvasPoint to,
                     TextureMap &tm, float leftTextX, float rightTextX, float leftTextY, float rightTextY, bool debug) {
    // if from and to have the same x-y coordinates, draw a point at whichever one has the closest depth
    float xDiff = to.x - from.x;
    float dDiff = to.depth - from.depth;
    if (xDiff == 0) {
        if (from.depth >= to.depth) {
            CanvasPoint point = CanvasPoint(from.x, from.y, from.depth);
            point.texturePoint = TexturePoint(leftTextX, leftTextY);
            drawTexturePoint(window, depthMap, from, tm);
        }
        else {
            CanvasPoint point = CanvasPoint(to.x, to.y, to.depth);
            point.texturePoint = TexturePoint(rightTextX, rightTextY);
            drawTexturePoint(window, depthMap, to, tm);
        }
    }
    else {
        // interpolate between the two points
        float dStep = dDiff / xDiff;
        for (int i = 0; i < xDiff; i++) {
            // round to avoid gaps at the end of rakes
            float x = from.x + i;
            float x2 = ceil(x);
            x = floor(x);
            float y = round(from.y);
            float d = from.depth + (dStep * i);
            float proportion = (x - from.x) / xDiff;
            float textX = leftTextX + proportion * (rightTextX - leftTextX);
            float textX1 = round(textX);
            float textX2 = ceil(textX);
            float textY = round(leftTextY + proportion * (rightTextY - leftTextY));
            CanvasPoint point = CanvasPoint(x, y, d);
            point.texturePoint = TexturePoint(textX1, textY);
            drawTexturePoint(window, depthMap, point, tm);
            if (i == xDiff - 1) {
                point.x = x2;
                point.texturePoint = TexturePoint(textX2, textY);
                drawTexturePoint(window, depthMap, point, tm);
            }
        }
    }
}

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle ct, Colour colour) {
    CanvasPoint p0 = ct.vertices[0];
    CanvasPoint p1 = ct.vertices[1];
    CanvasPoint p2 = ct.vertices[2];

    drawLine(window, p0, p1, colour);
    drawLine(window, p1, p2, colour);
    drawLine(window, p2, p0, colour);
}

// TODO: remove this
void drawStrokedTriangles(DrawingWindow &window, std::vector<CanvasTriangle> triangles, std::vector<Colour> colours) {
    assert(triangles.size() == colours.size());
    for (int i=0; i < triangles.size(); i++)
        drawStrokedTriangle(window, triangles[i], colours[i]);
}

// create a line, updating umap with the points it contains
void insertRakes(std::unordered_map<int, std::vector<float>> &rakes, CanvasPoint from, CanvasPoint to, bool left, bool debug) {
//    if (debug) cout << "from: " << from << ", to: " << to << endl;
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float dDiff = to.depth - from.depth;
    float numberOfSteps = max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;
    float dStepSize = dDiff / numberOfSteps;
    for (int i=0; i < numberOfSteps; i++) {
        float xf = from.x + (xStepSize * i);
        float yf = from.y + (yStepSize * i);
        float d = from.depth + (dStepSize * i);
        int y = floor(yf);
        // add the key if it doesn't exist
        if (rakes.find(y) == rakes.end()) {
            rakes[y] = {xf, xf, d, d};
        } else {
            if (left and rakes[y][0] > xf) {
                rakes[y][0] = xf;
                rakes[y][2] = d;
            }
            else if (not left and rakes[y][1] < xf) {
                rakes[y][1] = xf;
                rakes[y][3] = d;
            }
        }
    }
}

void insertTextureRakes(std::unordered_map<int, std::vector<float>> &rakes, CanvasPoint from, CanvasPoint to,
                        int tmWidth, int tmHeight, bool left, bool debug) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float dDiff = to.depth - from.depth;
    float numberOfSteps = max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;
    float dStepSize = dDiff / numberOfSteps;
    for (int i=0; i < numberOfSteps; i++) {
        float xf = from.x + (xStepSize * i);
        float yf = from.y + (yStepSize * i);
        float d = from.depth + (dStepSize * i);
        int y = floor(yf);
        float proportion = (y - from.y) / yDiff;
        float fromTPX = fmod(from.texturePoint.x, 1.0f) * tmWidth;
        float fromTPY = fmod(from.texturePoint.y, 1.0f) * tmHeight;
        float toTPX = fmod(to.texturePoint.x, 1.0f) * tmWidth;
        float toTPY = fmod(to.texturePoint.y, 1.0f) * tmHeight;
        float textX = fromTPX + proportion * (toTPX - fromTPX);
        float textY = floor(fromTPY + proportion * (toTPY - fromTPY));
        // add the key if it doesn't exist
        if (rakes.find(y) == rakes.end()) {
            rakes[y] = {xf, xf, d, d, textX, textX, textY, textY};
        } else {
            if (left and rakes[y][0] > xf) {
                rakes[y][0] = xf;
                rakes[y][2] = d;
                rakes[y][4] = textX;
                rakes[y][6] = textY;
            }
            else if (not left and rakes[y][1] < xf) {
                rakes[y][1] = xf;
                rakes[y][3] = d;
                rakes[y][5] = textX;
                rakes[y][7] = textY;
            }
        }
    }
}

// returns true if mid is left of the diagonal between top and bot
float isLeft(CanvasPoint top, CanvasPoint bot, CanvasPoint mid) {
        float k = (mid.y - top.y) / (bot.y - top.y);
        float projectedX = top.x + k * (bot.x - top.x);
        return mid.x <= projectedX;
}

// reorders the triangle points, sets the bottom point and returns a hashmap of rakes
std::unordered_map<int, std::vector<float>> createTriangleMap(CanvasTriangle &ct, bool debug) {
    // sort the points by height then width
    std::sort(ct.vertices.begin(), ct.vertices.end(), compare);
    CanvasPoint top = ct.vertices[0];
    CanvasPoint mid = ct.vertices[1];
    CanvasPoint bot = ct.vertices[2];

//    if (debug) cout << "Sort order: " << top << '\t' << mid << '\t' << bot << endl;

    // create a hashmap of the form {key:row, values:[leftPos, rightPos, leftDepth, rightDepth]}
    std::unordered_map<int, std::vector<float>> rakes;

    // insert rake data into the hashmap according to whether the triangle is:
    // flat-topped
    if (top.y == mid.y) {
        insertRakes(rakes, top, bot, true, debug);
        insertRakes(rakes, mid, bot, false, debug);
    }
    // flat-bottomed
    else if (mid.y == bot.y) {
        insertRakes(rakes, top, mid, true, debug);
        insertRakes(rakes, top, bot, false, debug);
    }
    // the midpoint is left of the top-bottom diagonal
    else if (isLeft(top, bot, mid)) {
        insertRakes(rakes, top, mid, true, debug);
        insertRakes(rakes, mid, bot, true, debug);
        insertRakes(rakes, top, bot, false, debug);
    }
    // the midpoint is right of the top-bottom diagonal
    else {
        insertRakes(rakes, top, bot, true, debug);
        insertRakes(rakes, top, mid, false, debug);
        insertRakes(rakes, mid, bot, false, debug);
    }

    return rakes;
}

std::unordered_map<int, std::vector<float>> createTextureTriangleMap(CanvasTriangle &ct, int tmWidth, int tmHeight, bool debug) {
    // sort the points by height then width
    std::sort(ct.vertices.begin(), ct.vertices.end(), compare);
    CanvasPoint top = ct.vertices[0];
    CanvasPoint mid = ct.vertices[1];
    CanvasPoint bot = ct.vertices[2];

    // create a hashmap of the form:
    // {key:row, values:[leftPos, rightPos, leftDepth, rightDepth, leftTextX, rightTextX, leftTextY, rightTextY]}
    std::unordered_map<int, std::vector<float>> rakes;

    // insert rake data into the hashmap according to whether the triangle is:
        // flat-topped
    if (top.y == mid.y) {
        insertTextureRakes(rakes, top, bot, tmWidth, tmHeight, true, debug);
        insertTextureRakes(rakes, mid, bot, tmWidth, tmHeight, false, debug);
    }
        // flat-bottomed
    else if (mid.y == bot.y) {
        insertTextureRakes(rakes, top, mid, tmWidth, tmHeight, true, debug);
        insertTextureRakes(rakes, top, bot, tmWidth, tmHeight, false, debug);
    }
        // the midpoint is left of the top-bottom diagonal
    else if (isLeft(top, bot, mid)) {
        insertTextureRakes(rakes, top, mid, tmWidth, tmHeight, true, debug);
        insertTextureRakes(rakes, mid, bot, tmWidth, tmHeight, true, debug);
        insertTextureRakes(rakes, top, bot, tmWidth, tmHeight, false, debug);
    }
        // the midpoint is right of the top-bottom diagonal
    else {
        insertTextureRakes(rakes, top, bot, tmWidth, tmHeight, true, debug);
        insertTextureRakes(rakes, top, mid, tmWidth, tmHeight, false, debug);
        insertTextureRakes(rakes, mid, bot, tmWidth, tmHeight, false, debug);
    }

    return rakes;
}

void testCompare() {
    CanvasPoint p1 = CanvasPoint(3.0, 1.0, 0.5);
    CanvasPoint p2 = CanvasPoint(1.0, 3.0, 0.5);
    bool result = compare(p1, p2);
    assert(result == true);

    p1 = CanvasPoint(1.0, 3.0, 0.5);
    p2 = CanvasPoint(3.0, 1.0, 0.5);
    result = compare(p1, p2);
    assert(result == false);

    p1 = CanvasPoint(3.0, 3.0, 0.5);
    p2 = CanvasPoint(1.0, 3.0, 0.5);
    result = compare(p1, p2);
    assert(result == false);

    p1 = CanvasPoint(3.0, 3.0, 0.5);
    p2 = CanvasPoint(3.0, 3.0, 0.4);
    result = compare(p1, p2);
    assert(result == true);
}

void testSort() {
    for (int x1=1; x1 < 4; x1++) {
        for (int y1=1; y1 < 4; y1++) {
            for (int x2=1; x2 < 4; x2++) {
                for (int y2=1; y2 < 4; y2++) {
                    for (int x3=1; x3 < 4; x3++) {
                        for (int y3=1; y3 < 4; y3++) {
                            CanvasPoint p1 = CanvasPoint(x1, y1);
                            CanvasPoint p2 = CanvasPoint(x2, y2);
                            CanvasPoint p3 = CanvasPoint(x3, y3);
                            std::array<CanvasPoint, 3> vertices = {p1, p2, p3};
                            std::sort(vertices.begin(), vertices.end(), compare);
                            CanvasPoint top = vertices[0];
                            CanvasPoint mid = vertices[1];
                            CanvasPoint bot = vertices[2];
                            assert(top.y <= mid.y and top.y <= bot.y);
                            if (top.y == mid.y) assert(top.x <= mid.x);
                            assert(mid.y <= bot.y);
                            if (mid.y == bot.y) assert(mid.x <= bot.x);
                        }
                    }
                }
            }
        }
    }
    cout << "All tests pass" << endl;
}

void drawFilledTriangle(DrawingWindow &window, float depthMap[][WIDTH], CanvasTriangle ct, Colour colour, bool debug) {
    std::unordered_map<int, std::vector<float>> umap = createTriangleMap(ct, debug);

    // iterate through the map, drawing rows from the left value to the right value
    for (auto x : umap) {
        int row = x.first;
        float leftPos = x.second[0];
        float rightPos = x.second[1];
        float leftDepth = x.second[2];
        float rightDepth = x.second[3];
        drawLine(window, depthMap, CanvasPoint(leftPos, row, leftDepth), CanvasPoint(rightPos, row, rightDepth), colour, debug);
    }
}

void drawTextureMappedTriangle(DrawingWindow &window, float depthMap[][WIDTH], CanvasTriangle ct, TextureMap &tm, bool debug) {
    unordered_map<int, vector<float>> toMap = createTextureTriangleMap(ct, tm.width, tm.height, debug);
//    for (auto x : toMap) {
//        cout << x.first << '\t'
//        << x.second[0] << ',' << x.second[1] << '\t'
//        << x.second[4] << ',' << x.second[6] << '\t'
//        << x.second[5] << ',' << x.second[7] << endl;
//    }
//    if (debug) {
//        cout << ct.vertices[0].texturePoint << endl;
//        cout << ct.vertices[1].texturePoint << endl;
//        cout << ct.vertices[2].texturePoint << endl;
//    }
    for (auto x : toMap) {
        // extract the data for this row
        int row = x.first;
        float leftPos = x.second[0];
        float rightPos = x.second[1];
        float leftDepth = x.second[2];
        float rightDepth = x.second[3];
        float leftTextX = x.second[4];
        float rightTextX = x.second[5];
        float leftTextY = x.second[6];
        float rightTextY = x.second[7];
        drawTextureRake(window, depthMap, CanvasPoint(leftPos, row, leftDepth), CanvasPoint(rightPos, row, rightDepth),
                        tm, leftTextX, rightTextX, leftTextY, rightTextY, debug);
    }
}

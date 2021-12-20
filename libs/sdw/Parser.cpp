#include "Parser.h"

using namespace std;

void parseMTLFile(const char *filename, TextureMap &tm, unordered_map<string, array<int, 3>> &colourMap,
                  unordered_map<string, bool> &reflectionMap) {
    ifstream file;
    file.open(filename, ios::in);
    if (file.is_open()) {
        string line;
        string name;
        array<float, 3> kd;
        array<int, 3> colour;
        while (getline(file, line)) {
            vector<string> words = split(line, ' ');
            if (words[0] == "newmtl") {
                name = words[1];
                reflectionMap[name] = false;
            }
            else if (words[0] == "Kd") {
                for (int i=0; i < 3; i++) {
                    kd[i] = stof(words[i+1]);
                    colour[i] = round(kd[i] * 255);
                }
                colourMap[name] = colour;
            }
            else if (words[0] == "map_Kd") tm = TextureMap(words[1]);
            else if (words[0] == "illum") {
                if (words[1] == "3") reflectionMap[name] = true;
            }
            else colourMap[name] = colour;
        }
    }
}

void parseOBJFile(const char *filename, vector<ModelTriangle> &triangles, vector<glm::vec3> &vertices,
                  TextureMap &tm, array<float, 3> scaleFactor) {
    ifstream file;
    file.open(filename, ios::in);
    if (file.is_open()) {
        string line;
        string mtlFilename = "models/";
        string col;
        unordered_map<string, array<int, 3>> colourMap;
        unordered_map<string, bool> reflectionMap;
        vector<TexturePoint> texturePoints;
        vector<VertexNormal> vertexNormals;
        vector<array<int, 3>> triangleToVertices;
        while (getline(file, line)) {
            vector<string> words = split(line, ' ');
            if (words[0] == "mtllib") {
                mtlFilename.append(words[1]);
                parseMTLFile(mtlFilename.c_str(), tm, colourMap, reflectionMap);
            }
            else if (words[0] == "usemtl") col = words[1];
            else if (words[0] == "v") {
                glm::vec3 vertex;
                for (int i = 0; i < 3; i++) {
                    vertex[i] = stof(words[i + 1]) * scaleFactor[i];
                }
                VertexNormal vn;
                vn.normalSum = glm::vec3(0,0,0);
                vn.adjacentFaces = 0;
                vertexNormals.push_back(vn);
                vertices.push_back(vertex);
            }
            else if (words[0] == "vt") {
                float tpx = stof(words[1]);
                float tpy = stof(words[2]);
                TexturePoint tp = TexturePoint(tpx, tpy);
                texturePoints.push_back(tp);
            }
            else if (words[0] == "f") {
                ModelTriangle triangle;
                triangle.colour = Colour(col, colourMap[col][0], colourMap[col][1], colourMap[col][2]);
                triangle.reflective = reflectionMap[col];
                array<int, 3> vertexIndices;
                for (int i = 0; i < 3; i++) {
                    vector<string> indices = split(words[i+1], '/');
                    int vertexIndex = stoi(indices[0]);
                    vertexIndices[i] = vertexIndex - 1;
                    triangle.vertices[i] = vertices[vertexIndex - 1];
                    if (indices[1] == "") triangle.texturePoints[i] = TexturePoint(-1, -1);
                    else {
                        int textureIndex = stoi(indices[1]);
                        triangle.texturePoints[i] = texturePoints[textureIndex - 1];
                    }
                }
                glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
                glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
                triangle.normal = glm::normalize(NORMAL_DIRECTION * glm::cross(e0, e1));
                for (int i=0; i < 3; i++) {
                    int index = vertexIndices[i];
                    vertexNormals[index].normalSum += triangle.normal;
                    vertexNormals[index].adjacentFaces++;
                }
                triangleToVertices.push_back(vertexIndices);
                triangles.push_back(triangle);
            }
        }
        // calculate the normal for each vertex in each triangle
        for (int i=0; i < triangles.size(); i++) {
            for (int j=0; j < 3; j++) {
                int index = triangleToVertices[i][j];
                VertexNormal vn = vertexNormals[index];
                glm::vec3 normal = glm::normalize(vn.normalSum / (float) vn.adjacentFaces);
                triangles[i].vertexNormals[j] = normal;
            }
        }
    }
    file.close();
}

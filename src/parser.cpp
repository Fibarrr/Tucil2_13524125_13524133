#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include <sstream>
#include <cmath>
#include <algorithm>

std::map<int, int> nodesPerDepth;
std::map<int, int> skippedPerDepth;
int totalVoxels = 0;
int totalVertices = 0;
int totalFaces = 0;

//posisi vertex
struct vektor3d {
    float x, y, z;
    vektor3d(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    // inisialisasi x y z
    vektor3d operator-(const vektor3d& o) const { 
        return {
            x - o.x, y - o.y, z - o.z
        }; 
    }
    // cross product
    vektor3d cross(const vektor3d& o) const {
        return {
            y * o.z - z * o.y, 
            z * o.x - x * o.z, 
            x * o.y - y * o.x
        };
    }
    // dot operator
    float dotop(const vektor3d& o) const { 
        return x * o.x + y * o.y + z * o.z; 
    }
};

// segitiga hasil gabungan titik
struct triangle {
    vektor3d v0, v1, v2;
};

// pembatas
struct AABB {
    vektor3d min, max;
    vektor3d center() const {
        return {
            (min.x + max.x) / 2, 
            (min.y + max.y) / 2, 
            (min.z + max.z) / 2
        };
    }
};

struct nodeOctree {
    AABB bounds;
    std::vector<int> triangles;
    // subdivisi 8 child 
    nodeOctree* children[8];
    int depth;
    bool isSurface;
    // inisialisasi node 
    nodeOctree(AABB b, int d) : bounds(b), depth(d), isSurface(false) {
        for (int i = 0; i < 8; i++) children[i] = nullptr;
    }
};

bool parserindexface(const std::string& input, int& result) {
    if (input == "") return false;
    // cari index vertex
    size_t slashIndex = input.find('/');
    std::string numberText;

    //not found 
    if (slashIndex == std::string::npos) { 
        numberText = input;
    }

    else {
        numberText = input.substr(0, slashIndex);
    }

    if (numberText == "") return false;
    try {
        result = std::stoi(numberText);
        return true;
    } catch (const std::exception&) {
        return false;
    }

    return true;
}

//obj parser

bool objParser(const std::string& path, std::vector<vektor3d>& vertices, std::vector<triangle>& triangles) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error: tidak bisa membuka file " << path << "\n";
        return false;
    }

    std::string line;
    bool hasVertex = false, hasFace = false;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string token;
        ss >> token;

        // token v = vertex
        // token f = face
        if (token == "v") {
            float x, y, z;
            if (!(ss >> x >> y >> z)) {
                std::cerr << "Error: format vertex tidak valid" << line << "\n";
                return false;
            }
            // Cek tidak boleh ada extra koordinat
            std::string extra;
            if (ss >> extra) {
                std::cerr << "Error: vertex harus 3 koordinat (v x y z): " << line << "\n";
                return false;
            }
            vertices.push_back({x, y, z});
            hasVertex = true;

        } else if (token == "f") {
            int idx[3];
            // Baca 3 vertices
            for (int i = 0; i < 3; i++) {
                std::string part;
                if (!(ss >> part)) {
                    std::cerr << "Error: face harus 3 vertices (f i j k): " << line << "\n";
                    return false;
                }
                if (!parserindexface(part, idx[i])) {
                    std::cerr << "Error: format face tidak valid (harus: f i j k): " << line << "\n";
                    return false;
                }
                if (idx[i] < 1 || idx[i] > static_cast<int>(vertices.size())) {
                    std::cerr << "Error: indeks face di luar jangkauan: " << idx[i] << "\n";
                    return false;
                }
                idx[i]--;
            }
            // Cek tidak boleh ada extra vertices (hanya triangular)
            std::string extra;
            if (ss >> extra) {
                std::cerr << "Error: face harus 3 vertices (f i j k): " << line << "\n";
                return false;
            }
            triangles.push_back({vertices[idx[0]], vertices[idx[1]], vertices[idx[2]]});
            hasFace = true;
        }


    } 
    if (!hasVertex || !hasFace) {
        std::cerr << "Error: file .obj tidak valid (harus punya baris v dan f)\n";
        return false;
    }
    return true;

}

AABB getBoundBox(const std::vector<vektor3d>& vertices) {
    AABB box;
    box.min = vertices[0];
    box.max = vertices[0];

    for (const vektor3d& v : vertices) {
        box.min.x = std::min(box.min.x, v.x);
        box.min.y = std::min(box.min.y, v.y);
        box.min.z = std::min(box.min.z, v.z);

        box.max.x = std::max(box.max.x, v.x);
        box.max.y = std::max(box.max.y, v.y);
        box.max.z = std::max(box.max.z, v.z);
    }

    float dx = box.max.x - box.min.x;
    float dy = box.max.y - box.min.y;
    float dz = box.max.z - box.min.z;
    float maxsize = std::max(std::max(dx, dy), dz);

    box.max.x = box.min.x + maxsize;
    box.max.y = box.min.y + maxsize;
    box.max.z = box.min.z + maxsize;

    float padding = maxsize * 0.001f;

    box.min.x -= padding;
    box.min.y -= padding;
    box.min.z -= padding;

    box.max.x += padding;
    box.max.y += padding;
    box.max.z += padding;

    return box;
}

bool quickBoxCheck(const triangle& tri, const AABB& box) {
    float minX = std::min({tri.v0.x, tri.v1.x, tri.v2.x});
    float minY = std::min({tri.v0.y, tri.v1.y, tri.v2.y});
    float minZ = std::min({tri.v0.z, tri.v1.z, tri.v2.z});

    float maxX = std::max({tri.v0.x, tri.v1.x, tri.v2.x});
    float maxY = std::max({tri.v0.y, tri.v1.y, tri.v2.y});
    float maxZ = std::max({tri.v0.z, tri.v1.z, tri.v2.z});

    if (maxX < box.min.x || minX > box.max.x) {
        return false;
    }
    if (maxY < box.min.y || minY > box.max.y) {
        return false;
    }
    if (maxZ < box.min.z || minZ > box.max.z) {
        return false;
    }

    return true;
}

bool detailedCheck(const triangle& tri, const AABB& box) {
    vektor3d center = box.center();

    float hx = (box.max.x - box.min.x) * 0.5f;
    float hy = (box.max.y - box.min.y) * 0.5f;
    float hz = (box.max.z - box.min.z) * 0.5f;

    vektor3d a = tri.v0 - center;
    vektor3d b = tri.v1 - center;
    vektor3d c = tri.v2 - center;

    vektor3d ab = b - a;
    vektor3d ac = c - a;

    // cek overlap sumbu utama 
    if (std::max({a.x, b.x, c.x}) < -hx || std::min({a.x, b.x, c.x}) > hx) {
        return false;
    }
    if (std::max({a.y, b.y, c.y}) < -hy || std::min({a.y, b.y, c.y}) > hy) {
        return false;
    }
    if (std::max({a.z, b.z, c.z}) < -hz || std::min({a.z, b.z, c.z}) > hz) {
        return false;
    }

    // cek plane triangle
    vektor3d normal = ab.cross(ac);
    float r = hx * std::abs(normal.x) + hy * std::abs(normal.y) + hz * std::abs(normal.z);

    if (std::abs(normal.dotop(a)) > r) return false;


    return true;
}

bool isTriangleTouching(const triangle&tri, const AABB& box) {

    if (!quickBoxCheck(tri, box)) return false;
    // double check

    return detailedCheck(tri, box);

}

// bagi box menjadi 8
AABB splitBox(const AABB& parent, int childIndex) {
    vektor3d mid = parent.center();
    AABB result;

    //  range X
    if (childIndex & 1) {
        result.min.x = mid.x;
        result.max.x = parent.max.x;
    } else {
        result.min.x = parent.min.x;
        result.max.x = mid.x;
    }

    // Y
    if (childIndex & 2) {
        result.min.y = mid.y;
        result.max.y = parent.max.y;
    } else {
        result.min.y = parent.min.y;
        result.max.y = mid.y;
    }

    // Z
    if (childIndex & 4) {
        result.min.z = mid.z;
        result.max.z = parent.max.z;
    } else {
        result.min.z = parent.min.z;
        result.max.z = mid.z;
    }

    return result;
}

// octree rekursif
void buildTree(nodeOctree* node,
               const std::vector<triangle>& tris,
               int maxDepth) {

    nodesPerDepth[node->depth]++;

    if (node->triangles.empty()) {
        skippedPerDepth[node->depth]++;
        return;
    }

    if (node->depth >= maxDepth) {
        node->isSurface = true;
        totalVoxels++;
        return;
    }

    // Bagi jadi 8 bagian
    for (int i = 0; i < 8; i++) {
        AABB childBox = splitBox(node->bounds, i);
        nodeOctree* child = new nodeOctree(childBox, node->depth + 1);
        node->children[i] = child;

        for (int t : node->triangles) {
            if (isTriangleTouching(tris[t], childBox)) {
                child->triangles.push_back(t);
            }
        }

        // Rekursi ke child
        buildTree(child, tris, maxDepth);
    }
    
    node->triangles.clear();
}

// Traversal tree dan tulis voxel 
void writeVoxels(nodeOctree* node, std::ofstream& out, int& offset) {
    if (!node) return;

    // Kalau voxel
    if (node->isSurface) {
        vektor3d mn = node->bounds.min;
        vektor3d mx = node->bounds.max;

        std::array<vektor3d, 8> pts = {{
            {mn.x, mn.y, mn.z}, {mx.x, mn.y, mn.z},
            {mx.x, mx.y, mn.z}, {mn.x, mx.y, mn.z},
            {mn.x, mn.y, mx.z}, {mx.x, mn.y, mx.z},
            {mx.x, mx.y, mx.z}, {mn.x, mx.y, mx.z}
        }};

        for (auto& p : pts) {
            out << "v " << p.x << " " << p.y << " " << p.z << "\n";
        }

        int face[12][3] = {
            {1,2,3},{1,3,4},{5,7,6},{5,8,7},
            {1,4,8},{1,8,5},{2,6,7},{2,7,3},
            {1,5,6},{1,6,2},{4,3,7},{4,7,8}
        };

        for (auto& f : face) {
            out << "f "
                << offset + f[0] << " "
                << offset + f[1] << " "
                << offset + f[2] << "\n";
        }

        offset += 8;
        totalVertices += 8;
        totalFaces += 12;
        return;
    }

    for (auto child : node->children) {
        writeVoxels(child, out, offset);
    }
}

void freeOctree(nodeOctree* node) {
    if (!node) return;
    for (int i = 0; i < 8; i++) freeOctree(node->children[i]);
    delete node;
}


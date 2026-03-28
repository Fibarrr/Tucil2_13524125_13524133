#include "parser.cpp"

int main() {
    std::string inputPath;
    int maxDepth = 0;

    std::cout << "========== OCTREE VOXEL ==========\n\n";
    
    // Input path file
    std::cout << "Masukkan path file (contoh: test/cow.obj): ";
    std::getline(std::cin, inputPath);
    
    if (inputPath.empty()) {
        std::cerr << "Error: path file tidak boleh kosong\n";
        return 1;
    }

    // Input depth
    bool validDepth = false;
    while (!validDepth) {
        std::cout << "Masukkan depth: ";
        std::string depthInput;
        std::getline(std::cin, depthInput);
        
        try {
            maxDepth = std::stoi(depthInput);
            if (maxDepth >= 1) {
                validDepth = true;
            } else {
                std::cerr << "Error: depth harus minimal 1\n";
            }
        } catch (...) {
            std::cerr << "Error: depth harus berupa angka\n";
        }
    }
    
    std::cout << "\n";

    auto startTime = std::chrono::high_resolution_clock::now();

    std::vector<vektor3d> vertices;
    std::vector<triangle> triangles;

    std::cout << "Membaca file: " << inputPath << "\n";
    if (!objParser(inputPath, vertices, triangles)) return 1;

    std::cout << "Vertex input: " << vertices.size() << "\n";
    std::cout << "Triangle input: " << triangles.size() << "\n";

    AABB rootBounds = getBoundBox(vertices);
    nodeOctree* root = new nodeOctree(rootBounds, 0);

    for (int i = 0; i < static_cast<int>(triangles.size()); i++) {
        root->triangles.push_back(i);
    }

    buildTree(root, triangles, maxDepth);

    std::string outputPath = inputPath.substr(0, inputPath.rfind('.')) + "-voxelized.obj";
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "Error: tidak bisa membuat file output\n";
        freeOctree(root);
        return 1;
    }

    outFile << "# Voxelized OBJ - IF2211 Strategi Algoritma\n";
    outFile << "# Max depth: " << maxDepth << "\n";

    int vertexOffset = 0;
    writeVoxels(root, outFile, vertexOffset);
    outFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    double elapsed = duration.count() / 1000.0;

    std::cout << "\n========== HASIL =========="  << "\n";
    std::cout << "Jumlah voxel      : " << totalVoxels << "\n";
    std::cout << "Jumlah vertex     : " << totalVertices << "\n";
    std::cout << "Jumlah faces      : " << totalFaces << "\n";
    std::cout << "Kedalaman octree  : " << maxDepth << "\n";
    std::cout << "Waktu eksekusi    : " << elapsed << " detik\n";
    std::cout << "Path file output  : " << outputPath << "\n";

    std::cout << "\n--- Statistik node per kedalaman ---\n";
    for (const auto& [d, n] : nodesPerDepth) {
        std::cout << d << " : " << n << "\n";
    }

    std::cout << "\n--- Node yang tidak perlu ditelusuri per kedalaman ---\n";
    for (const auto& [d, n] : skippedPerDepth) {
        std::cout << d << " : " << n << "\n";
    }

    freeOctree(root);
    return 0;
}

#pragma once
#include <string>
#include "Shader.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
    Model() {}
    void loadModel(std::string path);
    void Draw(const Shader& shader);

private:
    std::string directory;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
#include "AssetManager.h"

std::map<std::string, Model> AssetManager::Models;

void AssetManager::LoadModel(std::string name, const char* path) {
    Model model;
    model.loadModel(path);
    Models[name] = model;
}

Model& AssetManager::GetModel(std::string name) {
    return Models[name];
}
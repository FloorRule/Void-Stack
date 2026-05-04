#pragma once
#include <map>
#include <string>
#include "Model.h"

class AssetManager {
public:
    static std::map<std::string, Model> Models;

    static void LoadModel(std::string name, const char* path);
    static Model& GetModel(std::string name);

    static unsigned int loadCubemap(std::vector<std::string> faces);
};
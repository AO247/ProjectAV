#pragma once
#include "Prefab.h"
#include <map>
#include <string>

class PrefabManager {
public:
    static PrefabManager& GetInstance();

    void RegisterPrefab(const std::string& name, const Prefab& prefab);
    const Prefab* GetPrefab(const std::string& name) const;

private:
    std::map<std::string, Prefab> prefabs;
};
#include "PrefabManager.h"


PrefabManager& PrefabManager::GetInstance() {
    static PrefabManager instance;
    return instance;
}

// Register a prefab
void PrefabManager::RegisterPrefab(const std::string& name, const Prefab& prefab) {
    prefabs[name] = prefab;
}

// Retrieve a prefab by name
const Prefab* PrefabManager::GetPrefab(const std::string& name) const {
    auto it = prefabs.find(name);
    if (it != prefabs.end()) {
        return &it->second;
    }
    return nullptr;
}
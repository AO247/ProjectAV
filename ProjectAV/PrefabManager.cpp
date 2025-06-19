#include "PrefabManager.h"

Window* PrefabManager::wind = nullptr;
Node* PrefabManager::root = nullptr;
Node* PrefabManager::player = nullptr;
Rgph::MainRenderGraph* PrefabManager::rg = nullptr;
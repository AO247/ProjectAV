#include "Prefab.h"

Prefab::Prefab(const std::string& modelPath) : modelPath(modelPath)
{

}

Prefab::Prefab()
{
}

Prefab::~Prefab()
{
}

const std::string& Prefab::GetModelPath() const
{
	return modelPath;
}

void Prefab::SetModelPath(const std::string& path)
{
	modelPath = path;
}


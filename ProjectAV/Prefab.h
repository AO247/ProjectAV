#pragma once
#include <string>
#include "Collider.h"

class Prefab
{
public:
	Prefab(const std::string& modelPath);
	Prefab();
	~Prefab();
	const std::string& GetModelPath() const;
	void SetModelPath(const std::string& path);

private:
	std::string modelPath;
};


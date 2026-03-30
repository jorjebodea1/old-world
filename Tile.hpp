#pragma once
#include "StaticEntity.hpp"
namespace gps {
	class Tile :public StaticEntity
	{
	public:
		Tile(Transform staticEntityTransform, int objectType, AABB aabb, std::filesystem::path modelFile);
		float getHeight(float worldx, float worldy) const;
		void storeHeight(std::filesystem::path pngPath);
	private:
		std::vector<float> heightmap;
		
	};
}
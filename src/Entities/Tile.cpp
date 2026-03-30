#include "Tile.hpp"

namespace gps {
	Tile::Tile(Transform staticEntityTransform, int objectType, AABB aabb, std::filesystem::path modelFile)
		:StaticEntity(staticEntityTransform, objectType, aabb, modelFile) {
	}
		//:StaticEntity(staticEntityTransform,objectType,aabb,modelFile) {}
	void Tile::storeHeight(std::filesystem::path pngPath)
	{
		int width, height, channels;
		unsigned short* image_data = stbi_load_16(pngPath.string().c_str(), &width, &height, &channels, 1);
		// force grayscale (1 channel)

		if (!image_data) {
			std::cerr << "Failed to load heightmap\n";
			return;
		}
		heightmap.resize(width * height);
		AABB tileAabb = getGlobalAABB();
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int idx = y * width + x;
				unsigned short p = image_data[idx];
				heightmap[idx] = p / 65535.f;  // normalize
				heightmap[idx] = heightmap[idx] * (355.86f +270.56f) -270.56f;
			}
		}
		stbi_image_free(image_data);
	}

	float Tile::getHeight(float worldx, float worldy) const{
		AABB tileAabb = getGlobalAABB();
		float px = (tileAabb.max.x - worldx)/(tileAabb.max.x-tileAabb.min.x)*1023.0f;
		float py = (tileAabb.max.z-worldy)/(tileAabb.max.z-tileAabb.min.z)*1023.0f;
		int x0 = (int)px;
		int y0 = (int)py;
		int mapWidth = 1024;
		int mapHeight = 1024;
		int x1 = std::min(x0 + 1, mapWidth - 1);
		int y1 = std::min(y0 + 1, mapHeight - 1);

		float dx = px - x0;
		float dy = py - y0;

		float h00 = heightmap[y0 * mapWidth + x0];
		float h10 = heightmap[y0 * mapWidth + x1];
		float h01 = heightmap[y1 * mapWidth + x0];
		float h11 = heightmap[y1 * mapWidth + x1];

		float h0 = h00 * (1 - dx) + h10 * dx;
		float h1 = h01 * (1 - dx) + h11 * dx;
		return h0 * (1 - dy) + h1 * dy;
	}
}
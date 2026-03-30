#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "Models/Model3DOBJ.hpp"
#include "Entities/StaticEntity.hpp"
#include "Entities/Tile.hpp"
#include <filesystem>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#define RAPIDJSON_NOMEMBERITERATORCLASS
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "Transform.hpp"
#include "Skybox/SkyBox.hpp"
namespace gps {
	class WorldLoader
	{
	public:
		WorldLoader(const WorldLoader& obj) = delete;
		static WorldLoader* getInstance();
		void initializeTiles(std::filesystem::path& basePath);
		void initSkybox();
		std::vector<std::unique_ptr<StaticEntity>> entities;
		std::vector<glm::vec3> lightsPosition;
		gps::SkyBox skyBox;
	private:
		
		void ParseCSV(const std::filesystem::path& csvPath,StaticEntity* parentEntity);
		AABB ParseJSONForAABB(std::filesystem::path& jsonPath,std::string Type,const Transform& globalTransform);
		static WorldLoader* instancePtr;
		WorldLoader() {}
	};
}
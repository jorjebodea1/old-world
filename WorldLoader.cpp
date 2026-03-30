#include "WorldLoader.hpp"
#define RAPIDJSON_NOMEMBERITERATORCLASS
namespace gps {
	const float MAX_SIZE = 51200 / 3;
	WorldLoader* gps::WorldLoader::instancePtr = nullptr;

	void WorldLoader::initializeTiles(std::filesystem::path& basePath) {
		Transform t;
		t.position = glm::vec3(0.0f, 0.0f, 0.0f);
		t.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		t.scale = 1.f;

		AABB aabb1;
		aabb1.min = glm::vec3(3733.33f, -270.56f, 1066.67f);
		aabb1.max = glm::vec3(4266.67f,339.01f,1600.f);
		std::unique_ptr<Tile> tile1 = std::make_unique<Tile>(t, 1, aabb1, basePath / "adt_24_29.obj");
		tile1->storeHeight(basePath / "heightmaps/heightmap_24_29.png");
		entities.emplace_back(std::move(tile1));

		AABB aabb2;
		aabb2.min = glm::vec3(3200.f, -270.56f, 1066.67f);
		aabb2.max = glm::vec3(3733.33f, 321.75f, 1600.f);
		std::unique_ptr<Tile> tile2 = std::make_unique<Tile>(t, 1, aabb2, basePath / "adt_25_29.obj");
		tile2->storeHeight(basePath / "heightmaps/heightmap_25_29.png");
		entities.emplace_back(std::move(tile2));
		

		AABB aabb3;
		aabb3.min = glm::vec3(3733.33f, 70.76f, 533.33f);
		aabb3.max = glm::vec3(4266.67f, 355.86f, 1066.67f);
		std::unique_ptr<Tile> tile3 = std::make_unique<Tile>(t, 1, aabb3,basePath / "adt_24_30.obj");
		tile3->storeHeight(basePath / "heightmaps/heightmap_24_30.png");
		entities.emplace_back(std::move(tile3));
		
		AABB aabb4;
		aabb4.min = glm::vec3(3200.f, 58.86f, 533.33f);
		aabb4.max = glm::vec3(3733.33f, 296.36f, 1066.67f);
		std::unique_ptr<Tile> tile4 = std::make_unique<Tile>(t, 1, aabb4, basePath / "adt_25_30.obj");
		tile4->storeHeight(basePath / "heightmaps/heightmap_25_30.png");
		entities.emplace_back(std::move(tile4));
		
	
		ParseCSV(basePath / "adt_24_29_ModelPlacementInformation.csv", entities[0].get());
		ParseCSV(basePath / "adt_25_29_ModelPlacementInformation.csv", entities[1].get());
		ParseCSV(basePath / "adt_24_30_ModelPlacementInformation.csv", entities[2].get());
		ParseCSV(basePath / "adt_25_30_ModelPlacementInformation.csv", entities[3].get());
	}
	void WorldLoader::initSkybox(){
		std::vector<const GLchar*> faces;
		faces.push_back("skybox/right.tga");
		faces.push_back("skybox/left.tga");
		faces.push_back("skybox/top.tga");
		faces.push_back("skybox/bottom.tga");
		faces.push_back("skybox/back.tga");
		faces.push_back("skybox/front.tga");
		skyBox.Load(faces);
	}
	void WorldLoader::ParseCSV(const std::filesystem::path& csvPath,StaticEntity* parentEntity) {
		std::ifstream file(csvPath.string());
		std::string line;
		std::getline(file, line);
		std::stringstream ssHeader(line);
		std::string colName;
		std::string importType;
		std::unordered_map<std::string, int> columnIndex;
		int idx = 0;
		while (std::getline(ssHeader, colName, ';')) {
			columnIndex[colName] = idx++;
		}
		if (columnIndex.count("Type") > 0) {
			importType = "ADT";
		}
		else importType = "WMO";

		parentEntity->setFirstChild(entities.size());
		int numberOfChilds = 0;
		std::vector<StaticEntity*> directChildren;
		std::vector<std::filesystem::path> directChildrenCSVPath;
		while (std::getline(file, line)) {
			numberOfChilds++;
			std::stringstream ss(line);
			std::string cell;
			std::vector<std::string> row;
			while (std::getline(ss, cell, ';')) {
				row.push_back(cell);
			}
			Transform localTransform;
			if (importType == "ADT") {
				//avem datele din fisier positie and tot felul
				localTransform.position = glm::vec3(
					MAX_SIZE - std::stof(row[columnIndex["PositionX"]]),
					std::stof(row[columnIndex["PositionY"]]),
					MAX_SIZE - std::stof(row[columnIndex["PositionZ"]])
				);
				localTransform.rotation = glm::vec3(
					std::stof(row[columnIndex["RotationX"]]),
					(90.f + std::stof(row[columnIndex["RotationY"]])),
					std::stof(row[columnIndex["RotationZ"]])
				);
				localTransform.scale = std::stof(row[columnIndex["ScaleFactor"]]);

			}
			else {
				localTransform.position = glm::vec3(
					std::stof(row[columnIndex["PositionX"]]),
					std::stof(row[columnIndex["PositionZ"]]),
					-std::stof(row[columnIndex["PositionY"]])
				);
				glm::quat Q(
					std::stof(row[columnIndex["RotationW"]]),
					std::stof(row[columnIndex["RotationX"]]),
					std::stof(row[columnIndex["RotationZ"]]),
					-std::stof(row[columnIndex["RotationY"]])
				);
				localTransform.scale = std::stof(row[columnIndex["ScaleFactor"]]);

				//calculate position,rotation,scale based on parent
				glm::quat parentQ = glm::quat(glm::radians(parentEntity->getRotation()));
				glm::quat worldQ = parentQ * Q;
				localTransform.rotation = glm::degrees(glm::eulerAngles(worldQ));
				localTransform.position = parentEntity->getPosition() + (parentQ * localTransform.position);
				localTransform.scale *= parentEntity->getScale();
			}

			//verificare daca avem deja modelul curent
			bool ok = true;
			std::filesystem::path modelFilePath = row[columnIndex["ModelFile"]];
			std::shared_ptr<Model3DOBJ> modelPtr;
			for (size_t i = 0; i < entities.size(); i++) {
				if (entities[i]->getModelFile() == modelFilePath.filename().string()) {
					modelPtr = entities[i]->getModel();
					break;
				}
			}

			//creare path-uri pt model si csv
			std::filesystem::path modelPath = (csvPath.parent_path() / modelFilePath);
			std::filesystem::path newCSVPath = modelPath.parent_path() / (modelPath.stem().string() + "_ModelPlacementInformation.csv");
			newCSVPath = std::filesystem::absolute(newCSVPath);

			AABB aabb = ParseJSONForAABB(modelPath.replace_extension(".json"), row[columnIndex["Type"]], localTransform);

			//adaugare copii directi si adaugare entitati in entities
			int objectType = 0;
			if (modelFilePath.filename().string().find("brazier") != std::string::npos) {
				objectType = 2;
			}
			if (modelFilePath.filename().string().find("tree") != std::string::npos
				|| modelFilePath.filename().string().find("grass") != std::string::npos) {
				objectType = 3;
			}
			if (modelFilePath.filename().string().find("lantern") != std::string::npos) {
				objectType = 4;
			}
			if (modelPtr==NULL) {
				entities.emplace_back(std::make_unique<StaticEntity>(localTransform, objectType, aabb,csvPath.parent_path()/modelFilePath));
			}
			else entities.emplace_back(std::make_unique<StaticEntity>(modelPtr,localTransform, objectType,aabb));
			directChildren.push_back(entities.back().get());
			directChildrenCSVPath.push_back(newCSVPath);
			if (objectType == 2) {
				glm::vec3 position = entities.back().get()->getPosition();
				position.y += 2.f;
				lightsPosition.push_back(position);
			}
			
		}
		parentEntity->setChildCount(numberOfChilds);


		//parcurgere recursiva
		for (int i = 0; i < directChildren.size(); i++) {
			if (std::filesystem::exists(directChildrenCSVPath[i])) {
				ParseCSV(directChildrenCSVPath[i], directChildren[i]);
			}
		}

	}
	AABB WorldLoader::ParseJSONForAABB(std::filesystem::path& jsonPath,std::string Type,const Transform& globalTransform) {
		jsonPath=std::filesystem::absolute(jsonPath);
		FILE* fp = fopen(jsonPath.string().c_str(), "r");
		
		char readBuffer[65536];
		rapidjson::FileReadStream is(fp, readBuffer,
			sizeof(readBuffer));
		
		rapidjson::Document d;
		d.ParseStream(is);

		fclose(fp);
		AABB localAABB;
		if (Type != "wmo") {
			rapidjson::GenericArray genericArrayMin = d["collisionBox"]["min"].GetArray();
			rapidjson::GenericArray genericArrayMax = d["collisionBox"]["max"].GetArray();
			localAABB.min = glm::vec3(
				genericArrayMin[0].GetFloat(),//x
				genericArrayMin[2].GetFloat(),//z
				-genericArrayMax[1].GetFloat()//-y
			);
			localAABB.max = glm::vec3(
				genericArrayMax[0].GetFloat(),
				genericArrayMax[2].GetFloat(),
				-genericArrayMin[1].GetFloat()
			);
		}
		else {
			rapidjson::GenericArray genericArrayMin = d["boundingBox1"].GetArray();
			rapidjson::GenericArray genericArrayMax = d["boundingBox2"].GetArray();
			localAABB.min = glm::vec3(
				genericArrayMin[0].GetFloat(),
				genericArrayMin[2].GetFloat(),
				-genericArrayMax[1].GetFloat()
			);
			localAABB.max = glm::vec3(
				genericArrayMax[0].GetFloat(),
				genericArrayMax[2].GetFloat(),
				-genericArrayMin[1].GetFloat()
			);
		}

		//transformare din spatiu local in global a AABB
		return localAABB.getGlobalAABB(globalTransform);
		
	}
	WorldLoader* WorldLoader::getInstance() {
		if (instancePtr == nullptr) {
			if (instancePtr == nullptr) {
				instancePtr = new WorldLoader();
			}
		}
		return instancePtr;
	}
}
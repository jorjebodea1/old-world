#ifndef Model3D_hpp
#define Model3D_hpp

#include "Meshes/Mesh.hpp"

#include "Utilities/tiny_obj_loader.h"
#include "Utilities/stb_image.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
namespace gps {

    class Model3DOBJ {

    public:
        ~Model3DOBJ();

		void LoadModel(std::filesystem::path path);
		void LoadModel(std::string fileName);

		void LoadModel(std::string fileName, std::string basePath);

		void Draw(gps::Shader shaderProgram);
		std::string getModelFile();
		std::vector<gps::Mesh> meshes;
    private:
		// Component meshes - group of objects
        
		// Associated textures
        std::vector<gps::Texture> loadedTextures;
		std::string modelFile;
		// Does the parsing of the .obj file and fills in the data structure
		void ReadOBJ(std::string fileName, std::string basePath);

		// Retrieves a texture associated with the object - by its name and type
		gps::Texture LoadTexture(std::string path, std::string type);

		// Reads the pixel data from an image file and loads it into the video memory
		GLuint ReadTextureFromFile(const char* file_name);
    };
}

#endif /* Model3D_hpp */

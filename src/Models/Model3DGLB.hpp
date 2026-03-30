#pragma once

#include "Meshes/SkinnedMesh.hpp"

#include "Utilities/stb_image.h"

#include <iostream>
#include <string>
#include <vector>

namespace gps {
	class Model3DGLB {

	public:
		~Model3DGLB();

		void LoadModel(cgltf_data* data);

		void Draw(gps::Shader shaderProgram) const;
		GLuint getBoneBuffer() const;
	private:
		// Component meshes - group of objects
		std::vector<gps::SkinnedMesh> meshes;
		// Associated textures
		std::vector<gps::Texture> loadedTextures;

		//Texture for sending over the bones matrices
		GLuint bonesBuffer = 0;
		GLuint bonesTexture = 0;
		void initBoneTexture();

		// Does the parsing of the .obj file and fills in the data structure
		void ReadGLB(cgltf_data* data);

		// Retrieves a texture associated with the object - by its name and type
		gps::Texture LoadTexture(cgltf_material *mat, std::string type);

		// Reads the pixel data from an image file and loads it into the video memory
		GLuint ReadTextureFromBuffer(unsigned char* buffer,size_t size);

	};
}
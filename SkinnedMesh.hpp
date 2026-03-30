#pragma once
#include "Mesh.hpp"
#include "cgltf.h"
namespace gps {
	struct BoneData {
		glm::ivec4 joints;
		glm::vec4 weights;
	};
	
	class SkinnedMesh :public Mesh {
		public:
			std::vector<BoneData> bones;
			SkinnedMesh(std::vector<Vertex> vertices,std::vector<BoneData> bones,std::vector<GLuint> indices, std::vector<Texture> textures);
			void Draw(gps::Shader shader) const override;
			GLuint getBoneVBO() const;
		private:
			void setupMesh() override;
			GLuint boneVBO;
	};
}
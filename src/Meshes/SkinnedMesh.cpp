#include "SkinnedMesh.hpp"
namespace gps {

	/* Mesh Constructor */
	SkinnedMesh::SkinnedMesh(std::vector<Vertex> vertices, std::vector<BoneData> bones, std::vector<GLuint> indices, std::vector<Texture> textures) 
		:Mesh(vertices,indices,textures)
	{
		this->bones = bones;
		this->setupMesh();
	}

	/* Mesh drawing function - also applies associated textures */
	void SkinnedMesh::Draw(gps::Shader shader) const{
		shader.useShaderProgram();
		//set textures
		for (GLuint i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glUniform1i(glGetUniformLocation(shader.shaderProgram, this->textures[i].type.c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}

		glBindVertexArray(this->getBuffers().VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		for (GLuint i = 0; i < this->textures.size(); i++) {

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

	}
	GLuint SkinnedMesh::getBoneVBO() const {
		return boneVBO;
	}
	// Initializes all the buffer objects/arrays
	void SkinnedMesh::setupMesh() {
		Mesh::setupMesh();

		glGenBuffers(1, &this->boneVBO);

		glBindVertexArray(this->buffers.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->boneVBO);
		glBufferData(GL_ARRAY_BUFFER, this->bones.size() * sizeof(BoneData), &this->bones[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(BoneData), (GLvoid*)offsetof(BoneData, joints));

		// Layout 4: Weights (floats)
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BoneData), (void*)offsetof(BoneData, weights));
	
		// 3. Unbind
		glBindVertexArray(0);
	}
}
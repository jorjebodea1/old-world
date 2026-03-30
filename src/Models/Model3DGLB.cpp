#include "Model3DGLB.hpp"

namespace gps {

	void Model3DGLB::LoadModel(cgltf_data* data) {
		ReadGLB(data);
		initBoneTexture();
	}

	// Draw each mesh from the model
	void Model3DGLB::Draw(gps::Shader shaderProgram) const {

		for (int i = 0; i < meshes.size(); i++) {
			meshes[i].Draw(shaderProgram);
		}

		//sending over the bone data
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_BUFFER, bonesTexture);
		glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "boneTexture"), 5);
	}
	void Model3DGLB::initBoneTexture() {
		glGenBuffers(1, &bonesBuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, bonesBuffer);
		glBufferData(GL_TEXTURE_BUFFER, 429 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

		glGenTextures(1, &bonesTexture);
		glBindTexture(GL_TEXTURE_BUFFER, bonesTexture);
		// Bind the buffer to the texture as RGBA32F (4 floats per pixel)
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, bonesBuffer);
	}

	GLuint Model3DGLB::getBoneBuffer() const {
		return bonesBuffer;
	}

	// Does the parsing of the .obj file and fills in the data structure
	void Model3DGLB::ReadGLB(cgltf_data* data) {
		std::cout << "# of shapes    : " << data->meshes_count << std::endl;
		std::cout << "# of materials : " << data->materials_count << std::endl;
		// Loop over shapes
		for (size_t s = 0; s < data->meshes_count; s++) {

			std::vector<gps::Vertex> vertices;
			std::vector<GLuint> indices;
			std::vector<gps::Texture> textures;
			std::vector<gps::BoneData> bones;

			cgltf_mesh* mesh = &data->meshes[s];
			
			// Loop over faces(polygon)
			size_t vertex_offset = 0;
			for (size_t f = 0; f < mesh->primitives_count; f++) {

				cgltf_primitive* prim = &mesh->primitives[f];

				cgltf_accessor* pos_accesor = NULL;
				cgltf_accessor* uv_accesor = NULL;
				cgltf_accessor* normal_accesor = NULL;
				cgltf_accessor* joints_accessor = NULL;
				cgltf_accessor* weights_accessor = NULL;
				cgltf_accessor* indices_accesor = prim->indices;

				for (int z = 0; z < prim->attributes_count; z++) {
					if (prim->attributes[z].type == cgltf_attribute_type_position) {
						pos_accesor = prim->attributes[z].data;
					}
					if (prim->attributes[z].type == cgltf_attribute_type_texcoord && prim->attributes[z].index == 0) {
						uv_accesor = prim->attributes[z].data;
					}
					if (prim->attributes[z].type == cgltf_attribute_type_normal && prim->attributes[z].index == 0) {
						normal_accesor = prim->attributes[z].data;
					}
					if (prim->attributes[z].type == cgltf_attribute_type_joints && prim->attributes[z].index == 0) {
						joints_accessor = prim->attributes[z].data;
					}
					if (prim->attributes[z].type == cgltf_attribute_type_weights && prim->attributes[z].index == 0) {
						weights_accessor = prim->attributes[z].data;
					}
				}

				for (size_t v = 0; v < pos_accesor->count; v++) {
					glm::vec3 vertexPosition = glm::vec3(0.0f);
					if (pos_accesor != NULL) {
						float positionPrimitive[3];
						cgltf_accessor_read_float(pos_accesor, v, positionPrimitive, 3);
						vertexPosition=glm::vec3(positionPrimitive[0], positionPrimitive[1], positionPrimitive[2]);
					}

					glm::vec3 vertexNormal = glm::vec3(0.0f);
					if (normal_accesor != NULL) {
						float normalPrimitive[3];
						cgltf_accessor_read_float(normal_accesor, v, normalPrimitive, 3);
						vertexNormal=glm::vec3(normalPrimitive[0], normalPrimitive[1], normalPrimitive[2]);
					}

					glm::vec2 vertexTexCoords = glm::vec2(0.0f);
					if (uv_accesor != NULL) {
						float uvPrimitive[2];
						cgltf_accessor_read_float(uv_accesor, v, uvPrimitive, 2);
						vertexTexCoords=glm::vec2(uvPrimitive[0], uvPrimitive[1]);
					}
					gps::Vertex currentVertex;
					currentVertex.Position = vertexPosition;
					currentVertex.Normal = vertexNormal;
					currentVertex.TexCoords = vertexTexCoords;

					vertices.push_back(currentVertex);
					glm::ivec4 boneJoints = glm::ivec4(0);
					if (joints_accessor != NULL) {
						unsigned int jointsPrimitive[4];
						cgltf_accessor_read_uint(joints_accessor, v, jointsPrimitive, 4);
						boneJoints = glm::ivec4(jointsPrimitive[0], jointsPrimitive[1], jointsPrimitive[2], jointsPrimitive[3]);
					}

					glm::vec4 boneWeights=glm::vec4(0.0f);
					if (weights_accessor != NULL) {
						float weightsPrimitive[4];
						cgltf_accessor_read_float(weights_accessor, v, weightsPrimitive, 4);
						boneWeights=glm::vec4(weightsPrimitive[0], weightsPrimitive[1], weightsPrimitive[2], weightsPrimitive[3]);
					}
					gps::BoneData currentBone;
					currentBone.joints = boneJoints;
					currentBone.weights = boneWeights;

					float sum = currentBone.weights[0] + currentBone.weights[1] + currentBone.weights[2] + currentBone.weights[3];
					if (sum > 0.0f) {
						for (int i = 0; i < 4; ++i)
							currentBone.weights[i] /= sum;
					}
					else {
						currentBone.weights[0] = 1.0f;
						currentBone.weights[1] = currentBone.weights[2] = currentBone.weights[3] = 0.0f;
					}

					bones.push_back(currentBone);
				}

				for (size_t i = 0; i < indices_accesor->count; i++) {
					uint32_t index = 0;
					cgltf_accessor_read_uint(indices_accesor, i, &index, 1);
					indices.push_back(index + vertex_offset);
				}
				vertex_offset += pos_accesor->count;

			}
			// get material id
			gps::Texture currentTexture;
			currentTexture = LoadTexture(mesh->primitives[0].material, "diffuseTexture");
			textures.push_back(currentTexture);
			meshes.push_back(gps::SkinnedMesh(vertices, bones, indices, textures));
		}
	}
	// Retrieves a texture associated with the object - by its name and type
	gps::Texture Model3DGLB::LoadTexture(cgltf_material *mat,std::string type) {

		for (int i = 0; i < loadedTextures.size(); i++) {

			if (loadedTextures[i].material == mat) {
				//already loaded texture
				return loadedTextures[i];
			}
		}

		gps::Texture currentTexture;
		cgltf_texture* tex = mat->pbr_metallic_roughness.base_color_texture.texture;
		cgltf_image* img = tex->image;
		cgltf_buffer_view* view = img->buffer_view;
		unsigned char* buffer = (unsigned char*)view->buffer->data + view->offset;
		size_t size = view->size;
		currentTexture.id = ReadTextureFromBuffer(buffer,size);
		currentTexture.type = std::string(type);
		currentTexture.material = mat;

		loadedTextures.push_back(currentTexture);

		return currentTexture;
	}

	// Reads the pixel data from an image file buffer and loads it into the video memory
	GLuint Model3DGLB::ReadTextureFromBuffer(unsigned char* buffer,size_t size) {
		int x, y, n;
		int force_channels = 4;
		unsigned char* image_data = stbi_load_from_memory(buffer,size, &x, &y, &n, force_channels);

		if (!image_data) {
			fprintf(stderr, "ERROR: could not load %s\n", buffer);
			return 0;
		}
		// NPOT check
		if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
			fprintf(stderr, "WARNING: texture %s is not power-of-2 dimensions\n", buffer);
		}

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA, //GL_SRGB,//GL_RGBA,
			x,
			y,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;

	}

	Model3DGLB::~Model3DGLB() {

		for (size_t i = 0; i < loadedTextures.size(); i++) {

			glDeleteTextures(1, &loadedTextures.at(i).id);
		}

		for (size_t i = 0; i < meshes.size(); i++) {

			GLuint vertexVBO = meshes.at(i).getBuffers().vertexVBO;
			GLuint boneVBO = meshes.at(i).getBoneVBO();
			GLuint EBO = meshes.at(i).getBuffers().EBO;
			GLuint VAO = meshes.at(i).getBuffers().VAO;
			glDeleteBuffers(1, &vertexVBO);
			glDeleteBuffers(1, &boneVBO);
			glDeleteBuffers(1, &EBO);
			glDeleteVertexArrays(1, &VAO);
		}
	}
}

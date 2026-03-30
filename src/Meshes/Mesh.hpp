#ifndef Mesh_hpp
#define Mesh_hpp

#if defined (__APPLE__)
    #include <OpenGL/gl3.h>
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <glm/glm.hpp>

#include "Utilities/Shader.hpp"
#include <string>
#include <vector>
struct cgltf_material;
namespace gps {
    struct Triangle {
        glm::vec3 v0, v1, v2, centroid;
    };
    struct Vertex {

        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    struct Texture {

        GLuint id;
        //ambientTexture, diffuseTexture, specularTexture
        std::string type;
        std::string path;
        cgltf_material* material;
    };

    struct Material {

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };
    struct Buffers {
            GLuint VAO;
            GLuint vertexVBO;
            GLuint EBO;
    };
    class Mesh {

    public:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

	    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

	    Buffers getBuffers() const;

	    virtual void Draw(gps::Shader shader) const;

    protected:
        
        /*  Render data  */
        Buffers buffers;

	    // Initializes all the buffer objects/arrays
	    virtual void setupMesh();

    };

}
#endif /* Mesh_hpp */

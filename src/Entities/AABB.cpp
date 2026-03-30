#include "AABB.hpp"

namespace gps {
	AABB::AABB(){
        this->min = glm::vec3(1e30f);
        this->max = glm::vec3(-1e30f);
    }
	AABB::AABB(glm::vec3 min, glm::vec3 max) {
		this->min = min;
		this->max = max;
	}
	const AABB AABB::getGlobalAABB(Transform transform) const{
        // 1. Compute local center and half-extents
        glm::vec3 localCenter = (min + max) * 0.5f;
        glm::vec3 localExtent = (max - min) * 0.5f;

        // 2. Apply uniform scale
        glm::vec3 scaledExtent = localExtent * transform.scale; // uniform scale

        // 3. Build rotation matrix
       glm::mat3 rot = glm::mat3(glm::eulerAngleXYZ(
            glm::radians(transform.rotation.x),
            glm::radians(transform.rotation.y),
            glm::radians(transform.rotation.z)
        ));

        // 4. Rotate the scaled extents to get world AABB extents
        glm::vec3 worldExtent;
        worldExtent.x = std::abs(rot[0][0]) * scaledExtent.x + std::abs(rot[0][1]) * scaledExtent.y + std::abs(rot[0][2]) * scaledExtent.z;
        worldExtent.y = std::abs(rot[1][0]) * scaledExtent.x + std::abs(rot[1][1]) * scaledExtent.y + std::abs(rot[1][2]) * scaledExtent.z;
        worldExtent.z = std::abs(rot[2][0]) * scaledExtent.x + std::abs(rot[2][1]) * scaledExtent.y + std::abs(rot[2][2]) * scaledExtent.z;
        // 5. Transform the local center to world space
        glm::vec3 worldCenter = transform.position + rot * localCenter * transform.scale;

        // 6. Compute final global AABB
        AABB globalAABB;
        globalAABB.min = worldCenter - worldExtent;
        globalAABB.max = worldCenter + worldExtent;
        return globalAABB;
	}
    const AABB AABB::getGlobalAABB(glm::mat4 model) const{
        AABB globalAABB;
        globalAABB.min = glm::vec3(FLT_MAX);
        globalAABB.max = glm::vec3(-FLT_MAX);

        glm::vec3 corners[8] = {
            {min.x, min.y, min.z},
            {max.x, min.y, min.z},
            {min.x, max.y, min.z},
            {max.x, max.y, min.z},
            {min.x, min.y, max.z},
            {max.x, min.y, max.z},
            {min.x, max.y, max.z},
            {max.x, max.y, max.z}
        };

        for (auto& c : corners) {
            glm::vec3 p = glm::vec3(model * glm::vec4(c, 1.0f));
            globalAABB.min = glm::min(globalAABB.min, p);
            globalAABB.max = glm::max(globalAABB.max, p);
        }
        return globalAABB;
    }
    void AABB::grow(glm::vec3 p) { 
        min = glm::min(min, p);
        max = glm::max(max, p);
    }
    float AABB::area() const{
        glm::vec3 e = max - min; // box extent
        return e.x * e.y + e.y * e.z + e.z * e.x;
    }
    void AABB::grow(AABB& b) { 
        if (b.min.x != 1e30f){ 
            grow(b.min); 
            grow(b.max);
        }
    }
    glm::vec3 AABB::getCenter() const{
        return (min + max) * 0.5f;
    }
    std::vector<glm::vec3> AABB::getVertices() const{
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(min.x, max.y, max.z));
        vertices.push_back(glm::vec3(max.x, max.y, max.z));
        vertices.push_back(glm::vec3(max.x, min.y, max.z));
        vertices.push_back(glm::vec3(min.x, min.y, max.z));
        vertices.push_back(glm::vec3(min.x, max.y, min.z));
        vertices.push_back(glm::vec3(max.x, max.y, min.z));
        vertices.push_back(glm::vec3(max.x, min.y, min.z));
        vertices.push_back(glm::vec3(min.x, min.y, min.z));
        return vertices;
    }
    bool AABB::isColliding(const AABB& other) const{
        if (this->max.x < other.min.x || this->min.x > other.max.x) {
            return false;
        }
        if (this->max.y < other.min.y || this->min.y > other.max.y) {
            return false;
        }
        if (this->max.z < other.min.z || this->min.z > other.max.z) {
            return false;
        }
        return true;
    }
    void ProjectShapeOnAxis(const std::vector<glm::vec3>& vertices, const glm::vec3& axis,float& outMin, float& outMax){
        outMin = outMax = glm::dot(vertices[0], axis);

        for (size_t i = 1; i < vertices.size(); i++)
        {
            float p = glm::dot(vertices[i], axis);
            if (p < outMin) outMin = p;
            if (p > outMax) outMax = p;
        }
    }
    float getOverlap(float minAABB, float maxAABB, float minTriangle, float maxTriangle) {
        if (maxAABB < minTriangle || maxTriangle < minAABB) {
            return -1;
        }
        if (maxAABB > minTriangle) {
            return maxAABB - minTriangle;
        }
        if (maxTriangle > minAABB) {
            return maxTriangle - minAABB;
        }
    }
    bool AABB::checkSAT(Triangle& triangle) const{
        std::vector<glm::vec3> axes;

        //AABB axes
        glm::vec3 e0 = glm::vec3(1, 0, 0);
        glm::vec3 e1 = glm::vec3(0, 1, 0);
        glm::vec3 e2 = glm::vec3(0, 0, 1);
        axes.push_back(e0);
        axes.push_back(e1);
        axes.push_back(e2);

        //triangle normal
        std::vector<glm::vec3> triangleVertices;
        triangleVertices.push_back(triangle.v0);
        triangleVertices.push_back(triangle.v1);
        triangleVertices.push_back(triangle.v2);
        glm::vec3 edge0 = triangle.v1 - triangle.v0;
        glm::vec3 edge1 = triangle.v2 - triangle.v1;
        glm::vec3 edge2 = triangle.v0 - triangle.v2;
        axes.push_back(glm::cross(edge0, edge1));

        //cross products
        axes.push_back(glm::cross(e0, edge0));
        axes.push_back(glm::cross(e0, edge1));
        axes.push_back(glm::cross(e0, edge2));
        axes.push_back(glm::cross(e1, edge0));
        axes.push_back(glm::cross(e1, edge1));
        axes.push_back(glm::cross(e1, edge2));
        axes.push_back(glm::cross(e2, edge0));
        axes.push_back(glm::cross(e2, edge1));
        axes.push_back(glm::cross(e2, edge2));

        for (int i = 0; i < axes.size(); i++) {
            glm::vec3 axis =glm::normalize(axes[i]);
            float outMinAABB, outMaxAABB;
            float outMinTriangle, outMaxTriangle;
            ProjectShapeOnAxis(this->getVertices(), axis, outMinAABB, outMaxAABB);
            ProjectShapeOnAxis(triangleVertices, axis, outMinTriangle, outMaxTriangle);
            float o = getOverlap(outMinAABB,outMaxAABB,outMinTriangle,outMaxTriangle);
            if (o < 0) {
                return false;
            }
        }
        return true;
    }
}
#pragma once
#include "AABB.hpp"
namespace gps {
	class BVH
	{
		struct BVHNode
		{
			AABB aabb;
			int leftFirst, triCount;
			bool isLeaf() const { return triCount > 0; }
		};
		public:
			BVH() = default;
			BVH(std::vector<Mesh>& meshes,Transform &objectTransform);
			~BVH() = default;
			float getCollidingHeight(const AABB& aabb) const;

		private:
			std::vector<BVHNode> bvhNode;
			std::vector<Triangle> triangles;
			std::vector<int> trianglesIndices;
			int nodesUsed;
			void Build();
			void UpdateNodeBounds(int nodeIdx);
			void Subdivide(int nodeIdx);
			float FindBestSplitPlane(BVHNode& node, int& axis, float& splitPos);
			void Intersect(const AABB& aabb, int NodeIdx,float& highestY)const;
		};
}

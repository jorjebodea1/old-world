#include "BVH.hpp"

namespace gps {
	BVH::BVH(std::vector<Mesh>& meshes,Transform &objectTransform) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, objectTransform.position);
		model = glm::rotate(model, glm::radians(objectTransform.rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(objectTransform.rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(objectTransform.rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(objectTransform.scale));
		for (Mesh& m : meshes) {
			for (int i = 0; i < m.indices.size(); i += 3) {

				Triangle t;
				int i0 = m.indices[i];
				int i1 = m.indices[i + 1];
				int i2 = m.indices[i + 2];
				t.v0 = model*glm::vec4(m.vertices[i0].Position,1.0f);
				t.v1 = model*glm::vec4(m.vertices[i1].Position,1.0f);
				t.v2 = model*glm::vec4(m.vertices[i2].Position,1.0f);

				triangles.push_back(t);
			}
		}

		if (triangles.size() > 0) {
			bvhNode.resize(triangles.size() * 2);
			nodesUsed = 2;
			Build();
		}
		
	}

	void BVH::Build() {
		for (int i = 0; i < triangles.size(); i++) {
			trianglesIndices.push_back(i);
			triangles[i].centroid=(triangles[i].v0 + triangles[i].v1+ triangles[i].v2) * 0.3333f;
		}
		BVHNode& root = bvhNode[0];
		root.leftFirst = 0;
		root.triCount = triangles.size();
		UpdateNodeBounds(0);
		Subdivide(0);
	}
	void BVH::UpdateNodeBounds(int nodeIdx) {
		BVHNode& node = bvhNode[nodeIdx];
		node.aabb.min = glm::vec3(1e30f);
		node.aabb.max = glm::vec3(-1e30f);
		for (int first = node.leftFirst, i = 0; i < node.triCount; i++) {
			int leafTriIdx = trianglesIndices[first + i];
			Triangle& leafTri = triangles[leafTriIdx];
			node.aabb.min = glm::min(node.aabb.min, leafTri.v0);
			node.aabb.min = glm::min(node.aabb.min, leafTri.v1);
			node.aabb.min = glm::min(node.aabb.min, leafTri.v2);
			node.aabb.max = glm::max(node.aabb.max, leafTri.v0);
			node.aabb.max = glm::max(node.aabb.max, leafTri.v1);
			node.aabb.max = glm::max(node.aabb.max, leafTri.v2);
		}
	}
	float BVH::FindBestSplitPlane(BVHNode& node, int& axis, float& splitPos) {
		float bestCost = 1e30f;
		for (int a = 0; a < 3; a++) {
			float boundsMin = 1e30f, boundsMax = -1e30f;
			for (int i = 0; i < node.triCount; i++){
				Triangle& triangle = triangles[trianglesIndices[node.leftFirst + i]];
				boundsMin = std::min(boundsMin, triangle.centroid[a]);
				boundsMax = std::max(boundsMax, triangle.centroid[a]);
			}
			if (boundsMin == boundsMax) continue;

			struct Bin { AABB bounds; int triCount = 0; } bin[8];
			float scale = 8 / (boundsMax - boundsMin);
			for (int i = 0; i < node.triCount; i++){
				Triangle& triangle = triangles[trianglesIndices[node.leftFirst + i]];
				int binIdx = std::min(8 - 1, (int)((triangle.centroid[a] - boundsMin) * scale));
				bin[binIdx].triCount++;
				bin[binIdx].bounds.grow(triangle.v0);
				bin[binIdx].bounds.grow(triangle.v1);
				bin[binIdx].bounds.grow(triangle.v2);
			}

			float leftArea[8 - 1], rightArea[8 - 1];
			int leftCount[8 - 1], rightCount[8 - 1];
			AABB leftBox, rightBox;
			int leftSum = 0, rightSum = 0;
			for (int i = 0; i < 8 - 1; i++)
			{
				leftSum += bin[i].triCount;
				leftCount[i] = leftSum;
				leftBox.grow(bin[i].bounds);
				leftArea[i] = leftBox.area();
				rightSum += bin[8 - 1 - i].triCount;
				rightCount[8 - 2 - i] = rightSum;
				rightBox.grow(bin[8 - 1 - i].bounds);
				rightArea[8 - 2 - i] = rightBox.area();
			}

			scale = (boundsMax - boundsMin) / 8;
			for (int i = 0; i < 8 - 1; i++)
			{
				float planeCost = leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];
				if (planeCost < bestCost)
					axis = a, splitPos = boundsMin + scale * (i + 1), bestCost = planeCost;
			}
		}
		return bestCost;
	}
	void BVH::Subdivide(int nodeIdx) {
		BVHNode& node = bvhNode[nodeIdx];
		int axis;
		float splitPos;
		float splitCost = FindBestSplitPlane(node, axis, splitPos);
		float nosplitCost = node.aabb.area() * node.triCount;
		if (splitCost >= nosplitCost) return;

		int i = node.leftFirst;
		int j = i + node.triCount - 1;
		while (i <= j)
		{
			if (triangles[trianglesIndices[i]].centroid[axis] < splitPos)
				i++;
			else
				std::swap(trianglesIndices[i], trianglesIndices[j--]);
		}

		int leftCount = i - node.leftFirst;
		if (leftCount == 0 || leftCount == node.triCount) return;

		int leftChildIdx = nodesUsed++;
		int rightChildIdx = nodesUsed++;
		bvhNode[leftChildIdx].leftFirst = node.leftFirst;
		bvhNode[leftChildIdx].triCount = leftCount;
		bvhNode[rightChildIdx].leftFirst = i;
		bvhNode[rightChildIdx].triCount = node.triCount - leftCount;
		node.leftFirst = leftChildIdx;
		node.triCount = 0;
		UpdateNodeBounds(leftChildIdx);
		UpdateNodeBounds(rightChildIdx);

		Subdivide(leftChildIdx);
		Subdivide(rightChildIdx);
	}
	void BVH::Intersect(const AABB& aabb,int nodeIdx,float& highestY)const{
		if (bvhNode.size() <= 0) return;
		const BVHNode& node = bvhNode[nodeIdx];
		if (!aabb.isColliding(node.aabb)) return;
		if (node.isLeaf())
		{
			for (int i = 0; i < node.triCount; i++) {
				Triangle t = triangles[trianglesIndices[node.leftFirst + i]];
				if (aabb.checkSAT(t)) {
					highestY = fmaxf(highestY, t.v0.y);
					highestY = fmaxf(highestY, t.v1.y);
					highestY = fmaxf(highestY, t.v2.y);
				}
			}
		}
		else
		{
			Intersect(aabb, node.leftFirst,highestY);
			Intersect(aabb, node.leftFirst + 1,highestY);
		}
	}
	float BVH::getCollidingHeight(const AABB& aabb) const{
		float highestY = std::numeric_limits<float>::min();
		Intersect(aabb, 0, highestY);
		return highestY;
	}
}
#pragma once

#include "assimp_helper.h"

class MeshReduction
{
public:
	// Task 1: Mesh reduction
	static Mesh Reduction(const Mesh& mesh, float ratio);

	// Task 2: Mesh reduction with appearance presentation
	static Mesh ReductionWithAppearancePresentation(const Mesh& mesh, float ratio);

	// Task3: Mesh reduction with UV map
	static Mesh ReductionWithUVMap(const Mesh& mesh, float ratio);
};


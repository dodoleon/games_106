#include "mesh_reduction.h"

#include <Eigen/Sparse>
#include <igl/collapse_edge.h>
#include <igl/min_heap.h>
#include <igl/parallel_for.h>
#include <igl/vertex_triangle_adjacency.h>
#include <igl/edge_flaps.h>
#include <igl/shortest_edge_and_midpoint.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory_resource>
#include <set>
#include <thread>

#include "assimp_helper.h"

#ifdef QEM_USE_UHEMESH
#include <QEMUHEMesh.h>
#endif

#ifdef QEM_USE_OPENMESH
#include <QEMOpenMesh.h>
#endif


Mesh MeshReduction::Reduction(const Mesh &mesh, float ratio)
{
	QEM qem;
	qem.ImportMesh(mesh);
	// TODO: implement me
	int targetCount = std::ceil(mesh.F.rows() * ratio);
	qem.DoSimplification(QEM::SimplificationMode::Position, targetCount);

	Mesh outputMesh = qem.ExportMesh();
	outputMesh.texture = mesh.texture;
	return outputMesh;
}

Mesh MeshReduction::ReductionWithAppearancePresentation(const Mesh &mesh, float ratio)
{
	QEM qem;
	qem.ImportMesh(mesh);
	// TODO: implement me
	int targetCount = std::ceil(mesh.F.rows() * ratio);
	qem.DoSimplification(QEM::SimplificationMode::PositionNormal, targetCount);

	Mesh outputMesh = qem.ExportMesh();
	outputMesh.texture = mesh.texture;
	return outputMesh;
}

Mesh MeshReduction::ReductionWithUVMap(const Mesh &mesh, float ratio)
{
	QEM qem;
	qem.ImportMesh(mesh);
	// TODO: implement me
	int targetCount = std::ceil(mesh.F.rows() * ratio);
	qem.DoSimplification(QEM::SimplificationMode::PositionNormalUV, targetCount);

	Mesh outputMesh = qem.ExportMesh();
	outputMesh.texture = mesh.texture;
	return outputMesh;
}

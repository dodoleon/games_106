#pragma once

#include "details/ForwardDecl.h"

namespace Ubpa {
	template<typename Traits>
	class TPolygon {
	public:
		using V = HEMeshTraits_V<Traits>;
		using E = HEMeshTraits_E<Traits>;
		using P = HEMeshTraits_P<Traits>;
		using H = HEMeshTraits_H<Traits>;

	public:
		H* HalfEdge() noexcept { return halfEdge; }
		const H* HalfEdge() const noexcept { return const_cast<TPolygon*>(this)->HalfEdge(); }

		void SetHalfEdge(H* he) noexcept { halfEdge = he; }
		void Reset() noexcept { halfEdge = nullptr; }

		// p == nullptr
		static bool IsBoundary(const P* p) noexcept { return p == nullptr; }

		// number of edges/vertices
		std::size_t Degree() const noexcept { assert(HalfEdge()); return HalfEdge()->PolygonDegree(); }

		// halfedges : [ he, he.Next(), he.Next().Next(), ..., he)
		HalfEdgeNextView<false, Traits> AdjHalfEdges() { return HalfEdge()->NextLoop(); }

		// halfedges : [ he, he.Next(), he.Next().Next(), ..., he)
		HalfEdgeNextView<true, Traits> AdjHalfEdges() const { return HalfEdge()->NextLoop(); }

	private:
		friend HEMesh<Traits>;

		H* halfEdge{ nullptr };
	};

}

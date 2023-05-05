#pragma once

#include "details/ForwardDecl.h"

namespace Ubpa {
	template<typename Traits>
	class TEdge {
	public:
		using V = HEMeshTraits_V<Traits>;
		using E = HEMeshTraits_E<Traits>;
		using P = HEMeshTraits_P<Traits>;
		using H = HEMeshTraits_H<Traits>;

	public:
		H* HalfEdge() noexcept { return halfEdge; }
		const H* HalfEdge() const noexcept { return const_cast<TEdge*>(this)->HalfEdge(); }

		void SetHalfEdge(H* he) noexcept { halfEdge = he; }
		void Reset() noexcept { halfEdge = nullptr; }

		// edge is on boundary == any halfedge is on boundary
		bool IsOnBoundary() const noexcept { return HalfEdge()->IsOnBoundary() || HalfEdge()->Pair()->IsOnBoundary(); }

		// clockwise
		// + [he.RotateNext, he.RotateNext.RotateNext, ..., he)
		// + [he.next, he.next.RotateNext, ..., he.pair)
		std::vector<H*> AdjOutHalfEdges();
		// clockwise
		// + [he.RotateNext, he.RotateNext.RotateNext, ..., he)
		// + [he.next, he.next.RotateNext, ..., he.pair)
		std::vector<const H*> AdjOutHalfEdges() const;

		// { halfedge.End() for halfedge in OutHalfEdges() }
		std::set<V*> AdjVertices();
		// { halfedge.End() for halfedge in OutHalfEdges() }
		std::set<const V*> AdjVertices() const;

		// { halfedge.Edge() for halfedge in OutHalfEdges() }
		std::vector<E*> AdjEdges();
		// { halfedge.Edge() for halfedge in OutHalfEdges() }
		std::vector<const E*> AdjEdges() const;

	private:
		friend HEMesh<Traits>;
		bool IsFree() const noexcept { return HalfEdge()->IsFree() && HalfEdge()->Pair()->IsFree(); }

		H* halfEdge{ nullptr };
	};
}

#include "details/TEdge.inl"

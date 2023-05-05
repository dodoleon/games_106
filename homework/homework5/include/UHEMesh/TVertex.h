#pragma once

#include "details/ForwardDecl.h"

namespace Ubpa {
	template<typename Traits>
	class TVertex {
	public:
		using V = HEMeshTraits_V<Traits>;
		using E = HEMeshTraits_E<Traits>;
		using P = HEMeshTraits_P<Traits>;
		using H = HEMeshTraits_H<Traits>;

	public:
		H* HalfEdge() noexcept { return halfEdge; }
		const H* HalfEdge() const noexcept { return const_cast<TVertex*>(this)->HalfEdge(); }

		void SetHalfEdge(H* he) noexcept { halfEdge = he; }

		void Reset() noexcept { halfEdge = nullptr; }

		// halfedge == nullptr
		bool IsIsolated() const noexcept { return !halfEdge; }

		// vertex is on boundary == any adjacent polygon is boundary (nullptr)
		bool IsOnBoundary() const noexcept;

		// outward halfedges : [he, he.RotateNext(), he.RotateNext().RotateNext(), ..., he)
		HalfEdgeRotateNextView<false, Traits> OutHalfEdges();

		// outward halfedges : [he, he.RotateNext(), he.RotateNext().RotateNext(), ..., he)
		HalfEdgeRotateNextView<true, Traits> OutHalfEdges() const;

		VertexAdjVertexView<false, Traits> AdjVertices();
		VertexAdjVertexView<true, Traits> AdjVertices() const;

		VertexAdjEdgeView<false, Traits> AdjEdges();
		VertexAdjEdgeView<true, Traits> AdjEdges() const;

		// number of adjacent edges
		std::size_t Degree() const noexcept;

		// find halfedge (this -> end)
		// nullptr if no exist
		H* HalfEdgeTo(V* end) noexcept;
		const H* HalfEdgeTo(const V* end) const noexcept
		{ return const_cast<TVertex*>(this)->HalfEdgeTo(const_cast<V*>(end)); }

		// find halfedge (origin -> end)
		// nullptr if no exist
		static H* HalfEdgeAlong(V* origin, V* end) noexcept { return origin->HalfEdgeTo(end); }
		static const H* HalfEdgeAlong(const V* origin, const V* end) noexcept
		{ return HalfEdgeAlong(const_cast<V*>(origin), const_cast<V*>(end)); }

		// find edge (this -> v)
		// nullptr if no exist
		E* EdgeWith(V* v) noexcept;
		const E* EdgeWith(const V* v) const noexcept { return const_cast<TVertex*>(this)->EdgeWith(const_cast<V*>(v)); }

		// find edge (v0 <-> v1)
		// nullptr if no exist
		static E* EdgeBetween(V* v0, V* v1) noexcept { return v0->EdgeWith(v1); }
		static const E* EdgeBetween(const V* v0, const V* v1) noexcept
		{ return EdgeBetween(const_cast<V*>(v0), const_cast<V*>(v1)); }

		// true if exist edge (this <-> v1)
		bool IsConnectedWith(const V* v) const noexcept { return EdgeWith(v); }
		// true if exist edge (v0 <-> v1)
		static bool IsConnected(const V* v0, const V* v1) noexcept { return v0->IsConnectedWith(v1); }

	private:
		friend HEMesh<Traits>;
		H* FindFreeIncident() noexcept;

		H* halfEdge{ nullptr };
	};
}

#include "details/TVertex.inl"

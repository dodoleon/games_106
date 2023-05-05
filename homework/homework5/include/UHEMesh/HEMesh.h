#pragma once

#include "Empty.h"

#include "details/random_set.h"

#include <memory_resource>

namespace Ubpa {
	// [ example ]
	// // forward declaration
	// class V;
	// class E;
	// class P;
	// class H;
	// using Traits_VEPH = HEMeshTraits<V, E, P, H>;
	// // custom vertex, edge, polygon and half-edge class
	// class V : public TVertex  <Traits_VEPH> { /*...*/ }
	// class E : public TEdge    <Traits_VEPH> { /*...*/ }
	// class P : public TPolygon <Traits_VEPH> { /*...*/ }
	// class H : public THalfEdge<Traits_VEPH> { /*...*/ }
	// ...
	// HEMesh<Traits_VEPH> mesh({0,1,2,0,2,3}, 3);
	template<typename Traits = HEMeshTraits_EmptyVEPH>
	class HEMesh {
		static_assert(Traits::IsValid());

	public:
		using V = HEMeshTraits_V<Traits>;
		using E = HEMeshTraits_E<Traits>;
		using P = HEMeshTraits_P<Traits>;
		using H = HEMeshTraits_H<Traits>;

		//
		// Constructors
		/////////////////

		HEMesh() = default;
		HEMesh(const std::vector<std::vector<size_t>>& polygons) { Init(polygons); }
		HEMesh(const std::vector<size_t>& polygons, size_t sides) { Init(polygons, sides); }

		// minimal index is 0
		// [example]
		// polygons: {
		//    {0,1,2},
		//    {3,4,5,6}
		// }
		bool Init(const std::vector<std::vector<size_t>>& polygons);

		// minimal index is 0
		// [example]
		// polygons: { 0,1,3,2,3,1 }
		// sides : 3
		bool Init(const std::vector<size_t>& polygons, size_t sides);

		// export all polygons' vertices-indices
		std::vector<std::vector<size_t>> Export() const;

		// a halfedge mesh is valid if
		// - every halfedge has next, pair, origin and edge
		// - all halfedges form some circles
		// - every halfedge's pair-pair is itself
		// - every vertice's out-halfedges-origin is itself
		// - every edge has halfege, and it's two halfedge-edge is itself
		// - every polygon has halfege, and it's all halfedge-polygon is itself
		bool IsValid() const;

		// IsValid() and every polygon's degree is 3
		bool IsTriMesh() const;

		//
		// Element Access
		///////////////////

		const std::vector<V*>& Vertices() noexcept { return vertices.vec(); }
		const std::vector<E*>& Edges() noexcept { return edges.vec(); }
		const std::vector<P*>& Polygons() noexcept { return polygons.vec(); }
		const std::vector<H*>& HalfEdges() noexcept { return halfEdges.vec(); }

		// every halfedge in the result represents a boundary
		// you can use HalfEdge::NextLoop() to get the boundary's all halfedges
		std::vector<H*> Boundaries() const;

		// faster than Boundaries().size() > 0
		bool HasBoundary() const noexcept;

		//
		// Index
		//////////
		// 
		// - every vertex, edge, polygon and halfedge have an unique index in [0, size)
		// - if mesh's topology change, previous indices is invalid
		// - After initialization, vertices and polygons' indices are same with input
		// 

		size_t Index(const V* v) const { return vertices.idx(const_cast<V*>(v)); }
		size_t Index(const E* e) const { return edges.idx(const_cast<E*>(e)); }
		size_t Index(const P* p) const { return polygons.idx(const_cast<P*>(p)); }
		size_t Index(const H* h) const { return halfEdges.idx(const_cast<H*>(h)); }

		// get the polygon's vertices-index
		// index is useless after changing the topology
		std::vector<size_t> Indices(P* p) const;

		//
		// Capacity
		/////////////

		// vertices empty => halfedges, edges and polygons empty
		bool IsEmpty() const noexcept { return vertices.empty(); }

		void Clear() noexcept;

		// reserve
		// -  n vertices
		// - 2n polygons
		// - 3n edges
		// - 6n halfedges
		void Reserve(size_t n);

		//
		// Basic Mesh Editting
		////////////////////////

		template<typename... Args>
		V* AddVertex(Args&&... args) { return New<V>(std::forward<Args>(args)...); }
		// e's halfedge is form v0 to v1
		template<typename... Args>
		E* AddEdge(V* v0, V* v1, Args&&... args);
		// polygon's halfedge is heLoop[0]
		template<typename HalfEdgeLoop, typename... Args>
		P* AddPolygon(const HalfEdgeLoop& heLoop, Args&&... args);

		void RemovePolygon(P* polygon);
		void RemoveEdge(E* e);
		void RemoveVertex(V* v);

		//
		// High-Level Mesh Editting
		/////////////////////////////

		// RemoveVertex and AddPolygon
		const P* EraseVertex(V* v);

		// edge's halfedge : v0=>v1
		// nweV's halfedge : newV => v1
		template<typename... Args>
		V* AddEdgeVertex(E* e, Args&&... args);

		// connect he0.origin and he1.origin in he0/he1.polygon
		// [require] he0.polygon == he1.polygon, he0.origin != he1.origin
		// [return] edge with halfedge form he0.origin to he1.origin
		template<typename... Args>
		E* ConnectVertex(H* he0, H* he1, Args&&... args);

		// counter-clock, remain e in container, won't break iteration
		bool FlipEdge(E* e);

		// delete e
		template<typename... Args>
		V* SplitEdge(E* e, Args&&... args);

		bool IsCollapsable(const E* e) const;

		// call IsCollapsable(E* e) firstly
		template<typename... Args>
		V* CollapseEdge(E* e, Args&&... args);

	private:
		template<typename T> struct MemVarOf;
		template<typename T>
		friend struct MemVarOf;

		// new and insert
		template<typename T, typename... Args>
		T* New(Args&&... args);

		// clear and erase
		template<typename T>
		void Delete(T* elem);

	private:
		random_set<H> halfEdges;
		random_set<V> vertices;
		random_set<E> edges;
		random_set<P> polygons;

		std::pmr::synchronized_pool_resource rsrc;

		template<typename T>
		auto& GetSet() {
			if constexpr (std::is_same_v<T, H>)
				return halfEdges;
			else if constexpr (std::is_same_v<T, V>)
				return vertices;
			else if constexpr (std::is_same_v<T, E>)
				return edges;
			else{
				static_assert(std::is_same_v<T, P>);
				return polygons;
			}
		}
	};
}

#include "details/HEMesh.inl"

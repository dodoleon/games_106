#pragma once

namespace Ubpa {
	template<typename Traits>
	std::vector<HEMeshTraits_H<Traits>*> TEdge<Traits>::AdjOutHalfEdges() {
		std::vector<H*> hes;
		auto* he01 = HalfEdge(); // v0 => v1
		for (auto* he : he01->RotateNext()->RotateNextTo(he01))
			hes.push_back(he);
		for (auto* he : he01->Next()->RotateNextTo(he01->Pair()))
			hes.push_back(he);
		return hes;
	}

	template<typename Traits>
	std::vector<const HEMeshTraits_H<Traits>*> TEdge<Traits>::AdjOutHalfEdges() const {
		std::vector<const H*> hes;
		auto* he01 = HalfEdge(); // v0 => v1
		for (auto* he : he01->RotateNext()->RotateNextTo(he01))
			hes.push_back(he);
		for (auto* he : he01->Next()->RotateNextTo(he01->Pair()))
			hes.push_back(he);
		return hes;
	}

	template<typename Traits>
	std::set<HEMeshTraits_V<Traits>*> TEdge<Traits>::AdjVertices() {
		std::set<V*> vertices;
		for (auto* he : AdjOutHalfEdges())
			vertices.insert(he->End());
		return vertices;
	}

	template<typename Traits>
	std::set<const HEMeshTraits_V<Traits>*> TEdge<Traits>::AdjVertices() const {
		std::set<const V*> vertices;
		for (auto* he : AdjOutHalfEdges())
			vertices.insert(he->End());
		return vertices;
	}

	template<typename Traits>
	std::vector<HEMeshTraits_E<Traits>*> TEdge<Traits>::AdjEdges() {
		std::vector<E*> edges;
		for (auto* he : AdjOutHalfEdges())
			edges.push_back(he->Edge());
		return edges;
	}

	template<typename Traits>
	std::vector<const HEMeshTraits_E<Traits>*> TEdge<Traits>::AdjEdges() const {
		std::vector<const E*> edges;
		for (auto* he : AdjOutHalfEdges())
			edges.push_back(he->Edge());
		return edges;
	}
}

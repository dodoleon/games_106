#pragma once

#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>

namespace Ubpa {
	template<typename Traits>
	template<typename T, typename... Args>
	T* HEMesh<Traits>::New(Args&&... args) {
		T* elem = (T*)rsrc.allocate(sizeof(T), alignof(T));
		new (elem) T(std::forward<Args>(args)...);
		GetSet<T>().insert(elem);
		return elem;
	}

	// clear and erase
	template<typename Traits>
	template<typename T>
	void HEMesh<Traits>::Delete(T* elem) {
#ifndef NDEBUG
		elem->Reset();
#endif // !NDEBUG
		rsrc.deallocate(elem, sizeof(T), alignof(T));
		GetSet<T>().erase(elem);
	}

	template<typename Traits>
	std::vector<size_t> HEMesh<Traits>::Indices(P* p) const {
		assert(p != nullptr);
		std::vector<size_t> indices;
		for (auto* v : p->AdjVertices())
			indices.push_back(Index(v));
		return indices;
	}

	template<typename Traits>
	template<typename... Args>
	HEMeshTraits_E<Traits>* HEMesh<Traits>::AddEdge(V* v0, V* v1, Args&&... args) {
		assert(v0 != nullptr && v1 != nullptr && v0 != v1 && !V::IsConnected(v0, v1));

		auto* e = New<E>(std::forward<Args>(args)...);

		auto* he0 = New<H>();
		auto* he1 = New<H>();
		// [init]
		e->SetHalfEdge(he0);

		he0->SetNext(he1);
		he0->SetPair(he1);
		he0->SetOrigin(v0);
		he0->SetEdge(e);

		he1->SetNext(he0);
		he1->SetPair(he0);
		he1->SetOrigin(v1);
		he1->SetEdge(e);

		// [link he0]
		if (!v0->IsIsolated()) {
			auto* inV0 = v0->FindFreeIncident();
			if (inV0 == nullptr)
				return nullptr;
			auto* outV0 = inV0->Next();

			inV0->SetNext(he0);
			he1->SetNext(outV0);
		}
		else
			v0->SetHalfEdge(he0);

		// [link he1]
		if (!v1->IsIsolated()) {
			auto* inV1 = v1->FindFreeIncident();
			if (inV1 == nullptr)
				return nullptr;
			auto* outV1 = inV1->Next();

			inV1->SetNext(he1);
			he0->SetNext(outV1);
		}
		else
			v1->SetHalfEdge(he1);

		return e;
	}

	template<typename Traits>
	template<typename HalfEdgeLoop, typename... Args>
	HEMeshTraits_P<Traits>* HEMesh<Traits>::AddPolygon(const HalfEdgeLoop& heLoop, Args&&... args) {
		assert(heLoop.begin() != heLoop.end() && "heLoop must be non-empty");
		H* he0 = *heLoop.begin();
#ifndef NDEBUG
		{
			H* preHE = nullptr;
			for (auto* he : heLoop) {
				assert(he && he->IsFree());
				if (preHE)
					assert(preHE->End() == he->Origin());
				preHE = he;
			}
			assert(preHE->End() == he0->Origin());
		}
#endif // !NDEBUG

		// reorder link
		{
			H* preHE = nullptr;
			for (auto* he : heLoop) {
				if (preHE) {
					bool success = H::MakeAdjacent(preHE, he);
					assert(success && "the polygon would introduce a non - monifold condition");
				}
				preHE = he;
			}
			bool success = H::MakeAdjacent(preHE, he0);
			assert(success && "the polygon would introduce a non - monifold condition");
		}

		// link polygon and heLoop
		auto* polygon = New<P>(std::forward<Args>(args)...);

		polygon->SetHalfEdge(he0);
		for (auto* he : heLoop)
			he->SetPolygon(polygon);

		return polygon;
	}

	template<typename Traits>
	void HEMesh<Traits>::RemovePolygon(P* polygon) {
		assert(polygon != nullptr);
		for (auto* he : polygon->AdjHalfEdges())
			he->SetPolygon(nullptr);
		Delete<P>(polygon);
	}

	template<typename Traits>
	void HEMesh<Traits>::RemoveEdge(E* e) {
		assert(e != nullptr);
		auto* he0 = e->HalfEdge();
		auto* he1 = he0->Pair();

		if (!he0->IsFree())
			RemovePolygon(he0->Polygon());
		if (!he1->IsFree())
			RemovePolygon(he1->Polygon());

		// init
		auto* v0 = he0->Origin();
		auto* v1 = he1->Origin();
		auto* inV0 = he0->Pre();
		auto* inV1 = he1->Pre();
		auto* outV0 = he0->RotateNext();
		auto* outV1 = he1->RotateNext();

		// [link off he0]
		if (v0->HalfEdge() == he0)
			v0->SetHalfEdge(outV0 == he0 ? nullptr : outV0);

		inV0->SetNext(outV0);

		// [link off he1]
		if (v1->HalfEdge() == he1)
			v1->SetHalfEdge(outV1 == he1 ? nullptr : outV1);

		inV1->SetNext(outV1);

		// delete
		Delete<H>(he0);
		Delete<H>(he1);
		Delete<E>(e);
	}

	template<typename Traits>
	void HEMesh<Traits>::RemoveVertex(V* v) {
		assert(v != nullptr);
		auto view = v->OutHalfEdges();
		auto iter = view.begin();
		do {
			auto cpy_iter = iter;
			++iter;

			RemoveEdge(cpy_iter->Edge());
		} while (iter != view.end());
		Delete<V>(v);
	}

	template<typename Traits>
	bool HEMesh<Traits>::Init(const std::vector<std::vector<size_t>>& polygons) {
		assert(!polygons.empty());

		Clear();

		size_t max = 0;
		size_t min = std::numeric_limits<size_t>::max();
		for (const auto& polygon : polygons) {
			assert(polygon.size() > 2);

			for (auto idx : polygon) {
				if (idx > max)
					max = idx;
				if (idx < min)
					min = idx;
			}
		}

		assert(min == 0);

		for (size_t i = 0; i <= max; i++)
			New<V>();

		for (const auto& polygon : polygons) {
			std::vector<H*> heLoop;
			for (size_t i = 0; i < polygon.size(); i++) {
				size_t next = (i + 1) % polygon.size();
				assert(polygon[i] != polygon[next]);
				auto* u = vertices[polygon[i]];
				auto* v = vertices[polygon[next]];
				auto* he = u->HalfEdgeTo(v);
				if (!he)
					he = AddEdge(u, v)->HalfEdge();
				heLoop.push_back(he);
			}
			auto* p = AddPolygon(heLoop);
			assert(p != nullptr);
		}

		return true;
	}

	template<typename Traits>
	bool HEMesh<Traits>::Init(const std::vector<size_t>& polygons, size_t sides) {
		assert(polygons.size() % sides == 0);
		std::vector<std::vector<size_t>> arrangedPolygons;
		for (size_t i = 0; i < polygons.size(); i += sides) {
			std::vector<size_t> indices;
			for (size_t j = 0; j < sides; j++)
				indices.push_back(polygons[i + j]);
			arrangedPolygons.push_back(std::move(indices));
		}
		return Init(arrangedPolygons);
	}

	template<typename Traits>
	std::vector<std::vector<size_t>> HEMesh<Traits>::Export() const {
		if (!IsValid())
			return {};

		std::vector<std::vector<size_t>> rst;
		for (auto* polygon : polygons.vec()) {
			std::vector<size_t> indices;
			for (auto* v : polygon->AdjVertices())
				indices.push_back(Index(v));
			rst.push_back(std::move(indices));
		}
		return rst;
	}

	template<typename Traits>
	void HEMesh<Traits>::Clear() noexcept {
		for (auto* v : vertices.vec())
			rsrc.deallocate(v, sizeof(V), alignof(V));
		vertices.clear();

		for (auto* he : halfEdges.vec())
			rsrc.deallocate(he, sizeof(H), alignof(H));
		halfEdges.clear();

		for (auto* e : edges.vec())
			rsrc.deallocate(e, sizeof(E), alignof(E));
		edges.clear();

		for (auto* p : polygons.vec())
			rsrc.deallocate(p, sizeof(P), alignof(P));
		polygons.clear();
	}

	template<typename Traits>
	void HEMesh<Traits>::Reserve(size_t n) {
		vertices.reserve(n);
		halfEdges.reserve(6 * n);
		edges.reserve(3 * n);
		polygons.reserve(2 * n);
	}

	template<typename Traits>
	bool HEMesh<Traits>::HasBoundary() const noexcept {
		for (auto* he : halfEdges) {
			if (he->IsOnBoundary())
				return true;
		}
		return false;
	}

	template<typename Traits>
	std::vector<HEMeshTraits_H<Traits>*> HEMesh<Traits>::Boundaries() const {
		std::vector<H*> boundaries;
		std::unordered_set<H*> found;
		for (auto* he : halfEdges) {
			if (he->IsOnBoundary() && found.find(he) == found.end()) {
				auto* cur = he;
				do {
					found.insert(cur);
					cur = cur->Next();
				} while (cur != he);
				boundaries.push_back(he);
			}
		}
		return boundaries;
	}

	template<typename Traits>
	bool HEMesh<Traits>::IsValid() const {
		for (auto* he : halfEdges) {
			if (!he->Next() || !he->Pair() || !he->Origin() || !he->Edge())
				return false;
		}

		std::unordered_set<H*> uncheckHalfEdges(halfEdges.begin(), halfEdges.end());
		H* headHE = nullptr;
		H* curHE = nullptr;
		while (!uncheckHalfEdges.empty() || headHE != nullptr) {
			if (!headHE) {
				auto iter = uncheckHalfEdges.begin();
				headHE = curHE = *iter;
				uncheckHalfEdges.erase(iter);
			}
			curHE = curHE->Next();
			if (curHE == headHE)
				headHE = nullptr;
			else {
				auto target = uncheckHalfEdges.find(curHE);
				if (target == uncheckHalfEdges.end())
					return false;
				uncheckHalfEdges.erase(target);
			}
		}

		for (auto* he : halfEdges) {
			if (he->Pair()->Pair() != he)
				return false;
		}

		for (auto* v : vertices) {
			for (auto* he : v->OutHalfEdges()) {
				if (he->Origin() != v)
					return false;
			}
		}

		for (auto* e : edges) {
			if (!e->HalfEdge() || e->HalfEdge()->Edge() != e || e->HalfEdge()->Pair()->Edge() != e)
				return false;
		}

		for (auto* p : polygons) {
			if (!p->HalfEdge())
				return false;
			for (auto* he : p->AdjHalfEdges()) {
				if (he->Polygon() != p)
					return false;
			}
		}

		return true;
	}

	template<typename Traits>
	bool HEMesh<Traits>::IsTriMesh() const {
		if (!IsValid())
			return false;

		for (auto* poly : polygons) {
			if (poly->Degree() != 3)
				return false;
		}

		return true;
	}

	template<typename Traits>
	const HEMeshTraits_P<Traits>* HEMesh<Traits>::EraseVertex(V* v) {
		if (v->IsOnBoundary()) {
			RemoveVertex(v);
			return nullptr;
		}

		auto* he = v->HalfEdge();
		while (he->Next()->End() == v) {
			he = he->RotateNext();
			if (he == v->HalfEdge()) {
				RemoveVertex(v);
				return nullptr;
			}
		}
		he = he->Next();

		RemoveVertex(v);
		return AddPolygon(he->NextLoop());
	}

	template<typename Traits>
	template<typename... Args>
	HEMeshTraits_V<Traits>* HEMesh<Traits>::AddEdgeVertex(E* e, Args&&... args) {
		// prepare
		auto* he01 = e->HalfEdge();
		auto* he10 = he01->Pair();

		auto* v0 = he01->Origin();
		auto* v1 = he10->Origin();

		auto v0d = v0->Degree();
		auto v1d = v1->Degree();

		auto* p01 = he01->Polygon();
		auto* p10 = he10->Polygon();

		auto* he02 = New<H>();
		auto* he21 = New<H>();
		auto* he12 = New<H>();
		auto* he20 = New<H>();

		auto* e02 = New<E>();
		auto* e12 = New<E>();

		auto* v2 = New<V>(std::forward<Args>(args)...);

		// basic set
		if (v0->HalfEdge() == he01)
			v0->SetHalfEdge(he02);
		if (v1->HalfEdge() == he10)
			v1->SetHalfEdge(he12);
		v2->SetHalfEdge(he21);

		he02->SetNext(he21);
		he02->SetPair(he20);
		he02->SetOrigin(v0);
		he02->SetEdge(e02);
		he02->SetPolygon(p01);

		//he21->SetNext();
		he21->SetPair(he12);
		he21->SetOrigin(v2);
		he21->SetEdge(e12);
		he21->SetPolygon(p01);

		he12->SetNext(he20);
		he12->SetPair(he21);
		he12->SetOrigin(v1);
		he12->SetEdge(e12);
		he12->SetPolygon(p10);

		//he20->SetNext();
		he20->SetPair(he02);
		he20->SetOrigin(v2);
		he20->SetEdge(e02);
		he20->SetPolygon(p10);

		e02->SetHalfEdge(he02);
		e12->SetHalfEdge(he12);

		if (!P::IsBoundary(p01) && p01->HalfEdge() == he01)
			p01->SetHalfEdge(he02);
		if (!P::IsBoundary(p10) && p10->HalfEdge() == he10)
			p10->SetHalfEdge(he12);

		// 4 case
		if (v0d == 1 && v1d == 1) {
			he21->SetNext(he12);
			he20->SetNext(he02);
		}
		else if (v0d == 1) {
			auto* he01Next = he01->Next();
			auto* he10Pre = he10->Pre();

			he21->SetNext(he01Next);
			he20->SetNext(he02);
			he10Pre->SetNext(he12);
		}
		else if (v1d == 1) {
			auto* he01Pre = he01->Pre();
			auto* he10Next = he10->Next();

			he01Pre->SetNext(he02);
			he21->SetNext(he12);
			he20->SetNext(he10Next);
		}
		else {
			auto* he01Pre = he01->Pre();
			auto* he01Next = he01->Next();
			auto* he10Pre = he10->Pre();
			auto* he10Next = he10->Next();

			he01Pre->SetNext(he02);
			he10Pre->SetNext(he12);
			he21->SetNext(he01Next);
			he20->SetNext(he10Next);
		}

		// delete
		Delete<H>(he01);
		Delete<H>(he10);
		Delete<E>(e);

		return v2;
	}

	template<typename Traits>
	template<typename... Args>
	HEMeshTraits_E<Traits>* HEMesh<Traits>::ConnectVertex(H* he0, H* he1, Args&&... args) {
		assert(he0->Polygon() == he1->Polygon());

		auto* p = he0->Polygon();

		assert(!P::IsBoundary(p));

		auto* v0 = he0->Origin();
		auto* v1 = he1->Origin();

		assert(v0 != v1 && !V::IsConnected(v0, v1));

		RemovePolygon(p);

		auto* he0Pre = he0->Pre();
		auto* he1Pre = he1->Pre();

		auto he0Loop = he0->NextTo(he1);
		auto he1Loop = he1->NextTo(he0);

		auto* e01 = New<E>(std::forward<Args>(args)...);
		auto* he01 = New<H>();
		auto* he10 = New<H>();

		e01->SetHalfEdge(he01);

		he01->SetNext(he1);
		he01->SetPair(he10);
		he01->SetOrigin(v0);
		he01->SetEdge(e01);

		he10->SetNext(he0);
		he10->SetPair(he01);
		he10->SetOrigin(v1);
		he10->SetEdge(e01);

		he0Pre->SetNext(he01);
		he1Pre->SetNext(he10);

		he0Loop.push_back(he10);
		he1Loop.push_back(he01);

		AddPolygon(he0Loop);
		AddPolygon(he1Loop);

		return e01;
	}

	template<typename Traits>
	bool HEMesh<Traits>::FlipEdge(E* e) {
		assert(e != nullptr && !e->IsOnBoundary());

		// 1. prepare
		auto* he01 = e->HalfEdge();
		auto* he10 = he01->Pair();
		auto* he02 = he10->Next();
		auto* he13 = he01->Next();
		auto* he01Pre = he01->Pre();
		auto* he10Pre = he10->Pre();
		auto* he02Next = he02->Next();
		auto* he13Next = he13->Next();

		auto* p01 = he01->Polygon();
		auto* p10 = he10->Polygon();

		auto* v0 = he01->Origin();
		auto* v1 = he01->End();
		auto* v2 = he02->End();
		auto* v3 = he13->End();

		// 2. change
		if (v0->HalfEdge() == he01)
			v0->SetHalfEdge(he02);
		if (v1->HalfEdge() == he10)
			v1->SetHalfEdge(he13);

		auto* he23 = he01;
		auto* he32 = he10;

		he01Pre->SetNext(he02);

		he02->SetNext(he23);
		he02->SetPolygon(p01);

		he23->SetOrigin(v2);
		he32->SetOrigin(v3);

		he23->SetNext(he13Next);

		he10Pre->SetNext(he13);

		he13->SetNext(he32);
		he13->SetPolygon(p10);

		he32->SetNext(he02Next);

		if (p01->HalfEdge() == he13)
			p01->SetHalfEdge(he02);
		if (p10->HalfEdge() == he02)
			p10->SetHalfEdge(he13);

		return true;
	}

	template<typename Traits>
	template<typename... Args>
	HEMeshTraits_V<Traits>* HEMesh<Traits>::SplitEdge(E* e, Args&&... args) {
		assert(e != nullptr);

		auto* he01 = e->HalfEdge();
		auto* he10 = he01->Pair();

		assert((!he01->IsOnBoundary() || !he10->IsOnBoundary())
			&& "two side of edge are boundaries");

		if (he01->IsOnBoundary() || he10->IsOnBoundary()) {
			if (he01->IsOnBoundary())
				std::swap(he01, he10);

			auto* p01 = he01->Polygon();

			assert(p01->Degree() == 3);

			/*
			*     v1         v1
			*    / |        / |
			*   /  |       /  |
			* v2   | ==> v2--v3
			*   \  |       \  |
			*    \ |        \ |
			*     v0         v0
			*/

			// prepare
			auto* he12 = he01->Next();
			auto* he20 = he12->Next();
			auto* he10Next = he10->Next();
			auto* v0 = he01->Origin();
			auto* v1 = he10->Origin();
			auto* v2 = he20->Origin();

			// old to new
			auto* he03 = he01;
			auto* he13 = he10;
			auto* e03 = e;
			auto* p03 = p01;

			// new
			auto* v3 = New<V>(std::forward<Args>(args)...);
			auto* he30 = New<H>();
			auto* he31 = New<H>();
			auto* he32 = New<H>();
			auto* he23 = New<H>();
			auto* e32 = New<E>();
			auto* e13 = New<E>();
			auto* p31 = New<P>();

			// set
			v3->SetHalfEdge(he31);

			he12->SetNext(he23);
			he12->SetPolygon(p31);
			he03->Init(he32, he30, v0, e03, p03);
			he13->Init(he30, he31, v1, e13, nullptr);
			he30->Init(he10Next, he03, v3, e03, nullptr);
			he31->Init(he12, he13, v3, e13, p31);
			he32->Init(he20, he23, v3, e32, p03);
			he23->Init(he31, he32, v2, e32, p31);

			e03->SetHalfEdge(he03);
			e32->SetHalfEdge(he32);
			e13->SetHalfEdge(he13);

			if (p03->HalfEdge() == he12)
				p03->SetHalfEdge(he03);
			p31->SetHalfEdge(he31);

			return v3;
		}

		auto* p01 = he01->Polygon();
		auto* p10 = he10->Polygon();

		assert(p01->Degree() == 3 && p10->Degree() == 3);

		/*
		*     v1             v1
		*    /||\           /||\
		*   / || \         / || \
		* v2  ||  v3 ==> v2--v4--v3
		*   \ || /         \ || /
		*    \||/           \||/
		*     v0             v0
		*/

		// prepare
		auto* he12 = he01->Next();
		auto* he20 = he12->Next();
		auto* he03 = he10->Next();
		auto* he31 = he03->Next();
		auto* v0 = he01->Origin();
		auto* v1 = he10->Origin();
		auto* v2 = he20->Origin();
		auto* v3 = he31->Origin();

		// old to new
		auto* he04 = he01;
		auto* he14 = he10;
		auto* e04 = e;
		auto* p04 = p01;
		auto* p14 = p10;

		// new
		auto* v4 = New<V>(std::forward<Args>(args)...);
		auto* he40 = New<H>();
		auto* he41 = New<H>();
		auto* he42 = New<H>();
		auto* he24 = New<H>();
		auto* he43 = New<H>();
		auto* he34 = New<H>();
		auto* e42 = New<E>();
		auto* e14 = New<E>();
		auto* e43 = New<E>();
		auto* p41 = New<P>();
		auto* p40 = New<P>();

		// set
		v4->SetHalfEdge(he41);

		he12->SetNext(he24);
		he12->SetPolygon(p41);
		he03->SetNext(he34);
		he03->SetPolygon(p40);
		he04->Init(he42, he40, v0, e04, p04);
		he14->Init(he43, he41, v1, e14, p14);
		he40->Init(he03, he04, v4, e04, p40);
		he41->Init(he12, he14, v4, e14, p41);
		he42->Init(he20, he24, v4, e42, p04);
		he24->Init(he41, he42, v2, e42, p41);
		he43->Init(he31, he34, v4, e43, p14);
		he34->Init(he40, he43, v3, e43, p40);

		e04->SetHalfEdge(he04);
		e42->SetHalfEdge(he42);
		e14->SetHalfEdge(he14);
		e43->SetHalfEdge(he43);

		if (p04->HalfEdge() == he12)
			p04->SetHalfEdge(he04);
		if (p14->HalfEdge() == he03)
			p14->SetHalfEdge(he14);
		p41->SetHalfEdge(he41);
		p40->SetHalfEdge(he40);

		return v4;
	}

	template<typename Traits>
	bool HEMesh<Traits>::IsCollapsable(const E* e) const {
		assert(e != nullptr);

		auto* he01 = e->HalfEdge();
		auto* he10 = he01->Pair();

		auto* v0 = he01->Origin();
		auto* v1 = he01->End();

		if (v0->IsOnBoundary() && v1->IsOnBoundary() && !e->IsOnBoundary())
			return false;

		size_t cnt = 2;
		if (he01->PolygonDegree() > 3)
			--cnt;
		if (he10->PolygonDegree() > 3)
			--cnt;

		for (auto* he0 : v0->OutHalfEdges()) {
			auto* he0end = he0->End();
			for (auto* he1 : v1->OutHalfEdges()) {
				if (he0end == he1->End()) {
					if (cnt == 0)
						return false;
					--cnt;
				}
			}
		}

		return true;
	}

	template<typename Traits>
	template<typename... Args>
	HEMeshTraits_V<Traits>* HEMesh<Traits>::CollapseEdge(E* e, Args&&... args) {
		assert(IsCollapsable(e) && "use IsCollapsable before CollapseEdge");

		auto* he01 = e->HalfEdge();
		auto* he10 = he01->Pair();

		auto* v0 = he01->Origin();
		auto* v1 = he01->End();

		if (v0->IsOnBoundary() && v1->IsOnBoundary() && !e->IsOnBoundary())
			return nullptr;

		if (v0->Degree() == 1) {
			EraseVertex(v0);
			return v1;
		}

		if (v1->Degree() == 1) {
			EraseVertex(v1);
			return v0;
		}

		auto* p01 = he01->Polygon();
		auto* p10 = he10->Polygon();
		size_t p01D = he01->PolygonDegree();
		size_t p10D = he10->PolygonDegree();

		// set v
		auto* v = New<V>(std::forward<Args>(args)...);
		if(he01->Pre()->Pair()->Polygon() != p10)
			v->SetHalfEdge(he01->Pre()->Pair());
		else
			v->SetHalfEdge(he10->Pre()->Pair());

		for (auto* he : v0->OutHalfEdges())
			he->SetOrigin(v);
		for (auto* he : v1->OutHalfEdges())
			he->SetOrigin(v);

		if (p01D == 3) { // p01->Degree() == 3
			auto* he01Next = he01->Next();
			auto* he01Pre = he01->Pre();
			auto* he01NextPair = he01Next->Pair();
			auto* he01PrePair = he01Pre->Pair();

			auto* v2 = he01Pre->Origin();
			if (v2->HalfEdge() == he01Pre)
				v2->SetHalfEdge(he01NextPair);

			auto* newE = New<E>();

			he01NextPair->SetPair(he01PrePair);
			he01NextPair->SetEdge(newE);
			he01PrePair->SetPair(he01NextPair);
			he01PrePair->SetEdge(newE);
			newE->SetHalfEdge(he01NextPair);

			Delete<E>(he01Next->Edge());
			Delete<E>(he01Pre->Edge());
			Delete<H>(he01Next);
			Delete<H>(he01Pre);
			if (!P::IsBoundary(p01))
				Delete<P>(p01);
		}
		else { //p01->Degree >= 4
			if (!P::IsBoundary(p01) && p01->HalfEdge() == he01)
				p01->SetHalfEdge(he01->Next());
			he01->Pre()->SetNext(he01->Next());
		}

		if (p10D == 3) { // p10->Degree() == 3
			auto* he10Next = he10->Next();
			auto* he10Pre = he10->Pre();
			auto* he10NextPair = he10Next->Pair();
			auto* he10PrePair = he10Pre->Pair();

			auto* v2 = he10Pre->Origin();
			if (v2->HalfEdge() == he10Pre)
				v2->SetHalfEdge(he10NextPair);

			auto* newE = New<E>();

			he10NextPair->SetPair(he10PrePair);
			he10NextPair->SetEdge(newE);
			he10PrePair->SetPair(he10NextPair);
			he10PrePair->SetEdge(newE);
			newE->SetHalfEdge(he10NextPair);

			Delete<E>(he10Next->Edge());
			Delete<E>(he10Pre->Edge());
			Delete<H>(he10Next);
			Delete<H>(he10Pre);
			if (!P::IsBoundary(p10))
				Delete<P>(p10);
		}
		else { //p10->Degree >= 4
			if (!P::IsBoundary(p10) && p10->HalfEdge() == he10)
				p10->SetHalfEdge(he10->Next());
			he10->Pre()->SetNext(he10->Next());
		}

		Delete<V>(v0);
		Delete<V>(v1);
		Delete<H>(he01);
		Delete<H>(he10);
		Delete<E>(e);

		return v;
	}
}

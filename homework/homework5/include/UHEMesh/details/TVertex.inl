#pragma once

namespace Ubpa {
	template<bool IsConst, typename Traits>
	class VertexAdjEdgeView : public TLoopViewBase<VertexAdjEdgeView<IsConst, Traits>, HEMeshTraits_PtrH<IsConst, Traits>, HEMeshTraits_PtrE<IsConst, Traits>> {
		using PtrH = HEMeshTraits_PtrH<IsConst, Traits>;
		using PtrE = HEMeshTraits_PtrE<IsConst, Traits>;
		friend TLoopViewBase<VertexAdjEdgeView<IsConst, Traits>, PtrH, PtrE>;
		static PtrE ToValue(PtrH he) { return he->Edge(); }
		static PtrH Next(PtrH he) { return he->RotateNext(); }
	public:
		using TLoopViewBase<VertexAdjEdgeView<IsConst, Traits>, PtrH, PtrE>::TLoopViewBase;
	};

	template<bool IsConst, typename Traits>
	class VertexAdjVertexView : public TLoopViewBase<VertexAdjVertexView<IsConst, Traits>, HEMeshTraits_PtrH<IsConst, Traits>, HEMeshTraits_PtrV<IsConst, Traits>> {
		using PtrH = HEMeshTraits_PtrH<IsConst, Traits>;
		using PtrV = HEMeshTraits_PtrV<IsConst, Traits>;
		friend TLoopViewBase<VertexAdjVertexView<IsConst, Traits>, PtrH, PtrV>;
		static PtrV ToValue(PtrH he) { return he->End(); }
		static PtrH Next(PtrH he) { return he->RotateNext(); }
	public:
		using TLoopViewBase<VertexAdjVertexView<IsConst, Traits>, PtrH, PtrV>::TLoopViewBase;
	};

	template<typename Traits>
	bool TVertex<Traits>::IsOnBoundary() const noexcept {
		if (IsIsolated())
			return true;

		for (auto he : HalfEdge()->RotateNextLoop()) {
			if (he->IsOnBoundary())
				return true;
		}

		return false;
	}

	template<typename Traits>
	HalfEdgeRotateNextView<false, Traits> TVertex<Traits>::OutHalfEdges() {
		if (IsIsolated())
			return {};

		return { HalfEdge(), HalfEdge() };
	}

	template<typename Traits>
	HalfEdgeRotateNextView<true, Traits> TVertex<Traits>::OutHalfEdges() const {
		const auto rst = const_cast<TVertex*>(this)->OutHalfEdges();
		return *reinterpret_cast<const HalfEdgeRotateNextView<true, Traits>*>(&rst);
	}

	template<typename Traits>
	VertexAdjEdgeView<false, Traits> TVertex<Traits>::AdjEdges() {
		if (IsIsolated())
			return {};

		return { HalfEdge(), HalfEdge() };
	}

	template<typename Traits>
	VertexAdjEdgeView<true, Traits> TVertex<Traits>::AdjEdges() const {
		const auto rst = const_cast<TVertex*>(this)->AdjEdges();
		return *reinterpret_cast<const VertexAdjEdgeView<true, Traits>*>(&rst);
	}

	template<typename Traits>
	VertexAdjVertexView<false, Traits> TVertex<Traits>::AdjVertices() {
		if (IsIsolated())
			return {};

		return { HalfEdge(), HalfEdge() };
	}

	template<typename Traits>
	VertexAdjVertexView<true, Traits> TVertex<Traits>::AdjVertices() const {
		const auto rst = const_cast<TVertex*>(this)->AdjVertices();
		return *reinterpret_cast<const VertexAdjVertexView<true, Traits>*>(&rst);
	}

	template<typename Traits>
	std::size_t TVertex<Traits>::Degree() const noexcept {
		std::size_t cnt = 0;
		for (auto* he : OutHalfEdges())
			++cnt;
		return cnt;
	}

	template<typename Traits>
	HEMeshTraits_H<Traits>* TVertex<Traits>::FindFreeIncident() noexcept {
		if (IsIsolated())
			return nullptr;

		auto* begin = HalfEdge()->Pair();
		auto* he = begin;
		do {
			if (he->IsFree())
				return he;
			he = he->Next()->Pair();
		} while (he != begin);

		return nullptr;
	}

	template<typename Traits>
	HEMeshTraits_H<Traits>* TVertex<Traits>::HalfEdgeTo(V* end) noexcept {
		if (IsIsolated())
			return nullptr;

		auto* begin = HalfEdge();
		auto* he = begin;
		do {
			if (he->End() == end)
				return he;
			he = he->RotateNext();
		} while (he != begin);

		return nullptr;
	}

	template<typename Traits>
	HEMeshTraits_E<Traits>* TVertex<Traits>::EdgeWith(V* v) noexcept {
		auto* he = HalfEdgeTo(v);
		if (!he)
			return nullptr;

		return he->Edge();
	}
}

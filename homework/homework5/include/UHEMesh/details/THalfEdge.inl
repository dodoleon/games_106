#pragma once

namespace Ubpa {
	template<bool IsConst, typename Traits>
	class HalfEdgeNextView : public TLoopViewBase<HalfEdgeNextView<IsConst, Traits>, HEMeshTraits_PtrH<IsConst, Traits>, HEMeshTraits_PtrH<IsConst, Traits>> {
		using PtrH = HEMeshTraits_PtrH<IsConst, Traits>;
		friend TLoopViewBase<HalfEdgeNextView<IsConst, Traits>, PtrH, PtrH>;
		static PtrH ToValue(PtrH he) { return he; }
		static PtrH Next(PtrH he) { return he->Next(); }
	public:
		using TLoopViewBase<HalfEdgeNextView<IsConst, Traits>, PtrH, PtrH>::TLoopViewBase;
	};

	template<bool IsConst, typename Traits>
	class HalfEdgeRotateNextView : public TLoopViewBase<HalfEdgeRotateNextView<IsConst, Traits>, HEMeshTraits_PtrH<IsConst, Traits>, HEMeshTraits_PtrH<IsConst, Traits>> {
		using PtrH = HEMeshTraits_PtrH<IsConst, Traits>;
		friend TLoopViewBase<HalfEdgeRotateNextView<IsConst, Traits>, PtrH, PtrH>;
		static PtrH ToValue(PtrH he) { return he; }
		static PtrH Next(PtrH he) { return he->RotateNext(); }
	public:
		using TLoopViewBase<HalfEdgeRotateNextView<IsConst, Traits>, PtrH, PtrH>::TLoopViewBase;
	};

	template<typename Traits>
	HEMeshTraits_H<Traits>* THalfEdge<Traits>::Pre() noexcept {
		auto* he = this;
		auto* next = he->Next();
		do {
			he = next;
			next = he->Next();
		} while (next != this);
		return he->This();
	}

	template<typename Traits>
	void THalfEdge<Traits>::Init(H* next, H* pair, V* v, E* e, P* p) noexcept {
		assert(next != nullptr && pair != nullptr && v != nullptr && e != nullptr);
		this->next = next;
		this->pair = pair;
		origin = v;
		edge = e;
		polygon = p;
	}

	template<typename Traits>
	void THalfEdge<Traits>::Reset() noexcept {
		next = nullptr;
		pair = nullptr;
		origin = nullptr;
		edge = nullptr;
		polygon = nullptr;
	}

	template<typename Traits>
	HEMeshTraits_H<Traits>* THalfEdge<Traits>::FindFreeIncident(H* begin, H* end) {
		assert(begin->End() == end->End());

		for (auto* he = begin; he != end; he = he->Next()->Pair()) {
			if (he->IsFree())
				return he->This();
		}

		return nullptr;
	}

	template<typename Traits>
	bool THalfEdge<Traits>::MakeAdjacent(H* inHE, H* outHE) {
		assert(inHE->End() == outHE->Origin());

		if (inHE->Next() == outHE)
			return true;

		auto* freeIn = FindFreeIncident(outHE->Pair(), inHE);
		if (freeIn == nullptr)
			return false;

		auto* inNext = inHE->Next();
		auto* outPre = outHE->Pre();
		auto* freeInNext = freeIn->Next();

		inHE->SetNext(outHE);
		freeIn->SetNext(inNext);
		outPre->SetNext(freeInNext);

		return true;
	}

	template<typename Traits>
	std::size_t THalfEdge<Traits>::PolygonDegree() const noexcept {
		std::size_t cnt = 0;
		for (const auto& he : NextLoop())
			++cnt;
		return cnt;
	}
}

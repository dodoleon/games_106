#pragma once

#include "TLoopView.h"

namespace Ubpa {
	template<typename Traits>
	class THalfEdge {
	public:
		using V = HEMeshTraits_V<Traits>;
		using E = HEMeshTraits_E<Traits>;
		using P = HEMeshTraits_P<Traits>;
		using H = HEMeshTraits_H<Traits>;

	public:
		H* Next() noexcept { return next; }
		H* Pair() noexcept { return pair; }
		V* Origin() noexcept { return origin; }
		E* Edge() noexcept { return edge; }
		P* Polygon() noexcept { return polygon; }
		V* End() noexcept { return Next()->Origin(); }
		H* Pre() noexcept;
		H* RotateNext() noexcept { return Pair()->Next(); }
		H* RotatePre() noexcept { return Pre()->Pair(); }

		const H* Next() const noexcept { return const_cast<H*>(This())->Next(); }
		const H* Pair() const noexcept { return const_cast<H*>(This())->Pair(); }
		const V* Origin() const noexcept { return const_cast<H*>(This())->Origin(); }
		const E* Edge() const noexcept { return const_cast<H*>(This())->Edge(); }
		const P* Polygon() const noexcept { return const_cast<H*>(This())->Polygon(); }
		const V* End() const noexcept { return const_cast<H*>(This())->End(); }
		const H* Pre() const noexcept { return const_cast<H*>(This())->Pre(); }
		const H* RotateNext() const noexcept { return const_cast<H*>(This())->RotateNext(); }
		const H* RotatePre() const noexcept { return const_cast<H*>(This())->RotatePre(); }

		void SetNext(H* he) noexcept { next = he; }
		void SetPair(H* he) noexcept { pair = he; }
		void SetOrigin(V* v) noexcept { origin = v; }
		void SetEdge(E* e) noexcept { edge = e; }
		void SetPolygon(P* p) noexcept { polygon = p; }

		// next, pair, v, e can't be nullptr
		// p can be nullptr
		void Init(H* next, H* pair, V* v, E* e, P* p) noexcept;

		void Reset() noexcept;

		// polygon == nullptr
		bool IsOnBoundary() const noexcept { return !polygon; }

		// [begin, end), begin == end <=> loop
		static HalfEdgeNextView<false, Traits> NextBetween(H* begin, H* end) { return { begin, end }; }
		// [this, end), NextBetween(this, end);
		HalfEdgeNextView<false, Traits> NextTo(H* end) { return NextBetween(This(), end); }
		// NextBetween(this, this), a loop from this to this
		HalfEdgeNextView<false, Traits> NextLoop() { return NextTo(This()); }

		// [begin, end), begin == end <=> loop
		static HalfEdgeNextView<true, Traits> NextBetween(const H* begin, const H* end) { return { begin, end }; }
		// [this, end), NextBetween(this, end);
		HalfEdgeNextView<true, Traits> NextTo(const H* end) const { return NextBetween(This(), end); }
		// NextBetween(this, this), a loop from this to this
		HalfEdgeNextView<true, Traits> NextLoop() const { return NextTo(This()); }

		// [begin, end), begin == end <=> loop
		static HalfEdgeRotateNextView<true, Traits> RotateNextBetween(const H* begin, const H* end) { return { begin, end }; }
		// [this, end), RotateNextBetween(this, end);
		HalfEdgeRotateNextView<true, Traits> RotateNextTo(const H* end) const { return RotateNextBetween(This(), end); }
		// RotateNextBetween(this, this), a loop from this to this
		HalfEdgeRotateNextView<true, Traits> RotateNextLoop() const { return RotateNextTo(This()); }

		// [begin, end), begin == end <=> loop
		static HalfEdgeRotateNextView<false, Traits> RotateNextBetween(H* begin, H* end) { return { begin, end }; }
		// [this, end), RotateNextBetween(this, end);
		HalfEdgeRotateNextView<false, Traits> RotateNextTo(H* end) { return RotateNextBetween(This(), end); }
		// RotateNextBetween(this, this), a loop from this to this
		HalfEdgeRotateNextView<false, Traits> RotateNextLoop() { return RotateNextTo(This()); }

		std::size_t PolygonDegree() const noexcept;

	private:
		friend HEMesh<Traits>;
		friend TVertex<Traits>;

		bool IsFree() const noexcept { return !polygon; }
		static bool MakeAdjacent(H* inHE, H* outHE);
		static H* FindFreeIncident(H* begin, H* end);

		H* This() noexcept { return static_cast<H*>(this); }
		const H* This() const noexcept { return static_cast<const H*>(this); }

		H* next{ nullptr };
		H* pair{ nullptr };

		V* origin{ nullptr };
		E* edge{ nullptr };
		P* polygon{ nullptr };
	};
}

#include "details/THalfEdge.inl"

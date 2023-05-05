#pragma once

#include "../HEMeshTraits.h"

#include <vector>
#include <set>
#include <type_traits>

#include <cassert>

namespace Ubpa {
	template<typename V, typename E, typename P, typename H> struct HEMeshTraits;

	template<typename Traits> class THalfEdge;
	template<typename Traits> class TVertex;
	template<typename Traits> class TEdge;
	template<typename Traits> class TPolygon;
	template<typename Traits> class HEMesh;

	template<bool IsConst, typename Traits> class HalfEdgeNextView;
	template<bool IsConst, typename Traits> class HalfEdgeRotateNextView;

	template<bool IsConst, typename Traits> class VertexAdjEdgeView;
	template<bool IsConst, typename Traits> class VertexAdjVertexView;

	template <typename E, typename P, typename H> class EmptyV;
	template <typename V, typename P, typename H> class EmptyE;
	template <typename V, typename E, typename H> class EmptyP;
	template <typename V, typename E, typename P> class EmptyH;

	template <typename P, typename H> class EmptyVE_V;
	template <typename P, typename H> class EmptyVE_E;

	template <typename E, typename H> class EmptyVP_V;
	template <typename E, typename H> class EmptyVP_P;

	template <typename E, typename P> class EmptyVH_V;
	template <typename E, typename P> class EmptyVH_H;

	template <typename V, typename H> class EmptyEP_E;
	template <typename V, typename H> class EmptyEP_P;

	template <typename V, typename P> class EmptyEH_E;
	template <typename V, typename P> class EmptyEH_H;

	template <typename V, typename E> class EmptyPH_P;
	template <typename V, typename E> class EmptyPH_H;

	template <typename H> class EmptyVEP_V;
	template <typename H> class EmptyVEP_E;
	template <typename H> class EmptyVEP_P;

	template <typename P> class EmptyVEH_V;
	template <typename P> class EmptyVEH_E;
	template <typename P> class EmptyVEH_H;

	template <typename E> class EmptyVPH_V;
	template <typename E> class EmptyVPH_P;
	template <typename E> class EmptyVPH_H;

	template <typename V> class EmptyEPH_E;
	template <typename V> class EmptyEPH_P;
	template <typename V> class EmptyEPH_H;

	class EmptyVEPH_V;
	class EmptyVEPH_E;
	class EmptyVEPH_P;
	class EmptyVEPH_H;
}

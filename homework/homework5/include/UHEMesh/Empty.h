#pragma once

#include "THalfEdge.h"
#include "TVertex.h"
#include "TEdge.h"
#include "TPolygon.h"

namespace Ubpa {
	template <typename E, typename P, typename H> using HEMeshTraits_EmptyV = HEMeshTraits<EmptyV<E, P, H>, E, P, H>;
	template <typename V, typename P, typename H> using HEMeshTraits_EmptyE = HEMeshTraits<V, EmptyE<V, P, H>, P, H>;
	template <typename V, typename E, typename H> using HEMeshTraits_EmptyP = HEMeshTraits<V, E, EmptyP<V, E, H>, H>;
	template <typename V, typename E, typename P> using HEMeshTraits_EmptyH = HEMeshTraits<V, E, P, EmptyH<V, E, P>>;

	template <typename P, typename H> using HEMeshTraits_EmptyVE = HEMeshTraits<EmptyVE_V<P, H>, EmptyVE_E<P, H>, P, H>;
	template <typename E, typename H> using HEMeshTraits_EmptyVP = HEMeshTraits<EmptyVP_V<E, H>, E, EmptyVP_P<E, H>, H>;
	template <typename E, typename P> using HEMeshTraits_EmptyVH = HEMeshTraits<EmptyVH_V<E, P>, E, P, EmptyVH_H<E, P>>;
	template <typename V, typename H> using HEMeshTraits_EmptyEP = HEMeshTraits<V, EmptyEP_E<V, H>, EmptyEP_P<V, H>, H>;
	template <typename V, typename P> using HEMeshTraits_EmptyEH = HEMeshTraits<V, EmptyEH_E<V, P>, P, EmptyEH_H<V, P>>;
	template <typename V, typename E> using HEMeshTraits_EmptyPH = HEMeshTraits<V, E, EmptyPH_P<V, E>, EmptyPH_H<V, E>>;

	template <typename H> using HEMeshTraits_EmptyVEP = HEMeshTraits<EmptyVEP_V<H>, EmptyVEP_E<H>, EmptyVEP_P<H>, H>;
	template <typename P> using HEMeshTraits_EmptyVEH = HEMeshTraits<EmptyVEH_V<P>, EmptyVEH_E<P>, P, EmptyVEH_H<P>>;
	template <typename E> using HEMeshTraits_EmptyVPH = HEMeshTraits<EmptyVPH_V<E>, E, EmptyVPH_P<E>, EmptyVPH_P<E>>;
	template <typename V> using HEMeshTraits_EmptyEPH = HEMeshTraits<V, EmptyEPH_E<V>, EmptyEPH_P<V>, EmptyEPH_H<V>>;

	using HEMeshTraits_EmptyVEPH = HEMeshTraits<EmptyVEPH_V, EmptyVEPH_E, EmptyVEPH_P, EmptyVEPH_H>;

	template <typename E, typename P, typename H> class EmptyV : public TVertex  <HEMeshTraits_EmptyV<E, P, H>> {};
	template <typename V, typename P, typename H> class EmptyE : public TEdge    <HEMeshTraits_EmptyE<V, P, H>> {};
	template <typename V, typename E, typename H> class EmptyP : public TPolygon <HEMeshTraits_EmptyP<V, E, H>> {};
	template <typename V, typename E, typename P> class EmptyH : public THalfEdge<HEMeshTraits_EmptyH<V, E, P>> {};

	template <typename P, typename H> class EmptyVE_V : public TVertex  <HEMeshTraits_EmptyVE<P, H>> {};
	template <typename P, typename H> class EmptyVE_E : public TEdge    <HEMeshTraits_EmptyVE<P, H>> {};
	template <typename E, typename H> class EmptyVP_V : public TVertex  <HEMeshTraits_EmptyVP<E, H>> {};
	template <typename E, typename H> class EmptyVP_P : public TPolygon <HEMeshTraits_EmptyVP<E, H>> {};
	template <typename E, typename P> class EmptyVH_V : public TVertex  <HEMeshTraits_EmptyVH<E, P>> {};
	template <typename E, typename P> class EmptyVH_P : public THalfEdge<HEMeshTraits_EmptyVH<E, P>> {};
	template <typename V, typename H> class EmptyEP_E : public TEdge    <HEMeshTraits_EmptyEP<V, H>> {};
	template <typename V, typename H> class EmptyEP_P : public TPolygon <HEMeshTraits_EmptyEP<V, H>> {};
	template <typename V, typename P> class EmptyEH_E : public TEdge    <HEMeshTraits_EmptyEH<V, P>> {};
	template <typename V, typename P> class EmptyEH_H : public THalfEdge<HEMeshTraits_EmptyEH<V, P>> {};
	template <typename V, typename E> class EmptyPH_P : public TPolygon <HEMeshTraits_EmptyPH<V, E>> {};
	template <typename V, typename E> class EmptyPH_H : public THalfEdge<HEMeshTraits_EmptyPH<V, E>> {};

	template <typename H> class EmptyVEP_V : public TVertex  <HEMeshTraits_EmptyVEP<H>> {};
	template <typename H> class EmptyVEP_E : public TEdge    <HEMeshTraits_EmptyVEP<H>> {};
	template <typename H> class EmptyVEP_P : public TPolygon <HEMeshTraits_EmptyVEP<H>> {};
	template <typename P> class EmptyVEH_V : public TVertex  <HEMeshTraits_EmptyVEH<P>> {};
	template <typename P> class EmptyVEH_E : public TEdge    <HEMeshTraits_EmptyVEH<P>> {};
	template <typename P> class EmptyVEH_H : public THalfEdge<HEMeshTraits_EmptyVEH<P>> {};
	template <typename E> class EmptyVPH_V : public TVertex  <HEMeshTraits_EmptyVPH<E>> {};
	template <typename E> class EmptyVPH_P : public TPolygon <HEMeshTraits_EmptyVPH<E>> {};
	template <typename E> class EmptyVPH_H : public THalfEdge<HEMeshTraits_EmptyVPH<E>> {};
	template <typename V> class EmptyEPH_E : public TEdge    <HEMeshTraits_EmptyEPH<V>> {};
	template <typename V> class EmptyEPH_P : public TPolygon <HEMeshTraits_EmptyEPH<V>> {};
	template <typename V> class EmptyEPH_H : public THalfEdge<HEMeshTraits_EmptyEPH<V>> {};

	class EmptyVEPH_V : public TVertex  <HEMeshTraits_EmptyVEPH> {};
	class EmptyVEPH_E : public TEdge    <HEMeshTraits_EmptyVEPH> {};
	class EmptyVEPH_P : public TPolygon <HEMeshTraits_EmptyVEPH> {};
	class EmptyVEPH_H : public THalfEdge<HEMeshTraits_EmptyVEPH> {};
}

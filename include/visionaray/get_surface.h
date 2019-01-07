// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_GET_SURFACE_H
#define VSNRAY_GET_SURFACE_H 1

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

#include "math/array.h"
#include "texture/texture_traits.h"
#include "bvh.h"
#include "generic_primitive.h"
#include "get_color.h"
#include "get_normal.h"
#include "get_shading_normal.h"
#include "get_tex_coord.h"
#include "prim_traits.h"
#include "surface.h"
#include "tags.h"

namespace visionaray
{
namespace detail
{

//-------------------------------------------------------------------------------------------------
// Helper functions
//

// deduce simd surface type from params -------------------

template <typename Params, typename T>
struct simd_decl_surface
{
private:

    enum { Size_ = simd::num_elements<T>::value };
    using N_     = typename Params::normal_type;
    using C_     = typename Params::color_type;
    using M_     = typename Params::material_type;

public:
    using type = surface<
        decltype(simd::pack(std::declval<array<N_, Size_>>())),
        decltype(simd::pack(std::declval<array<C_, Size_>>())),
        decltype(simd::pack(std::declval<array<M_, Size_>>()))
        >;

    using array_type = array<surface<N_, C_, M_>, Size_>;
};


//-------------------------------------------------------------------------------------------------
// Get primitive from params
//

template <
    typename Params,
    typename HR,
    typename P = typename Params::primitive_type,
    typename = typename std::enable_if<!is_any_bvh<P>::value>::type
    >
VSNRAY_FUNC
inline P const& get_prim(Params const& params, HR const& hr)
{
    return params.prims.begin[hr.prim_id];
}

// overload for BVHs
template <
    typename Params,
    typename R,
    typename Base,
    typename P = typename Params::primitive_type,
    typename = typename std::enable_if<is_any_bvh<P>::value>::type,
    typename = typename std::enable_if<
                !is_any_bvh_inst<typename P::primitive_type>::value>::type // but not BVH of instances!
    >
VSNRAY_FUNC
inline typename P::primitive_type const& get_prim(Params const& params, hit_record_bvh<R, Base> const& hr)
{
    VSNRAY_UNUSED(params);
    VSNRAY_UNUSED(hr);

    // TODO: iterate over list of BVHs and find the right one!
    return params.prims.begin[0].primitive(hr.prim_id);
}

// overload for BVHs of instances
template <
    typename Params,
    typename R,
    typename Base,
    typename P = typename Params::primitive_type,
    typename = typename std::enable_if<is_any_bvh<P>::value>::type, // is BVH ...
    typename = typename std::enable_if<
                is_any_bvh_inst<typename P::primitive_type>::value>::type, // ... of instances!
    typename X = void
    >
VSNRAY_FUNC
inline auto get_prim(Params const& params, hit_record_bvh<R, Base> const& hr)
    -> typename P::primitive_type::primitive_type const&
{
    // Assume we only have one top-level BVH (TODO?)
    return params.prims.begin[0].primitive(hr.primitive_list_index).primitive(hr.primitive_list_index_inst);
}


//-------------------------------------------------------------------------------------------------
// Struct containing both the geometric and the shading normal
//

template <typename V>
struct normal_pair
{
    V geometric_normal;
    V shading_normal;
};


//-------------------------------------------------------------------------------------------------
// Get geometric normal and shading normal
//

template <
    typename Params,
    typename HR,
    typename Primitive = typename Params::primitive_type
    >
VSNRAY_FUNC
inline auto get_normals(Params const& params, HR const& hr)
    -> normal_pair<decltype(get_normal(params.geometric_normals, hr, get_prim(params, hr)))>
{
    auto const& prim = get_prim(params, hr);

    auto const& gns = params.geometric_normals;
    auto const& sns = params.shading_normals;

    auto gn = gns ? get_normal(gns, hr, prim) : get_normal(hr, prim);
    auto sn = sns ? get_shading_normal(sns, hr, prim, typename Params::normal_binding{}) : gn;

    return { gn, sn };
}


//-------------------------------------------------------------------------------------------------
// Sample textures with range check
//

template <typename HR, typename Params>
VSNRAY_FUNC
inline typename Params::color_type get_tex_color(
        HR const&                      hr,
        Params const&                  params,
        std::integral_constant<int, 1> /* */
        )
{
    using C = typename Params::color_type;

    if (params.tex_coords != nullptr)
    {
        auto coord = get_tex_coord(params.tex_coords, hr, get_prim(params, hr));

        auto const& tex = params.textures[hr.geom_id];
        return C(tex1D(tex, coord));
    }
    else
    {
        return C(1.0);
    }
}

template <typename HR, typename Params>
VSNRAY_FUNC
inline typename Params::color_type get_tex_color(
        HR const&                      hr,
        Params const&                  params,
        std::integral_constant<int, 2> /* */
        )
{
    using C = typename Params::color_type;

    if (params.tex_coords != nullptr)
    {
        auto coord = get_tex_coord(params.tex_coords, hr, get_prim(params, hr));

        auto const& tex = params.textures[hr.geom_id];
        return C(tex2D(tex, coord));
    }
    else
    {
        return C(1.0);
    }
}

template <typename HR, typename Params>
VSNRAY_FUNC
inline typename Params::color_type get_tex_color(
        HR const&                      hr,
        Params const&                  params,
        std::integral_constant<int, 3> /* */
        )
{
    using C = typename Params::color_type;

    if (params.tex_coords != nullptr)
    {
        auto coord = get_tex_coord(params.tex_coords, hr, get_prim(params, hr));

        auto const& tex = params.textures[hr.geom_id];
        return C(tex3D(tex, coord));
    }
    else
    {
        return C(1.0);
    }
}


//-------------------------------------------------------------------------------------------------
//
//

template <typename HR, typename Params>
VSNRAY_FUNC
inline auto get_surface_impl(
        has_no_colors_tag   /* */,
        HR const&           hr,
        Params const&       params
        )
    -> surface<
            typename Params::normal_type,
            typename Params::color_type,
            typename Params::material_type
            >
{
    auto ns = get_normals(params, hr);
    auto tc = get_tex_color(
                    hr,
                    params,
                    std::integral_constant<int, texture_dimensions<typename Params::texture_type>::value>{}
                    );

    return {
        ns.geometric_normal,
        ns.shading_normal,
        tc,
        params.materials[hr.geom_id]
        };
}

template <typename HR, typename Params>
VSNRAY_FUNC
inline auto get_surface_impl(
        has_colors_tag      /* */,
        HR const&           hr,
        Params const&       params
        )
    -> surface<
            typename Params::normal_type,
            typename Params::color_type,
            typename Params::material_type
            >
{
    using P = typename Params::primitive_type;

    auto ns    = get_normals(params, hr);
    auto color = get_color(params.colors, hr, P{}, typename Params::color_binding{});
    auto tc    = get_tex_color(
                        hr,
                        params,
                        std::integral_constant<int, texture_dimensions<typename Params::texture_type>::value>{}
                        );

    return {
        ns.geometric_normal,
        ns.shading_normal,
        color * tc,
        params.materials[hr.geom_id]
        };
}


//-------------------------------------------------------------------------------------------------
// SIMD
//

template <
    typename ColorsTag,
    typename HR,
    typename Params,
    typename = typename std::enable_if<simd::is_simd_vector<typename HR::scalar_type>::value>::type
    >
VSNRAY_FUNC
inline auto get_surface_impl(
        ColorsTag     /* */,
        HR const&     hr,
        Params const& params
        )
    -> typename simd_decl_surface<Params, typename HR::scalar_type>::type
{
    using T = typename HR::scalar_type;

    auto hrs = unpack(hr);

    typename simd_decl_surface<Params, T>::array_type surfs = {};

    for (int i = 0; i < simd::num_elements<T>::value; ++i)
    {
        if (hrs[i].hit)
        {
            surfs[i] = get_surface_impl(
                    ColorsTag{},
                    hrs[i],
                    params
                    );
        }
    }

    return simd::pack(surfs);
}

} // detail


template <typename HR, typename Params>
VSNRAY_FUNC
inline auto get_surface(HR const& hr, Params const& p)
    -> decltype( detail::get_surface_impl(
            detail::has_colors<Params>{},
            hr,
            p
            ) )
{
    return detail::get_surface_impl(
            detail::has_colors<Params>{},
            hr,
            p
            );
}

} // visionaray

#endif // VSNRAY_SURFACE_H

// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_TEXTURE_DETAIL_FILTER_CUBIC_H
#define VSNRAY_TEXTURE_DETAIL_FILTER_CUBIC_H 1

#include <array>

#include <visionaray/math/detail/math.h>
#include <visionaray/math/vector.h>

#include "common.h"

namespace visionaray
{
namespace detail
{

//-------------------------------------------------------------------------------------------------
// 1D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename TexSize,
    typename W0,
    typename W1,
    typename W2,
    typename W3
    >
inline ReturnT cubic(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<1, FloatT>                       coord,
        TexSize                                 texsize,
        std::array<tex_address_mode, 1> const&  address_mode,
        W0                                      w0,
        W1                                      w1,
        W2                                      w2,
        W3                                      w3
        )
{
    auto coord1 = map_tex_coord(
            coord[0] - FloatT(1.5) / FloatT(texsize[0]),
            texsize[0],
            address_mode
            );

    auto coord2 = map_tex_coord(
            coord[0] - FloatT(0.5) / FloatT(texsize[0]),
            texsize[0],
            address_mode
            );

    auto coord3 = map_tex_coord(
            coord[0] + FloatT(0.5) / FloatT(texsize[0]),
            texsize[0],
            address_mode
            );

    auto coord4 = map_tex_coord(
            coord[0] + FloatT(1.5) / FloatT(texsize[0]),
            texsize[0],
            address_mode
            );

    decltype(convert_to_int(FloatT{})) pos[4] =
    {
        convert_to_int(coord1 * FloatT(texsize[0])),
        convert_to_int(coord2 * FloatT(texsize[0])),
        convert_to_int(coord3 * FloatT(texsize[0])),
        convert_to_int(coord4 * FloatT(texsize[0]))
    };

    auto u = (coord2 * FloatT(texsize[0])) - FloatT(pos[1]);

    auto sample = [&](int i) -> InternalT
    {
        return InternalT( point(tex, pos[i], ReturnT{}) );
    };

    return ReturnT(w0(u) * sample(0) + w1(u) * sample(1) + w2(u) * sample(2) + w3(u) * sample(3));
}


//-------------------------------------------------------------------------------------------------
// 2D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename TexSize,
    typename W0,
    typename W1,
    typename W2,
    typename W3
    >
inline ReturnT cubic(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<2, FloatT>                       coord,
        TexSize                                 texsize,
        std::array<tex_address_mode, 2> const&  address_mode,
        W0                                      w0,
        W1                                      w1,
        W2                                      w2,
        W3                                      w3
        )
{
    auto coord1 = map_tex_coord(
            coord - FloatT(1.5) / vector<2, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord2 = map_tex_coord(
            coord - FloatT(0.5) / vector<2, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord3 = map_tex_coord(
            coord + FloatT(0.5) / vector<2, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord4 = map_tex_coord(
            coord + FloatT(1.5) / vector<2, FloatT>(texsize),
            texsize,
            address_mode
            );

    vector<2, decltype(convert_to_int(FloatT{}))> pos[4] =
    {
        convert_to_int(coord1 * vector<2, FloatT>(texsize)),
        convert_to_int(coord2 * vector<2, FloatT>(texsize)),
        convert_to_int(coord3 * vector<2, FloatT>(texsize)),
        convert_to_int(coord4 * vector<2, FloatT>(texsize))
    };

    auto uv = (coord2 * vector<2, FloatT>(texsize)) - vector<2, FloatT>(pos[1]);

    auto sample = [&](int i, int j) -> InternalT
    {
        return InternalT( point(
                tex,
                linear_index(pos[i].x, pos[j].y, texsize),
                ReturnT{}
                ) );
    };

    auto f0 = w0(uv.x) * sample(0, 0) + w1(uv.x) * sample(1, 0) + w2(uv.x) * sample(2, 0) + w3(uv.x) * sample(3, 0);
    auto f1 = w0(uv.x) * sample(0, 1) + w1(uv.x) * sample(1, 1) + w2(uv.x) * sample(2, 1) + w3(uv.x) * sample(3, 1);
    auto f2 = w0(uv.x) * sample(0, 2) + w1(uv.x) * sample(1, 2) + w2(uv.x) * sample(2, 2) + w3(uv.x) * sample(3, 2);
    auto f3 = w0(uv.x) * sample(0, 3) + w1(uv.x) * sample(1, 3) + w2(uv.x) * sample(2, 3) + w3(uv.x) * sample(3, 3);

    return ReturnT(w0(uv.y) * f0 + w1(uv.y) * f1 + w2(uv.y) * f2 + w3(uv.y) * f3);
}


//-------------------------------------------------------------------------------------------------
// 3D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename TexSize,
    typename W0,
    typename W1,
    typename W2,
    typename W3
    >
inline ReturnT cubic(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<3, FloatT>                       coord,
        TexSize                                 texsize,
        std::array<tex_address_mode, 3> const&  address_mode,
        W0                                      w0,
        W1                                      w1,
        W2                                      w2,
        W3                                      w3
        )
{
    auto coord1 = map_tex_coord(
            coord - FloatT(1.5) / vector<3, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord2 = map_tex_coord(
            coord - FloatT(0.5) / vector<3, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord3 = map_tex_coord(
            coord + FloatT(0.5) / vector<3, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord4 = map_tex_coord(
            coord + FloatT(1.5) / vector<3, FloatT>(texsize),
            texsize,
            address_mode
            );

    vector<3, decltype(convert_to_int(FloatT{}))> pos[4] =
    {
        convert_to_int(coord1 * vector<3, FloatT>(texsize)),
        convert_to_int(coord2 * vector<3, FloatT>(texsize)),
        convert_to_int(coord3 * vector<3, FloatT>(texsize)),
        convert_to_int(coord4 * vector<3, FloatT>(texsize))
    };

    auto uvw = (coord2 * vector<3, FloatT>(texsize)) - vector<3, FloatT>(pos[1]);

    auto sample = [&](int i, int j, int k) -> InternalT
    {
        return InternalT( point(
                tex,
                linear_index(pos[i].x, pos[j].y, pos[k].z, texsize),
                ReturnT{}
                ) );
    };

    auto f00 = w0(uvw.x) * sample(0, 0, 0) + w1(uvw.x) * sample(1, 0, 0) + w2(uvw.x) * sample(2, 0, 0) + w3(uvw.x) * sample(3, 0, 0);
    auto f01 = w0(uvw.x) * sample(0, 1, 0) + w1(uvw.x) * sample(1, 1, 0) + w2(uvw.x) * sample(2, 1, 0) + w3(uvw.x) * sample(3, 1, 0);
    auto f02 = w0(uvw.x) * sample(0, 2, 0) + w1(uvw.x) * sample(1, 2, 0) + w2(uvw.x) * sample(2, 2, 0) + w3(uvw.x) * sample(3, 2, 0);
    auto f03 = w0(uvw.x) * sample(0, 3, 0) + w1(uvw.x) * sample(1, 3, 0) + w2(uvw.x) * sample(2, 3, 0) + w3(uvw.x) * sample(3, 3, 0);

    auto f04 = w0(uvw.x) * sample(0, 0, 1) + w1(uvw.x) * sample(1, 0, 1) + w2(uvw.x) * sample(2, 0, 1) + w3(uvw.x) * sample(3, 0, 1);
    auto f05 = w0(uvw.x) * sample(0, 1, 1) + w1(uvw.x) * sample(1, 1, 1) + w2(uvw.x) * sample(2, 1, 1) + w3(uvw.x) * sample(3, 1, 1);
    auto f06 = w0(uvw.x) * sample(0, 2, 1) + w1(uvw.x) * sample(1, 2, 1) + w2(uvw.x) * sample(2, 2, 1) + w3(uvw.x) * sample(3, 2, 1);
    auto f07 = w0(uvw.x) * sample(0, 3, 1) + w1(uvw.x) * sample(1, 3, 1) + w2(uvw.x) * sample(2, 3, 1) + w3(uvw.x) * sample(3, 3, 1);

    auto f08 = w0(uvw.x) * sample(0, 0, 2) + w1(uvw.x) * sample(1, 0, 2) + w2(uvw.x) * sample(2, 0, 2) + w3(uvw.x) * sample(3, 0, 2);
    auto f09 = w0(uvw.x) * sample(0, 1, 2) + w1(uvw.x) * sample(1, 1, 2) + w2(uvw.x) * sample(2, 1, 2) + w3(uvw.x) * sample(3, 1, 2);
    auto f10 = w0(uvw.x) * sample(0, 2, 2) + w1(uvw.x) * sample(1, 2, 2) + w2(uvw.x) * sample(2, 2, 2) + w3(uvw.x) * sample(3, 2, 2);
    auto f11 = w0(uvw.x) * sample(0, 3, 2) + w1(uvw.x) * sample(1, 3, 2) + w2(uvw.x) * sample(2, 3, 2) + w3(uvw.x) * sample(3, 3, 2);

    auto f12 = w0(uvw.x) * sample(0, 0, 3) + w1(uvw.x) * sample(1, 0, 3) + w2(uvw.x) * sample(2, 0, 3) + w3(uvw.x) * sample(3, 0, 3);
    auto f13 = w0(uvw.x) * sample(0, 1, 3) + w1(uvw.x) * sample(1, 1, 3) + w2(uvw.x) * sample(2, 1, 3) + w3(uvw.x) * sample(3, 1, 3);
    auto f14 = w0(uvw.x) * sample(0, 2, 3) + w1(uvw.x) * sample(1, 2, 3) + w2(uvw.x) * sample(2, 2, 3) + w3(uvw.x) * sample(3, 2, 3);
    auto f15 = w0(uvw.x) * sample(0, 3, 3) + w1(uvw.x) * sample(1, 3, 3) + w2(uvw.x) * sample(2, 3, 3) + w3(uvw.x) * sample(3, 3, 3);

    auto f0  = w0(uvw.y) * f00 + w1(uvw.y) * f01 + w2(uvw.y) * f02 + w3(uvw.y) * f03;
    auto f1  = w0(uvw.y) * f04 + w1(uvw.y) * f05 + w2(uvw.y) * f06 + w3(uvw.y) * f07;
    auto f2  = w0(uvw.y) * f08 + w1(uvw.y) * f09 + w2(uvw.y) * f10 + w3(uvw.y) * f11;
    auto f3  = w0(uvw.y) * f12 + w1(uvw.y) * f13 + w2(uvw.y) * f14 + w3(uvw.y) * f15;

    return ReturnT(w0(uvw.z) * f0 + w1(uvw.z) * f1 + w2(uvw.z) * f2 + w3(uvw.z) * f3);
}

} // detail
} // visionaray

#endif // VSNRAY_TEXTURE_DETAIL_FILTER_CUBIC_H

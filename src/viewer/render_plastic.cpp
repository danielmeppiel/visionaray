// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <visionaray/kernels.h> // make_kernel_params()

#include "render.h"

namespace visionaray
{

void render_plastic_cpp(
        index_bvh<basic_triangle<3, float>> const& bvh,
        aligned_vector<vec3> const&                geometric_normals,
        aligned_vector<vec3> const&                shading_normals,
        aligned_vector<vec2> const&                tex_coords,
        aligned_vector<plastic_t> const&           materials,
        aligned_vector<texture_t> const&           textures,
        aligned_vector<point_light<float>> const&  lights,
        unsigned                                   bounces,
        float                                      epsilon,
        vec4                                       bgcolor,
        vec4                                       ambient,
        host_device_rt&                            rt,
#if defined(__INTEL_COMPILER) || defined(__MINGW32__) || defined(__MINGW64__)
        tbb_sched<basic_ray<simd::float4>>&        sched,
#else
        tiled_sched<basic_ray<simd::float4>>&      sched,
#endif
        pinhole_camera&                            cam,
        unsigned&                                  frame_num,
        algorithm                                  algo,
        unsigned                                   ssaa_samples
        )
{
    using bvh_ref = index_bvh<basic_triangle<3, float>>::bvh_ref;

    aligned_vector<bvh_ref> primitives;

    primitives.push_back(bvh.ref());

    bool has_shading_normals = shading_normals.size() >= bvh.num_primitives() * 3;
    bool has_tex_coords = tex_coords.size() >= bvh.num_primitives() * 3;

    if (has_shading_normals && has_tex_coords)
    {
        auto kparams = make_kernel_params(
                normals_per_vertex_binding{},
                primitives.data(),
                primitives.data() + primitives.size(),
                shading_normals.data(),
                tex_coords.data(),
                materials.data(),
                textures.data(),
                lights.data(),
                lights.data() + lights.size(),
                bounces,
                epsilon,
                bgcolor,
                ambient
                );

        call_kernel( algo, sched, kparams, frame_num, ssaa_samples, cam, rt );
    }
    else if (has_shading_normals && !has_tex_coords)
    {
        auto kparams = make_kernel_params(
                normals_per_vertex_binding{},
                primitives.data(),
                primitives.data() + primitives.size(),
                shading_normals.data(),
                materials.data(),
                lights.data(),
                lights.data() + lights.size(),
                bounces,
                epsilon,
                bgcolor,
                ambient
                );

        call_kernel( algo, sched, kparams, frame_num, ssaa_samples, cam, rt );
    }
    else if (!has_shading_normals && has_tex_coords)
    {
        auto kparams = make_kernel_params(
                normals_per_face_binding{},
                primitives.data(),
                primitives.data() + primitives.size(),
                geometric_normals.data(),
                tex_coords.data(),
                materials.data(),
                textures.data(),
                lights.data(),
                lights.data() + lights.size(),
                bounces,
                epsilon,
                bgcolor,
                ambient
                );

        call_kernel( algo, sched, kparams, frame_num, ssaa_samples, cam, rt );
    }
    else if (!has_shading_normals && !has_tex_coords)
    {
        auto kparams = make_kernel_params(
                normals_per_face_binding{},
                primitives.data(),
                primitives.data() + primitives.size(),
                geometric_normals.data(),
                materials.data(),
                lights.data(),
                lights.data() + lights.size(),
                bounces,
                epsilon,
                bgcolor,
                ambient
                );

        call_kernel( algo, sched, kparams, frame_num, ssaa_samples, cam, rt );
    }
}

} // visionaray

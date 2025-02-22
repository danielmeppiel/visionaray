## [Unreleased]
### Added
- Added cylinder as built-in primitive.

### Changed
- Light sample struct has changed, to no longer store the position,
but instead, a direction and distance.
- An accumulation buffer was now added to the builtin render targets
where colors are blended in. For blending kernels, the accumulation
buffer pixel format needs to be specified.

## [0.3.0] - 2021-12-25
### Added
- macro VSNRAY_VERSION that can be used in addition to the other
version-related macros.
- Texture storage classes for bricked 3D texture accesses. Those
however didn't prove to be faster than row-major texture storage so
were only added for optional use with the base texture templates.
- "Ray tracing in one weekend" example using CUDA.
- Added an -spp flag to various examples.

### Changed
- Remove VSNRAY_NOT_COPYABLE() macro. Better to use C++11 delete
on copy ctor/assignment operator for this.
- Switched to C++14.
- Significantly overhauled CPU texture implementation.
- Rays now store tmin and tmax, therefore the public interfaces of
functions like any_hit() or is_closer() has changed. It is required
for users who construct rays themselves to fill those values
accordingly.

## [0.2.0] - 2021-02-19
### Added
- Added an -spp flag to viewer that is used by the path tracer.
- Support for HDR/radiance file format in visionaray-common library.
- Added an optional ground plane in viewer.
- Add BVH cost debug kernel to viewer, can be activated by pressing
KEY-4, or by passing -algorithm=costs on the command line.
- Set number of convergence frames rendered in viewer, pause and
resume using Key-Space.
- Experimental short stack BVH traversal with restart trail based on
Laine 2010 HPG paper (only on GPU).
- Phantom ray-hair intersector example demonstrating how to extend
the API with a custom curve primitive type.
- CPU-parallel BVH refitter. Refitting is used in juggler example.
- New "juggler" example demonstrating dynamic scenes and procedural
textures.

### Changed
- Pixel sampler types now store parameters indicating ssaa factors or
spp. Those are now evaluated at runtime.
- Don't use clock as LCG seed anymore; rather compute hashes based on
pixelID and frameID.
- BVH instances now store 4x3 instead of 4x4 transform matrices.
- vec3 is no longer aligned to 16 byte boundaries.
- Switched to Ubuntu 16.04 with Travis-CI. We no longer support
Ubuntu 14.04 and gcc 4.8 as that is too old for pbrtParser.
- pbrtParser is now a submodule instead of an external dependency
supplied by the user.

### Removed
- pixel_sampler::ssaa_type<N> was removed and bumped into
pixel_sampler::uniform_type.
- pixel_sampler::jittered_type was removed; the same can be achieved
by using jittered_blend_type and tampering with the blend parameters.

### Fixed
- Various smaller fixes to make things run smoothly on Apple silicon.
- Fixed linking with homebrew GLEW on macOS.

## [0.1.1] - 2020-06-22
### Fixed
- CUDA LBVH builder based on Karras' construction algorithm now
working. Fixed an index bug where the wrong prim-bounds were used to
expand leaf node bounding boxes.

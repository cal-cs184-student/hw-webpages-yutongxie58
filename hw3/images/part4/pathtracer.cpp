#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"


using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
  delete gridSampler;
  delete hemisphereSampler;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
  sampleBuffer.resize(width, height);
  sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  sampleBuffer.clear();
  sampleCountBuffer.clear();
  sampleBuffer.resize(0, 0);
  sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
  sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Vector3D
PathTracer::estimate_direct_lighting_hemisphere(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere.

  // Note: When comparing Cornel Box (CBxxx.dae) results to importance sampling, you may find the "glow" around the light source is gone.
  // This is totally fine: the area lights in importance sampling has directionality, however in hemisphere sampling we don't model this behaviour.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);

  // This is the same number of total samples as
  // estimate_direct_lighting_importance (outside of delta lights). We keep the
  // same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Vector3D L_out;

  // TODO (Part 3): Write your sampling loop here
  // TODO BEFORE YOU BEGIN
  // UPDATE `est_radiance_global_illumination` to return direct lighting instead of normal shading 

  for (int i = 0; i < num_samples; i++) {
    Vector3D wi = hemisphereSampler->get_sample();
    Vector3D wi_world = o2w * wi;

    Ray shadow_ray(hit_p, wi_world);
    shadow_ray.min_t = EPS_F;
    shadow_ray.max_t = INF_D;

    Intersection light_isect;
    if (bvh->intersect(shadow_ray, &light_isect)) {
      Vector3D emission = light_isect.bsdf->get_emission();
      L_out += emission * isect.bsdf->f(w_out, wi) * wi.z / (1.0 / (2.0 * PI));
    }
  }

  return L_out / num_samples;

}

Vector3D
PathTracer::estimate_direct_lighting_importance(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // To implement importance sampling, sample only from lights, not uniformly in
  // a hemisphere.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);
  Vector3D L_out;


  for (SceneLight *light : scene->lights) {
    int num_samples = light->is_delta_light() ? 1 : ns_area_light;
    Vector3D L_light(0, 0, 0);

    for (int i = 0; i < num_samples; i++) {
      Vector3D wi_world;
      double distToLight;
      double pdf;

      Vector3D L = light->sample_L(hit_p, &wi_world, &distToLight, &pdf);
      Vector3D wi = w2o * wi_world;
      if (wi.z <= 0) continue;

      Ray shadow_ray(hit_p, wi_world);
      shadow_ray.min_t = EPS_F;
      shadow_ray.max_t = distToLight - EPS_F;

      Intersection shadow_isect;
      if (!bvh->intersect(shadow_ray, &shadow_isect)) {
        L_light += L * isect.bsdf->f(w_out, wi) * wi.z / pdf;
      }
    }

    L_out += L_light / num_samples;
  }

  return L_out;

}

Vector3D PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
  // TODO: Part 3, Task 2
  // Returns the light that results from no bounces of light


  return isect.bsdf->get_emission();


}

Vector3D PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
  // TODO: Part 3, Task 3
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`

  if (direct_hemisphere_sample) {
    return estimate_direct_lighting_hemisphere(r, isect);
  } else {
    return estimate_direct_lighting_importance(r, isect);
  }


}

Vector3D PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  Vector3D hit_p = r.o + r.d * isect.t;
  Vector3D w_out = w2o * (-r.d);

  Vector3D direct = one_bounce_radiance(r, isect);

  // TODO: Part 4, Task 2
  // Returns the one bounce radiance + radiance from extra bounces at this point.
  // Should be called recursively to simulate extra bounces.

  if (r.depth <= 1) {
    return direct;
  }

  Vector3D wi;
  double pdf;
  Vector3D f = isect.bsdf->sample_f(w_out, &wi, &pdf);
  if (pdf == 0 || wi.z <= 0) return isAccumBounces ? direct : Vector3D();
  Vector3D wi_world = o2w * wi;

  Ray bounce_ray(hit_p, wi_world);
  bounce_ray.min_t = EPS_F;
  bounce_ray.max_t = INF_D;
  bounce_ray.depth = r.depth - 1;

  Intersection next_isect;
  Vector3D indirect(0, 0, 0);
  if (r.depth == max_ray_depth) {
    if (bvh->intersect(bounce_ray, &next_isect)) {
      indirect = f * at_least_one_bounce_radiance(bounce_ray, next_isect) * wi.z / pdf;
    }
  } else {
    double continue_prob = 0.7;
    if (coin_flip(continue_prob)) {
      if (bvh->intersect(bounce_ray, &next_isect)) {
        indirect = f * at_least_one_bounce_radiance(bounce_ray, next_isect) * wi.z / (pdf * continue_prob);
      }
    }
  }

  return isAccumBounces ? direct + indirect : indirect;
}

Vector3D PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Vector3D L_out;

  // You will extend this in assignment 3-2.
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  // The following line of code returns a debug color depending
  // on whether ray intersection with triangles or spheres has
  // been implemented.
  //
  // REMOVE THIS LINE when you are ready to begin Part 3.
  
  if (!bvh->intersect(r, &isect))
    return envLight ? envLight->sample_dir(r) : L_out;

  if (max_ray_depth == 0) {
    return zero_bounce_radiance(r, isect);
  }


  if (isAccumBounces) {
    L_out = zero_bounce_radiance(r, isect) + at_least_one_bounce_radiance(r, isect);
  } else {
    L_out = at_least_one_bounce_radiance(r, isect);
  }

  // TODO (Part 3): Return the direct illumination.

  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct

  return L_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {
  // TODO (Part 1.2):
  // Make a loop that generates num_samples camera rays and traces them
  // through the scene. Return the average Vector3D.
  // You should call est_radiance_global_illumination in this function.

  int num_samples = ns_aa;
  Vector3D radiance(0,0,0);

  for (int i = 0; i < num_samples; i++) {
    // random point inside pixel
    Vector2D sample = gridSampler->get_sample();
    double sx = (x + sample.x) / sampleBuffer.w;
    double sy = (y + sample.y) / sampleBuffer.h;

    // generate ray
    Ray ray = camera->generate_ray(sx, sy);

    // set ray depth
    ray.depth = max_ray_depth;

    // compute color along ray
    radiance += est_radiance_global_illumination(ray);
  }

  radiance /= num_samples;
  sampleBuffer.update_pixel(radiance, x, y);
  sampleCountBuffer[x + y * sampleBuffer.w] = num_samples;


  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"
  // int num_samples = ns_aa;          // total samples to evaluate
  // Vector2D origin = Vector2D(x, y); // bottom left corner of the pixel


  // sampleBuffer.update_pixel(Vector3D(0.2, 1.0, 0.8), x, y);
  // sampleCountBuffer[x + y * sampleBuffer.w] = num_samples;


}

void PathTracer::autofocus(Vector2D loc) {
  Ray r = camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
  Intersection isect;

  bvh->intersect(r, &isect);

  camera->focalDistance = isect.t;
}

} // namespace CGL

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "vec3.hpp"
#include "ray.hpp"

class sphere {
public:
    sphere() {}
    sphere(point3 cen, double r) : center(cen), radius(r) {};

    bool hit(const ray& r, double t_min, double t_max, double& t_out, vec3& normal_out) const {
        vec3 oc = r.origin() - center;
        auto a = dot(r.direction(), r.direction());
        auto b = 2.0 * dot(oc, r.direction());
        auto c = dot(oc, oc) - radius*radius;
        auto discriminant = b*b - 4*a*c;

        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-b - sqrtd) / (2.0*a);
        if (root < t_min || t_max < root) {
            root = (-b + sqrtd) / (2.0*a);
            if (root < t_min || t_max < root)
                return false;
        }

        t_out = root;
        point3 hit_point = r.at(t_out);
        normal_out = (hit_point - center) / radius;
        return true;
    }

public:
    point3 center;
    double radius;
};

#endif

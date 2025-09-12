#ifndef INTERSECTIONMANAGER_H
#define INTERSECTIONMANAGER_H

#include "Point3.h"
#include "Vector3.h"
#include "Ray.h"

#include <memory>

using std::shared_ptr;

class Material;

class IntersectionManager{
    public:
        Point3 point;
        double t;
        Vector3 normal;
        bool frontFace;

        shared_ptr< Material > material;

        double u;
        double v;

        void setFaceNormal( const Ray &ray, const Vector3 &outwardNormal ){
            frontFace = dot( ray.direction(), outwardNormal ) < 0;
            normal = frontFace ? outwardNormal : -outwardNormal;
        }
}; 

#endif
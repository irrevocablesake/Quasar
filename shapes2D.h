#ifndef SHAPE2D_H
#define SHAPE2D_H

#include "Mesh.h"
#include "Point3.h"
#include "Vector3.h"
#include "Material.h"
#include "AABB.h"
#include "Ray.h"
#include "Interval.h"
#include "IntersectionManager.h"

#include <memory>

using std::shared_ptr;

class shape2D : public Primitive {
    public:
        virtual bool inShape( double alpha, double beta, IntersectionManager &intersectionManager ) const = 0;
};

#endif
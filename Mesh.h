#ifndef MESH_H
#define MESH_H

#include "Ray.h"
#include "Interval.h"
#include "IntersectionManager.h"
#include "AABB.h"

#include <vector>

class Mesh {
    public:
        virtual ~Mesh() = default;
        virtual bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const = 0;
        virtual AABB getBoundingBox() const = 0;
};

class Primitive : public Mesh {
    public: 
        virtual ~Primitive() = default;
};

class Composite: public Mesh {
    public:
        virtual ~Composite() = default;
        virtual std::vector< shared_ptr< Primitive > > getFaces() const = 0;
};

#endif
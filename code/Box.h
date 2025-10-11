#ifndef BOX_H
#define BOX_H

#include "Point3.h"
#include "Material.h"
#include "Parallelogram.h"
#include "Mesh.h"
#include "AABB.h"
#include "World.h"

#include <memory>
#include <vector>

using std::shared_ptr;

class Box : public Composite {

    public:
        Box( Point3 a, Point3 b, shared_ptr< Material > material ){
            Point3 min = Point3( std::fmin( a.x(), b.x() ), std::fmin( a.y(), b.y() ), std::fmin( a.z(), b.z() ) );
            Point3 max = Point3( std::fmax( a.x(), b.x() ), std::fmax( a.y(), b.y() ), std::fmax( a.z(), b.z() ) );

            Vector3 sideX = Vector3( max.x() - min.x(), 0, 0 );
            Vector3 sideY = Vector3( 0, max.y() - min.y(), 0 );
            Vector3 sideZ = Vector3( 0, 0, max.z() - min.z() );

            BLAS.add( make_shared< Parallelogram >( Point3( min.x(), min.y(), max.z() ), sideX, sideY, material ) );
            BLAS.add( make_shared< Parallelogram >( Point3( max.x(), min.y(), max.z() ), -sideZ, sideY, material ) );
            BLAS.add( make_shared< Parallelogram >( Point3( max.x(), min.y(), min.z() ), -sideX, sideY, material ) );
            BLAS.add( make_shared< Parallelogram >( Point3( min.x(), min.y(), min.z() ), sideZ, sideY, material ) );
            BLAS.add( make_shared< Parallelogram >( Point3( min.x(), max.y(), max.z() ), sideX, -sideZ, material ) );
            BLAS.add( make_shared< Parallelogram >( Point3( min.x(), min.y(), min.z() ), sideX, sideZ, material ) );

            BLAS.setupAccelerationStructure();
        }

        AABB getBoundingBox() const override {
            return BLAS.getBoundingBox();
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override{
            return BLAS.raycast( ray, interval, intersectionManager );
        }

        int getCount() const override{
            return BLAS.getCount();
        }

    private:
        World BLAS;
};

#endif
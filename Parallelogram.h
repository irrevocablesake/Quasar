#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include "Point3.h"
#include "Vector3.h"
#include "Material.h"
#include "AABB.h"
#include "Ray.h"
#include "Interval.h"
#include "IntersectionManager.h"
#include "shapes2D.h"

#include <memory>

using std::shared_ptr;

class Parallelogram : public shape2D {
    public:
        Parallelogram( const Point3 &Q, const Vector3 &u, const Vector3 &v, shared_ptr< Material > material ) : Q( Q ), u( u ), v( v ), material( material ) {

            Vector3 orthogonal = cross( u, v );
            normal = unitVector( orthogonal );

            D = dot( normal, Q.toVector() );
            w = orthogonal / dot( orthogonal, orthogonal );

            AABB diagonal1 = AABB( Q, Q + u + v );
            AABB diagonal2 = AABB( Q + u, Q + v );

            boundingBox = AABB( diagonal1, diagonal2 );
        }

        AABB getBoundingBox() const override {
            return boundingBox;
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override {
            double denominator = dot( normal, ray.direction() );

            if( std::fabs( denominator ) < 1e-8 ){
                return false;
            }

            double t = ( D - dot( normal, ray.origin().toVector() )) / denominator;
            if( !interval.contains( t ) ){
                return false;
            }

            Point3 intersectedPoint = ray.at( t );

            Vector3 intersectedPointVector = intersectedPoint - Q;
            double alpha = dot( w, cross( intersectedPointVector, v ));
            double beta = dot( w, cross( u, intersectedPointVector ));

            if( !inShape( alpha, beta, intersectionManager ) ){
                return false;
            }

            intersectionManager.t = t;
            intersectionManager.point = intersectedPoint;
            intersectionManager.material = material;
            intersectionManager.setFaceNormal( ray, normal );

            return true;
        }

        bool inShape( double a, double b, IntersectionManager &intersectionManager ) const override {
            Interval unit = Interval( 0, 1 );

            if( !unit.contains( a ) || !unit.contains( b ) ){
                return false;
            }
            
            intersectionManager.u = a;
            intersectionManager.v = b;
            
            return true;
        }

    private:
        Point3 Q;
        Vector3 u, v;
        shared_ptr< Material > material;
        AABB boundingBox;

        Vector3 normal;
        double D;
        Vector3 w;
};

#endif
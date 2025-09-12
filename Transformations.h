#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "Mesh.h"
#include "Ray.h"
#include "AABB.h"
#include "Vector3.h"
#include "constants.h"

double degreesToRadians(double degrees) {
    return degrees * PI / 180.0;
}

class Translate : public Mesh {
    public:
        Translate( shared_ptr< Mesh > mesh, const Vector3 offset ) : mesh( mesh ), offset( offset ) {
            boundingBox = mesh -> getBoundingBox() + offset;
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override {
            Ray offsetRay( ray.origin() - offset, ray.direction(), ray.time() );

            if( !( mesh -> hit( offsetRay, interval, intersectionManager ) )){
                return false;
            }

            intersectionManager.point = intersectionManager.point + offset;

            return true;
        }

        AABB getBoundingBox() const override {
            return boundingBox;
        }

    private:
        shared_ptr< Mesh > mesh;
        Vector3 offset;
        AABB boundingBox;

};

class RotateY : public Mesh {
    public:
        RotateY( shared_ptr< Mesh > mesh, double angle ) : mesh( mesh ){
            auto radians = degreesToRadians( angle );
            
            sinTheta = std::sin( radians );
            cosTheta = std::cos( radians );

            boundingBox = mesh -> getBoundingBox();

            Point3 min( INF, INF, INF );
            Point3 max( -INF, -INF, -INF );

            for( int i = 0; i < 2; i++ ){
                for( int j = 0; j < 2; j++ ){
                    for( int k = 0; k < 2; k++ ){
                        auto x = i * boundingBox.x.max + ( 1 - i ) * boundingBox.x.min;
                        auto y = j * boundingBox.y.max + ( 1 - j ) * boundingBox.y.min;
                        auto z = k * boundingBox.z.max + ( 1 - k ) * boundingBox.z.min;

                        auto newX = cosTheta * x + sinTheta * z;
                        auto newZ = -sinTheta * x + cosTheta * z;

                        Vector3 coordinates( newX, y, newZ );

                        for( int c = 0; c < 3; c++ ){
                            min[c] = std::fmin( min[c], coordinates[c] );
                            max[c] = std::fmax( max[c], coordinates[c] );
                        }
                    }
                }
            }

            boundingBox = AABB( min, max );
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override {
            auto origin = Point3( 
                ( cosTheta * ray.origin().x() ) - ( sinTheta * ray.origin().z() ),
                ray.origin().y(),
                ( sinTheta * ray.origin().x() ) + ( cosTheta * ray.origin().z() )
             );

             auto direction = Vector3(
                ( cosTheta * ray.direction().x() ) - ( sinTheta * ray.direction().z() ),
                ray.direction().y(),
                ( sinTheta * ray.direction().x() ) + ( cosTheta * ray.direction().z() ) 
             );

             Ray objectSpaceRay( origin, direction, ray.time() );

             if( !mesh -> hit( objectSpaceRay, interval, intersectionManager ) ){
                return false;
             }

            intersectionManager.point = Point3(
                ( cosTheta * intersectionManager.point.x() ) + ( sinTheta * intersectionManager.point.z() ),
                intersectionManager.point.y(),
                ( -sinTheta * intersectionManager.point.x() ) + ( cosTheta * intersectionManager.point.z() )
            );

            intersectionManager.normal = Vector3(
                ( cosTheta * intersectionManager.normal.x() ) + ( sinTheta * intersectionManager.normal.z() ),
                intersectionManager.normal.y(),
                ( -sinTheta * intersectionManager.normal.x() ) + ( cosTheta * intersectionManager.normal.z() )
            );

            return true;
        }

        AABB getBoundingBox() const override {
            return boundingBox;
        }

    private:
        shared_ptr< Mesh > mesh;
        double sinTheta;
        double cosTheta;
        AABB boundingBox;
};

#endif
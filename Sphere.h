#ifndef SPHERE_H
#define SPHERE_H

#include "Vector3.h"
#include "Mesh.h"
#include "Material.h"
#include "constants.h"

#include <cmath>
#include <memory>

using std::shared_ptr;
using std::make_shared;

class Sphere : public Primitive {
    public:  

        Ray position;
        double radius;
        shared_ptr< Material > material;
        AABB boundingBox;

        Sphere() : position( Point3( 0, 0, -1.5 ), Vector3( 0, 0, 0 ) ), radius( 0.5 ), material( make_shared<Normal>() ) { 
            Vector3 radiusVector = Vector3( radius, radius, radius );
            boundingBox = AABB( position.origin() - radiusVector, position.origin() + radiusVector );
        }
        Sphere( const Point3 &position, const double radius, shared_ptr< Material> material ) : position( position, Vector3( 0, 0, 0) ), radius( radius ), material( material ) {
            Vector3 radiusVector = Vector3( radius, radius, radius );
            boundingBox = AABB( this->position.origin() - radiusVector, this->position.origin() + radiusVector );
        }
        Sphere( const Point3 &position, const double radius ) : position( position, Vector3( 0, 0, 0 ) ), radius( radius ), material( make_shared<Normal>() ) {
            Vector3 radiusVector = Vector3( radius, radius, radius );
            boundingBox = AABB( this->position.origin() - radiusVector, this->position.origin() + radiusVector );
        }

        Sphere( const Point3 &startCenter, const Point3 &endCenter, double radius, shared_ptr< Material > material ) : position( startCenter, endCenter - startCenter ), radius( radius ), material( material ) { 
            Vector3 radiusVector = Vector3( radius, radius, radius );

            AABB startCenterBoundingBox( position.at( 0 ) - radiusVector, position.at( 0 ) + radiusVector );
            AABB endCenterBoundingBox( position.at( 1 ) - radiusVector, position.at( 1 ) + radiusVector );

            boundingBox = AABB( startCenterBoundingBox, endCenterBoundingBox );
        }
        Sphere( const Point3 &startCenter, const Point3 &endCenter, double radius ) : position( startCenter, endCenter - startCenter ), radius( radius ), material( make_shared<Normal>() ) { }

        AABB getBoundingBox() const override {
            return boundingBox;
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override {
            
            Point3 positionAtTime = position.at( ray.time() );

            Vector3 spherePositionRay = positionAtTime - ray.origin();
            double a = ray.direction().lengthSquared();
            double h = dot( ray.direction(), spherePositionRay );
            double c = spherePositionRay.lengthSquared() - radius * radius;

            double discriminant = h * h - a * c;
            if( discriminant < 0 ){
                return false;
            }

            double sqrtDisriminant = sqrt( discriminant );

            double root = ( h - sqrtDisriminant ) / a;
            if( !interval.surrounds( root ) ){
                root = ( h + sqrtDisriminant ) / a;
                if( !interval.surrounds( root ) ){
                    return false;
                }
            }

            intersectionManager.t = root;
            intersectionManager.point = ray.at( intersectionManager.t );
            intersectionManager.material = material;

            Vector3 outwardNormal = ( intersectionManager.point - positionAtTime ) / radius;
            intersectionManager.setFaceNormal( ray, outwardNormal );
            
            getSphereUV( outwardNormal, intersectionManager.u, intersectionManager.v );

            return true;
        }

        private:
            static void getSphereUV( const Vector3 &normal, double &u, double &v ){
                auto theta = std::acos( -normal.y() );
                auto phi = std::atan2( -normal.z(), normal.x() ) + PI;

                u = phi / ( 2 * PI );
                v = theta / PI;
            }
};

#endif
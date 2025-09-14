#ifndef TRIANGLE_H
#define TRIANGLE_H

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

class Triangle : public shape2D {
    public:
        Triangle( const Point3 &A, const Point3 &B,const Point3 &C, shared_ptr< Material > material ) : A( A ), B( B ), C( C ), material( material ) {

            AB = B - A;
            AC = C - A;
            BC = C - B;

            normal = cross( AB, AC );
            normalizedNormal = unitVector( normal );

            D = dot( normal, A.toVector() );
            denom = dot( normal, normal );

            AABB sideAB(A, B);
            AABB sideBC(B, C);
            AABB sideAC(A, C);
            boundingBox = AABB(sideAB, sideBC);
            boundingBox = AABB(boundingBox, sideAC); 
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
            Vector3 intersectedPointVector = intersectedPoint - A;

            double alpha = dot(cross(intersectedPointVector, AC), normal) / denom;
            double beta = dot(cross(AB, intersectedPointVector), normal) / denom;

            if( !inShape( alpha, beta, intersectionManager ) ){
                return false;
            }

            intersectionManager.t = t;
            intersectionManager.point = intersectedPoint;
            intersectionManager.material = material;
            intersectionManager.setFaceNormal( ray, normal );

            return true;
        }

        bool inShape( double alpha, double beta, IntersectionManager &intersectionManager ) const override {
            if( alpha < 0 || beta < 0 || alpha + beta > 1 ){
                return false;
            }
            
            intersectionManager.u = alpha;
            intersectionManager.v = beta;
            
            return true;
        }

    private:
        Point3 A, B, C;
        shared_ptr< Material > material;
        AABB boundingBox;

        Vector3 normal;
        Vector3 normalizedNormal;
        Vector3 AB, AC, BC;
        
        double D;
        double denom;
};

#endif
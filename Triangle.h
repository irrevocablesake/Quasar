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

            faceNormal = cross( AB, AC );
            faceNormalizedNormal = unitVector( faceNormal );

            D = dot( faceNormalizedNormal, A.toVector() );
            denom = dot( faceNormal, faceNormal );

            AABB sideAB(A, B);
            AABB sideBC(B, C);
            AABB sideAC(A, C);
            boundingBox = AABB(sideAB, sideBC);
            boundingBox = AABB(boundingBox, sideAC); 
            
            NormalA = NormalB = NormalC = faceNormalizedNormal;
            hasUVs = false;
        }

        AABB getBoundingBox() const override {
            return boundingBox;
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override {
            double denominator = dot( faceNormalizedNormal, ray.direction() );

            if( std::fabs( denominator ) < 1e-8 ){
                return false;
            }

            double t = ( D - dot( faceNormalizedNormal, ray.origin().toVector() )) / denominator;
            if( !interval.contains( t ) ){
                return false;
            }

            Point3 intersectedPoint = ray.at( t );
            Vector3 intersectedPointVector = intersectedPoint - A;

            double beta = dot(cross(intersectedPointVector, AC), faceNormal) / denom;
            double gamma = dot(cross(AB, intersectedPointVector), faceNormal) / denom;
            double alpha = 1 - beta - gamma;

            Vector3 normal = unitVector( NormalA * alpha + NormalB * beta + NormalC * gamma );

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
            
            double gamma = 1 - alpha - beta;

            double u,v;
            if( hasUVs ){
                u = beta * UVB.x() + gamma * UVC.x() + alpha * UVA.x();
                v = beta * UVB.y() + gamma * UVC.y() + alpha * UVA.y();
            }
            else{
                u = beta;
                v = gamma;
            }
          
            intersectionManager.u = u;
            intersectionManager.v = v;
            
            return true;
        }

        void setNormals( Vector3 A, Vector3 B, Vector3 C ){
            NormalA = A;
            NormalB = B;
            NormalC = C;
        }

        void setUVs( Point3 A, Point3 B, Point3 C ){
            UVA = A;
            UVB = B;
            UVC = C;

            hasUVs = true;
        }

    private:
        Point3 A, B, C;
        shared_ptr< Material > material;
        AABB boundingBox;

        Vector3 faceNormal;
        Vector3 faceNormalizedNormal;
        Vector3 AB, AC, BC;

        Vector3 NormalA;
        Vector3 NormalB;
        Vector3 NormalC;

        Point3 UVA;
        Point3 UVB;
        Point3 UVC;

        bool hasUVs;
        
        double D;
        double denom;
};

#endif
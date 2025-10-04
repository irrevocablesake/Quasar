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

            hasTangent = false;
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

            Vector3 worldNormal, tangent, bitangent;
            Color3 rawNormal = material -> getNormalTexture() -> value( intersectionManager.u, intersectionManager.v, intersectedPoint );
            Vector3 tangentNormal =  ( Vector3( rawNormal.r(), rawNormal.g(), rawNormal.b() ) * 2.0 - 1.0 ) ;
                
            if( !hasTangent ){
                Vector3 edge1 = B - A;
                Vector3 edge2 = C - A;
                Vector3 deltaUV1 = UVB - UVA;
                Vector3 deltaUV2 = UVC - UVA;
                
                float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());
                
                tangent = Vector3(
                    f * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x()),
                    f * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y()),
                    f * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z())
                );
                
                tangent = unitVector( tangent - dot( tangent, normal ) * normal );
                bitangent = unitVector( cross( normal, tangent ) );  
            }
            else{
                tangent = unitVector( TangentA * alpha + TangentB * beta + TangentC * gamma );
                bitangent = unitVector( BiTangentA * alpha + BiTangentB * beta + BiTangentC * gamma );
            }

            intersectionManager.T = tangent;
            intersectionManager.B = bitangent;
            intersectionManager.N = normal;

            worldNormal = unitVector( Vector3(
                    tangentNormal.x() * tangent.x() + tangentNormal.y() * bitangent.x() + tangentNormal.z() * normal.x(),
                    tangentNormal.x() * tangent.y() + tangentNormal.y() * bitangent.y() + tangentNormal.z() * normal.y(),
                    tangentNormal.x() * tangent.z() + tangentNormal.y() * bitangent.z() + tangentNormal.z() * normal.z()
            ));
                
            // worldNormal = Vector3( worldNormal.x() * material->getNormalTextureFactor(), worldNormal.y() * material -> getNormalTextureFactor(), worldNormal.z() );

            intersectionManager.t = t;
            intersectionManager.point = intersectedPoint;
            intersectionManager.material = material;

            bool frontFace = dot( ray.direction(), normal ) < 0;
            intersectionManager.frontFace = frontFace;
            intersectionManager.normal = frontFace ? normal : -normal;
            intersectionManager.shadingNormal = worldNormal;

            return true;
        }

        int getCount() const override{
            return 1;
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

        void setTangents( Vector3 A, Vector3 B, Vector3 C ){
            TangentA = A;
            TangentB = B;
            TangentC = C;

            hasTangent = true;
        }

        void setBiTangents( Vector3 A, Vector3 B, Vector3 C ){
            BiTangentA = A;
            BiTangentB = B;
            BiTangentC = C;

            hasTangent = true;
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

        Vector3 TangentA;
        Vector3 TangentB;
        Vector3 TangentC;

        Vector3 BiTangentA;
        Vector3 BiTangentB;
        Vector3 BiTangentC;

        Point3 UVA;
        Point3 UVB;
        Point3 UVC;

        bool hasUVs;
        bool hasTangent;
        
        double D;
        double denom;
};

#endif
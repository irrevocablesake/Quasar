#ifndef FOG_H
#define FOG_H

#include "Mesh.h"
#include "Texture.h"
#include "Color3.h"
#include "Interval.h"
#include "constants.h"
#include "Random.h"
#include "Material.h"

#include <memory> 

using std::shared_ptr;
using std::make_shared;

class Fog : public Mesh {
    public:
        Fog( shared_ptr< Mesh > boundary, double density, shared_ptr< Material > material  ) : boundary( boundary ), inverseDensity( -1 / density ), phaseFunction( material ){ }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override {
            IntersectionManager record1, record2;

            if( !boundary -> hit( ray, Interval::universe, record1 ) ){
                return false;
            }

            if( !boundary -> hit( ray, Interval( record1.t + 0.0001, INF ), record2 ) ){
                return false;
            }

            if( record1.t < interval.min ){
                record1.t = interval.min;
            }

            if( record2.t > interval.max ){
                record2.t = interval.max;
            }

            if( record1.t >= record2.t ){
                return false;
            }

            if( record1.t < 0 ){
                record1.t = 0;
            }

            auto rayLength = ray.direction().length();
            auto travelledDistance = ( record2.t - record1.t ) * rayLength;
            auto hitDistance = inverseDensity * std::log( generateRandomNumber() );

            if( hitDistance > travelledDistance ){
                return false;
            }

            intersectionManager.t = record1.t + hitDistance / rayLength;
            intersectionManager.point = ray.at( intersectionManager.t );

            intersectionManager.normal = Vector3( 1, 0, 0 );
            intersectionManager.frontFace = true;
            intersectionManager.material = phaseFunction;

            return true;
        }

        AABB getBoundingBox() const override {
            return boundary -> getBoundingBox();
        }

    private:
        shared_ptr< Mesh > boundary;
        double inverseDensity;
        shared_ptr< Material > phaseFunction;
};

#endif
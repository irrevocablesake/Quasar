#ifndef AABB_H
#define AABB_H

#include "Interval.h"
#include "Point3.h"
#include "Ray.h"

class AABB {
    public:
        Interval x, y, z;

        AABB() { }

        AABB( const Interval &x, const Interval &y, const Interval &z  ) : x( x ), y( y ), z( z ) {
            padBox();
        }

        AABB( const Point3 &a, const Point3 &b ){
            x = ( a.x() < b.x() ) ? Interval( a.x(), b.x() ) : Interval( b.x(), a.x() );
            y = ( a.y() < b.y() ) ? Interval( a.y(), b.y() ) : Interval( b.y(), a.y() );
            z = ( a.z() < b.z() ) ? Interval( a.z(), b.z() ) : Interval( b.z(), a.z() );
        
            padBox();
        }

        AABB( const AABB &box0, const AABB &box1 ){
            x = Interval( box0.x, box1.x );
            y = Interval( box0.y, box1.y );
            z = Interval( box0.z, box1.z );

            padBox();
        }

        void mergeBoundingBox( const AABB &box ){
            x = Interval( x, box.x );
            y = Interval( y, box.y );
            z = Interval( z, box.z );

            padBox();
        }

        const Interval &getIntervalForAxis( int n ) const {
            if( n == 1 ) return y;
            if( n == 2 ) return z;
            return x;
        }

        int getLongestAxis() const {
            if( x.size() > y.size() ){
                return x.size() > z.size() ? 0 : 2;
            }
            else{
                return y.size() > z.size() ? 1 : 2;
            }
        }

        bool hit( const Ray &ray, Interval interval ) const{
            const Point3 &rayOrigin = ray.origin();
            const Vector3 &rayDirection = ray.direction();

            for( int axis = 0; axis < 3; axis++ ){
                const Interval &axisInterval = getIntervalForAxis( axis );
                const double dx = 1.0 / rayDirection[ axis ];

                double t0 = ( axisInterval.min - rayOrigin[ axis ] ) * dx;
                double t1 = ( axisInterval.max - rayOrigin[ axis ] ) * dx;
                
                if( t0 < t1 ){
                    if( t0 > interval.min ) interval.min = t0;
                    if( t1 < interval.max ) interval.max = t1;
                }
                else {
                    if( t1 > interval.min ) interval.min = t1;
                    if( t0 < interval.max ) interval.max = t0;
                }

                if( interval.max <= interval.min ){
                    return false;
                }
            }
            return true;
        }

        static const AABB empty, universe;

    private:
        void padBox(){
            double delta = 0.0001;

            if( x.size() < delta ) x = x.expand( delta );
            if( y.size() < delta ) y = y.expand( delta );
            if( z.size() < delta ) z = z.expand( delta );
        }
};

const AABB AABB::empty = AABB( Interval::empty, Interval::empty, Interval::empty );
const AABB AABB::universe = AABB( Interval::universe, Interval:: universe, Interval::universe );

AABB operator+( const AABB &boundingBox, const Vector3 offset ){
    return AABB( boundingBox.x + offset.x(), boundingBox.y + offset.y(), boundingBox.z + offset.z() );
}

#endif
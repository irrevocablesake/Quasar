#ifndef BVH_H
#define BVH_H

#include "IntersectionManager.h"
#include "Ray.h"
#include "Interval.h"
#include "Random.h"
#include "Mesh.h"
#include "AABB.h"

#include<vector>
#include <memory>
#include<variant>

using std::shared_ptr;
using std::vector;
using std::make_shared;

#include<algorithm>

class BVHNode {
    public:
        using nodeType = std::variant< shared_ptr< BVHNode >, shared_ptr< Mesh >>;

        BVHNode( vector< shared_ptr< Mesh > > &objects, size_t start, size_t end ){
            
            boundingBox = AABB::empty;
            for( size_t index = start; index < end; index++ ){
                boundingBox = AABB( boundingBox, objects[ index ] -> getBoundingBox() );
            }

            int axis = boundingBox.getLongestAxis();

            auto comparator = static_cast<bool (*)(shared_ptr<Mesh>, shared_ptr<Mesh>)>(nullptr);

            if( axis == 0 ){
                comparator = boxXCompare;
            }
            else if( axis == 1 ){
                comparator = boxYCompare;
            }
            else if( axis == 2 ){
                comparator = boxZCompare;
            }

            size_t numberOfObjects = end - start;

            if( numberOfObjects == 1 ){
                left = right = objects[ start ];
              
            }
            else if( numberOfObjects == 2 ){
                left = objects[ start ];
                right = objects[ start + 1 ];
            }
            else{
                std::sort( std::begin( objects ) + start, std::begin( objects ) + end, comparator );

                size_t mid = start + numberOfObjects / 2;
                left = make_shared< BVHNode >( objects, start, mid );
                right = make_shared< BVHNode >( objects, mid, end );
            }
       }



       //also we can add a different way of doing this, basically a hold_alternative to check what type it is holding, in case the parameters are different
        bool nodeTypeHit( const nodeType &child, const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const {
            return std::visit([&]( auto &&object ) { return object->hit( ray, interval, intersectionManager ); }, child );
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ){
            if( !boundingBox.hit( ray, interval ) ){
                return false;
            }

            bool hitLeft = nodeTypeHit( left, ray, interval, intersectionManager );
            bool hitRight = nodeTypeHit( right, ray, Interval( interval.min, hitLeft ? intersectionManager.t : interval.max ), intersectionManager );

            return hitLeft || hitRight;
        }

        AABB getBoundingBox() const {
            return boundingBox;
        }
    
    private:
        nodeType left;
        nodeType right;
        AABB boundingBox;

        AABB getChildBoundingBox(const nodeType& child) const {
            return std::visit([](auto&& obj) { return obj->getBoundingBox(); }, child);
        }

        static bool boxCompare( const shared_ptr< Mesh > a, const shared_ptr< Mesh > b, int axis ){
            Interval aAxisInterval = a -> getBoundingBox().getIntervalForAxis( axis );
            Interval bAxisInterval = b -> getBoundingBox().getIntervalForAxis( axis );

            return aAxisInterval.min < bAxisInterval.min;
        }

        static bool boxXCompare( const shared_ptr< Mesh > a, const shared_ptr< Mesh > b ){
            return boxCompare( a, b, 0 );
        }

        static bool boxYCompare( const shared_ptr< Mesh > a, const shared_ptr< Mesh > b ){
            return boxCompare( a, b, 1 );
        }

        static bool boxZCompare( const shared_ptr< Mesh > a, const shared_ptr< Mesh > b ){
            return boxCompare( a, b, 2 );
        }
};

#endif
#ifndef WORLD_H
#define WORLD_H

#include "Mesh.h"
#include "Interval.h"
#include "IntersectionManager.h"
#include "AABB.h"
#include "BVHNode.h"

#include <vector>
#include <memory>

using std::shared_ptr;
using std::make_shared;

class World{
    public:
        std::vector< shared_ptr< Mesh > > meshList;
        shared_ptr< BVHNode > tree;

        World() {}

        void clear() {
            meshList.clear();
        }

        void add( shared_ptr< Mesh > mesh ){
            meshList.push_back( mesh );
            boundingBox = AABB( boundingBox, mesh -> getBoundingBox() );
        }

        bool raycast( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const{
            IntersectionManager record;
            bool hitAnything = false;
            double closestSoFar = interval.max;
            
            if( tree -> hit( ray, Interval( interval.min, closestSoFar ), record ) ){
                hitAnything = true;
                closestSoFar = record.t;
                intersectionManager = record;
            }

            return hitAnything;
        }

        void setupAccelerationStructure(){
            tree = make_shared< BVHNode >( meshList, 0, meshList.size() );
        }

        AABB getBoundingBox() const {
            return boundingBox;
        }

        int getCount() const {
            int total = 0;
            for( auto &mesh : meshList ){
                total = total + mesh -> getCount();
            }

            return total;
        }

    private:
        AABB boundingBox;
};

#endif
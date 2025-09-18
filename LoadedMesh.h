#ifndef LOADED_MESH_H
#define LOADED_MESH_H

#include "Point3.h"
#include "Material.h"
#include "Mesh.h"
#include "AABB.h"
#include "World.h"
#include "Triangle.h"
#include "GLTFLoader.h"

#include <memory>
#include <vector>
#include <string>

using std::shared_ptr;
using std::string;

class LoadedMesh : public Composite {

    public:
        LoadedMesh( string filePath, bool useMetal, double metalFactor ){
          
            GLTFLoader loader;
            loader.loadFile( filePath, useMetal, metalFactor );

            std::vector< shared_ptr< Triangle >> triangles = loader.getTriangles();
            for( auto &triangle : triangles ){
                BLAS.add( triangle );
            }
           
            BLAS.setupAccelerationStructure();
        }

        AABB getBoundingBox() const override {
            return BLAS.getBoundingBox();
        }

        bool hit( const Ray &ray, Interval interval, IntersectionManager &intersectionManager ) const override{
            return BLAS.raycast( ray, interval, intersectionManager );
        }

        int getCount() const override{
            return BLAS.getCount();
        }

    private:
        World BLAS;
};

#endif
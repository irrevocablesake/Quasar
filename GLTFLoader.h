#ifndef GLTF_LOADER
#define GLTF_LOADER

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<iostream>
#include<vector>
#include<string>

#include "Triangle.h"

using std::string;

class GLTFLoader {
    public:
        GLTFLoader(){
            material = std::make_shared< Diffuse >( Color3( 1.0, 1.0, 1.0 ) );
        }

        void loadFile( string filePath ){
            scene = importer.ReadFile( filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices );
            if( !scene || !( scene -> HasMeshes() ) ){
                std::clog << "ASSIMP Error:: No Scene or Meshes Found \n";
            }

            for( unsigned int i = 0; i < scene -> mNumMeshes; i++ ){
                aiMesh *mesh = scene -> mMeshes[ i ];
                for( unsigned int faceIndex = 0; faceIndex < mesh -> mNumFaces; faceIndex++ ){
                    aiFace face = mesh -> mFaces[ faceIndex ];

                    if( face.mNumIndices != 3 ) continue;

                    aiVector3D v0 = mesh -> mVertices[ face.mIndices[ 0 ] ];
                    aiVector3D v1 = mesh -> mVertices[ face.mIndices[ 1 ] ];
                    aiVector3D v2 = mesh -> mVertices[ face.mIndices[ 2 ] ];

                    triangleList.push_back(
                        std::make_shared< Triangle >(
                            Point3( v0.x, v0.y, v0.z ),
                            Point3( v1.x, v1.y, v1.z ),
                            Point3( v2.x, v2.y, v2.z ),
                            material
                        )
                    );
                }
            }
        }

        std::vector< std::shared_ptr< Triangle >> getTriangles(){
            return triangleList;
        }
    
    private:
        Assimp::Importer importer;
        const aiScene *scene;
        std::vector< std::shared_ptr< Triangle >> triangleList;

        std::shared_ptr< Material > material;

};

#endif
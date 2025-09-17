#ifndef GLTF_LOADER
#define GLTF_LOADER

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<iostream>
#include<vector>
#include<string>

#include "Triangle.h"
#include "Material.h"

using std::string;

/*
    * Assumptions:
    - GLB has Normals ( flat / smooth ), UVs
    - GLB, goes without saying, uses embedded textures
    - Our loader uses only diffuse color ( diffuse texture, if present )
*/

class GLTFLoader {
    public:
        GLTFLoader(){ }

        void loadFile( string filePath, bool useMetal, double metalFactor ){
            scene = importer.ReadFile( filePath, 0 );
            if( !scene || !( scene -> HasMeshes() ) ){
                std::clog << "ASSIMP Error:: No Scene or Meshes Found \n";
            }

            for( unsigned int i = 0; i < scene -> mNumMeshes; i++ ){
                aiMesh *mesh = scene -> mMeshes[ i ];
                aiMaterial *material = scene -> mMaterials[ mesh -> mMaterialIndex ];

                std::shared_ptr< Material > baseMaterial = std::make_shared< Diffuse >( Color3( 1.0, 1.0, 1.0 ) );
                std::shared_ptr< ImageTexture > imageTexture = nullptr;

                if( material -> GetTextureCount( aiTextureType_DIFFUSE ) > 0 ){
                    aiString texturePath;

                    if( material -> GetTexture( aiTextureType_DIFFUSE, 0, &texturePath ) == AI_SUCCESS ){
                        const aiTexture *texture = scene -> GetEmbeddedTexture( texturePath.C_Str() );
                        if( texture ){
                            if( texture -> mHeight == 0){
                                imageTexture = make_shared< ImageTexture >( ( unsigned char* ) texture -> pcData, texture -> mWidth );
                                if( useMetal ){
                                    baseMaterial = std::make_shared< Metal >( imageTexture, metalFactor );
                                }
                                else{
                                    baseMaterial = std::make_shared< Diffuse >( imageTexture );
                                }
                            }
                        }
                    }
                }

                if( !imageTexture ){
                    aiColor4D diffuseColor;
                    if( material -> Get( AI_MATKEY_COLOR_DIFFUSE, diffuseColor ) == AI_SUCCESS ){
                        if( useMetal ){
                            baseMaterial = std::make_shared< Metal >( Color3( diffuseColor.r, diffuseColor.g, diffuseColor.b ), metalFactor );
                        }
                        else{
                            baseMaterial = make_shared< Diffuse >( Color3( diffuseColor.r, diffuseColor.g, diffuseColor.b ) );
                        }
                    }
                }

                for( unsigned int faceIndex = 0; faceIndex < mesh -> mNumFaces; faceIndex++ ){
                    aiFace face = mesh -> mFaces[ faceIndex ];

                    if( face.mNumIndices != 3 ) continue;

                    aiVector3D v0 = mesh -> mVertices[ face.mIndices[ 0 ] ];
                    aiVector3D v1 = mesh -> mVertices[ face.mIndices[ 1 ] ];
                    aiVector3D v2 = mesh -> mVertices[ face.mIndices[ 2 ] ];

                    aiVector3D n0 = mesh ->mNormals[ face.mIndices[ 0  ] ];
                    aiVector3D n1 = mesh ->mNormals[ face.mIndices[ 1  ] ];
                    aiVector3D n2 = mesh ->mNormals[ face.mIndices[ 2  ] ];

                    aiVector3D uv0 = mesh -> mTextureCoords[0][ face.mIndices[ 0 ] ];
                    aiVector3D uv1 = mesh -> mTextureCoords[0][ face.mIndices[ 1 ] ];
                    aiVector3D uv2 = mesh -> mTextureCoords[0][ face.mIndices[ 2 ] ];

                    std::shared_ptr< Triangle > triangle = std::make_shared< Triangle >(
                        Point3( v0.x, v0.y, v0.z ),
                        Point3( v1.x, v1.y, v1.z ),
                        Point3( v2.x, v2.y, v2.z ),
                        baseMaterial
                    );

                    triangle -> setNormals(
                        Vector3( n0.x, n0.y, n0.z ), 
                        Vector3( n1.x, n1.y, n1.z ), 
                        Vector3( n2.x, n2.y, n2.z ) 
                    );

                    triangle -> setUVs(
                        Point3( uv0.x, uv0.y, 0 ),
                        Point3( uv1.x, uv1.y, 0 ),
                        Point3( uv2.x, uv2.y, 0 )
                    );

                    triangleList.push_back(
                        triangle
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

};

#endif
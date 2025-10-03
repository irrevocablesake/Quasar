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

        shared_ptr<ImageTexture> loadTextureMap(aiTextureType type, aiMaterial *material){
            aiString texturePath;
            shared_ptr<ImageTexture> texture;

            if( material -> GetTextureCount( type ) == 0 ){
                texture = nullptr;
                return texture;
            }

            aiReturn verdict = material->GetTexture(type, 0, &texturePath);
            
            if (verdict == AI_SUCCESS)
            {
                const aiTexture *embeddedTexture = scene->GetEmbeddedTexture(texturePath.C_Str());
                if (embeddedTexture)
                {
                    if (embeddedTexture->mHeight == 0)
                    {
                        texture = make_shared<ImageTexture>((unsigned char *)embeddedTexture->pcData, embeddedTexture->mWidth);
                        texture->loaded = true;
                    }
                }
            }

            if( !texture -> loaded ){
                texture = nullptr;
            }

            return texture;
        }

        Color3 getDiffuseColor( aiMaterial *material ){
            aiColor4D color;
            if( material->Get(AI_MATKEY_COLOR_DIFFUSE, color) ){
                return Color3( color.r, color.g, color.b );
            }
            return Color3( 1, 1, 1 );
        }

        float getRoughnessFactor( aiMaterial *material ){
            float factor;
            if( material->Get( AI_MATKEY_ROUGHNESS_FACTOR, factor ) ){
                return factor;
            }
            return 0.5;
        }

        void loadFile( string filePath ){
            scene = importer.ReadFile( filePath, 0 );
            if( !scene || !( scene -> HasMeshes() ) ){
                std::clog << "ASSIMP Error:: No Scene or Meshes Found \n";
            }

            for( unsigned int i = 0; i < scene -> mNumMeshes; i++ ){
                aiMesh *mesh = scene -> mMeshes[ i ];
                aiMaterial *material = scene -> mMaterials[ mesh -> mMaterialIndex ];

                if( mesh -> HasTangentsAndBitangents() ){
                    std::clog << "tangents and bitangents found" << std::endl;
                }

                MaterialProperties properties;

                properties.diffuseColor = getDiffuseColor(material);
                properties.roughnessFactor = getRoughnessFactor( material );
                properties.diffuseTexture = loadTextureMap( aiTextureType_DIFFUSE, material );
                if( properties.diffuseTexture == nullptr ){
                    properties.diffuseTexture = make_shared< solidColor >( properties.diffuseColor );
                }
                properties.normalTexture = loadTextureMap( aiTextureType_NORMALS, material );
                if( properties.normalTexture == nullptr ){
                    properties.normalTexture = make_shared< solidColor>( Color3( 0.5, 0.5, 1.0 ) );
                }

                std::shared_ptr< Material > baseMaterial = make_shared<DisneyBRDF>( properties );
                
                for( unsigned int faceIndex = 0; faceIndex < mesh -> mNumFaces; faceIndex++ ){
                    aiFace face = mesh -> mFaces[ faceIndex ];

                    if( face.mNumIndices != 3 ) continue;

                    aiVector3D v0 = mesh -> mVertices[ face.mIndices[ 0 ] ];
                    aiVector3D v1 = mesh -> mVertices[ face.mIndices[ 1 ] ];
                    aiVector3D v2 = mesh -> mVertices[ face.mIndices[ 2 ] ];

                    aiVector3D n0 = mesh ->mNormals[ face.mIndices[ 0  ] ];
                    aiVector3D n1 = mesh ->mNormals[ face.mIndices[ 1  ] ];
                    aiVector3D n2 = mesh ->mNormals[ face.mIndices[ 2  ] ];

                    aiVector3D t0 = mesh ->mTangents[ face.mIndices[ 0  ] ];
                    aiVector3D t1 = mesh ->mTangents[ face.mIndices[ 1  ] ];
                    aiVector3D t2 = mesh ->mTangents[ face.mIndices[ 2  ] ];

                    aiVector3D bt0 = mesh ->mBitangents[ face.mIndices[ 0  ] ];
                    aiVector3D bt1 = mesh ->mBitangents[ face.mIndices[ 1  ] ];
                    aiVector3D bt2 = mesh ->mBitangents[ face.mIndices[ 2  ] ];

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

                    triangle->setTangents(
                        Vector3(t0.x, t0.y, t0.z),
                        Vector3(t1.x, t1.y, t1.z),
                        Vector3(t2.x, t2.y, t2.z)
                    );

                    triangle->setBiTangents(
                        Vector3(bt0.x, bt0.y, bt0.z),
                        Vector3(bt1.x, bt1.y, bt1.z),
                        Vector3(bt2.x, bt2.y, bt2.z)
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
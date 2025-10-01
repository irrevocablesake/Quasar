#ifndef MATERIAL_H
#define MATERIAL_H

#include "Color3.h"
#include "Ray.h"
#include "IntersectionManager.h"
#include "Random.h"
#include "Vector3.h"
#include "Texture.h"

#include<optional>
#include<cmath>

struct MaterialProperties {
    Color3 diffuseColor;
    shared_ptr< ImageTexture > diffuseTexture;
    shared_ptr< ImageTexture > normalTexture;
    float roughnessFactor;
    float subSurfaceFactor = 0;
};

class Material {
    public:
        virtual ~Material() = default;

        virtual bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const {
            return false;
        }

        virtual Color3 emitted( double u, double v, const Point3 &point ) const {
            return Color3( 0, 0, 0 );
        }

        virtual shared_ptr<ImageTexture> getNormalTexture() const { 
            return nullptr; 
        }
};

class Normal : public Material {
    public:
        Normal() {}
        Normal( MaterialProperties properties ) {
            this -> properties = properties;
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            scattered = Ray( intersectionManager.point, intersectionManager.normal, ray.time() );
            attenuation = 0.5 * Color3( scattered.direction().x() + 1, scattered.direction().y() + 1, scattered.direction().z() + 1 );
        
            return false;
        }

        shared_ptr<ImageTexture> getNormalTexture() const {
            return properties.normalTexture;
        }

    private:
        MaterialProperties properties;
};

class Solid : public Material {
    public:
        Solid( Color3 albedo ) : texture( make_shared< solidColor >( albedo ) ) {}

        //remember this point3 is just a dummy and won't be used
        Color3 getAlbedo() const {
            return texture->value( 0, 0, Point3( 0, 0, 0 ));
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            scattered = Ray( intersectionManager.point, intersectionManager.normal, ray.time() );
            attenuation = texture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );
        
            return false;
        }

       
    private:
        shared_ptr< Texture > texture;
};

class Diffuse : public Material {
    public:
        Diffuse() {}
        Diffuse( Color3 color ) : texture( make_shared< solidColor >( color ) ) {}
        Diffuse( shared_ptr< Texture > texture ) : texture( texture ) {} 
        Diffuse( shared_ptr< ImageTexture > texture ) : texture( texture ) {}

        Vector3 reflected() const{
           return generateRandomUnitVector();
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 reflectedVector = intersectionManager.normal + reflected();

            if( reflectedVector.nearZero() ){
                reflectedVector = intersectionManager.normal;
            }
            
            scattered = Ray( intersectionManager.point, reflectedVector, ray.time() );
            attenuation = texture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );

            return true;
        }

    private:
        shared_ptr< Texture > texture;
};

class Metal : public Material {
    public:

        Metal( const Color3 &albedo, double fuzz ) : texture( make_shared< solidColor >( albedo )), fuzz( fuzz < 1 ? fuzz : 1 ) { }
        Metal( shared_ptr< Texture > texture, double fuzz ) : texture( texture ), fuzz( fuzz < 1 ? fuzz : 1 ) {}

        Vector3 reflected( const Vector3 & vector, const Vector3 &normal ) const{
            return vector - 2 * dot( vector, normal ) * normal;
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 reflectedVector = reflected( ray.direction(), intersectionManager.normal );
            reflectedVector = unitVector( reflectedVector ) + ( fuzz * generateRandomUnitVector() );
            scattered = Ray( intersectionManager.point, reflectedVector, ray.time() );
            attenuation = texture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );;

            return ( dot( scattered.direction(), intersectionManager.normal ) > 0 );
        }

    private:
        shared_ptr< Texture > texture;
        double fuzz;
};

class Dielectric : public Material {
    public:

        Dielectric( double refractionIndex ) : refractionIndex( refractionIndex ) { }

        Vector3 refract( const Vector3 &vector, const Vector3 &normal, double etaiOverEtat ) const{
            double cosTheta = std::fmin( dot( -vector, normal ), 1.0 );
            Vector3 rOutPerpendicular = etaiOverEtat * ( vector + cosTheta * normal );
            Vector3 rOutParallel = -std::sqrt( std::fabs( 1.0 - rOutPerpendicular.lengthSquared())) * normal;

            return rOutPerpendicular + rOutParallel;
        }

        Vector3 reflected( const Vector3 & vector, const Vector3 &normal ) const{
            return vector - 2 * dot( vector, normal ) * normal;
        }

        double reflectance( double cosine, double RI ) const{
            double r0 = ( 1 - RI ) / ( 1 + RI );
            r0 = r0 * r0;
            return r0 + ( 1 - r0 ) * std::pow( ( 1 - cosine ), 5 );
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            attenuation = Color3( 1.0, 1.0, 1.0 );
            double rI = intersectionManager.frontFace ? ( 1.0 / refractionIndex ) : refractionIndex;

            Vector3 unitDirection = unitVector( ray.direction() );
            
            double cosTheta = std::fmin( dot( -unitDirection, intersectionManager.normal ), 1.0 );
            double sinTheta = std::sqrt( 1.0 - cosTheta * cosTheta );

            bool cannotRefract = rI * sinTheta > 1.0;
            Vector3 direction;

            if( cannotRefract || reflectance( cosTheta, rI ) > generateRandomNumber() ){
                direction = reflected( unitDirection, intersectionManager.normal );
            }
            else{
                direction = refract( unitDirection, intersectionManager.normal, rI );
            }

            scattered = Ray( intersectionManager.point, direction, ray.time() );
            return true;
        } 

    private:
        double refractionIndex;
};

class Light : public Material {
    public:
        Light( shared_ptr< Texture > texture ) : texture( texture ) {}
        Light( const Color3 emit ) : texture( make_shared< solidColor >( emit )) {}

        Color3 emitted( double u, double v, const Point3 &point  ) const override {
            return texture -> value( u, v, point );
        }

    private:
        shared_ptr< Texture > texture;
};

class Isotropic : public Material {
    public:
        Isotropic( const Color3 &albedo ) : texture( make_shared< solidColor >( albedo ) ) { }
        Isotropic( shared_ptr< Texture > texture ) : texture( texture ) {}

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            scattered = Ray( intersectionManager.point, generateRandomUnitVector(), ray.time() );
            attenuation = texture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );

            return true;
        }

    private:
        shared_ptr< Texture > texture;
};

class DiffuseBRDF : public Material{
    public:
        DiffuseBRDF() {}
        DiffuseBRDF( Color3 albedo, double roughness, double subsurface ) : texture( make_shared< solidColor >( albedo ) ), roughnessParam( roughness ), subsurfaceParam( subsurface ) {}
        DiffuseBRDF( shared_ptr< ImageTexture > texture, double roughness, double subsurface ) : texture( texture), roughnessParam( roughness ), subsurfaceParam( subsurface ) {}

        Vector3 reflected() const{
           return generateRandomUnitVector();
        }

        double fresnelGrazingAngle( Vector3 incomingDirection, Vector3 outgoingDirection ) const{
            Vector3 halfVector = unitVector( incomingDirection + outgoingDirection );
            double approximation = 0.5 + 2 * roughnessParam * ( std::pow( std::abs( dot( halfVector, outgoingDirection )) , 2) );
            return approximation;
        }

        double subsurfaceGrazingAngle( Vector3 incomingDirection, Vector3 outgoingDirection ) const {
            Vector3 halfVector = unitVector( incomingDirection + outgoingDirection );
            double approximation = roughnessParam * std::pow( std::abs( dot( halfVector, outgoingDirection )), 2 );
            
            return approximation;
        }

        double calculateFresnel( Vector3 normal, Vector3 direction, double grazingApproximation ) const{
            double approximation = 1 + ( grazingApproximation - 1 ) * ( 1 - ( std::pow( std::abs( dot( normal, direction ) ) , 5)));
            return approximation;
        }

        double calculateSubSurface( Vector3 normal, Vector3 direction, double grazinApproximation ) const {
            double approximation = 1 + ( grazinApproximation - 1 ) * ( 1 - std::pow( std::abs( dot( normal, direction )), 5));
            return approximation;
        }

        double fresnelEffect( Vector3 normal, Vector3 incomingDirection, Vector3 outgoingDirection ) const{
            double grazingApproximation = fresnelGrazingAngle( incomingDirection, outgoingDirection );

            double approximation = calculateFresnel( normal, incomingDirection, grazingApproximation ) * calculateFresnel( normal, outgoingDirection, grazingApproximation ) * std::abs( dot( normal, outgoingDirection ) );
            return approximation; 
        }

        double subSurfaceEffect( Vector3 normal, Vector3 incomingDirection, Vector3 outgoingDirection ) const {
            double grazingApproximation = subsurfaceGrazingAngle( incomingDirection, outgoingDirection );
            
            double approximation = calculateSubSurface( normal, incomingDirection, grazingApproximation ) * calculateSubSurface( normal, outgoingDirection, grazingApproximation );
            return approximation;
        }

        Color3 evaluate( IntersectionManager &intersectionManager,  const Vector3 &incomingDirection, Vector3 outgoingDirection ) const {
            Color3 albedo = texture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );
            
            Color3 baseDiffuse = ( albedo / PI ) * fresnelEffect( intersectionManager.normal, incomingDirection, outgoingDirection );
            Color3 subSurface = ( ( 1.25 * albedo ) / PI ) * ( subSurfaceEffect( intersectionManager.normal, incomingDirection, outgoingDirection ) * ( ( 1 / ( std::abs( dot( intersectionManager.normal, incomingDirection ) ) + std::abs( dot( intersectionManager.normal, outgoingDirection )) )) -0.5 ) + 0.5 ) * std::abs( dot( intersectionManager.normal, outgoingDirection ));

            Color3 diffuse = ( 1 - subsurfaceParam ) * baseDiffuse + subsurfaceParam * subSurface;

            return subSurface;
        }

        double getPDF( Vector3 normal, Vector3 outDirection ) const {
            return std::max( dot( normal, outDirection ), 1e-6 )/ PI;
        }

        Vector3 sample( Vector3 normal, Vector3 shadingNormal ) const {
            if(dot( shadingNormal, normal) < 0){
                shadingNormal = -shadingNormal;
            } 

            Vector3 scatterDir = unitVector( shadingNormal + reflected() );
            if(dot(scatterDir, normal) < 0) {
                scatterDir = shadingNormal;
            }
            return scatterDir;
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 sampleDirection = sample( intersectionManager.normal, intersectionManager.shadingNormal );
            if( sampleDirection.nearZero() ){
                sampleDirection = intersectionManager.normal;
            }

            Color3 evaluateBRDF = evaluate( intersectionManager, ray.direction(), sampleDirection );
            double pdf = getPDF( intersectionManager.normal, sampleDirection );

            scattered = Ray( intersectionManager.point, sampleDirection, ray.time() );
            // attenuation = evaluateBRDF * dot( intersectionManager.shadingNormal, sampleDirection ) / pdf;
            attenuation = texture->value(intersectionManager.u, intersectionManager.v, intersectionManager.point);
            return true;
        }
    
    private:
        shared_ptr< Texture > texture;

    public:
        double roughnessParam;
        double subsurfaceParam;
};

class DisneyBRDF : public Material {
    public:
        DiffuseBRDF diffuseBRDF;
        MaterialProperties properties;

        DisneyBRDF( MaterialProperties properties ) {
            this -> properties = properties;
            diffuseBRDF = DiffuseBRDF( properties.diffuseTexture, 0, 0 );
        }

        bool scatter(const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager) const override {
            bool verdict = diffuseBRDF.scatter( ray, attenuation, scattered, intersectionManager );
            return verdict;
        };

        virtual shared_ptr<ImageTexture> getNormalTexture() const { 
           return properties.normalTexture;
        }

    };
#endif
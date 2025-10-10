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
    shared_ptr< Texture > diffuseTexture;
    shared_ptr< Texture > normalTexture;
    float roughnessFactor;
    float subSurfaceFactor;
    float normalTextureFactor;
    float anisotropicFactor;
    float metalFactor;
    float fuzz;
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

        virtual shared_ptr<Texture> getNormalTexture() const { 
            return nullptr; 
        }

        virtual float getNormalTextureFactor() const {
            return 1.0;
        }
};

class Normal : public Material {
    public:
        Normal() {}

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            scattered = Ray( intersectionManager.point, intersectionManager.normal, ray.time() );
            attenuation = 0.5 * Color3( scattered.direction().x() + 1, scattered.direction().y() + 1, scattered.direction().z() + 1 );
        
            return false;
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
        Diffuse( MaterialProperties properties ){
            this -> properties = properties;
        }

        Vector3 reflected() const{
           return generateRandomUnitVector();
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 reflectedVector = intersectionManager.normal + reflected();
            // Vector3 reflectedVector = unitVector( generateRandomUnitHemisphereVector( intersectionManager.shadingNormal  ));
            if( reflectedVector.nearZero() ){
                reflectedVector = intersectionManager.normal;
            }
            
            scattered = Ray( intersectionManager.point, reflectedVector, ray.time() );
            // attenuation = properties.diffuseTexture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );
            attenuation = Color3( 1.0, 0.0, 0.0 );

            return true;
        }

    private:
        shared_ptr< Texture > texture;
        MaterialProperties properties;
};

class Metal : public Material {
    public:
        Metal () {}
        Metal( const Color3 &albedo, double fuzz ) : texture( make_shared< solidColor >( albedo )), fuzz( fuzz < 1 ? fuzz : 1 ) { }
        Metal( shared_ptr< Texture > texture, double fuzz ) : texture( texture ), fuzz( fuzz < 1 ? fuzz : 1 ) {}
        Metal( MaterialProperties properties ) {
            this -> properties = properties;
            fuzz = 0.5;
        }

        Vector3 reflected( const Vector3 & vector, const Vector3 &normal ) const{
            return vector - 2 * dot( vector, normal ) * normal;
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 reflectedVector = reflected( ray.direction(), intersectionManager.shadingNormal );
            reflectedVector = unitVector( reflectedVector ) + ( fuzz * generateRandomUnitVector() );
            scattered = Ray( intersectionManager.point, reflectedVector, ray.time() );
            // attenuation = properties.diffuseTexture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );;

            attenuation = Color3( 1.0, 0.0, 0.0 );

            return ( dot( scattered.direction(), intersectionManager.normal ) > 0 );
        }

    private:
    MaterialProperties properties;
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
        DiffuseBRDF( MaterialProperties properties ){
            this -> properties = properties;
        }

        Vector3 reflected() const{
           return generateRandomUnitVector();
        }

        double fresnelGrazingAngle( Vector3 incomingDirection, Vector3 outgoingDirection ) const{
            Vector3 halfVector = unitVector( incomingDirection + outgoingDirection );
            double approximation = 0.5 + 2 * properties.roughnessFactor * ( std::pow( std::abs( dot( halfVector, outgoingDirection )) , 2) );
            return approximation;
        }

        double subsurfaceGrazingAngle( Vector3 incomingDirection, Vector3 outgoingDirection ) const {
            Vector3 halfVector = unitVector( incomingDirection + outgoingDirection );
            double approximation = properties.roughnessFactor * std::pow( std::abs( dot( halfVector, outgoingDirection )), 2 );
            
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
            Color3 albedo = properties.diffuseTexture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );
            
            Color3 baseDiffuse = ( albedo / PI ) * fresnelEffect( intersectionManager.shadingNormal, incomingDirection, outgoingDirection );
            Color3 subSurface = ( ( 1.25 * albedo ) / PI ) * ( subSurfaceEffect( intersectionManager.shadingNormal, incomingDirection, outgoingDirection ) * ( ( 1 / ( std::abs( dot( intersectionManager.shadingNormal, incomingDirection ) ) + std::abs( dot( intersectionManager.shadingNormal, outgoingDirection )) )) -0.5 ) + 0.5 ) * std::abs( dot( intersectionManager.shadingNormal, outgoingDirection ));

            Color3 diffuse = ( 1 - properties.subSurfaceFactor ) * baseDiffuse + properties.subSurfaceFactor * subSurface;

            return diffuse;
        }

        double getPDF( Vector3 normal, Vector3 outDirection ) const {
            return std::max( dot( normal, outDirection ), 1e-6 )/ PI;
        }

        Vector3 sample( Vector3 normal, Vector3 shadingNormal, bool frontFace ) const {            
            Vector3 scatterDir = unitVector( generateRandomUnitHemisphereVector( shadingNormal  ));
            if( !frontFace ){
                scatterDir = normal;
            }

            return scatterDir;
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 sampleDirection = sample( intersectionManager.normal, intersectionManager.shadingNormal, intersectionManager.frontFace );
            if( sampleDirection.nearZero() ){
                //won't enter here if using hemisphere
                sampleDirection = intersectionManager.shadingNormal;
            }

            Color3 evaluateBRDF = evaluate( intersectionManager, ray.direction(), sampleDirection );
            double pdf = getPDF( intersectionManager.shadingNormal, sampleDirection );

            scattered = Ray( intersectionManager.point, sampleDirection, ray.time() );
            attenuation = evaluateBRDF * dot( intersectionManager.shadingNormal, sampleDirection ) / pdf;

            // std::cout << evaluateBRDF << std::endl;


            return true;
        }

    public:
        MaterialProperties properties;
};

// class DiffuseBRDF : public Material {
// public:
//     DiffuseBRDF() {}
//     DiffuseBRDF(MaterialProperties properties) {
//         this->properties = properties;
//     }

//     Vector3 reflected() const {
//         return generateRandomUnitVector();
//     }

//     double fresnelGrazingAngle(Vector3 incomingDirection, Vector3 outgoingDirection) const {
//         Vector3 halfVector = unitVector(incomingDirection + outgoingDirection);
//         double approximation = 0.5 + 2 * properties.roughnessFactor * 
//                                (std::pow(std::abs(dot(halfVector, outgoingDirection)), 2.0));
//         return approximation;
//     }

//     double subsurfaceGrazingAngle(Vector3 incomingDirection, Vector3 outgoingDirection) const {
//         Vector3 halfVector = unitVector(incomingDirection + outgoingDirection);
//         double approximation = properties.roughnessFactor * 
//                                std::pow(std::abs(dot(halfVector, outgoingDirection)), 2.0);
//         return approximation;
//     }

//     double calculateFresnel(Vector3 normal, Vector3 direction, double grazingApproximation) const {
//         double ndotd = std::clamp(dot(normal, direction), -1.0, 1.0);
//         double approximation = 1.0 + (grazingApproximation - 1.0) * 
//                                (1.0 - std::pow(std::abs(ndotd), 5.0));
//         return approximation;
//     }

//     double calculateSubSurface(Vector3 normal, Vector3 direction, double grazingApproximation) const {
//         double ndotd = std::clamp(dot(normal, direction), -1.0, 1.0);
//         double approximation = 1.0 + (grazingApproximation - 1.0) * 
//                                (1.0 - std::pow(std::abs(ndotd), 5.0));
//         return approximation;
//     }

//     double fresnelEffect(Vector3 normal, Vector3 incomingDirection, Vector3 outgoingDirection) const {
//         double grazingApproximation = fresnelGrazingAngle(incomingDirection, outgoingDirection);
//         double fresIn = calculateFresnel(normal, incomingDirection, grazingApproximation);
//         double fresOut = calculateFresnel(normal, outgoingDirection, grazingApproximation);
//         return fresIn * fresOut * std::max(dot(normal, outgoingDirection), 0.0);
//     }

//     double subSurfaceEffect(Vector3 normal, Vector3 incomingDirection, Vector3 outgoingDirection) const {
//         double grazingApproximation = subsurfaceGrazingAngle(incomingDirection, outgoingDirection);
//         double subIn = calculateSubSurface(normal, incomingDirection, grazingApproximation);
//         double subOut = calculateSubSurface(normal, outgoingDirection, grazingApproximation);
//         return subIn * subOut;
//     }

//     Color3 evaluate(IntersectionManager &intersectionManager, const Vector3 &incomingDirection, Vector3 outgoingDirection) const {
//         Color3 albedo = properties.diffuseTexture->value(intersectionManager.u, intersectionManager.v, intersectionManager.point);

//         // Base Lambertian term
//         Color3 baseDiffuse = (albedo / PI) * fresnelEffect(intersectionManager.shadingNormal, incomingDirection, outgoingDirection);

//         // Subsurface term
//         Color3 subSurface = ((1.25 * albedo) / PI) *
//             (subSurfaceEffect(intersectionManager.shadingNormal, incomingDirection, outgoingDirection) *
//             ((1.0 / (std::abs(dot(intersectionManager.shadingNormal, incomingDirection)) +
//                       std::abs(dot(intersectionManager.shadingNormal, outgoingDirection)))) - 0.5) + 0.5) *
//             std::max(dot(intersectionManager.shadingNormal, outgoingDirection), 0.0);

//         Color3 diffuse = (1.0 - properties.subSurfaceFactor) * baseDiffuse + 
//                          properties.subSurfaceFactor * subSurface;

//         diffuse = Color3(std::max(diffuse.r(), 0.0),std::max(diffuse.g(), 0.0),std::max(diffuse.b(), 0.0));

//         return diffuse;
//     }

//     double getPDF(Vector3 normal, Vector3 outDirection) const {
//         double cosTheta = std::max(dot(normal, outDirection), 0.0);
//         return cosTheta / PI;
//     }

//     Vector3 sample(Vector3 normal, Vector3 shadingNormal, bool frontFace) const {
//         Vector3 scatterDir = unitVector(generateRandomUnitHemisphereVector(shadingNormal));
//         if (!frontFace) {
//             scatterDir = -normal;
//         }
//         return scatterDir;
//     }

//     bool scatter(const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager) const override {
//         Vector3 sampleDirection = sample(intersectionManager.normal, intersectionManager.shadingNormal, intersectionManager.frontFace);
//         if (sampleDirection.nearZero()) {
//             sampleDirection = intersectionManager.shadingNormal;
//         }

//         Color3 evaluateBRDF = evaluate(intersectionManager, ray.direction(), sampleDirection);
//         double pdf = std::max(getPDF(intersectionManager.shadingNormal, sampleDirection), 1e-6);

//         scattered = Ray(intersectionManager.point, sampleDirection, ray.time());
//         attenuation = evaluateBRDF * std::max(dot(intersectionManager.shadingNormal, sampleDirection), 0.0) / pdf;

//         return true;
//     }

// public:
//     MaterialProperties properties;
// };


class MetalBRDF : public Material{
    public:
        MetalBRDF() {}
        MetalBRDF( MaterialProperties properties ){
            this -> properties = properties;
        }

        Vector3 reflected( const Vector3 & vector, const Vector3 &normal ) const{
            return vector - 2 * dot( vector, normal ) * normal;
        }
        
        Color3 calculateFm( Vector3 incomingDirection, Vector3 outgoingDirection, IntersectionManager intersectionManager ) const{
            Color3 baseColor = properties.diffuseTexture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );

            Vector3 halfVector = unitVector(incomingDirection + outgoingDirection);

            Color3 Fm = baseColor + ( Color3( 1, 1, 1) - baseColor ) * ( 1 - std::pow( std::abs( dot( halfVector, outgoingDirection) ), 5 ));
            return Fm;
        }

        double calculateDm( Vector3 incomingDirection, Vector3 outgoingDirection, IntersectionManager intersectionManager ) const {

            Vector3 halfVector = unitVector(incomingDirection + outgoingDirection);
            Vector3 localHalfVector = Vector3(
                dot( halfVector, intersectionManager.T ),
                dot( halfVector, intersectionManager.B ),
                dot( halfVector, intersectionManager.N )
            );

            double aspect = std::sqrt( 1 - 0.9 * properties.anisotropicFactor );
            double alphaX = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) / aspect );
            double alphaY = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) *  aspect);
            double subDenom = (std::pow(localHalfVector.x(), 2) / std::pow( alphaX, 2 ))  + ( std::pow(localHalfVector.y(), 2) / std::pow( alphaY, 2 ) )+ std::pow( localHalfVector.z(), 2 );
            double denom = PI * alphaX * alphaY * std::pow( subDenom, 2 );
            double Dm = ( 1 / denom );

            return Dm;
        }

        double lambda( Vector3 direction, IntersectionManager intersectionManager ) const{
            
            double aspect = std::sqrt( 1 - 0.9 * properties.anisotropicFactor );
            double alphaX = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) / aspect );
            double alphaY = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) *  aspect);

             Vector3 localDirection = Vector3(
                dot( direction, intersectionManager.T ),
                dot( direction, intersectionManager.B ),
                dot( direction, intersectionManager.N )
            );

            double A = 1 + ( ( std::pow( ( localDirection.x() * alphaX ), 2 ) + std::pow( ( localDirection.y() * alphaY ), 2 ) ) / std::pow( localDirection.z(), 2 ) ) - 1;
            
            double value = std::sqrt( std::max( ( A - 1 ) / 2, 0.0 ) );

            return value;
        }

        double G( Vector3 direction, IntersectionManager intersectionManager ) const {
            double value = ( 1 / ( 1 + lambda( direction, intersectionManager ) ) );

            return value;
        }

        double calcualteGm( Vector3 incomingDirection, Vector3 outgoingDirection, IntersectionManager intersectionManager ) const{
            double Gm = G( incomingDirection, intersectionManager ) * G( outgoingDirection, intersectionManager );
            return Gm;
        }

        Color3 evaluate( IntersectionManager &intersectionManager,  const Vector3 &incomingDirection, Vector3 outgoingDirection ) const {
            Color3 Fm = calculateFm( incomingDirection, outgoingDirection, intersectionManager );
            double Dm = calculateDm( incomingDirection, outgoingDirection, intersectionManager );
            double Gm = calcualteGm( incomingDirection, outgoingDirection, intersectionManager );

            Color3 color = ( Fm * Dm * Gm ) / ( 4 * std::abs( dot( intersectionManager.shadingNormal, incomingDirection )));

            return color;
        }

          double getPDF( Vector3 incomingDirection, Vector3 outgoingDirection, IntersectionManager intersectionManager ) const {
            Vector3 halfVector = unitVector( incomingDirection + outgoingDirection );

            double alpha = std::max( std::pow( properties.roughnessFactor, 2 ), 1e-4 );

            double alpha2 = std::pow( alpha, 2 );
            double NdotH = std::max( dot( intersectionManager.shadingNormal, halfVector ), 0.0 );
            double HdotO = std::max( dot( halfVector, outgoingDirection ), 0.0 );

            double denom = PI * alpha2 * std::pow( NdotH * NdotH * ( alpha2 - 1.0 ) + 1.0, 2 );
            double D = alpha2 / denom;

            double lambdaO = ( -1 + std::sqrt(1 + alpha2 * (1 - HdotO*HdotO)/ (HdotO*HdotO) ) ) / 2.0;
            double G1 = 1 / ( 1 + lambdaO );

            double pdf = (D * G1)  /( 4 * HdotO );

            return pdf;
        }

        Vector3 sample( Vector3 incomingDirection, IntersectionManager intersectionManager ) const {            
            Vector3 scatterDir = Vector3( 0, 0, 0 );

            double alpha = std::max( std::pow( properties.roughnessFactor, 2 ), 1e-4 );

            double u1 = generateRandomNumber();
            double u2 = generateRandomNumber();

            double phi = 2.0 * PI * u1;
            double cosTheta = std::sqrt( ( 1.0 - u2 ) / ( 1.0 + ( std::pow( alpha, 2 ) - 1.0 ) * u2 ));
            double sinTheta = std::sqrt( 1.0 - std::pow( cosTheta, 2 ) );

            Vector3 halfTangent(
                sinTheta * std::cos( phi ),
                sinTheta * std::sin( phi ),
                cosTheta
            );

            Vector3 halfVector = unitVector( Vector3(
                halfTangent.x() * intersectionManager.T +
                halfTangent.y() * intersectionManager.B +
                halfTangent.z() * intersectionManager.N
            ));

            scatterDir = unitVector( reflected( -incomingDirection, halfVector ));

            if( dot( scatterDir, intersectionManager.shadingNormal ) < 0 ){
                scatterDir = intersectionManager.shadingNormal;
            }

            return scatterDir;
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 sampleDirection = sample( ray.direction(), intersectionManager  );
            if( sampleDirection.nearZero() ){
                sampleDirection = intersectionManager.shadingNormal;
            }

            Color3 evaluateBRDF = evaluate( intersectionManager, ray.direction(), sampleDirection );
            double pdf = getPDF( ray.direction(), sampleDirection, intersectionManager );

            scattered = Ray( intersectionManager.point, sampleDirection, ray.time() );
            attenuation = evaluateBRDF * dot( intersectionManager.shadingNormal, sampleDirection ) / pdf;

            return true;
        }

    public:
        MaterialProperties properties;
        double alphaMin = 0.0001;
};

class GlassBRDF : public Material{
    public:
        GlassBRDF() {}
        GlassBRDF( MaterialProperties properties ){
            this -> properties = properties;
        }

        //done
        Vector3 refract( const Vector3 &vector, const Vector3 &normal, double etaiOverEtat ) const{
            double cosTheta = std::fmin( dot( -vector, normal ), 1.0 );
            Vector3 rOutPerpendicular = etaiOverEtat * ( vector + cosTheta * normal );
            Vector3 rOutParallel = -std::sqrt( std::fabs( 1.0 - rOutPerpendicular.lengthSquared())) * normal;

            return rOutPerpendicular + rOutParallel;
        }

        //done
        Vector3 reflected( const Vector3 & vector, const Vector3 &normal ) const{
            return vector - 2 * dot( vector, normal ) * normal;
        }

        //done
        double reflectance( double cosine, double RI ) const{
            double r0 = ( 1 - RI ) / ( 1 + RI );
            r0 = r0 * r0;
            return r0 + ( 1 - r0 ) * std::pow( ( 1 - cosine ), 5 );
        }

        //done
        double Rs( Vector3 incomingDirection, Vector3 outgoingDirection ) const {
            Vector3 halfVector = unitVector( incomingDirection + outgoingDirection );

            double nume = ( dot( halfVector, incomingDirection ) - ( refractionIndex * dot( halfVector, outgoingDirection ) ));
            double denom = ( dot( halfVector, incomingDirection ) + ( refractionIndex * dot( halfVector, outgoingDirection ) ));

            double value = nume / denom;

            return value;
        }

        //done
        double Rp(Vector3 incomingDirection, Vector3 outgoingDirection) const {
            Vector3 halfVector = unitVector(incomingDirection + outgoingDirection);

            double nume = (refractionIndex * dot(halfVector, incomingDirection) -  dot(halfVector, outgoingDirection));
            double denom = ( refractionIndex * dot(halfVector, incomingDirection) +  dot(halfVector, outgoingDirection));

            double value = nume / denom;

            return value;
        }

        //done
        double calculateFg( Vector3 incomingDirection, Vector3 outgoingDirection ) const {
            double value = 0.5 * ( std::pow( Rs( incomingDirection, outgoingDirection ), 2 ) + std::pow( Rp( incomingDirection, outgoingDirection ), 2 ) );
            return value;
        }

        //done
        double calculateDg( Vector3 incomingDirection, Vector3 outgoingDirection, IntersectionManager intersectionManager ) const {

            Vector3 halfVector = unitVector(incomingDirection + outgoingDirection);
            Vector3 localHalfVector = Vector3(
                dot( halfVector, intersectionManager.T ),
                dot( halfVector, intersectionManager.B ),
                dot( halfVector, intersectionManager.N )
            );

            double aspect = std::sqrt( 1 - 0.9 * properties.anisotropicFactor );
            double alphaX = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) / aspect );
            double alphaY = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) *  aspect);
            double subDenom = (std::pow(localHalfVector.x(), 2) / std::pow( alphaX, 2 ))  + ( std::pow(localHalfVector.y(), 2) / std::pow( alphaY, 2 ) )+ std::pow( localHalfVector.z(), 2 );
            double denom = PI * alphaX * alphaY * std::pow( subDenom, 2 );
            double Dg = ( 1 / denom );

            return Dg;
        }

        //done
        double lambda( Vector3 direction, IntersectionManager intersectionManager ) const{
            
            double aspect = std::sqrt( 1 - 0.9 * properties.anisotropicFactor );
            double alphaX = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) / aspect );
            double alphaY = std::max( alphaMin, std::pow( properties.roughnessFactor, 2 ) *  aspect);

             Vector3 localDirection = Vector3(
                dot( direction, intersectionManager.T ),
                dot( direction, intersectionManager.B ),
                dot( direction, intersectionManager.N )
            );

            double A = 1 + ( ( std::pow( ( localDirection.x() * alphaX ), 2 ) + std::pow( ( localDirection.y() * alphaY ), 2 ) ) / std::pow( localDirection.z(), 2 ) ) - 1;
            
            double value = std::sqrt( std::max( ( A - 1 ) / 2, 0.0 ) );

            return value;
        }

        //done
        double G( Vector3 direction, IntersectionManager intersectionManager ) const {
            double value = ( 1 / ( 1 + lambda( direction, intersectionManager ) ) );

            return value;
        }

        //done
        double calculateGg( Vector3 incomingDirection, Vector3 outgoingDirection, IntersectionManager intersectionManager ) const{
            double Gg = G( incomingDirection, intersectionManager ) * G( outgoingDirection, intersectionManager );
            return Gg;
        }

        //todo: main formula
        Color3 evaluate( IntersectionManager &intersectionManager,  const Vector3 &incomingDirection, Vector3 outgoingDirection ) const {
            Color3 baseColor = properties.diffuseTexture -> value( intersectionManager.u, intersectionManager.v, intersectionManager.point );
            Color3 color = Color3( 0, 0, 0 );

            Vector3 halfVector = unitVector( incomingDirection + outgoingDirection );
            double Fg = calculateFg( incomingDirection, outgoingDirection );
            double Dg = calculateDg( incomingDirection, outgoingDirection, intersectionManager );
            double Gg = calculateGg( incomingDirection, outgoingDirection, intersectionManager );

            double condition = dot( intersectionManager.normal, incomingDirection ) * dot( intersectionManager.normal, outgoingDirection );
            if( condition > 0 ){
                color = ( baseColor * Fg * Dg * Gg ) / ( 4 * std::abs( dot( intersectionManager.normal, incomingDirection ) ) );
            }
            else{
                Color3 nume = ( sqrt( baseColor ) * ( 1 - Fg ) * Dg * Gg * std::abs( dot( halfVector, outgoingDirection ) * dot( halfVector, incomingDirection ) ) );
                double denom = std::abs( dot( intersectionManager.normal, incomingDirection ) ) * std::pow( dot( halfVector, incomingDirection ) + refractionIndex * dot( halfVector, outgoingDirection ) , 2 );
                
                color = nume / denom;
            }

            return color;
        }

        //todo: pdf
        double getPDF( Vector3 incomingDirection, Vector3 outgoingDirection, IntersectionManager intersectionManager ) const {
            
            Vector3 I = unitVector( incomingDirection );
            Vector3 O = unitVector( outgoingDirection );
            Vector3 N = intersectionManager.shadingNormal;

            bool entering = dot( I, N ) < 0.0;
            double etaI = 1.0;
            double etaT = refractionIndex;

            if( !entering ){
                std::swap( etaI, etaT );
            }

            double eta = etaI / etaT;
            double cosTheta = std::fmin( dot( -I, N ), 1.0 );

            //check this
            double F = reflectance( cosTheta, refractionIndex );

            bool reflectedRay = dot( I, N ) * dot( O, N ) > 0.0;
            return reflectedRay ? F : ( 1.0 - F );
        }

        //todo: sample
        Vector3 sample( Vector3 incomingDirection, IntersectionManager intersectionManager ) const {            
            Vector3 scatterDir = Vector3( 0, 0, 0 );
            
            Vector3 I = unitVector( incomingDirection );
            Vector3 N = intersectionManager.shadingNormal;

            double etaI = 1.0;
            double etaT = refractionIndex;
            bool entering = dot( I, N ) < 0.0;
            if( !entering ){
                std::swap( etaI, etaT );
                N = -N;
            }

            double eta = etaI / etaT;
            double cosTheta = std::fmin( dot( -I, N ), 1.0 );
            double sinTheta = std::sqrt( 1.0 - cosTheta * cosTheta );

            bool cannotRefract = eta * sinTheta > 1.0;
            double F = reflectance( cosTheta, refractionIndex );

            if( cannotRefract || generateRandomNumber() < F ){
                scatterDir = reflected( I, N );
            }
            else{
                scatterDir = refract( I, N, eta );
            }

            scatterDir = unitVector( scatterDir );

            return scatterDir;
        }

        bool scatter( const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager ) const override {
            Vector3 sampleDirection = sample( ray.direction(), intersectionManager  );
            if( sampleDirection.nearZero() ){
                sampleDirection = intersectionManager.shadingNormal;
            }

            Color3 evaluateBRDF = evaluate( intersectionManager, ray.direction(), sampleDirection );
            double pdf = getPDF( ray.direction(), sampleDirection, intersectionManager );

            scattered = Ray( intersectionManager.point, sampleDirection, ray.time() );
            // attenuation = evaluateBRDF * dot( intersectionManager.shadingNormal, sampleDirection ) / pdf;
            // attenuation = Color3( 1.0, 1.0, 1.0 ) / pdf;
            attenuation = evaluateBRDF / pdf;

            return true;
        }

    public:
        MaterialProperties properties;
        double alphaMin = 0.0001;
        double refractionIndex = 2;
};


// class DisneyBRDF : public Material {
//     public:
//         DiffuseBRDF diffuseBRDF;
//         MetalBRDF metalBRDF;

//         MaterialProperties properties;

//         DisneyBRDF( MaterialProperties properties ) {
//             this -> properties = properties;
//             diffuseBRDF = DiffuseBRDF( properties );
//             metalBRDF = MetalBRDF( properties );

//             // diffuseWeight = ( 1 - properties.specularTransmission ) * ( 1 - properties.metalFactor );
//             // metalWeight = ( 1 - properties.specularTransmission * ( 1 - properties.metalFactor ));

//             diffuseWeight = 1 - properties.metalFactor;
//             metalWeight = 1 - diffuseWeight;

//             float normalizeWeights = 1.0f / ( diffuseWeight + metalWeight );

//             diffuseWeight = diffuseWeight * normalizeWeights;
//             metalWeight = metalWeight * normalizeWeights;
//         }

//         Vector3 sample(const Ray &ray, IntersectionManager &intersectionManager) const {
//             float verdict = generateRandomNumber();

//             Vector3 sampledDir = Vector3( 0, 0, 0 );
//             if( verdict < diffuseWeight ){
//                 sampledDir = diffuseBRDF.sample( intersectionManager.normal, intersectionManager.shadingNormal, intersectionManager.frontFace );
//             }
//             else{
//                 sampledDir = metalBRDF.sample( ray.direction(), intersectionManager );
//             }

//             return sampledDir;
//         }

//         bool scatter(const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager) const override {            
//             Vector3 sampleDirection = sample( ray, intersectionManager );

//             Color3 diffuseComponent = diffuseBRDF.evaluate( intersectionManager, ray.direction(), sampleDirection );
//             Color3 metalComponent = metalBRDF.evaluate( intersectionManager, ray.direction(), sampleDirection );

//             float diffusePDF = diffuseBRDF.getPDF( intersectionManager.normal, sampleDirection );
//             float metalPDF = metalBRDF.getPDF( ray.direction(), sampleDirection, intersectionManager );

//             diffusePDF = diffusePDF * diffuseWeight;
//             metalPDF = metalPDF * metalWeight;

//             float totalPDF = diffusePDF + metalPDF;

//             attenuation = diffuseComponent * diffuseWeight + metalComponent * metalWeight;

//             scattered = Ray( intersectionManager.point, sampleDirection, ray.time() );
//             attenuation = attenuation * dot(intersectionManager.shadingNormal, sampleDirection) / totalPDF;

//             return true;
//         };

//         shared_ptr<Texture> getNormalTexture() const override { 
//            return properties.normalTexture;
//         }

//         float getNormalTextureFactor() const override {
//             return properties.normalTextureFactor;
//         }

//     public:
//         float diffuseWeight;
//         float metalWeight;
//     };

class DisneyBRDF : public Material {
    public:
        DiffuseBRDF metal;
        MaterialProperties properties;

        DisneyBRDF( MaterialProperties properties ) {
            this -> properties = properties;
            metal = DiffuseBRDF( properties );
        }

        bool scatter(const Ray &ray, Color3 &attenuation, Ray &scattered, IntersectionManager &intersectionManager) const override {            
            bool value = metal.scatter( ray, attenuation, scattered, intersectionManager );

            return true;
        };

        shared_ptr<Texture> getNormalTexture() const override { 
           return properties.normalTexture;
        }

        float getNormalTextureFactor() const override {
            return properties.normalTextureFactor;
        }
    };
#endif
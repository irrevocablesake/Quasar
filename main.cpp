#include "Renderer.h"

#include<iostream>
#include "Ray.h"
#include "Camera.h"
#include "Image.h"
#include "World.h"
#include "Sphere.h"
#include "Random.h"
#include "Texture.h"
#include "Parallelogram.h"
#include "Box.h"
#include "Transformations.h"
#include "Material.h"
#include "Fog.h"
#include "SphereBox.h"

#include <memory>

using std::make_shared;

void classicScene(){
    const int IMAGE_WIDTH = 1920;
    const double ASPECT_RATIO = 16.0 / 9.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto checkerTexture = make_shared< CheckerTexture >( 0.32, Color3( .2, .3, .1 ), Color3( .9, .9, .9 ));
    auto ground_material = make_shared<Diffuse>( checkerTexture );

    world.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = generateRandomNumber();
            Point3 center(a + 0.9*generateRandomNumber(), 0.2, b + 0.9*generateRandomNumber());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color3( generateRandomNumber(), generateRandomNumber(), generateRandomNumber() );
                    sphere_material = make_shared<Diffuse>(albedo);

                    Point3 center2 = center + Vector3( 0, generateRandomNumber( 0, 0.5 ), 0 );

                    world.add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Color3( generateRandomNumber( 0.5, 1 ), generateRandomNumber( 0.5, 1 ), generateRandomNumber( 0.5, 1 ) );
                    auto fuzz = generateRandomNumber(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Diffuse>(Color3(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 50;
    renderer.maxDepth = 20;
    renderer.vFOV = 20.0;
    renderer.lookFrom = Point3( 13,2,3 );
    renderer.lookAt = Point3( 0,0,0 );
    renderer.vUp = Vector3( 0, 1, 0 );

    renderer.defocusAngle = 0.6;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void checkerTextureScene(){
    const int IMAGE_WIDTH = 400;
    const double ASPECT_RATIO = 16.0 / 9.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto checker = make_shared< CheckerTexture >(0.32, Color3(.2, .3, .1), Color3(.9, .9, .9));

    world.add(make_shared<Sphere>(Point3(0,-10, 0), 10, make_shared<Diffuse>(checker)));
    world.add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Diffuse>(checker)));

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 100;
    renderer.maxDepth = 50;
    renderer.vFOV = 20.0;
    renderer.lookFrom = Point3( 13,2,3 );
    renderer.lookAt = Point3( 0,0,0 );
    renderer.vUp = Vector3( 0, 1, 0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;
    renderer.background = Color3( 0.70, 0.80, 1.00 );

    renderer.initialize();
    renderer.render();
}

void texturedSphere() {
    const int IMAGE_WIDTH = 400;
    const double ASPECT_RATIO = 16.0 / 9.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto texture = make_shared< ImageTexture >("earth.jpg");
    auto surface = make_shared< Diffuse >( texture );
    auto sphere = make_shared< Sphere >( Point3( 0, 0, 0 ), 2, surface );

    world.add( sphere );

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 50;
    renderer.maxDepth = 10;
    renderer.vFOV = 20.0;
    renderer.lookFrom = Point3( 0,0,12 );
    renderer.lookAt = Point3( 0,0,0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0, 0, 0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void parallelogramScene() {
    const int IMAGE_WIDTH = 400;
    const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto leftMaterial     = make_shared<Diffuse>(Color3(1.0, 0.2, 0.2));
    auto backMaterial   = make_shared<Diffuse>(Color3(0.2, 1.0, 0.2));
    auto rightMaterial   = make_shared<Diffuse>(Color3(0.2, 0.2, 1.0));
    auto upMaterial = make_shared<Diffuse>(Color3(1.0, 0.5, 0.0));
    auto downMaterial   = make_shared<Diffuse>(Color3(0.2, 0.8, 0.8));

    auto leftMesh = make_shared< Parallelogram >( Point3(-3,-2, 5), Vector3(0, 0,-4), Vector3(0, 4, 0), leftMaterial );
    auto backMesh = make_shared< Parallelogram >(Point3(-2,-2, 0), Vector3(4, 0, 0), Vector3(0, 4, 0), backMaterial);
    auto rightMesh = make_shared< Parallelogram >(Point3( 3,-2, 1), Vector3(0, 0, 4), Vector3(0, 4, 0), rightMaterial);
    auto upMesh = make_shared< Parallelogram >(Point3(-2, 3, 1), Vector3(4, 0, 0), Vector3(0, 0, 4), upMaterial);
    auto downMesh = make_shared< Parallelogram >(Point3(-2,-3, 5), Vector3(4, 0, 0), Vector3(0, 0,-4), downMaterial);

    world.add( leftMesh );
    world.add( backMesh );
    world.add( rightMesh );
    world.add( upMesh );
    world.add( downMesh );

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 100;
    renderer.maxDepth = 50;
    renderer.vFOV = 80.0;
    renderer.lookFrom = Point3( 0,0,9 );
    renderer.lookAt = Point3( 0,0,0 );
    renderer.vUp = Vector3( 0, 1, 0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void lightScene() {
    const int IMAGE_WIDTH = 1280;
    const double ASPECT_RATIO = 16.0 / 9.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    // auto earthTexture = make_shared< ImageTexture >("earth.jpg");
    // world.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, make_shared<Diffuse>( Color3( 1.0, 1.0, 1.0  ))));
    // world.add(make_shared<Sphere>(Point3(0,2,0), 2, make_shared<Diffuse>( Color3( 1.0, 1.0, 1.0 ))));
    // world.add( make_shared< Box >( Point3( 0, 0, 0 ), Point3( 1, 1, 1 ), make_shared< Diffuse >( Color3( 0.5, 0.5, 0.0 ))));

    // auto checker = make_shared< CheckerTexture >( 0.32, Color3(1, 0, 0), Color3(0,0,1) );

    // auto blueLight = make_shared< Light >( Color3( 0.0, 0.0, 0.5 ));
    auto redLight = make_shared< Light >( Color3( 0.5, 0.0, 0.0 ) );

    world.add(make_shared< Parallelogram >(Point3(3,1,-2), Vector3(2,0,0), Vector3(0,2,0), redLight));
    // world.add(make_shared< Sphere >(Point3(0,6,0), 1, blueLight ));

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 10;
    renderer.maxDepth = 10;
    renderer.vFOV = 20.0;
    renderer.lookFrom = Point3( 26, 3, 6 );
    renderer.lookAt = Point3( 0,2,0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0, 0.0, 0.0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void emptyCornellBox(){
    const int IMAGE_WIDTH = 800;
    const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto redMaterial = make_shared< Diffuse >( Color3( .65, .05, .05 ) );
    auto whiteMaterial = make_shared< Diffuse >( Color3( .73, .73, .73 ) );
    auto greenMaterial = make_shared< Diffuse >( Color3( .12, .45, .15 ) );

    auto lightEmissiveMaterial = make_shared< Light >( Color3( 15, 15, 15 ) );

    auto leftWall = make_shared< Parallelogram >( Point3( 555, 0, 0 ), Vector3( 0, 555, 0 ), Vector3( 0, 0, 555 ), greenMaterial );
    auto rightWall = make_shared< Parallelogram >( Point3( 0, 0, 0 ), Vector3( 0, 555, 0 ), Vector3( 0, 0, 555 ), redMaterial );
    auto backWall = make_shared< Parallelogram >( Point3( 0, 0, 555 ), Vector3( 555, 0, 0 ), Vector3( 0, 555, 0 ), whiteMaterial );
    auto topWall = make_shared< Parallelogram >( Point3( 0, 0, 0 ), Vector3( 555, 0, 0 ), Vector3( 0, 0, 555 ), whiteMaterial );
    auto bottomWall = make_shared< Parallelogram >( Point3( 555, 555, 555 ), Vector3( -555, 0, 0 ), Vector3( 0, 0, -555 ), whiteMaterial );

    auto light = make_shared< Parallelogram >( Point3(343, 554, 332), Vector3(-130,0,0), Vector3(0,0,-105), lightEmissiveMaterial );

    
    std::shared_ptr< Mesh > box1 = make_shared< Box >( Point3( 130, 0, 65 ), Point3( 295, 165, 230 ), make_shared< Diffuse >( Color3( 1.0, 1.0, 1.0 )));
    box1 = make_shared< Translate >( box1, Vector3( -212.5, -82.5, -147.5 ) );
    box1 = make_shared< RotateY >( box1, -15 );
    box1 = make_shared< Translate >( box1, Vector3( 212.5, 82.5, 147.5 ) );

    std::shared_ptr< Mesh > box2 = make_shared< Box >( Point3( 265, 0, 295 ), Point3( 430, 330, 460 ), make_shared< Diffuse >( Color3( 1.0, 1.0, 1.0 )));
    box2 = make_shared< Translate >( box2, Vector3( -347.5, -165, -377.5 ) );
    box2 = make_shared< RotateY >( box2, 18 );
    box2 = make_shared< Translate >( box2, Vector3( 347.5, 165, 377.5 ) );

    world.add( box1 );
    world.add( box2 ); 

    world.add( leftWall );
    world.add( rightWall );
    world.add( topWall );
    world.add( backWall );
    world.add( bottomWall );

    world.add( light );

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 1000;
    renderer.maxDepth = 30;
    renderer.vFOV = 40.0;
    renderer.lookFrom = Point3( 278, 278, -800 );
    renderer.lookAt = Point3( 278,278,0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0.0, 0.0, 0.0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void dualLightemptyCornellBox(){
    const int IMAGE_WIDTH = 400;
    const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto redMaterial = make_shared< Diffuse >( Color3( .65, .05, .05 ) );
    auto whiteMaterial = make_shared< Diffuse >( Color3( .73, .73, .73 ) );
    auto greenMaterial = make_shared< Diffuse >( Color3( .12, .45, .15 ) );

    auto lightRedEmissiveMaterial = make_shared< Light >( Color3( 1.0, 0.0, 0.0 ) );
    auto lightBlueEmissiveMaterial = make_shared< Light >( Color3( 0.0, 0.0, 1.0 ) );

    auto leftWall = make_shared< Parallelogram >( Point3( 555, 0, 0 ), Vector3( 0, 555, 0 ), Vector3( 0, 0, 555 ), greenMaterial );
    auto rightWall = make_shared< Parallelogram >( Point3( 0, 0, 0 ), Vector3( 0, 555, 0 ), Vector3( 0, 0, 555 ), redMaterial );
    auto backWall = make_shared< Parallelogram >( Point3( 0, 0, 555 ), Vector3( 555, 0, 0 ), Vector3( 0, 555, 0 ), whiteMaterial );
    auto topWall = make_shared< Parallelogram >( Point3( 0, 0, 0 ), Vector3( 555, 0, 0 ), Vector3( 0, 0, 555 ), whiteMaterial );
    auto bottomWall = make_shared< Parallelogram >( Point3( 555, 555, 555 ), Vector3( -555, 0, 0 ), Vector3( 0, 0, -555 ), whiteMaterial );

    auto redLight = make_shared< Parallelogram >( Point3(171.5, 554, 332), Vector3(-130,0,0), Vector3(0,0,-105), lightRedEmissiveMaterial );
    auto blueLight = make_shared< Parallelogram >( Point3( 514.5, 554, 332 ), Vector3( -130, 0, 0 ), Vector3( 0, 0, -105 ), lightBlueEmissiveMaterial );

    world.add( leftWall );
    world.add( rightWall );
    world.add( topWall );
    world.add( backWall );
    world.add( bottomWall );

    world.add( redLight );
    world.add( blueLight );

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 50;
    renderer.maxDepth = 5;
    renderer.vFOV = 40.0;
    renderer.lookFrom = Point3( 278, 278, -800 );
    renderer.lookAt = Point3( 278,278,0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0.0, 0.0, 0.0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void simpleBoxMesh(){
    const int IMAGE_WIDTH = 1280;
    const double ASPECT_RATIO = 16.0 / 9.0;
    // const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    world.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, make_shared<Diffuse>( Color3( 1.0, 1.0, 1.0  ))));
    world.add( make_shared< Box >( Point3( 0, 0, 0 ), Point3( 2, 2, 2 ), make_shared< Diffuse >( Color3( 1.0, 1.0, 0.0 ))));

    auto whiteLight = make_shared< Light >( Color3( 4.0, 4.0, 4.0 ) );

    world.add(make_shared< Parallelogram >(Point3(3,1,-2), Vector3(2,0,0), Vector3(0,2,0), whiteLight));
    world.add(make_shared< Sphere >(Point3(0,4,0), 1, whiteLight ));

    std::shared_ptr< Mesh > box = make_shared< Box >( Point3( 0, 0, 0 ), Point3( 2, 2, 2 ), make_shared< Diffuse >( Color3( 1.0, 1.0, 0.0 )));
    box = make_shared< Translate >( box, Vector3( -1, -1, -1 ) );
    box = make_shared< RotateY >( box, 90 );
    box = make_shared< Translate >( box, Vector3( 1, 1, 1 ) );
    box = make_shared< Fog >( box, 0.1, make_shared< Isotropic >( Color3( 1, 0, 0 ) ) );

    world.add( box );

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 15;
    renderer.maxDepth = 10;
    renderer.vFOV = 20.0;
    renderer.lookFrom = Point3( 26, 3, 6 );
    renderer.lookAt = Point3( 0,2,0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0.0, 0.0, 0.0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void perlinNoise() {
  const int IMAGE_WIDTH = 500;
    const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto texture = make_shared< PerlinTexture >( 1, 1, 1, Color3( 1.0, 1.0, 1.0 ), Color3( 0.0, 0.0, 0.0 ) );
    world.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, make_shared<Diffuse>( texture )));

    Renderer renderer( world, image );

    renderer.samplesPerPixel = 30;
    renderer.maxDepth = 15;
    renderer.vFOV = 20.0;
    renderer.lookFrom = Point3( 26, 3, 6 );
    renderer.lookAt = Point3( 0,2,0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 1.0, 1.0, 1.0 );

    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;

    renderer.initialize();
    renderer.render();
}

void motionBlur(){
    const int IMAGE_WIDTH = 800;
    const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto light = make_shared< Light >( Color3( 7, 7, 7 ) );
    world.add( make_shared< Parallelogram >( Point3( 123, 554, 147 ), Vector3( 300, 0, 0 ), Vector3( 0, 0, 265 ), light ) );

    auto groundMaterial = make_shared< Diffuse >( Color3( 0.48, 0.83, 0.53 ) );
    int boxesPerSides = 20;

    for (int i = 0; i < boxesPerSides; i++) {
        for (int j = 0; j < boxesPerSides; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = generateRandomNumber(1,101);
            auto z1 = z0 + w;

            auto boxMesh = make_shared< Box >(Point3(x0,y0,z0), Point3(x1,y1,z1), groundMaterial);

            world.add( boxMesh );
        }
    }

    std::shared_ptr< Mesh > box = make_shared< Box >( Point3( 123, 354, 147 ), Point3( 223,  454, 247 ), make_shared< Diffuse >( Color3( 1.0, 1.0, 1.0 )));
    box = make_shared< Translate >( box, Vector3( -173, -404, -197 ) );
    box = make_shared< RotateY >( box, 45 );
    box = make_shared< Translate >( box, Vector3( 173, 404, 197 ) );

    world.add( box );

    auto newFog = make_shared<Fog>(make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5)), 0.000009, make_shared<Isotropic>(Color3(1, 1, 1)));
    world.add(newFog);

    Renderer renderer( world, image );
    renderer.samplesPerPixel = 1000;
    renderer.maxDepth = 30;
    renderer.vFOV = 40;
    renderer.lookFrom = Point3( 478, 278, -600 );
    renderer.lookAt = Point3( 278, 278, 0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0, 0, 0 );
    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;
    renderer.initialize();
    renderer.render();
}

void heroRender(){
    const int IMAGE_WIDTH = 800;
    const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto light = make_shared< Light >( Color3( 7, 7, 7 ) );
    world.add( make_shared< Parallelogram >( Point3( 123, 554, 147 ), Vector3( 300, 0, 0 ), Vector3( 0, 0, 265 ), light ) );

    auto groundMaterial = make_shared< Diffuse >( Color3( 0.48, 0.83, 0.53 ) );
    int boxesPerSides = 20;

    for (int i = 0; i < boxesPerSides; i++) {
        for (int j = 0; j < boxesPerSides; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = generateRandomNumber(1,101);
            auto z1 = z0 + w;

            auto boxMesh = make_shared< Box >(Point3(x0,y0,z0), Point3(x1,y1,z1), groundMaterial);

            world.add( boxMesh );
        }
    }

    auto diffuseCenter1 = Point3( 400, 400, 200 );
    auto diffuseCenter2 = diffuseCenter1 + Vector3( 30, 0, 0 );
    auto diffuseSphereMaterial = make_shared< Diffuse >( Color3( 0.7, 0.3, 0.1 ) );
    auto diffuseSphereMesh = make_shared< Sphere >( diffuseCenter1, diffuseCenter2, 50, diffuseSphereMaterial );
    world.add( diffuseSphereMesh );

    auto dielectricCenter = Point3( 260, 150, 45 );
    auto dielectricSphereMaterial = make_shared< Dielectric >( 1.5 );
    auto dielectricSphereMesh = make_shared< Sphere >( dielectricCenter, 50, dielectricSphereMaterial );
    world.add( dielectricSphereMesh );
    
    auto checkerTexture = make_shared< CheckerTexture >( 12, Color3( 0.749, 0.098, 0.196 ), Color3( .9, .9, .9 ));
    auto metalCenter = Point3( 0, 150, 145 );
    auto metalSphereMaterial = make_shared< Diffuse >( checkerTexture );
    auto metalSphereMesh = make_shared< Sphere >( metalCenter, 50, metalSphereMaterial );
    world.add( metalSphereMesh );

    auto earthTexture = make_shared< ImageTexture >("earth.jpg");
    auto earthMaterial = make_shared< Diffuse >( earthTexture );
    auto earthMesh = make_shared< Sphere >( Point3( 400, 200, 400 ), 100, earthMaterial );
    world.add( earthMesh );
    

    Color3 surfaceColor( 1.0, 1.0, 1.0 );
    Color3 veinColor( 0.3, 0.3, 0.3 );
    double scale = 4;
    double turbulence = 8;
    double distortion = 10;
    auto perlinNoiseTexture = make_shared< PerlinTexture >( scale, turbulence, distortion, veinColor, surfaceColor );
    auto perlinNoiseMaterial = make_shared< Diffuse >( perlinNoiseTexture );
    auto perlinNoiseMesh = make_shared< Sphere >( Point3( 220, 280, 300 ), 80, perlinNoiseMaterial );
    world.add( perlinNoiseMesh );

    std::shared_ptr< Mesh > sphereBox = make_shared< SphereBox >();
    world.add( sphereBox );

    auto  ReflectiveMetalSphere = make_shared< Sphere >( Point3( 360, 150, 145 ), 70, make_shared< Metal >( Color3( 0.3, 0.0, 0.4), 0.0  ) );
    world.add( ReflectiveMetalSphere );

    auto newFog = make_shared< Fog >( make_shared< Sphere >( Point3( 0, 0, 0 ), 5000, make_shared< Dielectric >( 1.5 ) ), 0.0001, make_shared< Isotropic >( Color3( 1, 1, 1 ) ) );
    world.add( newFog );

    Renderer renderer( world, image );
    renderer.samplesPerPixel = 10000;
    renderer.maxDepth = 40;
    renderer.vFOV = 40;
    renderer.lookFrom = Point3( 478, 278, -600 );
    renderer.lookAt = Point3( 278, 278, 0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0, 0, 0 );
    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;
    renderer.initialize();
    renderer.render();
}

void BLAS(){
     const int IMAGE_WIDTH = 800;
    const double ASPECT_RATIO = 1.0;
    Image image( IMAGE_WIDTH, ASPECT_RATIO );

    World world;

    auto light = make_shared< Light >( Color3( 7, 7, 7 ) );
    world.add( make_shared< Parallelogram >( Point3( 123, 554, 147 ), Vector3( 300, 0, 0 ), Vector3( 0, 0, 265 ), light ) );

    std::shared_ptr< Mesh > sphereBox = make_shared< SphereBox >();
    sphereBox = make_shared< RotateY >( sphereBox, 45 );
    world.add( sphereBox );

    Renderer renderer( world, image );
    renderer.samplesPerPixel = 200;
    renderer.maxDepth = 25;
    renderer.vFOV = 40;
    renderer.lookFrom = Point3( 478, 278, -600 );
    renderer.lookAt = Point3( 278, 278, 0 );
    renderer.vUp = Vector3( 0, 1, 0 );
    renderer.background = Color3( 0, 0, 0 );
    renderer.defocusAngle = 0;
    renderer.focusDistance = 10.0;
    renderer.initialize();
    renderer.render();
}

int main(){
    
    int scene = 12;

    switch( scene ){
        case 1: classicScene();  break;
        case 2: checkerTextureScene(); break; 
        case 3: texturedSphere(); break;
        case 4: parallelogramScene(); break;
        case 5: lightScene(); break;
        case 6: emptyCornellBox(); break;
        case 7: dualLightemptyCornellBox(); break;
        case 8: simpleBoxMesh(); break;
        case 9: perlinNoise(); break;
        case 10: heroRender(); break;
        case 11: motionBlur(); break;
        case 12: BLAS(); break;
    }
    
    return 0;
}
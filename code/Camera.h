#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3.h"
#include "Point3.h"
#include "Ray.h"
#include "Random.h"

class Camera{
    public:
        Point3 position;

        double vFOV;

        Point3 lookFrom;
        Point3 lookAt;
        Vector3 vup;

        double defocusAngle = 0;
        double focusDistance = 10;

        Camera() : position( 0, 0, 0 ) { }

        void updatePosition() {
            position = Point3( lookFrom.x(), lookFrom.y(), lookFrom.z() );
        }

        void updateOrientation(){
            w = unitVector( ( lookFrom - lookAt ) );
            u = unitVector( cross( vup, w ) );
            v = cross( w, u );
        }

        Point3 defocusDiskSample() {
            Vector3 v = randomInUnitDisk();
            return position + ( v[0] * defocusDiskU ) + ( v[1] * defocusDiskV );
        }

    public:
        Vector3 u;
        Vector3 v;
        Vector3 w;

        Vector3 defocusDiskU;
        Vector3 defocusDiskV;
};

#endif
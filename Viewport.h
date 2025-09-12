#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "Camera.h"
#include "Image.h"
#include "Vector3.h"
#include "Point3.h"
#include "Ray.h"
#include "constants.h"

class Viewport {
    public: 
        double width;
        double height;

        Point3 position;

        Vector3 widthVector;
        Vector3 heightVector;

        Vector3 pixelDeltaWidth;
        Vector3 pixelDeltaHeight;

        Point3 topLeftCorner;

        Point3 pixel00Location;

        //is this correct to use
        double shrinkFactor = 0.009;

        Viewport() {} 

        double degreesToRadians(double degrees) {
            return degrees * PI / 180.0;
        }

        Viewport( const Image &image, Camera &camera ) {
            double theta = degreesToRadians( camera.vFOV );
            double h = std::tan( theta / 2 );
            height = 2 * h * camera.focusDistance;
            width = height * ( double ( image.width ) / image.height );

            widthVector = width * camera.u;
            heightVector = height * -camera.v;

            pixelDeltaWidth = widthVector / image.width;
            pixelDeltaHeight = heightVector / image.height;

            topLeftCorner = camera.position - (camera.focusDistance * camera.w ) - widthVector / 2 - heightVector / 2;

            pixel00Location = topLeftCorner + 0.5 * ( pixelDeltaWidth + pixelDeltaHeight  );

            double defocusRadius = camera.focusDistance * std::tan( degreesToRadians( camera.defocusAngle / 2 ));
            camera.defocusDiskU = camera.u * defocusRadius;
            camera.defocusDiskV = camera.v * defocusRadius;
        }
};

#endif
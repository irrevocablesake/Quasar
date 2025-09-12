#ifndef RANDOM_H
#define RANDOM_H

#include "Vector3.h"
#include <cmath>
#include <cstdlib> 

inline double generateRandomNumber(){
    return std::rand() / ( RAND_MAX + 1.0 );
}

inline double generateRandomNumber( double min, double max ){
    return min + ( max - min ) * generateRandomNumber();
} 

inline Vector3 generateRandomVector(){
    return Vector3( generateRandomNumber(), generateRandomNumber(), generateRandomNumber() );
}

inline Vector3 generateRandomVector( double min, double max ){
    return Vector3( generateRandomNumber( min, max ), generateRandomNumber( min, max ), generateRandomNumber( min, max ));
}

inline int generateRandomInteger( int min, int max ){
    return int( generateRandomNumber( min, max + 1 ) );
}

inline Vector3 generateRandomUnitVector(){
    while( true ){
        Vector3 vector = generateRandomVector( -1, 1 );
        double lengthSquared = vector.lengthSquared();
        if( 1e-160 < lengthSquared && lengthSquared <= 1 ){
            return vector / sqrt( lengthSquared );
        }
    }
}

inline Vector3 randomInUnitDisk() {
    while( true ){
        Vector3 p = Vector3( generateRandomNumber( -1, 1 ), generateRandomNumber( -1, 1 ), 0 );
        if( p.lengthSquared() < 1 ){
            return p;
        }
    }
}

#endif
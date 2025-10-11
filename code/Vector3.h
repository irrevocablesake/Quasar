#ifndef VECTOR3_H
#define VECTOR3_H

#include "Dimension3.h"

#include<iostream>
#include <cmath>

class Vector3 : public Dimension3<Vector3>{
    public: 

        using Dimension3::x;
        using Dimension3::y;
        using Dimension3::z;

        Vector3() : Dimension3() {}
        Vector3( double c0, double c1, double c2 ) : Dimension3( c0, c1, c2 ) {}

        double length() const;
        double lengthSquared() const;

        Vector3 operator-() const;

        bool nearZero() const;
};

bool Vector3::nearZero() const {
  double delta = 1e-8;
  return ( std::fabs(x()) < delta ) && ( std::fabs(y()) < delta ) && ( std::fabs(z()) < delta );
}

double Vector3::length() const{
    return sqrt( lengthSquared() );
}

double Vector3::lengthSquared() const{
    return x() * x() + y() * y() + z() * z();
}

Vector3 Vector3::operator-() const{
    return Vector3( -x(), -y(), -z() );
}

inline double dot( const Vector3 dimension0, const Vector3 dimension1 ){
  return dimension0.x() * dimension1.x() + dimension0.y() * dimension1.y() + dimension0.z() * dimension1.z();
}

inline Vector3 cross( const Vector3 &dimension0, const Vector3 &dimension1 ){
  return Vector3(
    dimension0.y() * dimension1.z() - dimension0.z() * dimension1.y(),
    dimension0.z() * dimension1.x() - dimension0.x() * dimension1.z(),
    dimension0.x() * dimension1.y() - dimension0.y() * dimension1.x()
  );
}

inline Vector3 unitVector( const Vector3 &vector ){
    return vector / vector.length();
}

inline Vector3 normalizeVector( const Vector3 &vector ){
    return Vector3( 0.5 * ( vector.x() + 1.0 ), 
                    0.5 * ( vector.y() + 1.0 ), 
                    0.5 * ( vector.z() + 1.0 ) 
                  );
}

inline std::ostream &operator<<( std::ostream &out, const Vector3 &vector ){
    return out << vector.x() << ' ' << vector.y() << ' ' << vector.z() << '\n';
}

#endif
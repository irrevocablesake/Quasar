#ifndef POINT3_H
#define POINT3_H

#include "Dimension3.h"
#include "Vector3.h"

#include<iostream>

class Point3 : public Dimension3<Point3>{
    public: 

        using Dimension3::x;
        using Dimension3::y;
        using Dimension3::z;

        Point3() : Dimension3() {}        
        Point3( double c0, double c1, double c2 ) : Dimension3( c0, c1, c2 ) {}

        Point3 operator-() const;
        Vector3 toVector() const;
};

Vector3 Point3::toVector() const {
    return Vector3( x(), y(), z() );
}

Point3 Point3::operator-() const{
    return Point3( -x(), -y(), -z() );
}

inline std::ostream &operator<<( std::ostream &out, const Point3 &point ){
    return out << point.x() << ' ' << point.y() << ' ' << point.z() << '\n';
}

#endif
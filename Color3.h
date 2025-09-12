#ifndef COLOR3_H
#define COLOR3_H

#include "Dimension3.h"

#include<iostream>

class Color3 : public Dimension3<Color3>{
    public: 
        Color3() : Dimension3() {}
        Color3( const double &c0, const double &c1, const double &c2 ) : Dimension3( c0, c1, c2 ) {}

        double r() const;
        double g() const;
        double b() const;
};

double Color3::r() const{
    return x();
}

double Color3::g() const{
    return y();
}

double Color3::b() const{
    return z();
}

Color3 operator*( double t, const Color3 &color ){
    return Color3( t * color.r(), t * color.g(), t * color.b() );
}

Color3 operator*( Color3 &a, Color3 &b ){
    return Color3( a.r() * b.r(), a.g() * b.g(), a.b() * b.b() );
}

Color3 operator+( const Color3 &colorA, const Color3 &colorB ){
    return Color3( colorA.r() + colorB.r(), colorA.g() + colorB.g(), colorA.b() + colorB.b() );
}

Color3 lerpColor( const Color3 &colorA, const Color3 &colorB, double factor ) {
    return ( 1 - factor ) * colorA + factor * colorB;
}

inline std::ostream &operator<<( std::ostream &out, const Color3 &color ){
    return out << color.r() << ' ' << color.g() << ' ' << color.b() << '\n';
}

#endif
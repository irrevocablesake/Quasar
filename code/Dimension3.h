#ifndef DIMENSION3_H
#define DIMENSION3_H

#include <concepts>

class Color3;

template<typename dimensionType>
class Dimension3{
    protected:
        double components[3];

    protected:
        double x() const;
        double y() const;
        double z() const;

    public:
        Dimension3() : components{ 0, 0, 0 } {}
        Dimension3( const double &c0, const double &c1, const double &c2 ) : components{ c0, c1, c2 } {}

        double operator[]( int i ) const;
        double &operator[]( int i );

        dimensionType &operator+=( const dimensionType &dimension );
        dimensionType &operator*=( double t );
        dimensionType &operator/=( double t );
};

template<typename dimensionType>
double Dimension3<dimensionType>::x() const {
    return components[0];
}

template<typename dimensionType>
double Dimension3<dimensionType>::y() const {
    return components[1];
}

template<typename dimensionType>
double Dimension3<dimensionType>::z() const {
    return components[2];
}

template<typename dimensionType>
double Dimension3<dimensionType>::operator[]( int i ) const {
    return components[i];
}

template<typename dimensionType>
double &Dimension3<dimensionType>::operator[]( int i ) {
    return components[i];
}

template<typename dimensionType>
dimensionType &Dimension3<dimensionType>::operator+=( const dimensionType &dimension ){
    components[0] += dimension.x();
    components[1] += dimension.y();
    components[2] += dimension.z();

    return static_cast<dimensionType&>(*this);
}

template<typename dimensionType>
dimensionType &Dimension3<dimensionType>::operator*=( double t ){
    components[0] *= t;
    components[1] *= t;
    components[2] *= t;

    return static_cast<dimensionType&>(*this);
}

template<typename dimensionType>
dimensionType &Dimension3<dimensionType>::operator/=( double t ){
    return static_cast<dimensionType&>(*this) *= ( 1/ t );
}


template<typename dimensionType>
requires std::derived_from< dimensionType, Dimension3< dimensionType >>
inline dimensionType operator-( const dimensionType &dimension0, const dimensionType &dimension1 ){
    return dimensionType( dimension0.x() - dimension1.x(), dimension0.y() - dimension1.y(), dimension0.z() - dimension1.z() );
}

template<typename dimensionType>
requires ( !std::same_as< dimensionType, Color3 > )
inline dimensionType operator*( const dimensionType &dimension0, const dimensionType &dimension1 ){
    return dimensionType( dimension0.x() * dimension1.x(), dimension0.y() * dimension1.y(), dimension0.z() * dimension1.z() );
}

template<typename dimensionType>
requires ( !std::same_as< dimensionType, Color3 > )
inline dimensionType operator+( const dimensionType &dimension0, const dimensionType &dimension1 ){
    return dimensionType( dimension0.x() + dimension1.x(), dimension0.y() + dimension1.y(), dimension0.z() + dimension1.z() );
}

template<typename dimensionType>
requires ( !std::same_as< dimensionType, Color3 > )
inline dimensionType operator*( double t, const dimensionType &dimension ){
   return dimensionType( t * dimension.x(), t * dimension.y(), t * dimension.z() );
}

template<typename dimensionType>
requires ( !std::same_as< dimensionType, Color3 > )
inline dimensionType operator-( const dimensionType &dimension, double t ){
   return dimensionType( dimension.x() - t, dimension.y() - t,  dimension.z() -t );
}

template<typename dimensionType>
inline dimensionType operator*(  const dimensionType &dimension, double t ){
   return t * dimension;
}

template<typename dimensionType>
inline dimensionType operator/( const dimensionType &dimension, double t ){
   return ( 1 / t ) * dimension;
}


#endif
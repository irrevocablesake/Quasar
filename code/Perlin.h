#ifndef PERLIN_H
#define PERLIN_H

#include "Random.h"
#include "Point3.h"
#include "Vector3.h"

class Perlin {
    public:
        Perlin() {
            for( int i = 0; i < randomSpace; i++ ){
                values[i] = unitVector( generateRandomVector( -1, 1 ) ); 
            }

            computePermutation( permX );
            computePermutation( permY );
            computePermutation( permZ );
        }

        double noise( const Point3 &point ) const {
            auto u = point.x() - std::floor( point.x() );
            auto v = point.y() - std::floor( point.y() );
            auto w = point.z() - std::floor( point.z() );

            auto x = int( std::floor( point.x() ) );
            auto y = int( std::floor( point.y() ) );
            auto z = int( std::floor( point.z() ) );
            Vector3 c[2][2][2];

            for( int i = 0; i < 2; i++ ){
                for( int j = 0; j < 2; j++ ){
                    for( int k = 0; k < 2; k++ ){
                        c[i][j][k] = values[ permX[ ( x + i ) & 255 ] ^ permY[ ( y + j ) & 255 ] ^ permZ[ ( z + k ) & 255 ] ];
                    }
                }
            }

            return perlinInterpolation( c, u, v, w );
        }

        double turbulence( const Point3 &point, int depth ) const {
            auto result = 0.0;
            auto copy = point;
            auto weight = 1.0;

            for( int i = 0; i < depth; i++ ){
                result = result + weight * noise( copy );
                weight = weight * 0.5;
                copy = copy * 2;
            }

            return std::fabs( result );
        }

    private:
        static const int randomSpace = 256;
        Vector3 values[ randomSpace ];

        int permX[ randomSpace ];
        int permY[ randomSpace ];
        int permZ[ randomSpace ];

        static void computePermutation( int *permAxis ){
            for( int i = 0; i < randomSpace; i++ ){
                permAxis[i] = i;
            }

            permute( permAxis );
        }

        static void permute( int *permAxis ){
            int count = randomSpace;
            for( int i = count - 1; i > 0; i-- ){
                int target = generateRandomInteger( 0, i );
                int tmp = permAxis[i];
                permAxis[i] = permAxis[ target ];
                permAxis[ target ] = tmp;
            }
        }

        static double perlinInterpolation( Vector3 c[2][2][2], double u, double v, double w ) {
            auto uu = u * u * ( 3 - 2 * u );
            auto vv = v * v * ( 3 - 2 * v );
            auto ww = w * w * ( 3 - 2 * w );

            auto result = 0.0;

            for( int i = 0; i < 2; i++ ){
                for( int j = 0; j < 2; j++ ){
                    for( int k = 0; k < 2; k++ ){
                        Vector3 weight( u - i, v - j, w - k );
                        result = result + ( i * uu + ( 1 - i ) * ( 1 - uu ) )
                                        * ( j * vv + ( 1 - j ) * ( 1 - vv ) )
                                        * ( k * ww + ( 1 - k ) * ( 1 - ww ) )
                                        * dot( c[i][j][k], weight );
                    }
                }
            }

            return result;
        }
};

#endif
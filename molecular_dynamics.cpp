/*
 [auto_generated]
 libs/numeric/odeint/examples/molecular_dynamics.cpp

 [begin_description]
 Molecular dynamics example.
 [end_description]

 Copyright 2009-2012 Karsten Ahnert
 Copyright 2009-2012 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/numeric/odeint.hpp>
#include <boost/crc.hpp>

#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include "emscripten.h"

using namespace boost::numeric::odeint;



using namespace std;
#define tab "\t"

const size_t n1 = 16;
const size_t n2 = 16;

struct md_system
{
    static const size_t n = n1 * n2;
    typedef std::vector< double > vector_type;

    md_system( double a = 0.0 ,            // strength of harmonic oscillator
               double gamma = 0.0   ,       // friction
               double eps = 0.1 ,          // interaction strenght
               double sigma = 1.0 ,            // interaction radius
               double xmax = 150.0 , double ymax = 150.0 )
    : m_a( a ) , m_gamma( gamma ) 
    , m_eps( eps ) , m_sigma( sigma ) 
    , m_xmax( xmax ) , m_ymax( ymax )
    { }
    
    static void init_vector_type( vector_type &x ) { x.resize( 2 * n ); }
    
    void operator()( vector_type const& x , vector_type const& v , vector_type &a , double t ) const
    {
        for( size_t i=0 ; i<n ; ++i )
        {
            double diffx = x[i] - 0.5 * m_xmax , diffy = x[i+n] - 0.5 * m_ymax;
            double r2 = diffx * diffx + diffy * diffy ;
            double r = std::sqrt( r2 );
            a[     i ] = - m_a * r * diffx - m_gamma * v[     i ] ;
            a[ n + i ] = - m_a * r * diffy - m_gamma * v[ n + i ] ;
        }
        
        for( size_t i=0 ; i<n ; ++i )
        {
            double xi = x[i] , yi = x[n+i];
            xi = periodic_bc( xi , m_xmax );
            yi = periodic_bc( yi , m_ymax );
            for( size_t j=0 ; j<i ; ++j )
            {
                double xj = x[j] , yj = x[n+j];
                xj = periodic_bc( xj , m_xmax );
                yj = periodic_bc( yj , m_ymax );
                
                double diffx = ( xj - xi ) , diffy = ( yj - yi );
                double r = sqrt( diffx * diffx + diffy * diffy );
                double f = lennard_jones( r );
                a[     i ] += diffx / r * f;
                a[ n + i ] += diffy / r * f;
                a[     j ] -= diffx / r * f;
                a[ n + j ] -= diffy / r * f;
            }
        }
    }
    
    void bc( vector_type &x )
    {
        for( size_t i=0 ; i<n ; ++i )
        {
            x[ i     ] = periodic_bc( x[ i     ] , m_xmax );
            x[ i + n ] = periodic_bc( x[ i + n ] , m_ymax );
        }
    }
    
    inline double lennard_jones( double r ) const
    {
        double c = m_sigma / r;
        double c3 = c * c * c;
        double c6 = c3 * c3;
        return 4.0 * m_eps * ( -12.0 * c6 * c6 / r + 6.0 * c6 / r );
    }
    
    static inline double periodic_bc( double x , double xmax )
    {
        return ( x < 0.0 ) ? x + xmax : ( x > xmax ) ? x - xmax : x ;
    }
    
    double m_a;
    double m_gamma;
    double m_eps ;
    double m_sigma ;
    double m_xmax , m_ymax;
};

namespace js
{
    extern "C" 
    {
        extern void init();
        extern void init_window(int window_id);
        extern void switch_draw_buffer(int window_id);
        extern void draw_point(int window_id, double x, double y, int color_r, int color_g, int color_b);   
        extern int get_cycel_waittime();
    }
} 


int main( int argc , char *argv[] )
{  
    const size_t n = md_system::n;
    typedef md_system::vector_type vector_type;
    
    const int window_id=4711;
    
    std::mt19937 rng;
    std::normal_distribution<> dist( 0.0 , 1.0 );
    
    vector_type x , v;
    md_system::init_vector_type( x );
    md_system::init_vector_type( v );
    
    for( size_t i=0 ; i<n1 ; ++i )
    {
        for( size_t j=0 ; j<n2 ; ++j )
        {
            size_t i1 = i*n2+j;
            size_t i2 = i*n2+j+n;
            x[ i1 ] = 5.0 + i * 4.0 ;
            x[ i2 ] = 5.0 + j * 4.0 ;
            v[ i1 ]   = dist( rng ) ;
            v[ i2 ] = dist( rng ) ;
        }
    }
    
    velocity_verlet< vector_type > stepper;
    const double dt = 0.025;
    double t = 0.0;
    md_system sys;
   
    js::init();
    js::init_window(window_id);   
    while(true)
    {

        for( size_t ii=0 ; ii<10 ; ++ii,t+=dt )
            stepper.do_step( sys , std::make_pair( std::ref( x ) , std::ref( v ) ) , t , dt );
        sys.bc( x );
        
        for( size_t i=0 ; i<n ; ++i )
        {
//             boost::crc_basic<8> color[3] = { boost::crc_basic<8>(50),boost::crc_basic<8>(128), boost::crc_basic<8>(180) };
//             for(auto & c: color)
//                 c.process_byte(i);
//             js::draw_point(window_id, x[i], x[i+n], color[0].checksum(), color[1].checksum(), color[2].checksum());
            js::draw_point(window_id, x[i], x[i+n], 255 , 0 , 0 );
        }
        
        std::chrono::milliseconds dura( js::get_cycel_waittime() );
        //std::this_thread::sleep_for( dura );
        js::switch_draw_buffer(window_id);
    }
    return 0;
}

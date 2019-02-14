#ifndef LINKQUALITY_H
#define LINKQUALITY_H

#include <stdint.h>
#include <stddef.h>

// #define USE_DEQUE

#ifdef USE_DEQUE
#include <deque>
#else
#include "circularbuffer.h"
#endif

template< size_t S >
class LinkQuality
{
public:
#ifdef USE_DEQUE
    LinkQuality( int horizon = S )
        :_horizon(horizon)
    #else
    LinkQuality()
#endif
    {

    }

    virtual ~LinkQuality()
    {

    }

    void pushPing()
    {
#ifdef USE_DEQUE
        _pushes.push_back( 0 );
        trim( _horizon );
#else
        _pushes.push( 0 );
#endif
    }

    void pushPong()
    {
#ifdef USE_DEQUE
        _pushes.push_back( 1 );
        trim( _horizon );
#else
        _pushes.push( 1 );
#endif
    }

    double quality()
    {
        double p, pp;
        count( p,  pp );
        return (pp / p);
    }

private:
#ifdef USE_DEQUE
    std::deque< int > _pushes;
#else

#endif
    void count( double& ping, double& pong )
    {
        ping = 0;
        pong = 0;
        for( int i = 0; i < _pushes.size(); ++i )
        {
            if( _pushes[i] == 0 )
                ping += 1.0;
            else if( _pushes[i] == 1 )
            {
                pong += 1.0;
            }
        }
    }

#ifdef USE_DEQUE
    void trim( int horizon )
    {
        while( _pushes.size() > horizon )
        {
            _pushes.pop_back();
        }
    }
    int _horizon;
#else
    CircularBuffer< int, S > _pushes;
#endif

    void reset()
    {
        _pushes.clear();
    }

protected:

};

#endif // LINKQUALITY_H

#ifndef LINKQUALITY_H
#define LINKQUALITY_H

#include <deque>

class LinkQuality
{
public:
    LinkQuality( int horizon = 50 )
        :_horizon(horizon)
    {

    }

    virtual ~LinkQuality()
    {

    }

    void pushPing()
    {
        _pushes.push_back( 0 );
        trim( _horizon );
    }

    void pushPong()
    {
        _pushes.push_back( 1 );
        trim( _horizon );
    }

    double quality()
    {
        double p, pp;
        count( p,  pp );
        return pp / p;
    }

private:
    std::deque< int > _pushes;
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

    void trim( int horizon )
    {
        while( _pushes.size() > horizon )
        {
            _pushes.pop_back();
        }
    }

    int _horizon;

protected:

};

#endif // LINKQUALITY_H

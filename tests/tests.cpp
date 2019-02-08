// 010-TestCase.cpp

// Let Catch provide main():
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

// STD
#include <iostream>
using namespace std;

//
#include <frame.h>
#include <frameparser.h>
#include <circularbuffer.h>

#include <commcontroller.h>

// VALIDATION VECTORS
unsigned char valid_ping[] = { 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0xef };
int valid_ping_size = 7;

// be 00 01 02 77 00 1a 89 ef
unsigned char valid_pong[] = { 0xbe, 0x00, 0x01, 0x02, 0x77, 0x01, 0x3b, 0x99, 0xef };
int valid_pong_size = 9;

int Factorial( int number ) {
    CommController<16> cc;
   return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail
// return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}

TEST_CASE( "PING frames are well formed", "[protocol]" ) {

    int numErrors = 0;
    uint8_t buffer[ 512 ];
    Frame pingFrame = createPingFrame(0x00);
    int bsize = frameToBuffer( pingFrame, buffer, 512 );
    if( bsize != valid_ping_size )
    {
        numErrors = 5000;
    }
    else
    {
        for( int k = 0; k < bsize; ++k )
        {
            if( buffer[k] != valid_ping[k] )
                numErrors++;
        }
    }

    REQUIRE( numErrors == 0 );

}

TEST_CASE( "PONG frames are well formed", "[protocol]" ) {

    int numErrors = 0;
    uint8_t buffer[ 512 ];
    Frame pongFrame = createPongFrame(0x77, 0x01);
    int bsize = frameToBuffer( pongFrame, buffer, 512 );
    if( bsize != valid_pong_size )
    {
        numErrors = 5000;
    }
    else
    {
        for( int k = 0; k < bsize; ++k )
        {
            if( buffer[k] != valid_pong[k] )
                numErrors++;
        }
    }
    REQUIRE( numErrors == 0 );
}

TEST_CASE( "Invalid CRCs are detected by parser", "[protocol]" ) {
    uint8_t buffer[ 512 ];
    FrameParser fp;
    int numValidFrame = 0;
    for( int k = 0; k < 20; ++k )
    {
        Frame pongFrame = createPongFrame(0x77, 0x00);
        int bsize = frameToBuffer( pongFrame, buffer, 512 );
        buffer[ bsize - 1 ] = 0x00;
        for( int i = 0; i < bsize; ++i )
        {
//            cerr << "#";
            if( fp.addByte( buffer[i] ) )
            {
                numValidFrame++;
//                cerr << "VALID" << endl;
            }
        }
    }
    REQUIRE( numValidFrame == 0 );
}

TEST_CASE( "All valid PINGs are detected by parser", "[protocol]" ) {
    uint8_t buffer[ 512 ];
    FrameParser fp;
    int numValidFrame = 0;
    const int numFrames = 2000;
    for( int k = 0; k < numFrames; ++k )
    {
        Frame pingFrame = createPingFrame();
        int bsize = frameToBuffer( pingFrame, buffer, 512 );
        for( int i = 0; i < bsize; ++i )
        {
            if( fp.addByte( buffer[i] ) )
            {
                numValidFrame++;
            }
        }
    }
    REQUIRE( numValidFrame == numFrames );
}

TEST_CASE( "All valid PONGs are detected by parser", "[protocol]" ) {
    uint8_t buffer[ 512 ];
    FrameParser fp;
    int numValidFrame = 0;
    const int numFrames = 2000;
    for( int k = 0; k < numFrames; ++k )
    {
        Frame pongFrame = createPongFrame( rand() % 256, 0x01 );
        int bsize = frameToBuffer( pongFrame, buffer, 512 );
        for( int i = 0; i < bsize; ++i )
        {
            if( fp.addByte( buffer[i] ) )
            {
                numValidFrame++;
            }
        }
    }
    REQUIRE( numValidFrame == numFrames );
}

TEST_CASE( "Factorials of 1 and higher are computed (pass)", "[single-file]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

// Compile & run:
// - g++ -std=c++11 -Wall -I$(CATCH_SINGLE_INCLUDE) -o 010-TestCase 010-TestCase.cpp && 010-TestCase --success
// - cl -EHsc -I%CATCH_SINGLE_INCLUDE% 010-TestCase.cpp && 010-TestCase --success

// Expected compact output (all assertions):
//
// prompt> 010-TestCase --reporter compact --success
// 010-TestCase.cpp:14: failed: Factorial(0) == 1 for: 0 == 1
// 010-TestCase.cpp:18: passed: Factorial(1) == 1 for: 1 == 1
// 010-TestCase.cpp:19: passed: Factorial(2) == 2 for: 2 == 2
// 010-TestCase.cpp:20: passed: Factorial(3) == 6 for: 6 == 6
// 010-TestCase.cpp:21: passed: Factorial(10) == 3628800 for: 3628800 (0x375f00) == 3628800 (0x375f00)
// Failed 1 test case, failed 1 assertion.

// app.cpp - Sample Application 1
//
// This small sample application built with
// the MeisterWerk Framework flashes the
// internal LED

// platform includes
#include <ESP8266WiFi.h>
//#include <Arduino.h>

// Let MeisterWerk output debugs on Serial
#define DEBUG 1

// framework includes
#include "util/debug.h"

#include "core/array.h"
#include <MeisterWerk.h>

#include <util/dumper.h>

using namespace meisterwerk;

typedef struct t_testcase {
    String pub;
    String sub;
    bool   groundTruth;
} T_TESTCASE;

T_TESTCASE tcs[] = {
    {"t1", "t2", false},
    {"t1", "t1", true},
    {"t12", "t1", false},
    {"t1", "t13", false},
    {"t1", "t12", false},
    {"t1", "t1/#", true},
    {"t1", "t1/+", false},
    {"t1/", "t1/#", true},
    {"t1/", "t1/+", true},
    {"t1", "t1/#", true},

    {"t1/t3", "t2/t#", false},
    {"t1/t3", "t2/t+", false},

    {"123/345/567", "#", true},
    {"123/345/567", "+/#", true},
    {"123/345/567", "+/+/+", true},
    {"123/345/567", "+/+/#", true},
    {"123/345/567", "+/+/+/#", true},
    {"123/345/567", "+/+/+/a", false},
    {"123/345/567", "+/345/567", true},
    {"123/45/567", "+/34/567", false},

    {"a", "+", true},
    {"a", "#", true},
    {"", "", true},
    {"a", "", false},
    {"", "a", false},
    {"", "#", false},

    {"abc/def/ghi", "abc/def/ghi", true},
    {"abc/def/ghi", "abc/def/ghi/", false},
    {"abc/def/ghi", "abc/def/gh", false},
    {"abc/def/ghi", "abc/df/ghi", false},
    {"abc/def/ghi", "ab/def/ghi", false},
    {"abc/def/ghi", "abc/def/ghj", false},
    {"abc/def/ghi", "abc/def/ghia", false},
};

// application class
class MyApp : public core::baseapp {
    public:
    // util::dumper dmp;
    core::array<int> ar;
    MyApp() : core::baseapp( "MyApp" ), ar( 10 ) /*, dmp( "dumper" )*/ {
    }

    unsigned int testcase( T_TESTCASE tc ) {
        int errs = 0;
        if ( core::Topic::mqttmatch( tc.pub, tc.sub ) != tc.groundTruth ) {
            Serial.println( tc.pub + "<->" + tc.sub + ", groundTruth=" + String( tc.groundTruth ) +
                            ": ERROR." );
            ++errs;
        } else {
            Serial.println( tc.pub + "<->" + tc.sub + ", groundTruth=" + String( tc.groundTruth ) +
                            ": OK." );
        }
        return errs;
    }

    unsigned int testcases() {
        int errs = 0;
        for ( int i = 0; i < sizeof( tcs ) / sizeof( T_TESTCASE ); i++ ) {
            errs += testcase( tcs[i] );
        }
        return errs;
    }

    unsigned int queueTests() {
        meisterwerk::core::queue<int> qi( 256 );
        int                           errs = 0;
        for ( auto i = 0; i < 257; i++ ) {
            int *pi = (int *)malloc( sizeof( int ) );
            *pi     = i;
            if ( !qi.push( pi ) ) {
                if ( i < 256 )
                    ++errs;
                else
                    Serial.println( "Failure-result writing beyond queue size, OK." );
            } else {
                if ( i >= 256 ) {
                    ++errs;
                    Serial.println( "Pushing beyond queue-size successful! ERROR!" );
                }
            }
        }
        for ( auto i = 0; i < 256; i++ ) {
            int *pi;
            pi = qi.pop();
            if ( *pi != i ) {
                ++errs;
                Serial.println( "Queue pop resulted in corrupted content! ERROR!" );
            }
            free( pi );
        }
        return errs;
    }

    void ArrayTests() {
        Serial.println( "Testing array:" );
        Serial.println( ar.length() );
        Serial.println( "Starting loop:" );
        for ( int i = 0; i < 10; i++ ) {
            Serial.println( "Adding:" + String( i ) );
            ar.add( i );
            Serial.println( "added." );
        }
        Serial.println( "Getting ar[5]:" + String( ar[5] ) );
        Serial.println( "Deleting 5, length:" );
        ar.erase( 5 );
        Serial.println( "Size now:" + String( ar.length() ) );
    }
    virtual void setup() override {
        // Debug console
        Serial.begin( 115200 );
        Serial.println( "\n\n\n" );
        int errs = testcases();
        if ( errs == 0 ) {
            Serial.println( "All tests OK!" );
        } else {
            Serial.println( String( errs ) + " errors, mqttmatches tests failed." );
        }

        ArrayTests();
        /*
        errs = queueTests();
        if ( errs == 0 ) {
            Serial.println( "Queue tests OK!" );
        } else {
            Serial.println( String( errs ) + " errors, queue tests failed." );
        }
        */
    }
};

// Application Instantiation
MyApp app;

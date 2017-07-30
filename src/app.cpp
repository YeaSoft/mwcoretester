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
#include <MeisterWerk.h>

using namespace meisterwerk;

typedef struct t_testcase {
    String pub;
    String sub;
    bool   groundTruth;
} T_TESTCASE;

std::list<T_TESTCASE> tcs = {
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
    MyApp() : core::baseapp( "MyApp" ) {
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
        for ( auto tc : tcs ) {
            errs += testcase( tc );
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

// app.cpp - Sample Application 1
//
// This small sample application built with
// the MeisterWerk Framework flashes the
// internal LED

// platform includes
#include <ESP8266WiFi.h>

// Let MeisterWerk output debugs on Serial
#define DEBUG 1

// framework includes
#include <MeisterWerk.h>

using namespace meisterwerk;

typedef struct t_testcase {
    String s1;
    String s2;
    bool groundTruth;
} T_TESTCASE;

std::list<T_TESTCASE> tcs = {
    {"t1","t2",false},
    {"t1","t1",true},
    {"t12","t1",false},
    {"t1","t13",false},
    {"t1*","t12",true},
    {"t1*","t1",true},
    {"t1*","t",false},
    {"t1*","",false},

    {"t1/t3","t2/t*",false},
    {"t1*/t","t1",false},
    {"t12/*","t1/234/234",false},
    {"*/td","t13/*",true},
    {"t1*/234","t12/234",true},
    {"t1*/234","t1/234",true},
    {"t1*/234","t1/2345",false},
    {"t1*/*","",false},

    {"123/345/567","*",true},
    {"123/345/567","*/*",true},
    {"123/345/567","*/*",true},
    {"123/345/567","*/*/*",true},
    {"123/345/567","*/*/*/a",false},
    {"123/345/567","*/34*/56*",true},
    {"123/45/567","*/34*/56*",false},

    {"","",true},
    {"*","",true},
    {"*","1/2/3/4/5/",true},

    {"abc/def/ghi","abc/def/ghi",true},
    {"abc/def/ghi","abc/def/ghi/",false},
    {"abc/def/ghi","abc/def/gh",false},
    {"abc/def/ghi","abc/df/ghi",false},
    {"abc/def/ghi","ab/def/ghi",false},
    {"abc/def/ghi","abc/def/ghj",false},
    {"abc/def/ghi","abc/def/ghia",false},
};

// application class
class MyApp : public core::baseapp {
    public:
    MyApp() : core::baseapp( "MyApp" ) {
    }

    unsigned int testcase(T_TESTCASE tc) {
        int errs=0;
        if (_app->sched.msgmatches(tc.s1, tc.s2) != tc.groundTruth) {
            Serial.println(tc.s1+"<->"+tc.s2+", groundTruth="+String(tc.groundTruth)+": ERROR.");
            ++errs;
        } else {
            Serial.println(tc.s1+"<->"+tc.s2+", groundTruth="+String(tc.groundTruth)+": OK.");
        }
        if (_app->sched.msgmatches(tc.s2, tc.s1) != tc.groundTruth) {
            Serial.println(tc.s2+"<->"+tc.s1+", groundTruth="+String(tc.groundTruth)+": ERROR.");
            ++errs;
        } else {
            Serial.println(tc.s2+"<->"+tc.s1+", groundTruth="+String(tc.groundTruth)+": OK.");
        }
        return errs;
    }
    unsigned int testcases() {
        int errs=0;
        for (auto tc : tcs) {
            errs += testcase(tc);
        }
        return errs;
    }

    virtual void onSetup() {
        // Debug console
        Serial.begin( 115200 );
        Serial.println("\n\n\n");
        int errs=testcases();
        if (errs==0) {
            Serial.println("All tests OK!");
        } else {
            Serial.println(String(errs)+" Tests failed.");
        }

    }
};

// Application Instantiation
MyApp app;

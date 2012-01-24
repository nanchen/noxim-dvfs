//: TestSuite: Suite.h

#ifndef SIP_SERVER_APP_SUITE_H
#define SIP_SERVER_APP_SUITE_H

#include <vector>
#include <stdexcept>

#include "Test.h"

namespace TestSuite {

class TestSuiteError : public std::logic_error {
public:
    TestSuiteError(const std::string& s = "") : logic_error(s) {}
};

class Suite {
private:
    std::string name;
    std::ostream* osptr;
    std::vector<Test*> tests;
    void reset();

    Suite(const Suite&);
    Suite& operator=(const Suite&);

public:
    Suite(const std::string& name, 
          std::ostream* osptr = &std::cout) : name(name) { 
      this->osptr = osptr; }
    std::string getName() const { return name; }
    long getNumPassed() const;
    long getNumFailed() const;
    const std::ostream* getStream() const { return osptr; }
    void setStream(std::ostream* osptr) { this->osptr = osptr; }
    void addTest(Test* t) throw(TestSuiteError);
    void addSuite(const Suite&);
    void run(); //Calls Test::run() repeatedly
    long report() const;
    void free(); //Deletes all tests
};

} //namespace TestSuite

#endif


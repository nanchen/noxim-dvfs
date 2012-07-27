//: TestSuite: Test.h

#ifndef SIP_SERVER_APP_TEST_H
#define SIP_SERVER_APP_TEST_H

#include <string>
#include <iostream>
#include <cassert>

// fail_() has an underscore to prevent collistion with
// ios::fail(). For consistency, test_() and succeed_()
// also have underscores

#define test_(cond) \
    do_test(cond, #cond, __FILE__,__LINE__)
#define fail_(str) \
    do_fail(str, __FILE__,__LINE__)

namespace TestSuite {
  
class Test{
private:
    std::ostream *osptr;
    long nPass;
    long nFail;
    Test(const Test&);
    Test& operator=(const Test&);
protected:
    void do_test(bool cond, const std::string& lbl,const char *fname, long lineno);
    void do_fail(const std::string& lbl,const char* fname, long lineno);
public:
    Test(std::ostream* osptr = &std::cout) {
        this->osptr = osptr;
        nPass = nFail = 0;
    }

    virtual ~Test() {}
    virtual void run() = 0;
    long getNumPassed() const { return nPass; }
    long getNumFailed() const { return nFail; }
    const std::ostream* getStream() const { return osptr; }
    void setStream(std::ostream* osptr) { this->osptr = osptr; }
    void succeed_() { ++nPass; }
    long report() const;
    virtual void reset() { nPass = nFail = 0; }
};

}

#endif //end define

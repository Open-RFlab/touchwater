#ifndef touchwater_h
#define touchwater_h

/* Author: Mehdi Khairy aka nats
 * Description: This library is used to parse touchstone file v1.2 (a v2.0 support is in the pipe)
 * Documents: https://ibis.org/connector/touchstone_spec11.pdf
 *            https://ibis.org/touchstone_ver2.0/touchstone_ver2_0.pdf
 */

#include <complex>
#include <sstream>
#include <cstring>
#include <fstream>

class TouchstoneParser {

  private:
    std::stringstream filebuffer;

    std::string comments;

    bool optionLineFound = false;
    bool dataValid = false;

    uint64_t freqMul = 0;

    void ParseV1();
    void ParseV2();
  
  public:
    /* Init with a file */
    TouchstoneParser(std::ifstream& in);

};

#endif

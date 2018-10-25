#include "touchwater.hpp"

TouchstoneParser::TouchstoneParser(std::ifstream & in) {
    this->filebuffer << in.rdbuf();
}

void TouchstoneParser::ParseV1() {
	std::string s;
	while(getline(filebuffer, s)) {
		if(s[0] == '!') { /* Detected Comment */
			this->comments = this->comments + s;
		} else if(s[0] == '#') { /* Detected Option Line */
			this->optionLineFound = true;
			
			/* Detect frequency Unit */
			std::string delimiter = " ";

			/* Following the spec page 5
			 * the "#" is followed by a space
			 */
			size_t pos = s.find(delimiter, 2) + 1;
			std::string fUnit = s.substr(1, pos);
			if(fUnit == "Hz") {
				freqMul = 1;
			} else if(fUnit == "KHz") {
				freqMul = 1e3;
			} else if(fUnit == "MHz") {
				freqMul = 1e6;
			} else if(fUnit == "GHz") {
				freqMul = 1e9;
			} else { /* Found an error we stop here */
				return; 
			}

		}
	}
}

void TouchstoneParser::ParseV2() {

}
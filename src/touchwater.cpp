#include "../inc/touchwater.hpp"

TouchstoneParser::TouchstoneParser(std::ifstream & in) {
    this->filebuffer << in.rdbuf();
    this->ParseV1();
}

void TouchstoneParser::ParseV1() {
	std::string s;
	while(getline(filebuffer, s)) {
		if(s[0] == '!') { /* Detected Comment */
			this->comments = this->comments + s;
		} else if(s[0] == '#') { /* Detected Option Line */
            /* Shit parsing because nobody respect the spec ! */
			this->optionLineFound = true;
			
            /* Option parsing state */
            ParseOptionState state = Freq;

			/* Detect frequency Unit */
			std::string delimiter = " ";

			/* Following the spec page 5
			 * the "#" is followed by a space
			 */
            size_t offset = 2;
            bool freqval = false;
            bool formatval = false;
            bool paramval = false;
            bool Rdetected = false;
            bool last =false;
            while(!last) {
                size_t pos = s.find(delimiter, offset);
                if(pos == std::string::npos) {
                    last = true;
                    pos = s.length() - 1;
                }

                std::string sPar = s.substr(offset, pos-offset);

                sPar.erase(std::remove(sPar.begin(), sPar.end(), '\r'),sPar.end());
                sPar.erase(std::remove(sPar.begin(), sPar.end(), '\n'), sPar.end());

                if(!freqval) {
                    freqval = detectFreqUnit(sPar);
                }

                if(!paramval) {
                    paramval = detectParam(sPar);
                }

                if(!formatval) {
                    formatval = detectFormat(sPar);
                }

                if(!Rdetected) {
                    Rdetected = (sPar == "R");
                }

                if(Rdetected) {
                    bool isnumber = true;
                    if(sPar != "") {
                        for(uint8_t i = 0; i < sPar.length(); i++) {
                            if(!std::isdigit(sPar[i])) isnumber = false;
                        }
                        Z0 = (uint64_t)(isnumber ? std::stoi(sPar, 0, 10) : 50);
                    }
                }

                offset = pos + 1;
            }
		}
	}
}

bool TouchstoneParser::detectParam(std::string sF) {
    bool valid = true;
    if(sF == "S")
        paramFormat = Sf;
    else if(sF == "Y")
        paramFormat = Yf;
    else if(sF == "Z")
        paramFormat = Zf;
    else if(sF == "H")
        paramFormat = Hf;
    else if(sF == "G")
        paramFormat = Gf;
    else
        valid = false;
    return valid;
}

bool TouchstoneParser::detectFormat(std::string sF) {
    bool valid = true;
    if(sF == "DB")
        dataFormat = DB;
    else if(sF == "MA")
        dataFormat = MA;
    else if(sF == "RI")
        dataFormat = RI;
    else
        valid = false;
    return valid;
}

bool TouchstoneParser::detectFreqUnit(std::string sF) {
    bool valid = true;
    if(sF == "Hz") {
        freqMul = 1;
    } else if(sF == "KHz") {
        freqMul = 1e3;
    } else if(sF == "MHz") {
        freqMul = 1e6;
    } else if(sF == "GHz") {
        freqMul = 1e9;
    } else { /* Set default value */
        freqMul = 1;
        valid = false;
    }
    return valid;
}

void TouchstoneParser::ParseV2() {

}

#include "../inc/touchwater.hpp"

TouchstoneParser::TouchstoneParser(std::ifstream & in) {
    this->filebuffer << in.rdbuf();
    this->ParseV1();
}

void TouchstoneParser::ParseV1() {
	std::string s;
    std::string data;

    /* File format crappy delimiter */
    std::string delimiter = " ";

	while(getline(filebuffer, s)) {
		if(s[0] == '!') { /* Detected Comment */
			this->comments = this->comments + s;
		} else if(s[0] == '#') { /* Detected Option Line */
            /* Shit parsing because nobody respect the spec ! */
			this->optionLineFound = true;
			
            /* Option parsing state */
            ParseOptionState state = Freq;

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
                    if(sPar != "") {
                        Z0 = std::atof(sPar.c_str());
                    }
                }

                offset = pos + 1;
            }
        } else { /* We expect Frequency data if F < prevF and 2-port file then noise data */
            data = data + s;
        }
	}

    std::istringstream datastream(data);
    uint8_t order = 0;

    while(getline(datastream, s, '\r')) {
        size_t offset = 0;
        bool last = false;

        uint8_t count = 0;
        uint64_t freq = 0;

        std::vector<double> n;

        while(!last && count < 9) {
            size_t pos = s.find(delimiter, offset);
            if(pos == std::string::npos) {
                last = true;
                pos = s.length() - 1;
            }

            std::string sPar = s.substr(offset, pos-offset);

            sPar.erase(std::remove(sPar.begin(), sPar.end(), '\r'),sPar.end());
            sPar.erase(std::remove(sPar.begin(), sPar.end(), '\n'), sPar.end());

            if(sPar[0] == '!') break;

            if(sPar != "") { /* We ignore empty and comment separator */
                if(true) { /* s1p or s2p, needs to be fixed to support sNp */
                    if(count == 0) { /* Frequency */
                        freq = std::atof(sPar.c_str()) * (double)freqMul;
                    } else if (count >= 1) {
                        n.push_back(std::stod(sPar));
                    }
                }
                count++;
            }

            offset = pos + 1;
        }
        order = (order == 0) ? (count-1)/2 : order;

        /* We transform data */
        switch(dataFormat) {
        case(DB):

            break;
        case(MA):
            convertMAtoRI(n, count);
            break;
        case(RI):
            /* Nothing to do :) */
            break;
        }

        /* We convert parameters */
        switch(paramFormat) {
        case(Sf):
            /* Nothing to do :) */
            break;
        case(Yf):
            break;
        }

        /* store data */
        SData sp;
        sp.freq = freq;

        for(uint8_t i = 0; i < count-1; i=i+2) {
            std::complex<double> a(n[i], n[i+1]);
            sp.S.push_back(a);
        }

        Sparam.push_back(sp);
    }
    dataValid = true;
}

void TouchstoneParser::convertMAtoRI(std::vector<double>& n, uint8_t count) {
    double mag = 0.0;
    for(uint8_t i = 0; i < count; i++) {
        if(i%2 == 0) {
            mag = n[i];
            n[i] = mag * std::cos(n[i]);
        } else {
            n[i] = mag * std::sin(n[i]);
        }
    }
}

bool TouchstoneParser::isNumber(std::string s) {
    bool isnum = true;
    for(uint8_t i = 0; i < s.length(); i++) {
        if(!std::isdigit(s[i])) isnum = false;
    }
    return isnum;
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

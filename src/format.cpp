#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long H = seconds / 3600;
    long Hrem = seconds % 3600;
    std::string HH = (H == 0 ? "00" : (H < 10 ? "0" + std::to_string(H) : std::to_string(H)));

    long M = Hrem / 60;
    long Mrem = Hrem % 60;
    std::string MM = (M == 0 ? "00" : (M < 10 ? "0" + std::to_string(M) : std::to_string(M)));

    long S = Mrem % 60;
    std::string SS = (S == 0 ? "00" : (S < 10 ? "0" + std::to_string(S) : std::to_string(S)));

    return HH+':'+MM+':'+SS; 
}

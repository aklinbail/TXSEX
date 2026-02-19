/*This test is for checking the compiler has the right libraries and settings for building Akai Force (and probably MPC) MIDI tools - with a view to forks of older tools*/

#include <iostream>
#include <alsa/asoundlib.h>

int main() {
    std::cout << "Akai Force C++ MIDI Test" << std::endl;
    std::cout << "ALSA Library Version: " << SND_LIB_VERSION_STR << std::endl;
    return 0;
}

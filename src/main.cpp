#include "hex.hpp"

#include <array>
#include <iomanip>
#include <iostream>
#include <string>
int main() {
    std::vector<int> val{0x12345678, 0x0ABCDEF0};
    am::crypto::hex h(val);

    return 0;
}
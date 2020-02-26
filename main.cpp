#include <algorithm>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <cmath>
#include <iostream>
#include <map>
#include <type_traits>

using integer_t = boost::multiprecision::cpp_int;

integer_t genRand2(const integer_t& a, const integer_t& b)
{
    using namespace boost::random;
    static random_device                rd;
    static mt19937                      gen{rd()};
    uniform_int_distribution<integer_t> dist{a, b};
    return dist(gen);
}

int main(const int argc, const char** argv)
try
{
    int radix = 0;
    int nDitits = 0;
    if(argc > 2)
    {
        radix = std::stoi(argv[1]);
        nDitits = std::stoi(argv[2]);
    }
    if(radix <= 0 || nDitits <= 0)
    {
        radix = 2;
        nDitits = 1024;
    }

    using boost::multiprecision::pow;
    const integer_t a = pow(integer_t{radix}, static_cast<unsigned>(nDitits - 1));
    const integer_t b = pow(integer_t{radix}, static_cast<unsigned>(nDitits)) - 1;
    if(b - a <= 2)
    {
        std::cout << 2 << std::endl;
        return 0;
    }

    integer_t randNum;
    int       nAttempts = 0;
    do
    {
        randNum = genRand2(a, b);
        // make it odd
        randNum |= 1;

        constexpr int millerAttempts = 25;
        while(randNum <= b
            && !boost::multiprecision::miller_rabin_test(randNum, millerAttempts))
        {
            randNum += 2;
            ++nAttempts;
        }
    } while(randNum > b);
    std::cout << randNum << std::endl;
    return 0;
}
catch(const std::exception& ex)
{
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
}

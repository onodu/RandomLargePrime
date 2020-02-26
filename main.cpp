#include <algorithm>
#include <atomic>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

using integer_t = boost::multiprecision::cpp_int;

integer_t genRand2(const integer_t& a, const integer_t& b)
{
    using namespace boost::random;
    const std::thread::id       this_id = std::this_thread::get_id();
    static random_device        rd;
    const auto                  sed = rd() + std::hash<std::thread::id>{}(this_id);
    static thread_local mt19937 gen{sed};
    uniform_int_distribution<integer_t> dist{a, b};
    return dist(gen);
}

integer_t parallelGenRandPrime(const integer_t& a, const integer_t& b)
{
    if(b - a <= 2 || a >= b)
        return 2;
    integer_t                globalRet;
    std::atomic_bool         found{false};
    std::mutex               resultMutex;
    std::vector<std::thread> threads;
    const std::size_t        size = std::thread::hardware_concurrency() + 1;
    threads.reserve(size);
    const auto threadFunc = [&a, &b, &found, &globalRet, &resultMutex] {
        integer_t localRet;
        do
        {
            localRet = genRand2(a, b);
            // make it odd
            localRet |= 1;
            constexpr int millerAttempts = 25;
            while(!found && localRet <= b
                && !boost::multiprecision::miller_rabin_test(localRet, millerAttempts))
            {
                localRet += 2;
            }
        } while(!found && localRet > b);
        if(!found)
        {
            found = true;
            std::lock_guard lock{resultMutex};
            globalRet = localRet;
        }
    };
    std::generate_n(std::back_inserter(threads), size,
        [&threadFunc] { return std::thread{threadFunc}; });
    for(auto& t : threads)
        if(t.joinable())
            t.join();
    return globalRet;
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

    const integer_t a =
        boost::multiprecision::pow(integer_t{radix}, static_cast<unsigned>(nDitits - 1));
    const integer_t b =
        boost::multiprecision::pow(integer_t{radix}, static_cast<unsigned>(nDitits)) - 1;
    std::cout << parallelGenRandPrime(a, b) << std::endl;
    return 0;
}
catch(const std::exception& ex)
{
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
}

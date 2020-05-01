#include <algorithm>
#include <boost/atomic/atomic.hpp>
#include <boost/chrono.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/seed_seq.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/scoped_thread.hpp>
#include <functional>
#include <iostream>

using integer_t = boost::multiprecision::cpp_int;

boost::mutex uniquePrintMutex;
template<class... Args>
void uniquePrintln(std::ostream& os, const Args&... args)
{
    boost::lock_guard lock{uniquePrintMutex};
    (os << ... << args);
    os << std::endl;
}

static boost::atomic<unsigned> sed = 0;
integer_t genRand2(const integer_t& a, const integer_t& b)
{
    const auto getSedseq = [] {
        using value_t = boost::random::mt19937::result_type;
        static const auto rdSed = static_cast<value_t>(boost::random::random_device{}());
        const auto chronoSed = static_cast<value_t>(
            boost::chrono::system_clock::now().time_since_epoch().count());
        const auto tidSed = static_cast<value_t>(
            boost::hash<boost::thread::id>{}(boost::this_thread::get_id()));
        return boost::random::seed_seq{rdSed, chronoSed, tidSed};
    };
    static thread_local boost::random::mt19937 gen{sed++};
    boost::random::uniform_int_distribution<integer_t> dist{a, b};
    return dist(gen);
}

integer_t parallelGenRandPrime(const integer_t& a, const integer_t& b)
{
    if(b - a <= 2 || a >= b)
        return 2;
    integer_t globalRet;

    {
        boost::atomic_bool found{false};
        boost::mutex resultMutex;
        using jthread_t = boost::scoped_thread<boost::join_if_joinable>;
        std::vector<jthread_t> threads;
        // const std::size_t size = 1;
        const std::size_t size = boost::thread::hardware_concurrency() + 1;
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
                    && !boost::multiprecision::miller_rabin_test(
                        localRet, millerAttempts))
                {
                    localRet += 2;
                }
            } while(!found && localRet > b);
            if(!found)
            {
                boost::lock_guard lock{resultMutex};
                if(!found)
                {
                    found = true;
                    globalRet = std::move(localRet);
                }
            }
        };
        std::generate_n(std::back_inserter(threads), size,
            [&threadFunc] { return jthread_t{threadFunc}; });
    }
    return globalRet;
}

auto measureGenRandPrime(const integer_t& a, const integer_t& b)
{
    const auto t1 = boost::chrono::steady_clock::now();
    std::cout << parallelGenRandPrime(a, b) << std::endl;
    const auto t2 = boost::chrono::steady_clock::now();
    return boost::chrono::duration<double>{t2 - t1};
}

int main(const int argc, const char** argv)
try
{
    int radix = 0;
    int nDigits = 0;
    if(argc > 2)
    {
        radix = std::stoi(argv[1]);
        nDigits = std::stoi(argv[2]);
    }
    if(radix <= 0 || nDigits <= 0)
    {
        radix = 2;
        nDigits = 1024;
    }

    const integer_t a =
        boost::multiprecision::pow(integer_t{radix}, static_cast<unsigned>(nDigits - 1));
    const integer_t b =
        boost::multiprecision::pow(integer_t{radix}, static_cast<unsigned>(nDigits)) - 1;

    std::cout << parallelGenRandPrime(a, b) << std::endl;
    return 0;
}
catch(const std::exception& ex)
{
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
}

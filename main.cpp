#include <algorithm>
#include <boost/atomic/atomic.hpp>
#include <boost/chrono.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/program_options.hpp>
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

struct Random
{
    using Gen_t = boost::random::mt19937;
    static thread_local Gen_t gen;
    static auto unf(const integer_t& a, const integer_t& b)
    {
        boost::random::uniform_int_distribution<integer_t> dist{a, b};
        return dist(gen);
    }

private:
    static const auto getSedseq()
    {
        using value_t = Gen_t::result_type;
        static const auto rdSed = static_cast<value_t>(boost::random::random_device{}());
        const auto chronoSed = static_cast<value_t>(
            boost::chrono::system_clock::now().time_since_epoch().count());
        const auto tidSed = static_cast<value_t>(
            boost::hash<boost::thread::id>{}(boost::this_thread::get_id()));
        return boost::random::seed_seq{rdSed, chronoSed, tidSed};
    }
};
thread_local Random::Gen_t Random::gen{getSedseq()};

integer_t parallelGenRandPrime(
    const integer_t& a, const integer_t& b, const std::size_t world)
{
    if(b - a <= 2 || a >= b)
        return 2;

    integer_t globalRet;
    {
        boost::atomic_bool found{false};
        boost::mutex resultMutex;
        using jthread_t = boost::scoped_thread<boost::join_if_joinable>;
        std::vector<jthread_t> threads;
        threads.reserve(world);
        const auto threadFunc = [&a, &b, &found, &globalRet, &resultMutex] {
            integer_t localRet;
            do
            {
                localRet = Random::unf(a, b);
                // make it odd
                localRet |= 1;
                constexpr int millerAttempts = 25;
                while(!found && localRet <= b
                    && !boost::multiprecision::miller_rabin_test(
                        localRet, millerAttempts, Random::gen))
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
        std::generate_n(std::back_inserter(threads), world,
            [&threadFunc] { return jthread_t{threadFunc}; });
    }
    return globalRet;
}

auto measureGenRandPrime(const integer_t& a, const integer_t& b, const std::size_t world)
{
    const auto t1 = boost::chrono::steady_clock::now();
    std::cout << parallelGenRandPrime(a, b, world) << std::endl;
    const auto t2 = boost::chrono::steady_clock::now();
    return boost::chrono::duration<double>{t2 - t1};
}

int main(const int argc, const char** argv)
try
{
    integer_t base;
    unsigned number_digits;
    std::size_t world;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message");

    desc.add_options()("base",
        po::value<decltype(base)>(&base)->default_value(2)->notifier(
            [](const auto value) {
                if(value < 2)
                {
                    po::invalid_option_value er{value.str()};
                    er.set_option_name("base");
                    throw er;
                }
            }),
        "integer >= 2, base of a number system");

    desc.add_options()("number_digits",
        po::value<decltype(number_digits)>(&number_digits)
            ->default_value(32)
            ->notifier([](const auto value) {
                if(value < 1)
                {
                    po::invalid_option_value er{std::to_string(value)};
                    er.set_option_name("number_digits");
                    throw er;
                }
            }),
        "integer >= 1, number of digits in base-ary system");

    desc.add_options()("world",
        po::value<decltype(world)>(&world)
            ->default_value(boost::thread::hardware_concurrency() + 1)
            ->notifier([](const auto value) {
                if(value < 1)
                {
                    po::invalid_option_value er{std::to_string(value)};
                    er.set_option_name("world");
                    throw er;
                }
            }),
        "integer >= 1, number of threads to create");

    po::positional_options_description positionalArgs;
    positionalArgs.add("base", 1).add("number_digits", 1).add("world", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                  .options(desc)
                  .positional(positionalArgs)
                  .run(),
        vm);
    po::notify(vm);
    if(vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    const integer_t a = boost::multiprecision::pow(base, number_digits - 1);
    const integer_t b = boost::multiprecision::pow(base, number_digits) - 1;
    std::cout << parallelGenRandPrime(a, b, world) << std::endl;

    return 0;
}
catch(const std::exception& ex)
{
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
}

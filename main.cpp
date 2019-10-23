#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <type_traits>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

using integer_t = boost::multiprecision::cpp_int;

integer_t genRand2(const integer_t& a, const integer_t& b)
{
	using namespace boost::random;
	static random_device rd;
	static mt19937 gen(rd());
	uniform_int_distribution<integer_t> dist(a, b);
	return dist(gen);
}

template<class T>
T lerp(const T& x0, const T& y0,
	const T& x1, const T& y1,
	const T& x)
{
  return y0 * (x - x1) / (x0 - x1)
      + y1 * (x - x0) / (x1 - x0);
}

template<class RandIntF>
void showStat(RandIntF f, const unsigned n)
{
	std::map<std::invoke_result_t<RandIntF>, unsigned> map;
	for(unsigned i = 0; i < n; i++)
	{
		map[f()]++;
	}
	const unsigned max = std::max_element(
		map.begin()
		,map.end()
		,[](const auto &p1, const auto &p2)
		{
			return p1.second < p2.second;
		})->second;
	for(const auto &p : map)
	{
		std::cout << p.first << "\t: " << std::string(std::round(lerp<double>(0,0, max, 60, p.second)), '*') << "\n";
	}
}


int main(int argc, char **argv) try
{
	//showStat([]{return genRand2(1,10);}, 10'000);

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
	const integer_t a = pow(integer_t(radix), unsigned(nDitits - 1));
	const integer_t b = pow(integer_t(radix), unsigned(nDitits)) - 1;
	if(b - a <= 2)
	{
		std::cout << 2 << '\n';
		return 0;
	}

	integer_t randNum;
	int nAttempts = 0;
	do
	{
		randNum = genRand2(a, b);
		// make it odd
		randNum |= 1;

		const int millerAttempts = 25;
		while(randNum <= b && !boost::multiprecision::miller_rabin_test(randNum, millerAttempts))
		{
			randNum += 2;
			++nAttempts;

		}
	}
	while(randNum > b);

	std::cout << randNum << "\n";

	return 0;
}
catch(const std::exception &ex)
{
	std::cerr << "Error: " << ex.what() << std::endl;
	return 1;
}

#include <algorithm>
#include <chrono>
#include <fstream>  
#include <iostream>
#include <map>
#include <math.h>
#include <sstream>
#include <vector>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>


using integer_t = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

integer_t pow(integer_t a, integer_t n)
{
	if(a == 1) return a;
	if(n < 0) return 0;
	integer_t res = 1;
	while(n)
	{
		if((n & 1) == 1) res *= a;
		n /= 2;
		if(n) a *= a;
	}
	return res;

}

integer_t genRand2(integer_t const &a, integer_t const &b)
{
	static boost::random::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	boost::random::uniform_int_distribution<integer_t> dist(a, b);

	return dist(gen);
}
template<class RandF>
void showStat(RandF f, const unsigned n)
{
	std::map<decltype(f()), unsigned> map;
	for(unsigned i = 0; i < n; i++)
	{
		map[f()]++;
	}
	for(const auto &p : map)
	{
		std::cout << p.first << "\t: " << std::string(p.second/static_cast<double>(n)*72, '*') << "\n";
	}
}


int main(int argc, char **argv)
{
	
	try	
	{
		//showStat([](){return genRand2(36,51);}, 1000*10);
		
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
		
		std::cout << "radix = " << radix << "\n";
		std::cout << "nDitits = " << nDitits << "\n";
		
		const integer_t a = pow(integer_t(radix), integer_t(nDitits - 1));
		const integer_t b = pow(integer_t(radix), integer_t(nDitits)) - 1;
		std::cout << "a = " << a << "\n";
		std::cout << "b = " << b << "\n";
		
		integer_t randNum;
		int nAttempts = 0;
		auto t1 = std::chrono::high_resolution_clock::now();
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
				
				auto t2 = std::chrono::high_resolution_clock::now();
				if(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() > 2000)
				{
					std::cout << "nAttempts = " << nAttempts << "...\n";
					t1 = t2;
				}
			}
		}
		while(randNum > b);
		
		std::cout << "nAttempts = " << nAttempts << "\n";
		std::cout << "probably prime = " << randNum << "\n";
	}
	catch(std::exception &ex)
	{
		std::cout << "error: " << ex.what() << "\n";
	}
	
	return 0;
}

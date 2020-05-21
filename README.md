# RandomLargePrime
Generates arbitrary precision random probably prime number.

Usage: ```random-large-prime [base [number_digits [world]]]```, where

- *base* - positive integer >= 2, base of a number system;

- *number_digits* - positive integer >= 1, number of digits in *base*-ary system;

- *world* - positive integer >= 1, number of threads to create (default is ```boost::thread::hardware_concurrency() + 1```).

Output is a random probably prime number with *number_digits* digits in *base*-ary system.

Depends on [Boost](https://www.boost.org/) for multiprecision integers, random and miller_rabin primality test, [conan](https://conan.io/index.html) to install Boost, [python](https://www.python.org/) to install conan.


# Build
```
pip install conan
mkdir build
cd build
cmake -GNinja .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
# Example
```
> random-large-prime 10 5
52561
> random-large-prime 2 128
298276215554587406842518592369777896253
```
# Pseudocode
```
1. Put a := radix ** (nDitits - 1).
2. Put b := radix ** nDitits - 1.
3. Generare randNum - odd random uniform integer within [a, b].
4. While randNum <= b and randNum is composite:
4.1. Increment: randNum += 2.
5. If randNum > b, go to 3.
6. Output randNum.
```

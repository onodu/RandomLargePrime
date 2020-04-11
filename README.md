# RandomLargePrime
Generates arbitrary precision random probably prime number.

Usage: ```random-large-prime base number_digits```,

- where *base* - is a positive integer, base of a number system,

- *number_digits* - is a positive integer, number of digits in *base*-ary system.

Output is a random probably prime number with *number_digits* digits in *base*-ary system.

Depends on Boost for multiprecision integers, random and miller_rabin primality test.

# Build
```
mkdir build
cd build
cmake ..
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
1. Put a := radix ^ (nDitits - 1).
2. Put b := radix ^ (nDitits) - 1.
3. Generare randNum - odd random uniform integer within [a, b].
4. While randNum <= b and randNum is composite:
4.1. Increment: randNum += 2.
5. If randNum > b, go to 3.
6. Output randNum.
```

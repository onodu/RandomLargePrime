# RandomLargePrime
Generates arbitrary precision random probably prime number.

Usage: random-large-prime <i>radix</i> <i>number_digits</i>,

where <i>radix</i> is a positive integer, radix of a numeral system,

<i>number_digits</i> - is a positive integer, number of digits in <i>radix</i>-ary system.

Output is a random probably prime number having <i>number_digits</i> digits in <i>radix</i>-ary system.

Depends on Boost for multiprecision integers, random and miller_rabin primality test.

# Build
```
mkdir
cd build
cmake ..
cmake --build .
```
## Example
```
> random-large-prime 10 5
52561
> random-large-prime 2 128
298276215554587406842518592369777896253
```
## Pseudocode
```
1. Put a := radix ^ (nDitits - 1).
2. Put b := radix ^ (nDitits) - 1.
3. Generare randNum - odd random uniform integer within [a, b].
4. While randNum <= b and randNum is composite:
4.1. Increment: randNum += 2.
5. If randNum > b, go to 3.
6. Output randNum.
```

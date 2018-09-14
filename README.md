# RandomLargePrime
Generates arbitrary precision random probably prime number.

Usage: random-large-prime <i>radix</i> <i>number_digits</i>,

where <i>radix</i> is a positive integer denoting radix of a numeral system,

<i>number_digits</i> - is a positive integer, number of digits in <i>radix</i>-ary system.

Output is a random probably prime number having <i>number_digits</i> digits in <i>radix</i>-ary system.

Depends on Boost for multiprecision integers and miller_rabin primality test.

Example: 
```
> random-large-prime 10 5
radix = 10
nDitits = 5
a = 10000
b = 99999
nAttempts = 14
probably prime = 45737
> random-large-prime 2 128
radix = 2
nDitits = 128
a = 170141183460469231731687303715884105728
b = 340282366920938463463374607431768211455
nAttempts = 11
probably prime = 193777102137037377117124143507618618881
```
## Pseudocode
```
1. Put a := radix**(nDitits - 1).
2. Put b := radix**(nDitits) - 1.
3. Generare randNum - odd random uniform integer within [a,b].
4. While randNum <= b and randNum is composite:
4.1. Increment: randNum += 2.
5. If randNum > b, go to 3.
6. Output randNum.

```

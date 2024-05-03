# single header bit-cast
a simple single header file that defines a function `zxshady::bit_cast<To>(From)` for versions prior to C++20 


# How to use?
```cpp
#include "zxshady/bit_cast.hpp" // just that!
```

# Uses

```cpp

// avoid undefined behavior and ugly casts!
// fast inverse sqaure root
float Q_rsqrt(float number)
{
  long i;
  float x2, y;

  x2 = number * 0.5F;
  y  = number;
// undefined here
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the duck?
// undefined here too
  y  = * ( float * ) &i;
  y  = y * ( 1.5f - ( x2 * y * y ) );      // 1st iteration
}

// correct and safe version using memcpy

#include <cstring>

float Q_rsqrt(float number)
{
  long i;
  float x2, y;

  x2 = number * 0.5F;
  y  = number;
  std::memcpy(&i,&y,sizeof(i))                // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the duck?
  std::memcpy(&i,&y,sizeof(y));
  y  = y * ( 1.5f - ( x2 * y * y ) );      // 1st iteration
}
// if you looked carefully i actually messed up the second memcpy and i copied y to i instead of i to y and this code
// still has undefined behavoir if sizeof(long) != sizeof(float) you can put an assert but that is too much work so we can use zxshady::bit_cast instead 

float Q_rsqrt(float number)
{
  long i;
  float x2, y;

  x2 = number * 0.5F;
  y  = number;
  i = zxshady::bit_cast<long>(y)              // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the duck?
  y = zxshady::bit_cast<float>(i);
  y  = y * ( 1.5f - ( x2 * y * y ) );      // 1st iteration
}

```

# Note
`constexpr` since C++20 (it will use std::bit_cast internally)

# Other advantages over std::bit_cast

1.  Works in C++11 and above
2.  has conversions to arrays of types (will return std::array) and deduces size for unbounded arrays
```cpp

// auto bytes = std::bit_cast<char[4]>(1.0f); error cant return arrays
// auto uhg_ugly = std::bit_cast<std::array<char,4>>(1.0f); // longer and more annoying to type
auto bytes = zxshady::bit_cast<char[4]>(1.0f); // returns std::array<char,4>;
auto bytes_2 = zxshady::bit_cast<char[]>(1.0f); //  deduces array length to 4 returns std::array<char,4>
// auto bytes_3 = zxshady::bit_cast<char[5]>(1.0f); //  static assert will activate saying size mismatch
auto shorts = zxshady::bit_cast<short[]>(1.0f); // deduces array length = 2 returns std::array<short,2>


```


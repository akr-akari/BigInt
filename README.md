# **akr::BigInt**

## **Contents**

  - [1. Require](#1-require)
  - [2. Usage](#2-usage)
  - [3. Operators](#3-operators)
  - [4. Methods](#4-methods)

## **1. Require**
* ### `C++2b`
* ### `boost`

## **2. Usage**
* ### **Zero:**
```c++
#include "bigint.hh"

using namespace akr::literal;

auto bigNum1 = 0_akr_bigint;
auto bigNum2 = akr::BigInt(   );
auto bigNum3 = akr::BigInt( 0 );
auto bigNum2 = akr::BigInt("0");
```

* ### **DEC:**
```c++
auto bigNum1 =  2022_akr_bigint;
auto bigNum2 = akr::BigInt(  2022 );
auto bigNum3 = akr::BigInt( "2022");

auto bigNum4 = +2022_akr_bigint;
auto bigNum5 = akr::BigInt( +2022 );
auto bigNum6 = akr::BigInt("+2022");

auto bigNum7 = -2022_akr_bigint;
auto bigNum8 = akr::BigInt( -2022 );
auto bigNum9 = akr::BigInt("-2022");
```
* ### **BIN:**
```c++
auto bigNum1 =  0B11111100110_akr_bigint;
auto bigNum2 =  0b11111100110_akr_bigint;
auto bigNum3 = akr::BigInt(   "11111100110", 2);
auto bigNum4 = akr::BigInt( "0B11111100110"   );
auto bigNum5 = akr::BigInt( "0b11111100110"   );

auto bigNum6 = +0B11111100110_akr_bigint;
auto bigNum7 = +0b11111100110_akr_bigint;
auto bigNum8 = akr::BigInt(  "+11111100110", 2);
auto bigNum9 = akr::BigInt("+0B11111100110"   );
auto bigNumA = akr::BigInt("+0b11111100110"   );

auto bigNumB = -0B11111100110_akr_bigint;
auto bigNumC = -0b11111100110_akr_bigint;
auto bigNumD = akr::BigInt(  "-11111100110", 2);
auto bigNumE = akr::BigInt("-0B11111100110"   );
auto bigNumF = akr::BigInt("-0b11111100110"   );
```
* ### **OCT:**
```c++
auto bigNum1 =  03746_akr_bigint;
auto bigNum2 = akr::BigInt(  "3746", 8);
auto bigNum3 = akr::BigInt( "03746"   );

auto bigNum4 = +03746_akr_bigint;
auto bigNum5 = akr::BigInt( "+3746", 8);
auto bigNum6 = akr::BigInt("+03746"   );

auto bigNum7 = -03746_akr_bigint;
auto bigNum8 = akr::BigInt( "-3746", 8);
auto bigNum9 = akr::BigInt("-03746"   );
```
* ### **HEX:**
```c++
auto bigNum1 =  0X7E6_akr_bigint;
auto bigNum2 =  0x7e6_akr_bigint;
auto bigNum3 = akr::BigInt(   "7E6", 16);
auto bigNum4 = akr::BigInt( "0X7E6"    );
auto bigNum5 = akr::BigInt( "0x7e6"    );

auto bigNum6 = +0X7E6_akr_bigint;
auto bigNum7 = +0x7e6_akr_bigint;
auto bigNum8 = akr::BigInt(  "+7E6", 16);
auto bigNum9 = akr::BigInt("+0X7E6"    );
auto bigNumA = akr::BigInt("+0x7e6"    );

auto bigNumB = -0X7E6_akr_bigint;
auto bigNumC = -0x7e6_akr_bigint;
auto bigNumD = akr::BigInt(  "-7E6", 16);
auto bigNumE = akr::BigInt("-0X7E6"    );
auto bigNumF = akr::BigInt("-0x7e6"    );
```
* ### **Other base:**
```c++
auto bigNum1 = akr::BigInt("2202220", 3);
auto bigNum2 = akr::BigInt("2686", 9);
auto bigNum3 = akr::BigInt("1K6", 36);
auto bigNum4 = akr::BigInt("1k6", 36);

assert(2022 == bigNum1.ToInteger<int>());
assert(2022 == bigNum2.ToInteger<int>());
assert(2022 == bigNum3.ToInteger<int>());
assert(2022 == bigNum4.ToInteger<int>());
```

## **3. Operators**

* ### **`=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`, `pow_assign`**
```c++
auto num1 = 2022;

auto intNum1 = num1;

auto bigNum1 = 0_akr_bigint;

bigNum1   = num1;
assert(bigNum1 == intNum1);
bigNum1  += num1;
assert(bigNum1 == (intNum1  += num1));
bigNum1  -= num1;
assert(bigNum1 == (intNum1  -= num1));
bigNum1  *= num1;
assert(bigNum1 == (intNum1  *= num1));
bigNum1  /= num1;
assert(bigNum1 == (intNum1  /= num1));
bigNum1  %= num1;
assert(bigNum1 == (intNum1  %= num1));
bigNum1  &= num1;
assert(bigNum1 == (intNum1  &= num1));
bigNum1  |= num1;
assert(bigNum1 == (intNum1  |= num1));
bigNum1  ^= num1;
assert(bigNum1 == (intNum1  ^= num1));

intNum1 = num1;

bigNum1 = num1;

auto i = 9;

bigNum1 <<= i;
assert(bigNum1 == (intNum1 <<= i));
bigNum1 >>= i;
assert(bigNum1 == (intNum1 >>= i));

auto e = 2;
bigNum1.pow_assign(e);
assert(bigNum1 == static_cast<int>(std::pow(intNum1, e)));
```

* ### **`++`, `--`**
```c++
auto intNum1 = 2022;

auto bigNum1 = 2022_akr_bigint;

assert(++bigNum1 == ++intNum1);
assert(bigNum1++ == intNum1++);

assert(--bigNum1 == --intNum1);
assert(bigNum1-- == intNum1--);
```

* ### **`+`, `-`**
```c++
auto num1 = 2022;

assert(+akr::BigInt(num1) == +num1);
assert(-akr::BigInt(num1) == -num1);
```

* ### **`+`, `-`**
```c++
auto num1 = 2022;
auto num2 = 1997;

assert((akr::BigInt(num1) +  num2) == (num1 +  num2));
assert((akr::BigInt(num1) -  num2) == (num1 -  num2));
```

* ### **`*`, `/`, `%`, `pow`**
```c++
auto num1 = 2022;
auto num2 = 1997;

assert((akr::BigInt(num1) *  num2) == (num1 *  num2));
assert((akr::BigInt(num1) /  num2) == (num1 /  num2));
assert((akr::BigInt(num1) %  num2) == (num1 %  num2));

auto e = 2;

assert(akr::pow(akr::BigInt(num1), e) == static_cast<int>(std::pow(num1, e)));
assert(akr::pow(akr::BigInt(num2), e) == static_cast<int>(std::pow(num2, e)));
```

* ### **`~`, `&`, `|`, `^`**
```c++
auto num1 = 2022;
auto num2 = 1997;

assert(~BigInt(num1) == ~num1);

assert((akr::BigInt(num1) &  num2) == (num1 &  num2));
assert((akr::BigInt(num1) |  num2) == (num1 |  num2));
assert((akr::BigInt(num1) ^  num2) == (num1 ^  num2));
```

* ### **`<<`, `>>`**
```c++
auto num1 = 2022;
auto num2 = 1997;

auto i = 9;

assert((akr::BigInt(num1) >> i) == (num1 >> i));
assert((akr::BigInt(num1) << i) == (num1 << i));

assert((akr::BigInt(num2) >> i) == (num2 >> i));
assert((akr::BigInt(num2) << i) == (num2 << i));
```

* ### **`==`, `!=`, `<`, `>=`, `>`, `<=`**
```c++
auto num1 = 2022;
auto num2 = 1997;

assert((akr::BigInt(num1) == num2) == (num1 == num2));
assert((akr::BigInt(num1) != num2) == (num1 != num2));
assert((akr::BigInt(num1) <  num2) == (num1 <  num2));
assert((akr::BigInt(num1) >= num2) == (num1 >= num2));
assert((akr::BigInt(num1) >  num2) == (num1 >  num2));
assert((akr::BigInt(num1) <= num2) == (num1 <= num2));
```

* ### **`<<`, `>>`**
```c++
auto bigNum1 = 0_akr_bigint;

std::cin  >> bigNum1;

std::cout << bigNum1 << '\n';
```

## **4. Methods**
* ### **`void swap(BigInt& rhs) noexcept`**
```c++
auto bigNum1 = 2022_akr_bigint;
auto bigNum2 = 1997_akr_bigint;

bigNum1.swap(bigNum2);

assert(bigNum1 == 1997);
assert(bigNum2 == 2022);
```

* ### **`auto ToInteger<T>() const -> T`**
```c++
auto bigNum1 = 2022_akr_bigint;

auto num1 = bigNum1.ToInteger<int>();
assert(num1 == 2022);

auto num2 = bigNum1.ToInteger<std::int32_t>();
assert(num1 == static_cast<std::int32_t>(2022));

auto num3 = bigNum1.ToInteger<long long>();
assert(num1 == static_cast<long long>(2022));

auto num4 = bigNum1.ToInteger<std::size_t>();
assert(num1 == static_cast<std::size_t>(2022));

try
{
    bigNum1 = -2022_akr_bigint;
    auto num4 = bigNum1.ToInteger<unsigned>();
}
catch (const std::invalid_argument& e)
{
    std::cout << e.what() << '\n';
}

try
{
    bigNum1 = 111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_akr_bigint;
    auto num4 = bigNum1.ToInteger<unsigned>();
}
catch (const std::out_of_range& e)
{
    std::cout << e.what() << '\n';
}
```

* ### **`auto ToBinString() const -> std::string`**
```c++
auto bigNum1 = 2022_akr_bigint;

auto str1 = bigNum1.ToBinString();
assert(str1 == "11111100110");
```

* ### **`auto ToStdString(int base = 10) const -> std::string`**
```c++
auto bigNum1 = 2022_akr_bigint;

auto str1 = bigNum1.ToStdString( );
assert(str1 == "2022");

auto str2 = bigNum1.ToStdString(2);
assert(str2 == "11111100110");

auto str3 = bigNum1.ToStdString(4);
assert(str3 == "133212");

auto str4 = bigNum1.ToStdString(8);
assert(str4 == "3746");

auto str5 = bigNum1.ToStdString(10);
assert(str5 == "2022");

auto str6 = bigNum1.ToStdString(16);
assert(str6 == "7E6");

auto str7 = bigNum1.ToStdString(32);
assert(str7 == "1V6");

try
{
    auto str8 = bigNum1.ToStdString(1);
}
catch (const std::invalid_argument& e)
{
    std::cout << e.what() << '\n';
}
```

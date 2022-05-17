#define D_AKR_TEST
#include "akr_test.hh"

#include "..\bigint.hh"

#include <chrono>
#include <iostream>

using namespace std;
using namespace akr;
using namespace akr::literals;

template<class F>
auto Test(const char* str, const F& func, std::size_t count) noexcept
{
    using namespace std::chrono;

    auto a = 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_akr_bigint;
    auto b = 1234567890_akr_bigint;

    auto t1 = system_clock::now();
    for (auto i = 1zu; i <= count; i++)
    {
        func(a, b);

        if (i == count)
        {
            cout << str << ": " << func(a, b) << '\n';
        }
    }
    auto t2 = system_clock::now();

    printf("time: %4lldms\n", duration_cast<milliseconds>(t2 - t1).count());

    puts("--------------------------------------------------------------------------------");
}

int main()
{
    auto num = 0_akr_bigint;

    constexpr auto Count1 = 10'0000zu;
    constexpr auto Count2 = Count1 / 1000;

    Test("a +  b", [](auto&& a_, auto&& b_) noexcept { return a_ +  b_; }, Count1);
    Test("a += b", [](auto&& a_, auto&& b_) noexcept { return a_ += b_; }, Count1);
    Test("b +  a", [](auto&& a_, auto&& b_) noexcept { return b_ +  a_; }, Count1);
    Test("b += a", [](auto&& a_, auto&& b_) noexcept { return b_ += a_; }, Count1);

    Test("a -  b", [](auto&& a_, auto&& b_) noexcept { return a_ -  b_; }, Count1);
    Test("a -= b", [](auto&& a_, auto&& b_) noexcept { return a_ -= b_; }, Count1);
    Test("b -  a", [](auto&& a_, auto&& b_) noexcept { return b_ -  a_; }, Count1);
    Test("b -= a", [](auto&& a_, auto&& b_) noexcept { return b_ -= a_; }, Count1);

    Test("a *  b", [](auto&& a_, auto&& b_) noexcept { return a_ *  b_; }, Count1);
    Test("a *= b", [](auto&& a_, auto&& b_) noexcept { return a_ *= b_; }, Count2);
    Test("b *  a", [](auto&& a_, auto&& b_) noexcept { return b_ *  a_; }, Count1);
    Test("b *= a", [](auto&& a_, auto&& b_) noexcept { return b_ *= a_; }, Count2);

    Test("a /  b", [](auto&& a_, auto&& b_) noexcept { return a_ /  b_; }, Count2);
    Test("a /= b", [](auto&& a_, auto&& b_) noexcept { return a_ /= b_; }, Count1);
    Test("b /  a", [](auto&& a_, auto&& b_) noexcept { return b_ /  a_; }, Count1);
    Test("b /= a", [](auto&& a_, auto&& b_) noexcept { return b_ /= a_; }, Count1);

    Test("a %  b", [](auto&& a_, auto&& b_) noexcept { return a_ %  b_; }, Count2);
    Test("a %= b", [](auto&& a_, auto&& b_) noexcept { return a_ %= b_; }, Count1);
    Test("b %  a", [](auto&& a_, auto&& b_) noexcept { return b_ %  a_; }, Count1);
    Test("b %= a", [](auto&& a_, auto&& b_) noexcept { return b_ %= a_; }, Count1);

    Test("a ^= 5", [](auto&& a_, auto&&) noexcept { return a_.pow_assign(5); }, 1);
}

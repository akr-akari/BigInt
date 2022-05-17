#ifndef Z_AKR_BIGINT_HH
#define Z_AKR_BIGINT_HH

#include <boost/dynamic_bitset.hpp>

#include <algorithm>
#include <charconv>
#include <concepts>
#include <cstring>
#include <initializer_list>
#include <iosfwd>
#include <numeric>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>

#ifdef  _MSC_VER
#if _MSC_VER <= 1932
#pragma warning(push)
#pragma warning(disable: 4455)
consteval auto operator""zu(unsigned long long value) noexcept
{
    return static_cast<std::size_t>(value);
}
#pragma warning(pop)
#endif
#endif//_MSC_VER

namespace akr
{
    struct BigInt final
    {
        private:
        using BitSet = boost::dynamic_bitset<std::size_t>;

        private:
        struct Trim final
        {
            private:
            std::size_t count0 = 0;

            std::size_t count1 = 0;

            public:
            Trim() = default;

            public:
            void Count(bool bit) noexcept
            {
                if (bit)
                {
                    count0 = 0;

                    count1++;
                }
                else
                {
                    count1 = 0;

                    count0++;
                }
            }

            public:
            void operator()(BigInt& bigInt) const
            {
                bigInt.binNumber.resize(bigInt.binNumber.size() - (!bigInt.isNegative ? count0 : count1));
            }
        };

        private:
        BitSet binNumber;

        bool   isNegative = false;

        public:
        BigInt() = default;

        template<std::integral T>
        BigInt(T number):
            BigInt(std::to_string(number))
        {
        }

        template<std::size_t N>
        BigInt(const char(&numChars)[N], std::optional<int> base = std::nullopt):
            BigInt(std::string_view(numChars, N - 1), base)
        {
        }

        template<class T>
        requires(std::same_as<typename T::value_type, char>)
        BigInt(const T& numStrContainer, std::optional<int> base = std::nullopt):
            BigInt(std::string_view(numStrContainer.begin(), numStrContainer.end()), base)
        {
        }

        private:
        BigInt(std::string_view numStrView, std::optional<int> base = std::nullopt)
        {
            static const auto charToDigit = [](char c_)
            {
                if ('0' <= c_ && c_ <= '9')
                {
                    return c_ - '0';
                }
                else if ('A' <= c_ && c_ <= 'Z')
                {
                    return c_ - 'A' + 10;
                }
                else if ('a' <= c_ && c_ <= 'z')
                {
                    return c_ - 'a' + 10;
                }
                else
                {
                    throw std::invalid_argument("character is invalid.");
                }
            };

            static const auto checkNumStr = [](auto&& numStrView_, auto base_)
            {
                return !std::ranges::all_of(numStrView_, [&base_](char c_)
                                            {
                                                const auto digit_ = charToDigit(c_);

                                                return 0 <= digit_ && digit_ < base_;
                                            });
            };

            static const auto mod2 = [](auto&& strSpan_, auto base_) -> bool
            {
                return std::accumulate(strSpan_.rbegin(), strSpan_.rbegin() + (base_ % 2 == 0 ? 1 : strSpan_.size()),
                                       0, [](auto a_, char c_)
                                       {
                                           return a_ + charToDigit(c_) % 2;
                                       }) % 2;
            };

            static const auto div2 = [](auto&& strSpan_, auto base_)
            {
                const bool isOddBase_ = base_ % 2;

                const auto carryBase_ = base_ / 2;

                auto carry_ = 0;

                auto tmpSpan_ = std::decay_t<decltype(strSpan_)> {};

                auto tmpSize_ = 0zu;

                for (auto i_ = 0zu; auto&& e_ : strSpan_)
                {
                    const bool isCarry0_ = carry_;

                    const auto curDigit_ = charToDigit(e_);

                    const bool oddDigit_ = curDigit_ % 2;

                    const auto newDigit_ = curDigit_ / 2 + carry_ + (isCarry0_ & isOddBase_ & oddDigit_);

                    carry_ = carryBase_ * ((isCarry0_ & isOddBase_) ^ oddDigit_);

                    if (const auto newChar_ = digitToChar(newDigit_); !tmpSpan_.empty())
                    {
                        tmpSpan_[tmpSize_++] = newChar_;
                    }
                    else if (newDigit_ != 0)
                    {
                        tmpSpan_ = strSpan_.subspan(i_);

                        tmpSpan_[tmpSize_++] = newChar_;
                    }
                    else
                    {
                        i_++;
                    }
                }

                return tmpSpan_;
            };

            if (base.has_value())
            {
                if (!(2 <= base.value() && base.value() <= 36))
                {
                    throw std::invalid_argument("base is invalid.");
                }

                for (;;)
                {
                    switch (numStrView.front())
                    {
                        case '0':
                            if (numStrView.size() == 1)
                            {
                                isNegative = false;
                                return;
                            }
                            break;
                        case '1':
                            if (numStrView.size() == 1 && isNegative)
                            {
                                return;
                            }
                            break;
                        case '+':
                            numStrView.remove_prefix(1);
                            continue;
                        case '-':
                            isNegative = true;
                            numStrView.remove_prefix(1);
                            continue;
                        default:
                            break;
                    }
                    break;
                }
            }
            else
            {
                for (;;)
                {
                    switch (numStrView.front())
                    {
                        case '0':
                            if (numStrView.size() == 1)
                            {
                                isNegative = false;
                                return;
                            }
                            else
                            {
                                base = 010;
                                numStrView.remove_prefix(1);
                                continue;
                            }
                        case '1':
                            if (numStrView.size() == 1 && isNegative)
                            {
                                return;
                            }
                            break;
                        case '+':
                            numStrView.remove_prefix(1);
                            continue;
                        case '-':
                            isNegative = true;
                            numStrView.remove_prefix(1);
                            continue;
                        case 'B':
                            [[fallthrough]];
                        case 'b':
                            if (base.has_value())
                            {
                                base = 0B10;
                                numStrView.remove_prefix(1);
                            }
                            break;
                        case 'X':
                            [[fallthrough]];
                        case 'x':
                            if (base.has_value())
                            {
                                base = 0X10;
                                numStrView.remove_prefix(1);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }

                if (!base.has_value())
                {
                    base = 10;
                }
            }

            const auto newBase = base.value();

            if (checkNumStr(numStrView, newBase))
            {
                throw std::invalid_argument("character is invalid.");
            }

            auto dupStr = std::string(numStrView);

            bool isFlip = true;

            for (auto dupSpan = std::span(dupStr.begin(), dupStr.end()); !dupSpan.empty();)
            {
                const auto bit = mod2(dupSpan, newBase);

                if (!isNegative)
                {
                    binNumber.push_back(bit);
                }
                else if (isFlip)
                {
                    binNumber.push_back(bit);

                    if (bit)
                    {
                        isFlip = false;
                    }
                }
                else
                {
                    binNumber.push_back(!bit);
                }

                dupSpan = div2(dupSpan, newBase);
            }

            if (isNegative && binNumber[binNumber.size() - 1])
            {
                binNumber.pop_back();
            }
        }

        public:
        auto operator+= (const BigInt& rhs) -> BigInt&
        {
            auto&& lhs = *this;

            if (rhs.isZero())
            {
                return lhs;
            }

            if (lhs.isZero())
            {
                return lhs = rhs;
            }

            auto needCarry = false;

            if (lhs.isNegative == rhs.isNegative)
            {
                forEachBit(rhs, [&needCarry       ](bool lhs_, bool rhs_) noexcept
                           {
                               const auto newBit_ = lhs_ ^ rhs_ ^ needCarry;

                               if (lhs_ == rhs_)
                               {
                                   needCarry = lhs_;
                               }

                               return newBit_;
                           });

                const bool newBit = lhs.isNegative ^ rhs.isNegative ^ needCarry;

                if (newBit ^ lhs.isNegative)
                {
                    lhs.binNumber.push_back(newBit);
                }
            }
            else
            {
                Trim trim;

                forEachBit(rhs, [&needCarry, &trim](bool lhs_, bool rhs_) noexcept
                           {
                               const auto newBit_ = lhs_ ^ rhs_ ^ needCarry;

                               if (lhs_ == rhs_)
                               {
                                   needCarry = lhs_;
                               }

                               trim.Count(newBit_);

                               return newBit_;
                           });

                lhs.isNegative = lhs.isNegative ^ rhs.isNegative ^ needCarry;

                trim(lhs);
            }

            return lhs;
        }
        auto operator-= (const BigInt& rhs) -> BigInt&
        {
            auto&& lhs = *this;

            if (rhs.isZero())
            {
                return lhs;
            }

            if (lhs.isZero())
            {
                (-rhs).swap(lhs);

                return lhs;
            }

            if (lhs == rhs)
            {
                BigInt("0").swap(lhs);

                return lhs;
            }

            return lhs += -rhs;
        }
        auto operator*= (const BigInt& rhs) -> BigInt&
        {
            auto&& lhs = *this;

            if (lhs.isZero())
            {
                return lhs;
            }

            if (rhs.isZero())
            {
                BigInt("0").swap(lhs);

                return lhs;
            }

            static const auto mul = [](auto&& lhs_, auto&& rhs_, auto&& ret_)
            {
                BigInt("0").swap(ret_);

                const auto posLhsLength_ = lhs_.binNumber.size();

                for (auto i_ = 0zu; i_ < posLhsLength_; i_++)
                {
                    if (lhs_.binNumber[i_])
                    {
                        ret_ += rhs_;
                    }

                    rhs_ <<= 1;
                }
            };

            const bool isLhsNegative = lhs.isNegative ^ rhs.isNegative;

            auto posLhs = lhs.isNegative ? -lhs : lhs;

            auto posRhs = rhs.isNegative ? -rhs : rhs;

            if (posLhs.binNumber.size() >= posRhs.binNumber.size())
            {
                mul(posRhs, posLhs, lhs);
            }
            else
            {
                mul(posLhs, posRhs, lhs);
            }

            if (isLhsNegative)
            {
                lhs.unaryMinus();
            }

            return lhs;
        }
        auto operator/= (const BigInt& rhs) -> BigInt&
        {
            if (rhs.isZero())
            {
                throw std::invalid_argument("right operand is zero.");
            }

            auto&& lhs = *this;

            if (lhs.isZero())
            {
                return lhs;
            }

            if (lhs == rhs)
            {
                BigInt("1").swap(lhs);

                return lhs;
            }

            static const auto div = [](auto&& lhs_, auto&& rhs_, auto&& ret_)
            {
                BigInt mul_("1");

                for (; lhs_ > rhs_;)
                {
                    rhs_ <<= 1;
                    mul_ <<= 1;
                }

                for (; !mul_.isZero();)
                {
                    if (lhs_ >= rhs_)
                    {
                        lhs_ -= rhs_;
                        ret_ += mul_;
                    }

                    rhs_ >>= 1;
                    mul_ >>= 1;
                }
            };

            const bool isLhsNegative = lhs.isNegative ^ rhs.isNegative;

            auto posLhs = lhs.isNegative ? -lhs : lhs;

            auto posRhs = rhs.isNegative ? -rhs : rhs;

            BigInt("0").swap(lhs);

            if (posLhs.binNumber.size() >= posRhs.binNumber.size())
            {
                div(posLhs, posRhs, lhs);

                if (isLhsNegative)
                {
                    lhs.unaryMinus();
                }
            }

            return lhs;
        }
        auto operator%= (const BigInt& rhs) -> BigInt&
        {
            if (rhs.isZero())
            {
                throw std::invalid_argument("right operand is zero.");
            }

            auto&& lhs = *this;

            if (lhs.isZero())
            {
                return lhs;
            }

            if (lhs == rhs)
            {
                BigInt("0").swap(lhs);

                return lhs;
            }

            static const auto mod = [](auto&& lhs_, auto&& rhs_)
            {
                BigInt mul_("1");

                for (; lhs_ > rhs_;)
                {
                    rhs_ <<= 1;
                    mul_ <<= 1;
                }

                for (; !mul_.isZero();)
                {
                    if (lhs_ >= rhs_)
                    {
                        lhs_ -= rhs_;
                    }

                    rhs_ >>= 1;
                    mul_ >>= 1;
                }
            };

            const bool isLhsNegative = lhs.isNegative;

            if (isLhsNegative)
            {
                lhs.unaryMinus();
            }

            auto posRhs = rhs.isNegative ? -rhs : rhs;

            if (lhs.binNumber.size() >= posRhs.binNumber.size())
            {
                mod(lhs, posRhs);
            }

            if (isLhsNegative)
            {
                lhs.unaryMinus();
            }

            return lhs;
        }

        auto pow_assign (const BigInt& rhs) -> BigInt
        {
            if (rhs.isNegative)
            {
                throw std::invalid_argument("right operand is negative.");
            }

            auto&& lhs = *this;

            if (lhs.isZero())
            {
                return lhs;
            }

            if (rhs.isZero())
            {
                BigInt("1").swap(lhs);

                return lhs;
            }

            BigInt tmp;

            tmp.swap(lhs);

            BigInt("1").swap(lhs);

            const auto rhsLength = rhs.binNumber.size();

            for (auto i = 0zu; i < rhsLength; i++)
            {
                if (rhs.binNumber[i])
                {
                    lhs *= tmp;
                }

                tmp *= tmp;
            }

            return lhs;
        }

        auto operator&= (const BigInt& rhs) -> BigInt&
        {
            auto&& lhs = *this;

            if (lhs.isZero() || rhs.isNOne())
            {
                return lhs;
            }

            if (rhs.isZero() || lhs.isNOne())
            {
                return lhs = rhs;
            }

            if (lhs == rhs)
            {
                return lhs;
            }

            return lhs.bitwiseOpr(rhs, [](auto&& lhs_, auto&& rhs_) noexcept
                                  {
                                      return lhs_ & rhs_;
                                  });
        }
        auto operator|= (const BigInt& rhs) -> BigInt&
        {
            auto&& lhs = *this;

            if (rhs.isZero() || lhs.isNOne())
            {
                return lhs;
            }

            if (lhs.isZero() || rhs.isNOne())
            {
                return lhs = rhs;
            }

            if (lhs == rhs)
            {
                return lhs;
            }

            return lhs.bitwiseOpr(rhs, [](auto&& lhs_, auto&& rhs_) noexcept
                                  {
                                      return lhs_ | rhs_;
                                  });
        }
        auto operator^= (const BigInt& rhs) -> BigInt&
        {
            auto&& lhs = *this;

            if (rhs.isZero())
            {
                return lhs;
            }

            if (rhs.isNOne())
            {
                return lhs.bitwiseNot();
            }

            if (lhs.isZero())
            {
                lhs = rhs;

                return lhs;
            }

            if (lhs.isNOne())
            {
                lhs = rhs;

                return lhs.bitwiseNot();
            }

            if (lhs == rhs)
            {
                BigInt("0").swap(lhs);

                return lhs;
            }

            return lhs.bitwiseOpr(rhs, [](auto&& lhs_, auto&& rhs_) noexcept
                                  {
                                      return lhs_ ^ rhs_;
                                  });
        }

        auto operator<<=(std::size_t shift) -> BigInt&
        {
            auto&& lhs = *this;

            if (shift > 0)
            {
                lhs.binNumber.resize(lhs.binNumber.size() + shift);

                lhs.binNumber <<= shift;
            }

            return lhs;
        }
        auto operator<<=(const BigInt& rhs) -> BigInt&
        {
            if (rhs.isNegative)
            {
                throw std::invalid_argument("right operand is negative.");
            }

            auto&& lhs = *this;

            lhs <<= rhs.ToInteger<std::size_t>();

            return lhs;
        }

        auto operator>>=(std::size_t shift) -> BigInt&
        {
            auto&& lhs = *this;

            if (shift > 0)
            {
                if (lhs.binNumber.size() > shift)
                {
                    lhs.binNumber >>= shift;

                    lhs.binNumber.resize(lhs.binNumber.size() - shift);
                }
                else
                {
                    binNumber.clear();
                }
            }

            return lhs;
        }
        auto operator>>=(const BigInt& rhs) -> BigInt&
        {
            if (rhs.isNegative)
            {
                throw std::invalid_argument("right operand is negative.");
            }

            auto&& lhs = *this;

            lhs >>= rhs.ToInteger<std::size_t>();

            return lhs;
        }

        auto operator++ (   ) -> BigInt&
        {
            auto&& rhs = *this;

            return rhs += 1;
        }
        auto operator++ (int) -> BigInt
        {
            auto&& lhs = *this;

            BigInt tmp(lhs);

            ++lhs;

            return tmp;
        }

        auto operator-- (   ) -> BigInt&
        {
            auto&& rhs = *this;

            return rhs -= 1;
        }
        auto operator-- (int) -> BigInt
        {
            auto&& lhs = *this;

            BigInt tmp(lhs);

            --lhs;

            return tmp;
        }

        public:
        friend auto operator+ (const BigInt& rhs) noexcept -> const BigInt&
        {
            return rhs;
        }
        friend auto operator- (const BigInt& rhs) -> BigInt
        {
            return BigInt(rhs).unaryMinus();
        }
        friend auto operator~ (const BigInt& rhs) -> BigInt
        {
            return BigInt(rhs).bitwiseNot();
        }

        friend auto operator+ (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return bigOprSmall(lhs, rhs, [](auto&& lhs_, auto&& rhs_)
                               {
                                   return lhs_ += rhs_;
                               });
        }
        friend auto operator- (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            if (rhs.isZero())
            {
                return  lhs;
            }

            if (lhs.isZero())
            {
                return -rhs;
            }

            if (lhs == rhs)
            {
                return BigInt("0");
            }

            return lhs + -rhs;
        }
        friend auto operator* (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return bigOprSmall(lhs, rhs, [](auto&& lhs_, auto&& rhs_)
                               {
                                   return lhs_ *= rhs_;
                               });
        }
        friend auto operator/ (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return BigInt(lhs) /= rhs;
        }
        friend auto operator% (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return BigInt(lhs) %= rhs;
        }

        friend auto operator& (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return bigOprSmall(lhs, rhs, [](auto&& lhs_, auto&& rhs_)
                               {
                                   return lhs_ &= rhs_;
                               });
        }
        friend auto operator| (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return bigOprSmall(lhs, rhs, [](auto&& lhs_, auto&& rhs_)
                               {
                                   return lhs_ |= rhs_;
                               });
        }
        friend auto operator^ (const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return bigOprSmall(lhs, rhs, [](auto&& lhs_, auto&& rhs_)
                               {
                                   return lhs_ ^= rhs_;
                               });
        }

        friend auto operator<<(const BigInt& lhs, std::size_t shift) -> BigInt
        {
            return BigInt(lhs) <<=shift;
        }
        friend auto operator>>(const BigInt& lhs, std::size_t shift) -> BigInt
        {
            return BigInt(lhs) >>=shift;
        }

        friend auto operator<<(const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return BigInt(lhs) <<=rhs;
        }
        friend auto operator>>(const BigInt& lhs, const BigInt& rhs) -> BigInt
        {
            return BigInt(lhs) >>=rhs;
        }

        friend auto operator==(const BigInt& lhs, const BigInt& rhs) -> bool
        {
            if (lhs.isNegative != rhs.isNegative)
            {
                return false;
            }

            const auto lhsLength = lhs.binNumber.size();

            const auto rhsLength = rhs.binNumber.size();

            if (lhsLength != rhsLength)
            {
                return false;
            }
            else
            {
                return lhs.binNumber == rhs.binNumber;
            }
        }
        friend auto operator!=(const BigInt& lhs, const BigInt& rhs) -> bool
        {
            return !(lhs == rhs);
        }

        friend auto operator< (const BigInt& lhs, const BigInt& rhs) -> bool
        {
            if (lhs.isNegative != rhs.isNegative)
            {
                return lhs.isNegative;
            }

            const auto lhsLength = lhs.binNumber.size();

            const auto rhsLength = rhs.binNumber.size();

            if (lhsLength != rhsLength)
            {
                return lhs.isNegative != (lhsLength < rhsLength);
            }
            else
            {
                return lhs.binNumber <  rhs.binNumber;
            }
        }
        friend auto operator>=(const BigInt& lhs, const BigInt& rhs) -> bool
        {
            return !(lhs <  rhs);
        }
        friend auto operator> (const BigInt& lhs, const BigInt& rhs) -> bool
        {
            return   rhs <  lhs;
        }
        friend auto operator<=(const BigInt& lhs, const BigInt& rhs) -> bool
        {
            return !(rhs <  lhs);
        }

        friend auto operator>>(std::istream& lhs,       BigInt& rhs) -> std::istream&
        {
            auto stdString = std::string();

            lhs >> stdString;

            BigInt(stdString).swap(rhs);

            return lhs;
        }

        friend auto operator<<(std::ostream& lhs, const BigInt& rhs) -> std::ostream&
        {
            return lhs << rhs.ToStdString();
        }

        public:
        void swap(BigInt& rhs) noexcept
        {
            auto&& lhs = *this;

            auto lhsPair = std::tie(lhs.binNumber, lhs.isNegative);

            auto rhsPair = std::tie(rhs.binNumber, rhs.isNegative);

            lhsPair.swap(rhsPair);
        }

        public:
        template<std::integral T>
        auto ToInteger  () const -> T
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                if (isNegative)
                {
                    throw std::invalid_argument("unsigned integer is negative.");
                }
            }

            auto decString = ToStdString();

            auto decNumber = T {};

            const auto result = std::from_chars(decString.c_str(), decString.c_str() + decString.size(), decNumber);

            if (result.ec == std::errc::result_out_of_range)
            {
                throw std::out_of_range("integer out of range.");
            }

            return decNumber;
        }

        public:
        auto ToBinString() const -> std::string
        {
            auto binString = std::string(binNumber.size(), '0');

            boost::to_string(binNumber, binString);

            return binString;
        }

        auto ToStdString(int base = 10) const -> std::string
        {
            static const auto toBinString = [](auto&& bigInt_)
            {
                auto binString_ = std::string(bigInt_.binNumber.size(), '0');

                if (!bigInt_.isNegative)
                {
                    boost::to_string(bigInt_.binNumber, binString_);
                }
                else
                {
                    const auto size_ = bigInt_.binNumber.size();

                    for (auto i_ = 0zu; i_ < size_; i_++)
                    {
                        binString_[i_] = bigInt_.binNumber[i_] ? '0' : '1';
                    }

                    for (auto&& c_ : binString_)
                    {
                        c_ = (c_ == '0' ? '1' : '0');

                        if (c_ == '1')
                        {
                            break;
                        }
                    }

                    if (binString_.empty() || binString_.ends_with('0'))
                    {
                        binString_.push_back('1');
                    }

                    std::ranges::reverse(binString_);
                }

                return binString_;
            };

            if (!(2 <= base && base <= 36))
            {
                throw std::invalid_argument("base is invalid.");
            }

            auto binString = toBinString(*this);

            auto stdString = std::string();

            auto tmpString = std::string();

            const auto zeroString = std::string(binString.size(), '0');

            for (;;)
            {
                auto digit = 0;

                for (auto&& c : binString)
                {
                    digit = digit * 2 + (c - '0');

                    if (digit >= base)
                    {
                        tmpString.push_back('1');

                        digit -= base;
                    }
                    else
                    {
                        tmpString.push_back('0');
                    }
                }

                stdString.push_back(digitToChar(digit));

                tmpString.swap(binString);

                if (binString == zeroString)
                {
                    break;
                }

                tmpString.clear();
            }

            if (isNegative)
            {
                stdString.push_back('-');
            }

            std::ranges::reverse(stdString);

            return stdString;
        }

        private:
        auto bitwiseNot () -> BigInt&
        {
            auto&& rhs = *this;

            rhs.isNegative = !rhs.isNegative;

            rhs.binNumber.flip();

            return rhs;
        }

        template<class F>
        auto bitwiseOpr (const BigInt& rhs, const F& func) -> BigInt&
        {
            auto&& lhs = *this;

            Trim trim;

            forEachBit(rhs, [&func, &trim](bool lhs_, bool rhs_) noexcept
                       {
                           const auto newBit_ = func(lhs_, rhs_);

                           trim.Count(newBit_);

                           return newBit_;
                       });

            lhs.isNegative = func(lhs.isNegative, rhs.isNegative);

            if (lhs.isNegative)
            {
                lhs.binNumber.push_back(lhs.isNegative);

                trim.Count(lhs.isNegative);
            }

            trim(lhs);

            return lhs;
        }

        template<class F>
        void forEachBit (const BigInt& rhs, const F& func)
        {
            auto&& lhs = *this;

            const auto lhsLength = lhs.binNumber.size();

            const auto rhsLength = rhs.binNumber.size();

            const auto minLength = std::min(lhsLength, rhsLength);

            auto i = 0zu;

            for (; i < minLength; i++)
            {
                lhs.binNumber[i] = func(lhs.binNumber[i], rhs.binNumber[i]);
            }

            if (lhsLength >= rhsLength)
            {
                for (; i < lhsLength; i++)
                {
                    lhs.binNumber[i] = func(lhs.binNumber[i], rhs.isNegative);
                }
            }
            else
            {
                lhs.binNumber.resize(rhsLength);

                for (; i < rhsLength; i++)
                {
                    lhs.binNumber[i] = func(rhs.binNumber[i], lhs.isNegative);
                }
            }
        }

        auto unaryMinus () -> BigInt&
        {
            static const auto twosComplement = [](auto&& bigInt_)
            {
                bigInt_.binNumber.flip();

                const auto size_ = bigInt_.binNumber.size();

                for (auto i_ = 0zu; i_ < size_; i_++)
                {
                    bigInt_.binNumber[i_].flip();

                    if (bigInt_.binNumber[i_] == 1)
                    {
                        break;
                    }
                }

                if (!bigInt_.isNegative)
                {
                    if (bigInt_.binNumber[size_ - 1] == 0)
                    {
                        bigInt_.binNumber.resize(size_ + 1, 1);
                    }
                }
                else
                {
                    if (bigInt_.binNumber[size_ - 1] == 1)
                    {
                        bigInt_.binNumber.resize(size_ - 1, 0);
                    }
                }
            };

            auto&& rhs = *this;

            if (rhs.isZero())
            {
                return rhs;
            }

            if (rhs.isNOne())
            {
                BigInt("1").swap(rhs);

                return rhs;
            }

            rhs.isNegative = !rhs.isNegative;

            twosComplement(rhs);

            return rhs;
        }

        private:
        auto isNOne     () const noexcept -> bool
        {
            return binNumber.empty() &&  isNegative;
        }

        auto isZero     () const noexcept -> bool
        {
            return binNumber.empty() && !isNegative;
        }

        private:
        template<class F>
        static auto bigOprSmall(const BigInt& lhs, const BigInt& rhs, const F& func) -> BigInt
        {
            if (lhs.binNumber.size() >= rhs.binNumber.size())
            {
                return func(BigInt(lhs), rhs);
            }
            else
            {
                return func(BigInt(rhs), lhs);
            }
        }

        private:
        static auto digitToChar(int d) -> char
        {
            if (0 <= d && d <= 9)
            {
                return static_cast<char>(d + '0');
            }
            else if ('A' - 'A' + 10 <= d && d <= 'Z' - 'A' + 10)
            {
                return static_cast<char>(d + 'A' - 10);
            }
            else if ('a' - 'a' + 10 <= d && d <= 'z' - 'a' + 10)
            {
                return static_cast<char>(d + 'a' - 10);
            }
            else
            {
                throw std::invalid_argument("character is invalid.");
            }
        }
    };

    inline static auto pow      (const akr::BigInt& lhs, const akr::BigInt& rhs) -> akr::BigInt
    {
        return akr::BigInt(lhs).pow_assign(rhs);
    }

    inline static auto to_string(const akr::BigInt& value) -> std::string
    {
        return value.ToStdString();
    }
}

namespace std
{
    inline static auto pow      (const akr::BigInt& lhs, const akr::BigInt& rhs) -> akr::BigInt
    {
        return akr::BigInt(lhs).pow_assign(rhs);
    }

    inline static auto to_string(const akr::BigInt& value) -> std::string
    {
        return value.ToStdString();
    }
}

namespace akr::literals
{
    template<char... C>
    inline auto operator""_akr_bigint() -> BigInt
    {
        return std::initializer_list<char> { C... };
    }
}

#ifdef  D_AKR_TEST
#include <random>

namespace akr::test
{
    AKR_TEST(BigInt,
    {
        using namespace literals;

        assert(0_akr_bigint == 0);
        assert(BigInt(    ) == 0);
        assert(BigInt(  0 ) == 0);
        assert(BigInt( "0") == 0);
        assert(BigInt("+0") == 0);
        assert(BigInt("-0") == 0);

        assert(BigInt() + 0 == 0);
        assert(BigInt() - 0 == 0);
        assert(BigInt() * 0 == 0);

        using T = std::int32_t;

        auto rd = std::random_device();
        auto re = std::mt19937_64(rd());

        const auto sm = static_cast<T>(std::sqrt(std::numeric_limits<T>::max()));

        auto id = std::uniform_int_distribution<T>(-sm, sm);

        for (auto i = 0; i < 100; i++)
        {
            auto intNum = id(re);

            for (; intNum == 0;)
            {
                intNum = id(re);
            }

            // std::printf("[%07d] num: %6d, ", i, intNum);

            auto bigNum = BigInt(intNum);

            assert(bigNum == intNum && bigNum == std::to_string(intNum));

            for (int j = 2; j <= 36; j++)
            {
                auto intStr = std::string(sizeof(intNum) * 8, 0);

                std::to_chars(intStr.data(), intStr.data() + intStr.size(), intNum, j);

                intStr.resize(bigNum.ToStdString(j).size());

                std::ranges::for_each(intStr, [](auto&& e_)
                                    {
                                        e_ = static_cast<char>(std::toupper(e_));
                                    });

                assert(bigNum == BigInt(intStr, j));

                assert(intNum == bigNum.ToInteger<decltype(intNum)>());

                assert(intStr == bigNum.ToStdString(j));

                switch (j)
                {
                    case 0B10:
                        assert(bigNum == std::string(intStr).insert(intNum < 0, "0B"));
                        assert(bigNum == std::string(intStr).insert(intNum < 0, "0b"));
                        break;
                    case 010:
                        assert(bigNum == std::string(intStr).insert(intNum < 0, "0"));
                        break;
                    case 0X10:
                        assert(bigNum == std::string(intStr).insert(intNum < 0, "0X"));
                        assert(bigNum == std::string(intStr).insert(intNum < 0, "0x"));
                        break;
                    default:
                        break;
                }
            }

            assert((+bigNum) == (+intNum));
            assert((-bigNum) == (-intNum));
            assert((~bigNum) == (~intNum));

            assert((bigNum - intNum) == 0);
            assert((bigNum / intNum) == 1);
            assert((bigNum % intNum) == 0);
            assert((bigNum ^ intNum) == 0);

            assert(++bigNum == ++intNum);
            assert(bigNum++ == intNum++);

            assert(--bigNum == --intNum);
            assert(bigNum-- == intNum--);

            for (int j = -1; j <= 1; j++)
            {
                assert((j + bigNum) == (j + intNum));
                assert((j - bigNum) == (j - intNum));
                assert((j * bigNum) == (j * intNum));
                assert((j / bigNum) == (j / intNum));
                assert((j % bigNum) == (j % intNum));
                assert((j & bigNum) == (j & intNum));
                assert((j | bigNum) == (j | intNum));
                assert((j ^ bigNum) == (j ^ intNum));

                assert((bigNum + j) == (intNum + j));
                assert((bigNum - j) == (intNum - j));
                assert((bigNum * j) == (intNum * j));
                assert((bigNum & j) == (intNum & j));
                assert((bigNum | j) == (intNum | j));
                assert((bigNum ^ j) == (intNum ^ j));
            }

            assert((bigNum + 2022 - intNum - 1997) == (intNum + 2022 - intNum - 1997));
            assert((bigNum - 2022 + intNum + 1997) == (intNum - 2022 + intNum + 1997));
            assert((bigNum - 1997 - intNum - 2022) == (intNum - 1997 - intNum - 2022));
            assert((bigNum - 1997 + intNum + 2022) == (intNum - 1997 + intNum + 2022));

            auto num1 = id(re);
            auto num2 = id(re);

            for (; num1 == 0;)
            {
                num1 = id(re);
            }

            for (; num2 == 0;)
            {
                num2 = id(re);
            }

            // std::printf("num1: %6d, num2: %6d\n", num1, num2);

            assert((BigInt(num1) +  num1) == (num1 +  num1));
            assert((BigInt(num1) -  num1) == (num1 -  num1));
            assert((BigInt(num1) *  num1) == (num1 *  num1));
            assert((BigInt(num1) /  num1) == (num1 /  num1));
            assert((BigInt(num1) %  num1) == (num1 %  num1));
            assert((BigInt(num1) &  num1) == (num1 &  num1));
            assert((BigInt(num1) |  num1) == (num1 |  num1));
            assert((BigInt(num1) ^  num1) == (num1 ^  num1));
            assert((BigInt(num1) == num1) == (num1 == num1));
            assert((BigInt(num1) != num1) == (num1 != num1));
            assert((BigInt(num1) <  num1) == (num1 <  num1));
            assert((BigInt(num1) >= num1) == (num1 >= num1));
            assert((BigInt(num1) >  num1) == (num1 >  num1));
            assert((BigInt(num1) <= num1) == (num1 <= num1));

            assert((BigInt(num2) +  num2) == (num2 +  num2));
            assert((BigInt(num2) -  num2) == (num2 -  num2));
            assert((BigInt(num2) *  num2) == (num2 *  num2));
            assert((BigInt(num2) /  num2) == (num2 /  num2));
            assert((BigInt(num2) %  num2) == (num2 %  num2));
            assert((BigInt(num2) &  num2) == (num2 &  num2));
            assert((BigInt(num2) |  num2) == (num2 |  num2));
            assert((BigInt(num2) ^  num2) == (num2 ^  num2));
            assert((BigInt(num2) == num2) == (num2 == num2));
            assert((BigInt(num2) != num2) == (num2 != num2));
            assert((BigInt(num2) <  num2) == (num2 <  num2));
            assert((BigInt(num2) >= num2) == (num2 >= num2));
            assert((BigInt(num2) >  num2) == (num2 >  num2));
            assert((BigInt(num2) <= num2) == (num2 <= num2));

            assert((BigInt(num1) +  num2) == (num1 +  num2));
            assert((BigInt(num1) -  num2) == (num1 -  num2));
            assert((BigInt(num1) *  num2) == (num1 *  num2));
            assert((BigInt(num1) /  num2) == (num1 /  num2));
            assert((BigInt(num1) %  num2) == (num1 %  num2));
            assert((BigInt(num1) &  num2) == (num1 &  num2));
            assert((BigInt(num1) |  num2) == (num1 |  num2));
            assert((BigInt(num1) ^  num2) == (num1 ^  num2));
            assert((BigInt(num1) == num2) == (num1 == num2));
            assert((BigInt(num1) != num2) == (num1 != num2));
            assert((BigInt(num1) <  num2) == (num1 <  num2));
            assert((BigInt(num1) >= num2) == (num1 >= num2));
            assert((BigInt(num1) >  num2) == (num1 >  num2));
            assert((BigInt(num1) <= num2) == (num1 <= num2));

            if (i <= std::numeric_limits<decltype(num1)>::digits
                - (std::numeric_limits<std::make_unsigned_t<decltype(num1)>>::digits
                - (num1 > 0 ? std::countl_zero(std::bit_cast<std::make_unsigned_t<decltype(num1)>>(num1))
                    : std::countl_one(std::bit_cast<std::make_unsigned_t<decltype(num1)>>(num1)))))
            {
                assert((BigInt(num1) >> i) == (num1 >> i));
                assert((BigInt(num1) << i) == (num1 << i));

                assert((BigInt(num1) << i) == (BigInt(num1) * pow(2, i)));

                auto ret1 = (BigInt(num1) >> i) - (BigInt(num1) / akr::pow(2, i));
                assert(-1 <= ret1 && ret1 <= 1);
            }

            if (i <= std::numeric_limits<decltype(num2)>::digits
                - (std::numeric_limits<std::make_unsigned_t<decltype(num2)>>::digits
                - (num2 > 0 ? std::countl_zero(std::bit_cast<std::make_unsigned_t<decltype(num2)>>(num2))
                    : std::countl_one(std::bit_cast<std::make_unsigned_t<decltype(num2)>>(num2)))))
            {
                assert((BigInt(num2) >> i) == (num2 >> i));
                assert((BigInt(num2) << i) == (num2 << i));

                assert((BigInt(num2) << i) == (BigInt(num2) * pow(2, i)));

                auto ret2 = (BigInt(num2) >> i) - (BigInt(num2) / akr::pow(2, i));
                assert(-1 <= ret2 && ret2 <= 1);
            }
        }
    });
}
#endif//D_AKR_TEST

#endif//Z_AKR_BIGINT_HH

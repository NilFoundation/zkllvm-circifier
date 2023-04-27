#pragma once

namespace mask_int { 

  #ifdef MASK_INT_DEFAULT_PROXY_OPERATORS
  #error Overriding MASK_INT_DEFAULT_PROXY_OPERATORS macros
  #endif
  #define MASK_INT_DEFAULT_PROXY_OPERATORS(T, V)                                                            \
    constexpr bool operator<(T x) const { return val_ < x.val_; }                                  \
    constexpr bool operator<(V x) const { return val_ < x; }                                       \
    template<unsigned _len> constexpr bool operator<(T<_len> x) const { return val_ < x.val_; }    \
    constexpr bool operator<=(T x) const { return val_ <= x.val_; }                                \
    constexpr bool operator<=(V x) const { return val_ <= x; }                                     \
    template<unsigned _len> constexpr bool operator<=(T<_len> x) const { return val_ <= x.val_; }  \
    constexpr bool operator>(T x) const { return val_ > x.val_; }                                  \
    constexpr bool operator>(V x) const { return val_ > x; }                                       \
    template<unsigned _len> constexpr bool operator>(T<_len> x) const { return val_ > x.val_; }    \
    constexpr bool operator>=(T x) const { return val_ >= x.val_; }                                \
    constexpr bool operator>=(V x) const { return val_ >= x; }                                     \
    template<unsigned _len> constexpr bool operator>=(T<_len> x) const { return val_ >= x.val_; }  \
    constexpr bool operator==(T x) const { return val_ == x.val_; }                                \
    constexpr bool operator==(V x) const { return val_ == x; }                                     \
    template<unsigned _len> constexpr bool operator==(T<_len> x) const { return val_ == x.val_; }  \
    constexpr bool operator!=(T x) const { return val_ != x.val_; }                                \
    constexpr bool operator!=(V x) const { return val_ != x; }                                     \
    template<unsigned _len> constexpr bool operator!=(T<_len> x) const { return val_ != x.val_; }  \
    constexpr T& operator+=(T x) { _val_set(val_ + x.val_); return *this; }                                 \
    constexpr T& operator+=(V x) { _val_set(val_ + x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator+=(T<_len> x) { _val_set(val_ + x.val_); return *this; }   \
    constexpr T& operator-=(T x) { _val_set(val_ - x.val_); return *this; }                                 \
    constexpr T& operator-=(V x) { _val_set(val_ - x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator-=(T<_len> x) { _val_set(val_ - x.val_); return *this; }   \
    constexpr T& operator*=(T x) { _val_set(val_ * x.val_); return *this; }                                 \
    constexpr T& operator*=(V x) { _val_set(val_ * x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator*=(T<_len> x) { _val_set(val_ * x.val_); return *this; }   \
    constexpr T& operator/=(T x) { _val_set(val_ / x.val_); return *this; }                                 \
    constexpr T& operator/=(V x) { _val_set(val_ / x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator/=(T<_len> x) { _val_set(val_ / x.val_); return *this; }   \
    constexpr T& operator%=(T x) { _val_set(val_ % x.val_); return *this; }                                 \
    constexpr T& operator%=(V x) { _val_set(val_ % x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator%=(T<_len> x) { _val_set(val_ % x.val_); return *this; }   \
    constexpr T& operator|=(T x) { _val_set(val_ | x.val_); return *this; }                                 \
    constexpr T& operator|=(V x) { _val_set(val_ | x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator|=(T<_len> x) { _val_set(val_ | x.val_); return *this; }   \
    constexpr T& operator&=(T x) { _val_set(val_ & x.val_); return *this; }                                 \
    constexpr T& operator&=(V x) { _val_set(val_ & x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator&=(T<_len> x) { _val_set(val_ & x.val_); return *this; }   \
    constexpr T& operator^=(T x) { _val_set(val_ ^ x.val_); return *this; }                                 \
    constexpr T& operator^=(V x) { _val_set(val_ ^ x); return *this; }                                      \
    template<unsigned _len> constexpr T& operator^=(T<_len> x) { _val_set(val_ ^ x.val_); return *this; }   \
    constexpr T& operator++() { ++val_; return *this; }                                            \
    constexpr T& operator--() { --val_; return *this; }                                            \
    constexpr T operator++(int) { T old(*this); ++val_; return old; }                              \
    constexpr T operator--(int) { T old(*this); --val_; return old; }                              \
    constexpr T operator~() const { return T(~val_); }

  template<unsigned _bitlen>
  struct mask_uint_t;

  template<unsigned _bitlen>
  struct mask_int_t {

    constexpr mask_int_t() : val_(0) {}
    explicit constexpr mask_int_t(int val) : val_(val) {}
    template<unsigned _len>
    explicit constexpr  mask_int_t(mask_int_t<_len> val) : val_(val.get()) {}
    template<unsigned _len>
    explicit constexpr  mask_int_t(mask_uint_t<_len> val) : val_(val.get()) {}
    constexpr  mask_int_t(mask_uint_t<_bitlen> val) : val_(val.get()) {}
    int operator()() const { return val_; }
    void operator()(int val) { val_ = val; }
    auto& operator=(int val) { val_ = val; return *this; }
    template<unsigned _len>
    auto& operator=(mask_int_t<_len> val) { val_ = val.get(); return *this; }
    MASK_INT_DEFAULT_PROXY_OPERATORS(mask_int_t, int)
    constexpr int get() const { return val_; }
    explicit operator bool() const { return val_ != 0; }
    explicit operator int() const { return val_; }
    
    __always_inline constexpr void _val_set(int val) {
      val_=val;
    }

    int val_;
  };

  template<unsigned _len>
  __always_inline constexpr auto operator-(mask_int_t<_len> l) {
    return mask_int_t<_len>(-l.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator+(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ + r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator+(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ + r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator+(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l + r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator-(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ - r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator-(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ - r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator-(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l - r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator*(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ * r.val_);
  }
  __always_inline constexpr auto operator*(mask_int_t<8> l, mask_int_t<8> r) { return mask_int_t<8>{l.val_ * r.val_}; }
  __always_inline constexpr auto operator*(mask_int_t<16> l, mask_int_t<16> r) { return mask_int_t<16>{l.val_ * r.val_}; }
  __always_inline constexpr auto operator*(mask_int_t<32> l, mask_int_t<32> r) { return mask_int_t<32>{l.val_ * r.val_}; }
  __always_inline constexpr auto operator*(mask_int_t<64> l, mask_int_t<64> r) { return mask_int_t<64>{l.val_ * r.val_}; }
  __always_inline constexpr auto operator*(mask_int_t<128> l, mask_int_t<128> r) { return mask_int_t<128>{l.val_ * r.val_}; }
  template<unsigned _len>
  __always_inline constexpr auto operator*(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ * r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator*(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l * r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator/(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ / r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator/(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ / r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator/(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l / r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator%(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ % r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator%(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ % r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator%(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l % r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator|(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ | r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator|(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ | r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator|(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l | r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator&(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ & r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator&(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ & r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator&(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l & r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator^(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ ^ r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator^(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ ^ r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator^(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l ^ r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator<<(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ << r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator<<(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ << r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator<<(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l << r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr auto operator>>(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return mask_int_t<std::max(_left_len, _right_len)>(l.val_ >> r.val_);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator>>(mask_int_t<_len> l, unsigned r) {
    return mask_int_t<_len>(l.val_ >> r);
  }
  template<unsigned _len>
  __always_inline constexpr auto operator>>(unsigned l, mask_int_t<_len> r) {
    return mask_int_t<_len>(l >> r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr bool operator==(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return l.val_ == r.val_;
  }
  template<unsigned _len>
  __always_inline constexpr bool operator==(unsigned l, mask_int_t<_len> r) {
    return l == r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr bool operator!=(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return l.val_ != r.val_;
  }
  template<unsigned _len>
  __always_inline constexpr bool operator!=(unsigned l, mask_int_t<_len> r) {
    return l != r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr bool operator<(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return l.val_ < r.val_;
  }
  template<unsigned _len>
  __always_inline constexpr bool operator<(unsigned l, mask_int_t<_len> r) {
    return l < r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr bool operator<=(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return l.val_ <= r.val_;
  }
  template<unsigned _len>
  __always_inline constexpr bool operator<=(unsigned l, mask_int_t<_len> r) {
    return l <= r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr bool operator>(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return l.val_ > r.val_;
  }
  template<unsigned _len>
  __always_inline constexpr bool operator>(unsigned l, mask_int_t<_len> r) {
    return l > r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  __always_inline constexpr bool operator>=(mask_int_t<_left_len> l, mask_int_t<_right_len> r) {
    return l.val_ >= r.val_;
  }
  template<unsigned _len>
  __always_inline constexpr bool operator>=(unsigned l, mask_int_t<_len> r) {
    return l >= r.val_;
  }

  template<unsigned _bitlen>
  struct mask_uint_t {
    constexpr static const unsigned mask = (1u << _bitlen) -1;

    constexpr mask_uint_t() : val_(0) {}
    constexpr mask_uint_t(unsigned val) : val_(val&mask) {}
    template<unsigned _len>
    explicit constexpr  mask_uint_t(mask_uint_t<_len> val) : mask_uint_t(val.get()) {}
    template<unsigned _len>
    explicit constexpr  mask_uint_t(mask_int_t<_len> val) : mask_uint_t(val.get()) {}
    unsigned operator()() const { return val_; }
    void operator()(unsigned val) { val_ = val; }
    constexpr auto& operator=(unsigned val) { val_ = val; return *this; }
    template<unsigned _len>
    auto& operator=(mask_uint_t<_len> val) { val_ = val.get(); return *this; }
    MASK_INT_DEFAULT_PROXY_OPERATORS(mask_uint_t, unsigned)
    template<unsigned _len>
    constexpr auto& operator<<=(mask_uint_t<_len> x) { _val_set(val_ << x.val_); return *this; }
    constexpr auto& operator<<=(unsigned x) { _val_set(val_ << x); return *this; }
    template<unsigned _len>
    constexpr auto& operator>>=(mask_uint_t<_len> x) { _val_set(val_ >> x.val_); return *this; }
    constexpr auto& operator>>=(unsigned x) { _val_set(val_ >> x); return *this; }                                 \
    constexpr unsigned get() const { return val_; }
    explicit constexpr operator bool() const { return val_ != 0; }
    explicit constexpr operator unsigned() const { return val_; }
    explicit constexpr operator int() const { return val_; }
    explicit constexpr operator long() const { return val_; }
    explicit constexpr operator tvm::schema::uint_t<_bitlen>() const { return tvm::schema::uint_t<_bitlen>(val_); }

    __always_inline constexpr void _val_set(unsigned val) {
      val_=val&mask;
    }

    unsigned val_;
  };

  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator+(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ + r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator+(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ + r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator+(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l + r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator-(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ - r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator-(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ - r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator-(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l - r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator*(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ * r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator*(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ * r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator*(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l * r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator/(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ / r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator/(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ / r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator/(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l / r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator%(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ % r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator%(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ % r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator%(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l % r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator|(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ | r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator|(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ | r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator|(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l | r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator&(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ & r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator&(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ & r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator&(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l & r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator^(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ ^ r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator^(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ ^ r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator^(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l ^ r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator<<(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ << r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator<<(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ << r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator<<(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l << r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline auto operator>>(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return mask_uint_t<std::max(_left_len, _right_len)>(l.val_ >> r.val_);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator>>(mask_uint_t<_len> l, unsigned r) {
    return mask_uint_t<_len>(l.val_ >> r);
  }
  template<unsigned _len>
  constexpr __always_inline auto operator>>(unsigned l, mask_uint_t<_len> r) {
    return mask_uint_t<_len>(l >> r.val_);
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline bool operator==(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return l.val_ == r.val_;
  }
  template<unsigned _len>
  constexpr __always_inline bool operator==(unsigned l, mask_uint_t<_len> r) {
    return l == r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline bool operator!=(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return l.val_ != r.val_;
  }
  template<unsigned _len>
  constexpr __always_inline bool operator!=(unsigned l, mask_uint_t<_len> r) {
    return l != r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline bool operator<(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return l.val_ < r.val_;
  }
  template<unsigned _len>
  constexpr __always_inline bool operator<(unsigned l, mask_uint_t<_len> r) {
    return l < r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline bool operator<=(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return l.val_ <= r.val_;
  }
  template<unsigned _len>
  constexpr __always_inline bool operator<=(unsigned l, mask_uint_t<_len> r) {
    return l <= r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline bool operator>(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return l.val_ > r.val_;
  }
  template<unsigned _len>
  constexpr __always_inline bool operator>(unsigned l, mask_uint_t<_len> r) {
    return l > r.val_;
  }
  template<unsigned _left_len, unsigned _right_len>
  constexpr __always_inline bool operator>=(mask_uint_t<_left_len> l, mask_uint_t<_right_len> r) {
    return l.val_ >= r.val_;
  }
  template<unsigned _len>
  constexpr __always_inline bool operator>=(unsigned l, mask_uint_t<_len> r) {
    return l >= r.val_;
  }

  using mask_int128 = mask_int_t<128>;
  using mask_int256 = mask_int_t<256>;

  using mask_uint128 = mask_uint_t<128>;
  using mask_uint256 = mask_uint_t<256>;

} // mask_int

namespace std {
  using namespace mask_int;
    template<unsigned _bitlen>
    struct is_signed<mask_int::mask_int_t<_bitlen>> : public std::integral_constant<bool, true> { };
    template<unsigned _bitlen>
    struct is_signed<mask_int::mask_uint_t<_bitlen>> : public std::integral_constant<bool, false> { };
    template<unsigned _bitlen>
    struct is_unsigned<mask_int::mask_int_t<_bitlen>> : public std::integral_constant<bool, false> { };
    template<unsigned _bitlen>
    struct is_unsigned<mask_int::mask_uint_t<_bitlen>> : public std::integral_constant<bool, true> { };
    template<unsigned _bitlen>
    struct is_integral<mask_int::mask_int_t<_bitlen>> : public std::integral_constant<bool, true> { };
    template<unsigned _bitlen>
    struct is_integral<mask_int::mask_uint_t<_bitlen>> : public std::integral_constant<bool, true> { };
    template<unsigned _bitlen>
    struct is_arithmetic<mask_int::mask_int_t<_bitlen>> : public std::integral_constant<bool, true> { };
    template<unsigned _bitlen>
    struct is_arithmetic<mask_int::mask_uint_t<_bitlen>> : public std::integral_constant<bool, true> { };
    template<unsigned _bitlen>
    struct make_unsigned<mask_int::mask_int_t<_bitlen>> {
        using type = mask_int::mask_uint_t<_bitlen>;
    };
    template<unsigned _bitlen>
    struct make_unsigned<mask_int::mask_uint_t<_bitlen>> {
        using type = mask_int::mask_uint_t<_bitlen>;
    };
    template<unsigned _bitlen>
    struct make_signed<mask_int::mask_int_t<_bitlen>> {
        using type = mask_int::mask_int_t<_bitlen>;
    };
    template<unsigned _bitlen>
    struct make_signed<mask_int::mask_uint_t<_bitlen>> {
        using type = mask_int::mask_int_t<_bitlen>;
    };

    template<unsigned _bitlen>
    class numeric_limits<mask_int::mask_uint_t<_bitlen>> {
  public:
      static constexpr bool is_specialized = true;
      // static constexpr T min() noexcept;
      static constexpr mask_uint_t<_bitlen> max() noexcept { return ~mask_uint_t<_bitlen>(0); };
      // static constexpr T lowest() noexcept;

      static constexpr int  digits = _bitlen;
      // static constexpr int  digits10 = 0;
      // static constexpr int  max_digits10 = 0;
      // static constexpr bool is_signed = false;
      // static constexpr bool is_integer = false;
      // static constexpr bool is_exact = false;
      // static constexpr int  radix = 0;
      // static constexpr T epsilon() noexcept;
      // static constexpr T round_error() noexcept;

      // static constexpr int  min_exponent = 0;
      // static constexpr int  min_exponent10 = 0;
      // static constexpr int  max_exponent = 0;
      // static constexpr int  max_exponent10 = 0;

      // static constexpr bool has_infinity = false;
      // static constexpr bool has_quiet_NaN = false;
      // static constexpr bool has_signaling_NaN = false;
      // static constexpr float_denorm_style has_denorm = denorm_absent;
      // static constexpr bool has_denorm_loss = false;
      // static constexpr T infinity() noexcept;
      // static constexpr T quiet_NaN() noexcept;
      // static constexpr T signaling_NaN() noexcept;
      // static constexpr T denorm_min() noexcept;

      // static constexpr bool is_iec559 = false;
      // static constexpr bool is_bounded = false;
      // static constexpr bool is_modulo = false;

      // static constexpr bool traps = false;
      // static constexpr bool tinyness_before = false;
      // static constexpr float_round_style round_style = round_toward_zero;
  };
}
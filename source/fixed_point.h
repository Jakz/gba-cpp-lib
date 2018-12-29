#pragma once

#include <cstddef>
#include <type_traits>

template <typename T, typename C, size_t I, size_t F>
struct fixed_point
{
  static constexpr C unit = 1 << F;
  
  T value;
  
  fixed_point() { }
  //fixed_point(C value) : value(value*unit) { }
  fixed_point(C i, C f)  : value(i << F | f) { }
  
  void rawSet(T value) { this->value = value; }
  T raw() const { return value; }
  
  constexpr fixed_point(C v) : value(v*unit) { }
  constexpr fixed_point(float f) : value(static_cast<T>(f*unit))
  {
    static_assert(sizeof(T)*8 >= I+F, "Must be large enough");
    static_assert(sizeof(C) >= sizeof(T), "Must be large enough");
  }
  
  template <typename T2, typename C2, size_t I2, size_t F2, size_t U = F2, typename std::enable_if<(F2 > F), int>::type = 0>
  inline fixed_point(const fixed_point<T2,C2,I2,F2>& other) : value(other.value >> (F2 - F)) { }
  
  template <typename T2, typename C2, size_t I2, size_t F2, size_t U = F2, typename std::enable_if<(F2 < F), int>::type = 0>
  inline fixed_point(const fixed_point<T2,C2,I2,F2>& other) : value(other.value << (F - F2)) { }
  
  inline fixed_point(const fixed_point& other) : value(other.value) { }
  
  inline fixed_point& operator=(float v) { value = static_cast<T>(v*unit); return *this;}
  inline fixed_point& operator=(C v) { value = v*unit; return *this; }
  
  inline bool operator!() const { return !value; }
  inline fixed_point operator~() const { return fixed_point(~value); }
  inline fixed_point& operator++() { value += unit; return *this; }
  inline fixed_point& operator--() { value += unit; return *this; }
  
  inline bool operator<(const fixed_point& o) const { return value < o.value; }
  inline bool operator>(const fixed_point& o) const { return value > o.value; }
  inline bool operator==(const fixed_point& o) const { return value == o.value; }
  inline bool operator!=(const fixed_point& o) const { return value != o.value; }
  inline bool operator<=(const fixed_point& o) const { return value <= o.value; }
  inline bool operator>=(const fixed_point& o) const { return value >= o.value; }
  
  
  
  fixed_point operator+(const fixed_point& other) const
  {
    C tmp = value + other.value;
    return fixed_point(tmp);
  }
  
  fixed_point operator-(const fixed_point& other) const
  {
    C tmp = value - other.value;
    return fixed_point(tmp);
  }
  
  fixed_point operator*(const fixed_point& other) const
  {
    C tmp = value*other.value;
    fixed_point fp;
    fp.rawSet(tmp >> F);
    return fp;
  }
  
  fixed_point operator/(const fixed_point& other) const
  {
    C tmp = (value << F) / other.value;
    fixed_point fp;
    fp.rawSet(tmp);
    return fp;
  }
  
  fixed_point& operator +=(const fixed_point& other) { value += other.value; return *this; }
  fixed_point& operator -=(const fixed_point& other) { value -= other.value; return *this; }
  fixed_point& operator *=(const fixed_point& other)
  {
    C tmp = value*other.value;
    this->value = tmp >> F;
  }
  
  fixed_point operator-() const { fixed_point<T,C,I,F> f(*this); f.value = -f.value; return f; }
  
  explicit operator float() const { return static_cast<float>(value) / unit; }
  explicit operator C() const { return static_cast<C>(value >> F); }
  
  float toFloat() const { return this->operator float(); }
  C toInt() const { return this->operator int(); }
};

using fp = fixed_point<signed short, signed int, 8, 8>;
using fpp = fixed_point<signed short, signed int, 4, 12>;

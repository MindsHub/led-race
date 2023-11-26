#include "gravity.hpp"
#include "constants.hpp"

struct GravityRange {
  int fromInclusive;
  int toInclusive;
  byte amount;
};

constexpr GravityRange ranges[] = {
  {51, 64, 255},
  {90, 110, 255},
  {110, 120, 0},
  {158, 175, 0},
  {188, 209, 255},
  {240, 274, 255},
  {275, 295, 100},
  {299, 315, 255},
  {362, 370, 0},
  {382, 392, 0},
  {404, 413, 0},
  {430, 437, 0},
  {460, 498, 255},
  {555, 590, 0},
  {602, 621, 0},
  {625, 631, 220},
  {636, 650, 100},
  {653, 700, 255},
  {705, 761, 0},
  {768, 820, 255},
  {824, 890, 0},
};


constexpr size_t rangesCount = sizeof(ranges) / sizeof(GravityRange);

constexpr byte computeGravity(int pixelIndex, size_t rangeIndex) {
  return (rangeIndex >= rangesCount)
    ? 127
    : (pixelIndex >= ranges[rangeIndex].fromInclusive && pixelIndex <= ranges[rangeIndex].toInclusive)
      ? ranges[rangeIndex].amount
      : computeGravity(pixelIndex, rangeIndex + 1);
}


template<int N, int... Rest>
struct Array_impl {
  // doing three calculations at a time to reduce recursion depth and not hit the gcc limit of 900
  static constexpr auto& value = Array_impl<N - 3, computeGravity(N - 3, 0), computeGravity(N - 2, 0), computeGravity(N - 1, 0), Rest...>::value;
};

template<int... Rest>
struct Array_impl<0, Rest...> {
  static constexpr byte PROGMEM value[] = { Rest... };
};

template<int... Rest>
struct Array_impl<1, Rest...> {
  static constexpr byte PROGMEM value[] = { computeGravity(0, 0), Rest... };
};

template<int... Rest>
struct Array_impl<2, Rest...> {
  static constexpr byte PROGMEM value[] = { computeGravity(0, 0), computeGravity(1, 0), Rest... };
};

template<int... Rest>
constexpr byte Array_impl<0, Rest...>::value[];

template<int N>
struct Array {
  static_assert(N >= 0, "N must be at least 0");

  static constexpr auto& value = Array_impl<N>::value;

  Array() = delete;
  Array(const Array&) = delete;
  Array(Array&&) = delete;
};

/// 127 is the normal gravity. Values under this value means there is a hill,
/// while values above 127 means there is a downhill.
static constexpr auto& gravityMapImpl = Array<PIXEL_COUNT>::value;


byte gravityMap(int i) {
  return pgm_read_byte(gravityMapImpl + i);
}

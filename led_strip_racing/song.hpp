#pragma once

extern const int melodyCount;
extern const double melodyDuration;
int melody(int i);

constexpr double dividerToDuration(int divider) {
  return (divider > 0) ? (1.0 / divider) : (-1.5 / divider);
}

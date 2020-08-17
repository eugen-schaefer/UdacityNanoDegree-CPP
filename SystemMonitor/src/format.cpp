#include "format.h"

#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  std::string shours{};
  std::string sminutes{};
  std::string sseconds{};

  int huors{static_cast<int>(seconds / 3600)};
  int minutes{(static_cast<int>(seconds / 60)) % 60};
  int sec{(static_cast<int>(seconds)) % 60};

  if (huors < 10) {
    shours = "0" + std::to_string(huors);
  } else {
    shours = std::to_string(huors);
  }

  if (minutes < 10) {
    sminutes = "0" + std::to_string(minutes);
  } else {
    sminutes = std::to_string(minutes);
  }

  if (sec < 10) {
    sseconds = "0" + std::to_string(sec);
  } else {
    sseconds = std::to_string(sec);
  }

  return shours + ":" + sminutes + ":" + sseconds;
}

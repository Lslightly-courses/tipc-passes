#include <algorithm>
#include <cstdlib>
#include <set>
#include "Interval.h"

using namespace interval;

// Simple builder and accessor for pair representation
Interval interval::make(int l, int r) { return std::make_pair(l,r); }
int interval::lower(Interval i) { return i.first; }
int interval::upper(Interval i) { return i.second; }

// Pre-defined intervals 
Interval interval::full() { return make(minf,pinf); }
Interval interval::empty() { return make(pinf,minf); }
Interval interval::unit() { return make(0,1); }

/* Least Upper Bound
 *   Edge cases lead to extreme intervals or extreme bounds.
 *   General case takes the lowest of the lows and the highest of the highs
 *   as the bounds.
 */
Interval interval::lub(Interval l, Interval r) {
  Interval result;
  if (l == full()) {
    result = full(); 
  } else if (l == empty()) {
    result = r;
  } else if (lower(l) == minf && upper(r) == pinf) {
    result = full();
  } else if (lower(l) == minf) {
    result = make(minf, std::max(upper(l), upper(r)));
  } else if (upper(l) == pinf) {
    result = make(std::min(lower(l), lower(r)), pinf);
  } else {
    result = make(std::min(lower(l), lower(r)), std::max(upper(l), upper(r)));
  }
  return result;
}

/* Unary negation
 *  Numerous special cases where the extreme bounds are involved. 
 *  General case is to negate the bounds and use min/max to establish bounds.
 */
Interval interval::neg(Interval i) {
  Interval result;
  if (minf == lower(i) && pinf == upper(i)) {
    result = full();
  } else if (pinf == lower(i) && minf == upper(i)) {
    result = empty();
  } else if (minf == lower(i) && minf == upper(i)) {
    result = make(pinf, pinf);
  } else if (pinf == lower(i) && pinf == upper(i)) {
    result = make(minf, minf);
  } else if (pinf == upper(i)) {
    result = make(minf, -(lower(i)));
  } else if (minf == lower(i)) {
    result = make(-(upper(i)), pinf);
  } else {
    result = make(std::min(-(upper(i)),-(lower(i))), 
                  std::max(-(upper(i)),-(lower(i))));
  }
  return result;
}

/* Addition
 *  Edge cases for empty intervals and maximal bounds
 *  General case is to add the corresponding bounds.
 */
Interval interval::add(Interval l, Interval r) {
  int low, up;

  if (pinf == lower(l) || pinf == lower(r)) {
    low = pinf; // one of the arguments is empty
  } else if (minf == lower(l) || minf == lower(r)) {
    low = minf; 
  } else { 
    low = lower(l) + lower(r); 
  } 

  if (minf == upper(l) || minf == upper(r)) {
    up = minf; // one of the arguments is empty
  } else if (pinf == upper(l) || pinf == upper(r)) {
    up = pinf; 
  } else { 
    up = upper(l) + upper(r); 
  } 

  return make(low, up);
}

Interval interval::sub(Interval l, Interval r) {
  return interval::add(l, interval::neg(r));
}

template <typename T>
int multiply_with_overflow_check(T a, T b, std::set<T>& intSet) {
    static_assert(std::is_integral<T>::value, "Only integral types are supported");

    T result;
    // Check for overflow or underflow based on the signs of a and b
    if (a > 0 && b > 0) {
        if (a > std::numeric_limits<T>::max() / b) {
            intSet.insert(pinf);
            return 1; // Overflow
        }
    } else if (a < 0 && b < 0) {
        if (a < std::numeric_limits<T>::max() / b) {
            intSet.insert(pinf);
            return 1; // Overflow
        }
    } else if ((a > 0 && b < 0) || (a < 0 && b > 0)) {
        if (b < std::numeric_limits<T>::min() / a) {
            intSet.insert(minf);
            return -1; // Underflow
        }
    }

    // If no overflow or underflow, calculate the result
    result = a * b;
    intSet.insert(result);
    return 0; // No overflow or underflow
}

/* Multiplication
 */
Interval interval::mul(Interval l, Interval r) {
  if (l == empty() || r == empty() || l == full() || r == full())
    return full();
  std::set<int> s{};
  multiply_with_overflow_check(lower(l), lower(r), s);
  multiply_with_overflow_check(upper(l), lower(r), s);
  multiply_with_overflow_check(lower(l), upper(r), s);
  multiply_with_overflow_check(upper(l), upper(r), s);
  int low = *std::min_element(s.begin(), s.end());
  int high = *std::max_element(s.begin(), s.end());
  return make(low, high);
}

/* Division
 */
Interval interval::div(Interval l, Interval r) {
  if (l == empty() || r == empty() || l == full() || r == full())
    return full();
  if (lower(r) < 0 && upper(r) > 0) { // div -1/+1
    std::set<int> s{abs(lower(l)), abs(upper(l)), -abs(lower(l)), -abs(upper(l))};
    return make(*std::min_element(s.begin(), s.end()), *std::max_element(s.begin(), s.end()));
  }
  if (lower(r) == 0 && upper(r) == 0) { // div by zero
    return full();
  }
  if (lower(r) == 0) {
    r = make(1, upper(r));
  }
  if (upper(r) == 0) {
    r = make(lower(r), -1);
  }
  if (lower(r) >= 0) {
    bool llowpos = lower(l) > 0;
    bool lhighpos = upper(l) > 0;
    if (llowpos && lhighpos) {
      return make(lower(l) / upper(r), upper(l) / lower(r));
    }
    if (!llowpos && !lhighpos) {
      return make(lower(l) / lower(r), upper(l) / upper(r));
    }
    if (!llowpos && lhighpos) {
      return make(lower(l) / lower(r), upper(l) / lower(r));
    }
  }
  if (upper(r) <= 0) {
    bool llowpos = lower(l) > 0;
    bool lhighpos = upper(l) > 0;
    if (llowpos && lhighpos) {
      return make(upper(l) / upper(r), lower(l) / lower(r));
    }
    if (!llowpos && !lhighpos) {
      return make(upper(l) / lower(r), lower(l) / upper(r));
    }
    if (!llowpos && lhighpos) {
      return make(upper(l) / upper(r), lower(l) / upper(r));
    }
  }
  return interval::full();
}

/* Comparison Operators
 *   Trivial imprecise definitions
 */
Interval interval::lt(Interval l, Interval r) { return unit(); }
Interval interval::gt(Interval l, Interval r) { return unit(); }
Interval interval::eq(Interval l, Interval r) { return unit(); }
Interval interval::ne(Interval l, Interval r) { return unit(); }

std::string istr(int b) {
  std::string result = "";
  if (b == minf) {
    result = "-inf";
  } else if (b == pinf) {
    result = "+inf"; 
  } else {
    result = std::to_string(b);
  }
  return result;
}

std::string interval::str(Interval i) {
  std::string f = istr(lower(i));
  std::string s = istr(upper(i));
  return "[" + f + "," + s + "]";
}

// Deep equality for intervals
bool interval::operator==(Interval l, Interval r) {
  return (lower(l) == lower(r)) && (upper(l) == upper(r));
}

bool interval::operator!=(Interval l, Interval r) {
  return !(l == r);
}

Interval interval::widen(Interval in, std::set<int> &knownInts) {
  int new_left = pinf;
  int new_right = minf;
  
  for (auto iter = knownInts.rbegin(); iter != knownInts.rend(); iter++) {
    if (*iter <= lower(in)) {
      new_left = *iter;
      break;
    }
  }
  for (auto i: knownInts) {
    if (i >= upper(in)) {
      new_right = i;
      break;
    }
  }
  return make(new_left, new_right);
}

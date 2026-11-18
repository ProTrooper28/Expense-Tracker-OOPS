#pragma once

#include <algorithm>
#include <vector>

namespace rupee {

// OOP syllabus: templates plus std::copy_if/stable_sort demonstrate generic code and STL algorithms.
template <typename Range, typename Predicate>
auto filteredCopy(const Range& source, Predicate predicate) {
  using Value = typename Range::value_type;
  std::vector<Value> result;
  std::copy_if(source.begin(), source.end(), std::back_inserter(result), predicate);
  return result;
}

template <typename Range, typename Comparator>
void sortValues(Range& values, Comparator comparator) {
  std::stable_sort(values.begin(), values.end(), comparator);
}

} // namespace rupee

#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace rupee {

class ExpenseManager;

class Analytics {
 public:
  nlohmann::json dashboard(const ExpenseManager& manager, const std::string& selectedMonth) const;
  nlohmann::json report(const ExpenseManager& manager, const std::string& selectedMonth) const;
  nlohmann::json monthlySeries(const ExpenseManager& manager,
                               const std::string& endingMonth, int count = 6) const;
};

} // namespace rupee

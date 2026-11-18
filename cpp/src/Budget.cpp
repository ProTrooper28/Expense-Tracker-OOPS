#include "Budget.h"
#include "Money.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace rupee {
Budget::Budget(std::string category, std::string month, std::int64_t monthlyLimitPaise,
               std::int64_t amountSpentPaise)
    : category_(std::move(category)), month_(std::move(month)),
      monthlyLimitPaise_(monthlyLimitPaise), amountSpentPaise_(amountSpentPaise) {
  if (monthlyLimitPaise_ <= 0) throw std::invalid_argument("Budget limit must be greater than zero.");
}
std::int64_t Budget::calculateRemaining() const noexcept { return monthlyLimitPaise_ - amountSpentPaise_; }
double Budget::calculateUsagePercentage() const noexcept {
  if (monthlyLimitPaise_ <= 0) return 0.0;
  return static_cast<double>(amountSpentPaise_) * 100.0 / static_cast<double>(monthlyLimitPaise_);
}
bool Budget::isExceeded() const noexcept { return amountSpentPaise_ >= monthlyLimitPaise_; }
bool Budget::isApproaching() const noexcept { return calculateUsagePercentage() >= 80.0; }
void Budget::setAmountSpent(std::int64_t spentPaise) noexcept { amountSpentPaise_ = std::max<std::int64_t>(0, spentPaise); }
void Budget::setLimit(std::int64_t limitPaise) {
  if (limitPaise <= 0) throw std::invalid_argument("Budget limit must be greater than zero.");
  monthlyLimitPaise_ = limitPaise;
}
void Budget::setLimit(const std::string& rupees) { setLimit(parseRupeesToPaise(rupees)); }
} // namespace rupee

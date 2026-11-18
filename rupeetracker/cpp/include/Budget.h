#pragma once

#include <cstdint>
#include <string>

namespace rupee {

// OOP syllabus: Budget encapsulates its private state behind validated behavior and accessors.
class Budget {
 public:
  Budget(std::string category, std::string month, std::int64_t monthlyLimitPaise,
         std::int64_t amountSpentPaise = 0);
  const std::string& getCategory() const noexcept { return category_; }
  const std::string& getMonth() const noexcept { return month_; }
  std::int64_t getMonthlyLimit() const noexcept { return monthlyLimitPaise_; }
  std::int64_t getAmountSpent() const noexcept { return amountSpentPaise_; }
  std::int64_t calculateRemaining() const noexcept;
  double calculateUsagePercentage() const noexcept;
  bool isExceeded() const noexcept;
  bool isApproaching() const noexcept;
  void setAmountSpent(std::int64_t spentPaise) noexcept;
  // OOP syllabus: overloaded setters accept either paise or decimal INR input.
  void setLimit(std::int64_t limitPaise);
  void setLimit(const std::string& rupees);

 private:
  std::string category_;
  std::string month_;
  std::int64_t monthlyLimitPaise_;
  std::int64_t amountSpentPaise_;
};

} // namespace rupee

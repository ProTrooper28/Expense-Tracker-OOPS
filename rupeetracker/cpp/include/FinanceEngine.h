#pragma once

#include "Analytics.h"
#include "ExpenseManager.h"
#include "StorageManager.h"

#include <string>

namespace rupee {

class FinanceEngine {
 public:
  FinanceEngine();
  void seedDemoData();
  void clearAll();
  void load(const std::string& serializedState);
  std::string serialize() const;
  std::string dashboard(const std::string& selectedMonth) const;
  std::string analytics(const std::string& selectedMonth) const;
  std::string transactions(const std::string& query, const std::string& category,
      const std::string& type, const std::string& dateFrom, const std::string& dateTo,
      const std::string& sortBy, const std::string& sortDirection) const;
  std::string addTransaction(const std::string& title, const std::string& amountRupees,
      const std::string& category, const std::string& type, const std::string& date,
      const std::string& description, const std::string& paymentMethod);
  bool updateTransaction(const std::string& id, const std::string& title,
      const std::string& amountRupees, const std::string& category,
      const std::string& type, const std::string& date,
      const std::string& description, const std::string& paymentMethod);
  bool deleteTransaction(const std::string& id);
  void setBudget(const std::string& category, const std::string& month,
                 const std::string& amountRupees);
  std::string budgets(const std::string& month) const;
  std::string csv() const;
  std::string currentMonth() const;
  std::string categories() const;

 private:
  // OOP syllabus: a FinanceEngine object composes cohesive domain objects instead of duplicating their rules.
  ExpenseManager manager_;
  Analytics analytics_;
  StorageManager storage_;
};

} // namespace rupee

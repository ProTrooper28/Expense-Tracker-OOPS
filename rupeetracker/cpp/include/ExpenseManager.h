#pragma once

#include "Budget.h"
#include "Transaction.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rupee {

class ExpenseManager {
 public:
  ExpenseManager();
  ~ExpenseManager();
  ExpenseManager(const ExpenseManager&) = delete;
  ExpenseManager& operator=(const ExpenseManager&) = delete;

  std::string addTransaction(const std::string& title, const std::string& amountRupees,
      const std::string& category, const std::string& type, const std::string& date,
      const std::string& description, const std::string& paymentMethod);
  std::string addTransactionPaise(const std::string& id, const std::string& title,
      std::int64_t amountPaise, const std::string& category, const std::string& type,
      const std::string& date, const std::string& description,
      const std::string& paymentMethod);
  void updateTransaction(const std::string& id, const std::string& title,
      const std::string& amountRupees, const std::string& category,
      const std::string& type, const std::string& date,
      const std::string& description, const std::string& paymentMethod);
  bool deleteTransaction(const std::string& id);
  void clearTransactions();
  // OOP syllabus: overloads support lookup by external string ID or numeric sequence ID.
  const Transaction* findById(const std::string& id) const;
  const Transaction* findById(std::size_t numericId) const;
  std::vector<const Transaction*> searchTransactions(const std::string& query,
      const std::string& category, const std::string& type,
      const std::string& dateFrom, const std::string& dateTo,
      const std::string& sortBy, const std::string& sortDirection) const;

  std::int64_t totalIncome(const std::string& month = "") const;
  std::int64_t totalExpenses(const std::string& month = "") const;
  std::int64_t savings(const std::string& month = "") const;
  std::int64_t categoryExpenses(const std::string& category, const std::string& month) const;
  std::int64_t categoryIncome(const std::string& category, const std::string& month) const;
  std::vector<std::string> expenseCategories() const;
  std::vector<std::string> incomeCategories() const;
  const std::vector<std::unique_ptr<Transaction>>& transactions() const noexcept { return transactions_; }
  std::size_t transactionCount() const noexcept { return transactions_.size(); }
  void setNextId(std::uint64_t value) noexcept { nextId_ = value; }
  std::uint64_t nextId() const noexcept { return nextId_; }

  void setBudget(const std::string& category, const std::string& month,
                 const std::string& amountRupees);
  void setBudgetPaise(const std::string& category, const std::string& month,
                      std::int64_t amountPaise);
  const std::vector<Budget>& budgets() const noexcept { return budgets_; }
  std::vector<Budget>& budgets() noexcept { return budgets_; }
  std::vector<Budget> budgetsForMonth(const std::string& month) const;
  void clearBudgets();

 private:
  // OOP syllabus: STL vectors compose the manager; unique_ptr owns polymorphic objects and frees them via RAII.
  std::vector<std::unique_ptr<Transaction>> transactions_;
  std::vector<Budget> budgets_;
  std::uint64_t nextId_;
  std::unique_ptr<Transaction> makeTransaction(const std::string& id,
      const std::string& title, std::int64_t amountPaise, const std::string& category,
      const std::string& type, const std::string& date, const std::string& description,
      const std::string& paymentMethod) const;
};

} // namespace rupee

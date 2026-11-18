#include "FinanceEngine.h"

#include "Money.h"
#include "Transaction.h"

#include <algorithm>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace rupee {
using json = nlohmann::json;
namespace {
std::string dateInMonth(const std::string& month, int day) {
  return month + "-" + (day < 10 ? "0" : "") + std::to_string(day);
}
json transactionJson(const Transaction& item) {
  return {{"id", item.getId()}, {"title", item.getTitle()},
    {"amountPaise", item.getAmount()}, {"amount", static_cast<double>(item.getAmount()) / 100.0},
    {"formatted", formatINR(item.getAmount())}, {"amountInput", formatRupeesInput(item.getAmount())},
    {"category", item.getCategory()},
    {"type", item.getType()}, {"date", item.getDate()},
    {"description", item.getDescription()}, {"paymentMethod", item.getPaymentMethod()},
    {"details", item.getDetails()}};
}
} // namespace

FinanceEngine::FinanceEngine() = default;
void FinanceEngine::seedDemoData() {
  if (manager_.transactionCount() > 0) return;
  const auto month = rupee::currentMonth();
  manager_.addTransactionPaise("", "Monthly salary", 3500000, "Salary", "Income", dateInMonth(month, 1), "Monthly pay", "Bank transfer");
  manager_.addTransactionPaise("", "Home rent", 1200000, "Rent", "Expense", dateInMonth(month, 2), "Monthly rent", "Bank transfer");
  manager_.addTransactionPaise("", "Metro and cab", 150000, "Transport", "Expense", dateInMonth(month, 4), "Commute", "UPI");
  manager_.addTransactionPaise("", "Weekly groceries", 320000, "Groceries", "Expense", dateInMonth(month, 8), "Household shopping", "UPI");
  manager_.addTransactionPaise("", "Pharmacy", 80000, "Health", "Expense", dateInMonth(month, 10), "Health essentials", "UPI");
  manager_.addTransactionPaise("", "Lunch and coffee", 200000, "Food", "Expense", dateInMonth(month, 12), "Meals", "UPI");
  manager_.addTransactionPaise("", "Weekend shopping", 150000, "Shopping", "Expense", dateInMonth(month, 15), "Personal items", "Card");
  manager_.addTransactionPaise("", "Design project", 500000, "Freelance", "Income", dateInMonth(month, 17), "Freelance payment", "Bank transfer");

  // A compact five-month history makes the six-month charts meaningful at first launch.
  for (int offset = -5; offset <= -1; ++offset) {
    const auto past = shiftMonth(month, offset);
    const int salary = 35000 + offset * 250;
    const int transit = 1100 + ((-offset) % 3) * 175;
    const int groceries = 2700 + ((-offset) % 4) * 130;
    manager_.addTransactionPaise("", "Monthly salary", salary * 100, "Salary", "Income", dateInMonth(past, 1), "Monthly pay", "Bank transfer");
    manager_.addTransactionPaise("", "Home rent", 1200000, "Rent", "Expense", dateInMonth(past, 2), "Monthly rent", "Bank transfer");
    manager_.addTransactionPaise("", "Commute", transit * 100, "Transport", "Expense", dateInMonth(past, 7), "Travel", "UPI");
    manager_.addTransactionPaise("", "Groceries", groceries * 100, "Groceries", "Expense", dateInMonth(past, 12), "Household shopping", "UPI");
    manager_.addTransactionPaise("", "Meals and bills", (2500 + ((-offset) % 3) * 100) * 100,
      offset % 2 == 0 ? "Food" : "Bills", "Expense", dateInMonth(past, 19), "Monthly spending", "UPI");
    if (offset % 2 == 0)
      manager_.addTransactionPaise("", "Freelance project", 350000, "Freelance", "Income", dateInMonth(past, 23), "Project payment", "Bank transfer");
  }

  manager_.setBudgetPaise("__monthly__", month, 3000000);
  manager_.setBudgetPaise("Rent", month, 1500000);
  manager_.setBudgetPaise("Transport", month, 300000);
  manager_.setBudgetPaise("Groceries", month, 500000);
  manager_.setBudgetPaise("Health", month, 250000);
  manager_.setBudgetPaise("Food", month, 400000);
}
void FinanceEngine::clearAll() { manager_.clearTransactions(); manager_.clearBudgets(); }
void FinanceEngine::load(const std::string& serializedState) { storage_.deserialize(serializedState, manager_); }
std::string FinanceEngine::serialize() const { return storage_.serialize(manager_); }
std::string FinanceEngine::dashboard(const std::string& selectedMonth) const {
  return analytics_.dashboard(manager_, selectedMonth).dump();
}
std::string FinanceEngine::analytics(const std::string& selectedMonth) const {
  return analytics_.report(manager_, selectedMonth).dump();
}
std::string FinanceEngine::transactions(const std::string& query, const std::string& category,
    const std::string& type, const std::string& dateFrom, const std::string& dateTo,
    const std::string& sortBy, const std::string& sortDirection) const {
  json rows = json::array();
  for (const auto* item : manager_.searchTransactions(query, category, type, dateFrom, dateTo, sortBy, sortDirection))
    rows.push_back(transactionJson(*item));
  return rows.dump();
}
std::string FinanceEngine::addTransaction(const std::string& title, const std::string& amountRupees,
    const std::string& category, const std::string& type, const std::string& date,
    const std::string& description, const std::string& paymentMethod) {
  return manager_.addTransaction(title, amountRupees, category, type, date, description, paymentMethod);
}
bool FinanceEngine::updateTransaction(const std::string& id, const std::string& title,
    const std::string& amountRupees, const std::string& category, const std::string& type,
    const std::string& date, const std::string& description, const std::string& paymentMethod) {
  manager_.updateTransaction(id, title, amountRupees, category, type, date, description, paymentMethod);
  return true;
}
bool FinanceEngine::deleteTransaction(const std::string& id) { return manager_.deleteTransaction(id); }
void FinanceEngine::setBudget(const std::string& category, const std::string& month,
                              const std::string& amountRupees) {
  manager_.setBudget(category, month, amountRupees);
}
std::string FinanceEngine::budgets(const std::string& month) const {
  json result = json::array();
  for (const auto& budget : manager_.budgetsForMonth(month)) {
    result.push_back({{"category", budget.getCategory()}, {"month", budget.getMonth()},
      {"limitPaise", budget.getMonthlyLimit()}, {"limitFormatted", formatINR(budget.getMonthlyLimit())},
      {"spentPaise", budget.getAmountSpent()}, {"spentFormatted", formatINR(budget.getAmountSpent())},
      {"remainingPaise", budget.calculateRemaining()}, {"remainingFormatted", formatINR(budget.calculateRemaining())},
      {"usagePercentage", budget.calculateUsagePercentage()}, {"isExceeded", budget.isExceeded()},
      {"isApproaching", budget.isApproaching()},
      {"barPercentage", std::max(0.0, std::min(100.0, budget.calculateUsagePercentage()))}});
  }
  return result.dump();
}
std::string FinanceEngine::csv() const { return storage_.exportCsv(manager_); }
std::string FinanceEngine::currentMonth() const { return rupee::currentMonth(); }
std::string FinanceEngine::categories() const {
  return json{{"expenses", manager_.expenseCategories()}, {"income", manager_.incomeCategories()}}.dump();
}

} // namespace rupee

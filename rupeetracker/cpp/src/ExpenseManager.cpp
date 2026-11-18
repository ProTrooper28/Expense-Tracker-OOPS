#include "ExpenseManager.h"

#include "Algorithms.h"
#include "Expense.h"
#include "Income.h"
#include "Money.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace rupee {
namespace {
std::string lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  return value;
}
bool validDate(const std::string& value) {
  if (value.size() != 10 || value[4] != '-' || value[7] != '-') return false;
  for (std::size_t i = 0; i < value.size(); ++i)
    if (i != 4 && i != 7 && (value[i] < '0' || value[i] > '9')) return false;
  try {
    const int year = std::stoi(value.substr(0, 4));
    const int month = std::stoi(value.substr(5, 2));
    const int day = std::stoi(value.substr(8, 2));
    if (year < 1900 || month < 1 || month > 12) return false;
    static constexpr int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const bool leapYear = year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
    const int maximumDay = month == 2 && leapYear ? 29 : daysInMonth[month - 1];
    return day >= 1 && day <= maximumDay;
  } catch (...) { return false; }
}
} // namespace

ExpenseManager::ExpenseManager() : nextId_(1) {}
ExpenseManager::~ExpenseManager() = default;

// OOP syllabus: exception handling rejects invalid domain objects before they enter the owned collection.
std::unique_ptr<Transaction> ExpenseManager::makeTransaction(
    const std::string& id, const std::string& title, std::int64_t amountPaise,
    const std::string& category, const std::string& type, const std::string& date,
    const std::string& description, const std::string& paymentMethod) const {
  if (title.empty()) throw std::invalid_argument("Transaction title is required.");
  if (category.empty()) throw std::invalid_argument("Choose a transaction category.");
  if (amountPaise <= 0) throw std::invalid_argument("Transaction amount must be greater than zero.");
  if (!validDate(date)) throw std::invalid_argument("Choose a valid transaction date.");
  if (type == "Income")
    return std::make_unique<Income>(id, title, amountPaise, category, date, description, paymentMethod);
  if (type == "Expense")
    return std::make_unique<Expense>(id, title, amountPaise, category, date, description, paymentMethod);
  throw std::invalid_argument("Transaction type must be Income or Expense.");
}

std::string ExpenseManager::addTransaction(const std::string& title, const std::string& amountRupees,
    const std::string& category, const std::string& type, const std::string& date,
    const std::string& description, const std::string& paymentMethod) {
  return addTransactionPaise("", title, parseRupeesToPaise(amountRupees), category, type,
                             date, description, paymentMethod);
}
std::string ExpenseManager::addTransactionPaise(const std::string& id, const std::string& title,
    std::int64_t amountPaise, const std::string& category, const std::string& type,
    const std::string& date, const std::string& description, const std::string& paymentMethod) {
  const std::string actualId = id.empty() ? "tx-" + std::to_string(nextId_++) : id;
  auto transaction = makeTransaction(actualId, title, amountPaise, category, type, date,
                                     description, paymentMethod);
  transactions_.push_back(std::move(transaction));
  if (!id.empty() && id.rfind("tx-", 0) == 0) {
    try { nextId_ = std::max(nextId_, static_cast<std::uint64_t>(std::stoull(id.substr(3)) + 1)); }
    catch (...) { /* Imported IDs need not follow the generated sequence. */ }
  }
  return actualId;
}
void ExpenseManager::updateTransaction(const std::string& id, const std::string& title,
    const std::string& amountRupees, const std::string& category, const std::string& type,
    const std::string& date, const std::string& description, const std::string& paymentMethod) {
  const auto amount = parseRupeesToPaise(amountRupees);
  for (auto& transaction : transactions_) {
    if (transaction->getId() == id) {
      auto replacement = makeTransaction(id, title, amount, category, type, date,
                                         description, paymentMethod);
      transaction = std::move(replacement);
      return;
    }
  }
  throw std::invalid_argument("Transaction was not found.");
}
bool ExpenseManager::deleteTransaction(const std::string& id) {
  const auto before = transactions_.size();
  transactions_.erase(std::remove_if(transactions_.begin(), transactions_.end(),
      [&](const auto& value) { return value->getId() == id; }), transactions_.end());
  return transactions_.size() != before;
}
void ExpenseManager::clearTransactions() { transactions_.clear(); nextId_ = 1; }
const Transaction* ExpenseManager::findById(const std::string& id) const {
  const auto found = std::find_if(transactions_.begin(), transactions_.end(),
      [&](const auto& value) { return value->getId() == id; });
  return found == transactions_.end() ? nullptr : found->get();
}
const Transaction* ExpenseManager::findById(std::size_t numericId) const {
  return findById("tx-" + std::to_string(numericId));
}
std::vector<const Transaction*> ExpenseManager::searchTransactions(const std::string& query,
    const std::string& category, const std::string& type, const std::string& dateFrom,
    const std::string& dateTo, const std::string& sortBy, const std::string& sortDirection) const {
  std::vector<const Transaction*> all;
  all.reserve(transactions_.size());
  for (const auto& item : transactions_) all.push_back(item.get());
  const auto q = lower(query);
  auto result = filteredCopy(all, [&](const Transaction* value) {
    const bool typeMatches = type.empty() || type == "All" || value->getType() == type;
    const bool categoryMatches = category.empty() || category == "All" || value->getCategory() == category;
    const bool startMatches = dateFrom.empty() || value->getDate() >= dateFrom;
    const bool endMatches = dateTo.empty() || value->getDate() <= dateTo;
    const std::string searchable = lower(value->getTitle() + " " + value->getCategory() + " " +
      value->getDescription() + " " + value->getPaymentMethod());
    return typeMatches && categoryMatches && startMatches && endMatches &&
           (q.empty() || searchable.find(q) != std::string::npos);
  });
  const bool descending = lower(sortDirection) == "desc";
  sortValues(result, [&](const Transaction* a, const Transaction* b) {
    bool less = false;
    if (sortBy == "amount") less = *a < *b;
    else if (sortBy == "title") less = lower(a->getTitle()) < lower(b->getTitle());
    else less = a->getDate() == b->getDate() ? a->getId() < b->getId() : a->getDate() < b->getDate();
    return descending ? !less && !(*a == *b) : less;
  });
  return result;
}
std::int64_t ExpenseManager::totalIncome(const std::string& month) const {
  std::int64_t total = 0;
  for (const auto& item : transactions_)
    if (item->getType() == "Income" && (month.empty() || monthFromDate(item->getDate()) == month))
      total += item->getAmount();
  return total;
}
std::int64_t ExpenseManager::totalExpenses(const std::string& month) const {
  std::int64_t total = 0;
  for (const auto& item : transactions_)
    if (item->getType() == "Expense" && (month.empty() || monthFromDate(item->getDate()) == month))
      total += item->getAmount();
  return total;
}
std::int64_t ExpenseManager::savings(const std::string& month) const {
  return totalIncome(month) - totalExpenses(month);
}
std::int64_t ExpenseManager::categoryExpenses(const std::string& category, const std::string& month) const {
  std::int64_t total = 0;
  for (const auto& item : transactions_)
    if (item->getType() == "Expense" && item->getCategory() == category &&
        (month.empty() || monthFromDate(item->getDate()) == month)) total += item->getAmount();
  return total;
}
std::int64_t ExpenseManager::categoryIncome(const std::string& category, const std::string& month) const {
  std::int64_t total = 0;
  for (const auto& item : transactions_)
    if (item->getType() == "Income" && item->getCategory() == category &&
        (month.empty() || monthFromDate(item->getDate()) == month)) total += item->getAmount();
  return total;
}
std::vector<std::string> ExpenseManager::expenseCategories() const {
  return {"Rent", "Transport", "Food", "Groceries", "Health", "Shopping", "Entertainment", "Education", "Bills", "Other"};
}
std::vector<std::string> ExpenseManager::incomeCategories() const {
  return {"Salary", "Freelance", "Scholarship", "Allowance", "Investments", "Other"};
}
void ExpenseManager::setBudget(const std::string& category, const std::string& month,
                               const std::string& amountRupees) {
  setBudgetPaise(category, month, parseRupeesToPaise(amountRupees));
}
void ExpenseManager::setBudgetPaise(const std::string& category, const std::string& month,
                                    std::int64_t amountPaise) {
  if (category.empty() || month.size() != 7 || month[4] != '-')
    throw std::invalid_argument("Choose a valid budget category and month.");
  auto found = std::find_if(budgets_.begin(), budgets_.end(), [&](const Budget& budget) {
    return budget.getCategory() == category && budget.getMonth() == month;
  });
  if (found != budgets_.end()) found->setLimit(amountPaise);
  else budgets_.emplace_back(category, month, amountPaise);
}
std::vector<Budget> ExpenseManager::budgetsForMonth(const std::string& month) const {
  std::vector<Budget> result;
  for (const auto& budget : budgets_) {
    if (budget.getMonth() != month) continue;
    Budget computed = budget;
    const auto spent = budget.getCategory() == "__monthly__"
        ? totalExpenses(month) : categoryExpenses(budget.getCategory(), month);
    computed.setAmountSpent(spent);
    result.push_back(computed);
  }
  return result;
}
void ExpenseManager::clearBudgets() { budgets_.clear(); }

} // namespace rupee

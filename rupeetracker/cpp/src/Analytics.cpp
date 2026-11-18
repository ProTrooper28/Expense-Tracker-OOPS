#include "Analytics.h"

#include "ExpenseManager.h"
#include "Money.h"
#include "Transaction.h"

#include <algorithm>
#include <array>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

namespace rupee {
namespace {
std::string weekStartMonday(const std::string& isoDate, int weeksBack = 0) {
  std::tm value{};
  value.tm_year = std::stoi(isoDate.substr(0, 4)) - 1900;
  value.tm_mon = std::stoi(isoDate.substr(5, 2)) - 1;
  value.tm_mday = std::stoi(isoDate.substr(8, 2));
  value.tm_hour = 12;
  std::mktime(&value);
  value.tm_mday -= ((value.tm_wday + 6) % 7) + (weeksBack * 7);
  std::mktime(&value);
  std::ostringstream out;
  out << std::put_time(&value, "%Y-%m-%d");
  return out.str();
}
std::int64_t weeklyTotal(const ExpenseManager& manager, const std::string& type,
                         const std::string& start) {
  std::int64_t total = 0;
  const std::string end = [&] {
    std::tm value{};
    value.tm_year = std::stoi(start.substr(0, 4)) - 1900;
    value.tm_mon = std::stoi(start.substr(5, 2)) - 1;
    value.tm_mday = std::stoi(start.substr(8, 2)) + 6;
    value.tm_hour = 12;
    std::mktime(&value);
    std::ostringstream out;
    out << std::put_time(&value, "%Y-%m-%d");
    return out.str();
  }();
  for (const auto& item : manager.transactions())
    if (item->getType() == type && item->getDate() >= start && item->getDate() <= end)
      total += item->getAmount();
  return total;
}
nlohmann::json categoryRows(const ExpenseManager& manager, const std::string& month) {
  nlohmann::json rows = nlohmann::json::array();
  const auto categories = manager.expenseCategories();
  std::int64_t total = 0;
  for (const auto& category : categories) total += manager.categoryExpenses(category, month);
  for (const auto& category : categories) {
    const auto amount = manager.categoryExpenses(category, month);
    if (amount <= 0) continue;
    const double share = total > 0 ? static_cast<double>(amount) * 100.0 / total : 0.0;
    rows.push_back({{"category", category}, {"amountPaise", amount},
                    {"amount", static_cast<double>(amount) / 100.0},
                    {"formatted", formatINR(amount)}, {"share", share}});
  }
  std::sort(rows.begin(), rows.end(), [](const nlohmann::json& a, const nlohmann::json& b) {
    return a["amountPaise"].get<std::int64_t>() > b["amountPaise"].get<std::int64_t>();
  });
  return rows;
}
} // namespace

nlohmann::json Analytics::monthlySeries(const ExpenseManager& manager,
    const std::string& endingMonth, int count) const {
  nlohmann::json series = nlohmann::json::array();
  if (count < 1) count = 1;
  if (count > 24) count = 24;
  std::int64_t maximumSavingsMagnitude = 1;
  for (int index = count - 1; index >= 0; --index) {
    const auto month = shiftMonth(endingMonth, -index);
    const auto savings = manager.totalIncome(month) - manager.totalExpenses(month);
    maximumSavingsMagnitude = std::max(maximumSavingsMagnitude, savings < 0 ? -savings : savings);
  }
  for (int index = count - 1; index >= 0; --index) {
    const auto month = shiftMonth(endingMonth, -index);
    const auto income = manager.totalIncome(month);
    const auto expense = manager.totalExpenses(month);
    const auto savings = income - expense;
    const int barHeight = std::max(8, static_cast<int>((savings < 0 ? -savings : savings) * 100 / maximumSavingsMagnitude));
    series.push_back({{"month", month}, {"label", monthLabel(month)},
      {"incomePaise", income}, {"income", static_cast<double>(income) / 100.0},
      {"incomeFormatted", formatINR(income)}, {"expensePaise", expense},
      {"expense", static_cast<double>(expense) / 100.0},
      {"expenseFormatted", formatINR(expense)},
      {"savingsPaise", savings}, {"savings", static_cast<double>(savings) / 100.0},
      {"savingsFormatted", formatINR(savings)},
      {"savingsStatus", savings < 0 ? "negative" : savings > 0 ? "positive" : "neutral"},
      {"savingsBarHeight", barHeight}});
  }
  return series;
}

nlohmann::json Analytics::dashboard(const ExpenseManager& manager,
                                    const std::string& selectedMonth) const {
  const std::string month = selectedMonth.empty() ? rupee::currentMonth() : selectedMonth;
  const auto income = manager.totalIncome();
  const auto expense = manager.totalExpenses();
  const auto savings = income - expense;
  const auto monthIncome = manager.totalIncome(month);
  const auto monthExpense = manager.totalExpenses(month);
  const std::string monday = weekStartMonday(rupee::currentISODate());
  const std::string lastMonday = weekStartMonday(rupee::currentISODate(), 1);
  const auto weekIncome = weeklyTotal(manager, "Income", monday);
  const auto weekExpense = weeklyTotal(manager, "Expense", monday);
  const auto prevWeekIncome = weeklyTotal(manager, "Income", lastMonday);
  const auto prevWeekExpense = weeklyTotal(manager, "Expense", lastMonday);

  nlohmann::json root;
  root["selectedMonth"] = month;
  root["currentDate"] = rupee::currentISODate();
  root["totals"] = {{"incomePaise", income}, {"income", static_cast<double>(income) / 100.0},
    {"incomeFormatted", formatINR(income)}, {"expensePaise", expense},
    {"expense", static_cast<double>(expense) / 100.0}, {"expenseFormatted", formatINR(expense)},
    {"balancePaise", savings}, {"balance", static_cast<double>(savings) / 100.0},
    {"balanceFormatted", formatINR(savings)}, {"savingsPaise", savings},
    {"savingsFormatted", formatINR(savings)}};
  root["monthTotals"] = {{"incomePaise", monthIncome}, {"incomeFormatted", formatINR(monthIncome)},
    {"expensePaise", monthExpense}, {"expenseFormatted", formatINR(monthExpense)},
    {"savingsPaise", monthIncome - monthExpense},
    {"savingsFormatted", formatINR(monthIncome - monthExpense)}};
  root["weekly"] = {{"incomePaise", weekIncome}, {"incomeFormatted", formatINR(weekIncome)},
    {"expensePaise", weekExpense}, {"expenseFormatted", formatINR(weekExpense)},
    {"previousIncomePaise", prevWeekIncome}, {"previousIncomeFormatted", formatINR(prevWeekIncome)},
    {"previousExpensePaise", prevWeekExpense}, {"previousExpenseFormatted", formatINR(prevWeekExpense)},
    {"incomeChangePaise", weekIncome - prevWeekIncome},
    {"expenseChangePaise", weekExpense - prevWeekExpense},
    {"incomeUp", weekIncome >= prevWeekIncome}, {"expenseUp", weekExpense >= prevWeekExpense}};

  const auto categories = categoryRows(manager, month);
  root["categories"] = categories;
  root["categoryCards"] = nlohmann::json::array();
  const std::array<const char*, 4> featured = {"Rent", "Transport", "Health", "Groceries"};
  for (const char* category : featured) {
    const auto amount = manager.categoryExpenses(category, month);
    root["categoryCards"].push_back({{"category", category}, {"amountPaise", amount},
      {"amount", static_cast<double>(amount) / 100.0}, {"formatted", formatINR(amount)}});
  }

  const auto monthlyBudget = manager.budgetsForMonth(month);
  std::int64_t budgetLimit = 0;
  for (const auto& budget : monthlyBudget)
    if (budget.getCategory() == "__monthly__") budgetLimit = budget.getMonthlyLimit();
  const int usage = budgetLimit > 0
      ? static_cast<int>((monthExpense * 100 + budgetLimit / 2) / budgetLimit) : 0;
  const bool atOrOverBudget = budgetLimit > 0 && monthExpense >= budgetLimit;
  std::string insightText;
  if (budgetLimit == 0) insightText = "Set a monthly budget to start tracking your pace.";
  else if (atOrOverBudget) insightText = "You have reached your monthly spending limit. Review your recent expenses.";
  else if (usage >= 80) insightText = "You have spent " + std::to_string(usage) + "% of your monthly budget. Keep an eye on the rest.";
  else insightText = "Good job! You have spent " + std::to_string(usage) + "% of your monthly budget.";
  root["insight"] = {{"text", insightText}, {"usagePercent", usage},
    {"status", budgetLimit == 0 ? "no-budget" : atOrOverBudget ? "exceeded" : usage >= 80 ? "warning" : "good"},
    {"budgetPaise", budgetLimit}, {"budgetFormatted", budgetLimit > 0 ? formatINR(budgetLimit) : "—"},
    {"monthExpensePaise", monthExpense}, {"monthExpenseFormatted", formatINR(monthExpense)}};

  const auto recent = manager.searchTransactions("", "", "", "", "", "date", "desc");
  root["latestTransactions"] = nlohmann::json::array();
  const auto count = std::min<std::size_t>(7, recent.size());
  for (std::size_t i = 0; i < count; ++i) {
    const auto* item = recent[i];
    root["latestTransactions"].push_back({{"id", item->getId()}, {"title", item->getTitle()},
      {"category", item->getCategory()}, {"type", item->getType()},
      {"amountPaise", item->getAmount()}, {"amount", static_cast<double>(item->getAmount()) / 100.0},
      {"formatted", formatINR(item->getAmount())}, {"date", item->getDate()},
      {"description", item->getDescription()}, {"paymentMethod", item->getPaymentMethod()}});
  }
  root["monthlySeries"] = monthlySeries(manager, month, 6);
  return root;
}

nlohmann::json Analytics::report(const ExpenseManager& manager,
                                 const std::string& selectedMonth) const {
  const std::string month = selectedMonth.empty() ? rupee::currentMonth() : selectedMonth;
  const std::string previousMonth = shiftMonth(month, -1);
  const auto income = manager.totalIncome(month);
  const auto expense = manager.totalExpenses(month);
  const auto previousIncome = manager.totalIncome(previousMonth);
  const auto previousExpense = manager.totalExpenses(previousMonth);
  const auto categories = categoryRows(manager, month);
  std::string topCategory = "—";
  if (!categories.empty()) topCategory = categories.front()["category"].get<std::string>();
  nlohmann::json root = {{"month", month}, {"monthLabel", monthLabel(month)},
    {"previousMonth", previousMonth}, {"topCategory", topCategory},
    {"categories", categories}, {"monthlySeries", monthlySeries(manager, month, 6)},
    {"totals", {{"incomePaise", income}, {"incomeFormatted", formatINR(income)},
      {"expensePaise", expense}, {"expenseFormatted", formatINR(expense)},
      {"savingsPaise", income - expense}, {"savingsFormatted", formatINR(income - expense)},
      {"allTimeSavingsPaise", manager.savings()}, {"allTimeSavingsFormatted", formatINR(manager.savings())}}},
    {"comparison", {{"incomeChangePaise", income - previousIncome},
      {"expenseChangePaise", expense - previousExpense},
      {"previousIncomeFormatted", formatINR(previousIncome)},
      {"previousExpenseFormatted", formatINR(previousExpense)}}}};
  return root;
}

} // namespace rupee

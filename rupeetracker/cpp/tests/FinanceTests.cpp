#include "Analytics.h"
#include "Budget.h"
#include "ExpenseManager.h"
#include "Money.h"
#include "StorageManager.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace rupee;

int main() {
  assert(parseRupeesToPaise("12530.00") == 1253000);
  assert(parseRupeesToPaise("1,25,000.50") == 12500050);
  assert(formatINR(1253000) == "₹12,530.00");
  assert(formatINR(12500050) == "₹1,25,000.50");
  bool rejected = false;
  try { (void)parseRupeesToPaise("12.345"); } catch (const std::invalid_argument&) { rejected = true; }
  assert(rejected);

  ExpenseManager dateManager;
  const auto leapDayId = dateManager.addTransaction("Leap day", "1.00", "Other", "Expense", "2024-02-29", "", "Cash");
  assert(dateManager.findById(leapDayId) != nullptr);
  for (const auto& invalidDate : {"2026-02-29", "2026-02-31", "2026-04-31", "2026-13-01", "2026-00-10", "2026-11-1x"}) {
    bool invalidDateRejected = false;
    try { dateManager.addTransaction("Invalid date", "1.00", "Other", "Expense", invalidDate, "", "Cash"); }
    catch (const std::invalid_argument&) { invalidDateRejected = true; }
    assert(invalidDateRejected);
  }

  ExpenseManager manager;
  const auto month = currentMonth();
  manager.addTransaction("Pay", "35000.00", "Salary", "Income", month + "-01", "salary", "Bank transfer");
  manager.addTransaction("Rent", "12000.00", "Rent", "Expense", month + "-02", "home", "UPI");
  manager.addTransaction("Bus", "325.90", "Transport", "Expense", month + "-03", "travel", "Card");
  assert(manager.totalIncome(month) == 3500000);
  assert(manager.totalExpenses(month) == 1232590);
  assert(manager.savings(month) == 2267410);
  assert(manager.categoryExpenses("Transport", month) == 32590);
  assert(manager.findById("tx-1") != nullptr);
  assert(manager.findById(static_cast<std::size_t>(2)) != nullptr);

  const auto filtered = manager.searchTransactions("bus", "All", "Expense", "", "", "amount", "desc");
  assert(filtered.size() == 1 && filtered.front()->getTitle() == "Bus");
  manager.setBudget("Rent", month, "15000.00");
  const auto budgets = manager.budgetsForMonth(month);
  assert(budgets.size() == 1);
  assert(budgets.front().calculateRemaining() == 300000);
  assert(budgets.front().calculateUsagePercentage() > 79.9);
  assert(budgets.front().isApproaching());

  Analytics analytics;
  const auto summary = analytics.dashboard(manager, month);
  assert(summary.at("totals").at("incomeFormatted") == "₹35,000.00");
  assert(summary.at("categoryCards").size() == 4);
  assert(summary.at("monthlySeries").size() == 6);

  StorageManager storage;
  const auto encoded = storage.serialize(manager);
  ExpenseManager restored;
  storage.deserialize(encoded, restored);
  assert(restored.transactionCount() == 3);
  assert(restored.totalExpenses(month) == manager.totalExpenses(month));
  assert(restored.budgetsForMonth(month).size() == 1);
  const auto csv = storage.exportCsv(restored);
  assert(csv.find("Amount (INR)") != std::string::npos);
  assert(csv.find("\"₹35,000.00\"") != std::string::npos);

  const auto id = manager.addTransaction("Tutoring", "1000.00", "Freelance", "Income", month + "-04", "", "UPI");
  manager.updateTransaction(id, "Tutoring revised", "1500.00", "Freelance", "Income", month + "-05", "", "UPI");
  assert(manager.findById(id)->getAmount() == 150000);
  assert(manager.deleteTransaction(id));
  assert(!manager.deleteTransaction("does-not-exist"));
  assert(!manager.deleteTransaction("does-not-exist"));

  Budget atLimit("Test", month, 10000, 10000);
  assert(atLimit.isExceeded());

  std::cout << "All C++ finance tests passed.\n";
  return 0;
}

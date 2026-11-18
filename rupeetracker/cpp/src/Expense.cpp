#include "Expense.h"

namespace rupee {
std::string Expense::getType() const { return "Expense"; }
std::string Expense::getDetails() const { return "Expense recorded: " + Transaction::getDetails(); }
std::unique_ptr<Transaction> Expense::clone() const { return std::make_unique<Expense>(*this); }
} // namespace rupee

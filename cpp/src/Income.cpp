#include "Income.h"

namespace rupee {
std::string Income::getType() const { return "Income"; }
std::string Income::getDetails() const { return "Income received: " + Transaction::getDetails(); }
std::unique_ptr<Transaction> Income::clone() const { return std::make_unique<Income>(*this); }
} // namespace rupee

#pragma once

#include "Transaction.h"

namespace rupee {

// OOP syllabus: inheritance, function overriding and virtual dispatch.
class Expense final : public Transaction {
 public:
  using Transaction::Transaction;
  std::string getType() const override;
  std::string getDetails() const override;
  std::unique_ptr<Transaction> clone() const override;
};

} // namespace rupee

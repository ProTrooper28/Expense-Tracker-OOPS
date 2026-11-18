#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace rupee {

class Transaction {
 public:
  // OOP syllabus: constructors initialize domain objects; the virtual destructor makes base-pointer cleanup safe.
  Transaction(std::string id, std::string title, std::int64_t amountPaise,
              std::string category, std::string date, std::string description,
              std::string paymentMethod);
  Transaction(const Transaction& other);
  virtual ~Transaction();

  const std::string& getId() const noexcept { return id_; }
  const std::string& getTitle() const noexcept { return title_; }
  std::int64_t getAmount() const noexcept { return amountPaise_; }
  const std::string& getCategory() const noexcept { return category_; }
  const std::string& getDate() const noexcept { return date_; }
  const std::string& getDescription() const noexcept { return description_; }
  const std::string& getPaymentMethod() const noexcept { return paymentMethod_; }

  // OOP syllabus: this virtual interface enables runtime dispatch to Income/Expense overrides.
  virtual std::string getType() const = 0;
  virtual std::string getDetails() const;
  virtual std::unique_ptr<Transaction> clone() const = 0;

  // OOP syllabus: operator overloading compares records by amount, then ID.
  bool operator<(const Transaction& other) const noexcept;
  bool operator==(const Transaction& other) const noexcept;
  static std::size_t liveObjects() noexcept { return liveObjects_; }

 protected:
  std::string id_;
  std::string title_;
  std::int64_t amountPaise_;
  std::string category_;
  std::string date_;
  std::string description_;
  std::string paymentMethod_;

 private:
  // OOP syllabus: static data member observes the lifetime of domain objects.
  static std::size_t liveObjects_;
};

} // namespace rupee

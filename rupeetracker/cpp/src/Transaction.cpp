#include "Transaction.h"

#include <sstream>

namespace rupee {

std::size_t Transaction::liveObjects_ = 0;

Transaction::Transaction(std::string id, std::string title, std::int64_t amountPaise,
                         std::string category, std::string date,
                         std::string description, std::string paymentMethod)
    : id_(std::move(id)), title_(std::move(title)), amountPaise_(amountPaise),
      category_(std::move(category)), date_(std::move(date)),
      description_(std::move(description)), paymentMethod_(std::move(paymentMethod)) {
  ++liveObjects_;
}
Transaction::Transaction(const Transaction& other)
    : id_(other.id_), title_(other.title_), amountPaise_(other.amountPaise_),
      category_(other.category_), date_(other.date_), description_(other.description_),
      paymentMethod_(other.paymentMethod_) {
  ++liveObjects_;
}
Transaction::~Transaction() { --liveObjects_; }

std::string Transaction::getDetails() const {
  std::ostringstream out;
  out << getType() << " · " << title_ << " · " << category_ << " · " << date_;
  return out.str();
}
bool Transaction::operator<(const Transaction& other) const noexcept {
  if (amountPaise_ != other.amountPaise_) return amountPaise_ < other.amountPaise_;
  return id_ < other.id_;
}
bool Transaction::operator==(const Transaction& other) const noexcept { return id_ == other.id_; }

} // namespace rupee

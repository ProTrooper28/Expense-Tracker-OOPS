#include "StorageManager.h"

#include "Budget.h"
#include "Expense.h"
#include "ExpenseManager.h"
#include "Income.h"
#include "Money.h"

#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>

namespace rupee {
using json = nlohmann::json;

std::string StorageManager::serialize(const ExpenseManager& manager) const {
  json root;
  root["version"] = 1;
  root["nextId"] = manager.nextId();
  root["transactions"] = json::array();
  for (const auto& value : manager.transactions()) {
    const auto& item = *value;
    root["transactions"].push_back({
      {"id", item.getId()}, {"title", item.getTitle()}, {"amountPaise", item.getAmount()},
      {"category", item.getCategory()}, {"type", item.getType()}, {"date", item.getDate()},
      {"description", item.getDescription()}, {"paymentMethod", item.getPaymentMethod()}
    });
  }
  root["budgets"] = json::array();
  for (const auto& budget : manager.budgets()) {
    root["budgets"].push_back({{"category", budget.getCategory()}, {"month", budget.getMonth()},
                              {"limitPaise", budget.getMonthlyLimit()}});
  }
  return root.dump();
}
void StorageManager::deserialize(const std::string& snapshot, ExpenseManager& manager) const {
  try {
    const auto root = json::parse(snapshot);
    if (!root.is_object() || root.value("version", 0) != 1 ||
        !root.contains("transactions") || !root["transactions"].is_array())
      throw std::invalid_argument("Saved data format is not supported.");
    manager.clearTransactions();
    manager.clearBudgets();
    for (const auto& item : root["transactions"]) {
      manager.addTransactionPaise(item.at("id").get<std::string>(),
        item.at("title").get<std::string>(), item.at("amountPaise").get<std::int64_t>(),
        item.at("category").get<std::string>(), item.at("type").get<std::string>(),
        item.at("date").get<std::string>(), item.value("description", std::string{}),
        item.value("paymentMethod", std::string{"Other"}));
    }
    if (root.contains("budgets") && root["budgets"].is_array()) {
      for (const auto& item : root["budgets"])
        manager.setBudgetPaise(item.at("category").get<std::string>(),
          item.at("month").get<std::string>(), item.at("limitPaise").get<std::int64_t>());
    }
    if (root.contains("nextId")) manager.setNextId(root["nextId"].get<std::uint64_t>());
  } catch (const std::exception& error) {
    throw std::invalid_argument(std::string("Could not restore saved records: ") + error.what());
  }
}
std::string StorageManager::exportCsv(const ExpenseManager& manager) const {
  auto escape = [](const std::string& value) {
    std::string result = "\"";
    for (char ch : value) { if (ch == '\"') result += "\"\""; else result += ch; }
    return result + "\"";
  };
  std::ostringstream out;
  out << "ID,Title,Amount (INR),Category,Type,Date,Description,Payment method\r\n";
  for (const auto& value : manager.transactions()) {
    const auto& item = *value;
    out << escape(item.getId()) << ',' << escape(item.getTitle()) << ','
        << escape(formatINR(item.getAmount())) << ',' << escape(item.getCategory()) << ','
        << escape(item.getType()) << ',' << escape(item.getDate()) << ','
        << escape(item.getDescription()) << ',' << escape(item.getPaymentMethod()) << "\r\n";
  }
  return out.str();
}
void StorageManager::writeSnapshotFile(const std::string& path, const std::string& snapshot) const {
  std::ofstream output(path, std::ios::binary);
  if (!output) throw std::runtime_error("Could not create the snapshot file.");
  output << snapshot;
  if (!output.good()) throw std::runtime_error("Could not write the snapshot file.");
}
std::string StorageManager::readSnapshotFile(const std::string& path) const {
  std::ifstream input(path, std::ios::binary);
  if (!input) throw std::runtime_error("Could not open the snapshot file.");
  std::ostringstream buffer;
  buffer << input.rdbuf();
  return buffer.str();
}

} // namespace rupee

#pragma once

#include <string>

namespace rupee {

class ExpenseManager;

class StorageManager {
 public:
  std::string serialize(const ExpenseManager& manager) const;
  // Loading the serialized IndexedDB snapshot imports saved records into the C++ domain model.
  void deserialize(const std::string& snapshot, ExpenseManager& manager) const;
  std::string exportCsv(const ExpenseManager& manager) const;

  // File-stream examples use Emscripten's in-memory filesystem in WASM; IndexedDB
  // remains the durable browser store and receives the returned serialized text.
  void writeSnapshotFile(const std::string& path, const std::string& snapshot) const;
  std::string readSnapshotFile(const std::string& path) const;
};

} // namespace rupee

#pragma once

#include <cstdint>
#include <string>

namespace rupee {

// Money enters the domain as decimal text and is stored only as integer paise.
std::int64_t parseRupeesToPaise(const std::string& rupees);
std::string formatINR(std::int64_t paise);
std::string formatRupeesInput(std::int64_t paise);
std::string currentISODate();
std::string currentMonth();
std::string monthFromDate(const std::string& isoDate);
std::string shiftMonth(const std::string& yearMonth, int offset);
std::string monthLabel(const std::string& yearMonth);
int dayOfWeekMondayZero(const std::string& isoDate);

} // namespace rupee

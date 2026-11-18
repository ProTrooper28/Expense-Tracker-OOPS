#include "Money.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace rupee {
namespace {
std::string trim(std::string value) {
  const auto first = value.find_first_not_of(" \t\n\r");
  if (first == std::string::npos) return "";
  const auto last = value.find_last_not_of(" \t\n\r");
  return value.substr(first, last - first + 1);
}
std::tm parseDate(const std::string& date) {
  if (date.size() < 10 || date[4] != '-' || date[7] != '-')
    throw std::invalid_argument("Date must use YYYY-MM-DD format.");
  std::tm value{};
  value.tm_year = std::stoi(date.substr(0, 4)) - 1900;
  value.tm_mon = std::stoi(date.substr(5, 2)) - 1;
  value.tm_mday = std::stoi(date.substr(8, 2));
  value.tm_hour = 12;
  if (value.tm_mon < 0 || value.tm_mon > 11 || value.tm_mday < 1 || value.tm_mday > 31)
    throw std::invalid_argument("Date is outside the supported calendar range.");
  std::mktime(&value);
  return value;
}
std::string formatDate(const std::tm& value) {
  std::ostringstream out;
  out << std::put_time(&value, "%Y-%m-%d");
  return out.str();
}
std::string groupedIndian(std::string digits) {
  if (digits.size() <= 3) return digits;
  const std::string lastThree = digits.substr(digits.size() - 3);
  std::string left = digits.substr(0, digits.size() - 3);
  std::string groups;
  while (left.size() > 2) {
    groups = "," + left.substr(left.size() - 2) + groups;
    left.resize(left.size() - 2);
  }
  return left + groups + "," + lastThree;
}
} // namespace

std::int64_t parseRupeesToPaise(const std::string& input) {
  std::string value = trim(input);
  value.erase(std::remove(value.begin(), value.end(), ','), value.end());
  if (value.empty() || value.front() == '-' || value.front() == '+')
    throw std::invalid_argument("Enter a positive INR amount.");
  const auto dot = value.find('.');
  if (dot != std::string::npos && value.find('.', dot + 1) != std::string::npos)
    throw std::invalid_argument("Amount contains more than one decimal point.");
  std::string whole = dot == std::string::npos ? value : value.substr(0, dot);
  std::string fraction = dot == std::string::npos ? "" : value.substr(dot + 1);
  if (whole.empty()) whole = "0";
  if (fraction.size() > 2) throw std::invalid_argument("Use no more than two decimal places.");
  if (fraction.empty()) fraction = "00";
  if (fraction.size() == 1) fraction.push_back('0');
  if (whole.find_first_not_of("0123456789") != std::string::npos ||
      fraction.find_first_not_of("0123456789") != std::string::npos)
    throw std::invalid_argument("Enter a valid INR amount.");
  try {
    const auto rupees = std::stoll(whole);
    const auto paise = std::stoll(fraction);
    if (rupees > (std::numeric_limits<std::int64_t>::max() - paise) / 100)
      throw std::out_of_range("Amount exceeds the supported limit.");
    const auto result = rupees * 100 + paise;
    if (result <= 0) throw std::invalid_argument("Amount must be greater than zero.");
    return result;
  } catch (const std::invalid_argument&) {
    throw;
  } catch (const std::exception&) {
    throw std::invalid_argument("Amount exceeds the supported limit.");
  }
}

std::string formatINR(std::int64_t paise) {
  const bool negative = paise < 0;
  const std::uint64_t absolute = negative
      ? static_cast<std::uint64_t>(-(paise + 1)) + 1
      : static_cast<std::uint64_t>(paise);
  std::ostringstream out;
  if (negative) out << '-';
  out << "₹" << groupedIndian(std::to_string(absolute / 100)) << '.'
      << std::setw(2) << std::setfill('0') << (absolute % 100);
  return out.str();
}

std::string formatRupeesInput(std::int64_t paise) {
  const bool negative = paise < 0;
  const std::uint64_t absolute = negative
      ? static_cast<std::uint64_t>(-(paise + 1)) + 1
      : static_cast<std::uint64_t>(paise);
  std::ostringstream out;
  if (negative) out << '-';
  out << (absolute / 100) << '.' << std::setw(2) << std::setfill('0') << (absolute % 100);
  return out.str();
}

std::string currentISODate() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t raw = std::chrono::system_clock::to_time_t(now);
  std::tm local{};
#if defined(_WIN32)
  localtime_s(&local, &raw);
#else
  localtime_r(&raw, &local);
#endif
  return formatDate(local);
}
std::string currentMonth() { return currentISODate().substr(0, 7); }
std::string monthFromDate(const std::string& date) {
  if (date.size() < 7) throw std::invalid_argument("Date must include a year and month.");
  return date.substr(0, 7);
}
std::string shiftMonth(const std::string& yearMonth, int offset) {
  if (yearMonth.size() < 7 || yearMonth[4] != '-')
    throw std::invalid_argument("Month must use YYYY-MM format.");
  const int year = std::stoi(yearMonth.substr(0, 4));
  const int month = std::stoi(yearMonth.substr(5, 2));
  if (month < 1 || month > 12) throw std::invalid_argument("Month is invalid.");
  const int total = year * 12 + (month - 1) + offset;
  const int shiftedYear = total >= 0 ? total / 12 : (total - 11) / 12;
  const int shiftedMonth = total - shiftedYear * 12 + 1;
  std::ostringstream out;
  out << std::setw(4) << std::setfill('0') << shiftedYear << '-'
      << std::setw(2) << shiftedMonth;
  return out.str();
}
std::string monthLabel(const std::string& yearMonth) {
  static const std::array<const char*, 12> names = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  if (yearMonth.size() < 7) return yearMonth;
  int month = std::stoi(yearMonth.substr(5, 2));
  if (month < 1 || month > 12) return yearMonth;
  return names[month - 1];
}
int dayOfWeekMondayZero(const std::string& isoDate) {
  auto value = parseDate(isoDate);
  return (value.tm_wday + 6) % 7;
}

} // namespace rupee

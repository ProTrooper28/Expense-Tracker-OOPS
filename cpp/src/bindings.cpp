#include "FinanceEngine.h"
#include <emscripten/bind.h>

using namespace emscripten;
using rupee::FinanceEngine;

EMSCRIPTEN_BINDINGS(rupee_finance_module) {
  class_<FinanceEngine>("FinanceEngine")
    .constructor<>()
    .function("seedDemoData", &FinanceEngine::seedDemoData)
    .function("clearAll", &FinanceEngine::clearAll)
    .function("load", &FinanceEngine::load)
    .function("serialize", &FinanceEngine::serialize)
    .function("dashboard", &FinanceEngine::dashboard)
    .function("analytics", &FinanceEngine::analytics)
    .function("transactions", &FinanceEngine::transactions)
    .function("addTransaction", &FinanceEngine::addTransaction)
    .function("updateTransaction", &FinanceEngine::updateTransaction)
    .function("deleteTransaction", &FinanceEngine::deleteTransaction)
    .function("setBudget", &FinanceEngine::setBudget)
    .function("budgets", &FinanceEngine::budgets)
    .function("csv", &FinanceEngine::csv)
    .function("currentMonth", &FinanceEngine::currentMonth)
    .function("categories", &FinanceEngine::categories);
}

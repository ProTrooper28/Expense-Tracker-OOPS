cmake_minimum_required(VERSION 3.20)
project(rupee_finance_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(BUILD_TESTS "Build native C++ domain tests" ON)

set(FINANCE_CORE_SOURCES
  src/Money.cpp
  src/Transaction.cpp
  src/Income.cpp
  src/Expense.cpp
  src/Budget.cpp
  src/ExpenseManager.cpp
  src/StorageManager.cpp
  src/Analytics.cpp
  src/FinanceEngine.cpp
)
add_library(finance_core STATIC ${FINANCE_CORE_SOURCES})
target_include_directories(finance_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(finance_core PRIVATE -Wall -Wextra -Wpedantic -fexceptions)

if(EMSCRIPTEN)
  add_executable(finance_wasm src/bindings.cpp src/main.cpp)
  target_link_libraries(finance_wasm PRIVATE finance_core)
  target_compile_options(finance_wasm PRIVATE -O2 -fexceptions)
  target_link_options(finance_wasm PRIVATE
    --bind
    -fexceptions
    -sMODULARIZE=1
    -sEXPORT_ES6=1
    -sEXPORT_NAME=createFinanceModule
    -sENVIRONMENT=web,worker
    -sALLOW_MEMORY_GROWTH=1
    -sNO_EXIT_RUNTIME=1
    -sFORCE_FILESYSTEM=1
    -sEXPORTED_RUNTIME_METHODS=FS
    -sWASM=1
  )
  set_target_properties(finance_wasm PROPERTIES
    OUTPUT_NAME "finance"
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../frontend/public/wasm"
  )
else()
  if(BUILD_TESTS)
    enable_testing()
    add_executable(finance_tests tests/FinanceTests.cpp)
    target_link_libraries(finance_tests PRIVATE finance_core)
    add_test(NAME finance_tests COMMAND finance_tests)
  endif()
endif()

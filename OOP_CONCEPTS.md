# OOP Concepts and Project Demonstration Guide

This document points to the implementation that demonstrates the OOP syllabus through the actual expense tracker. Financial rules stay in C++ and the same engine is exposed to the UI through Embind/WASM.

| Concept | Where it appears | How to explain it |
|---|---|---|
| Classes and objects | `cpp/include/Transaction.h`, `Income.h`, `Expense.h`, `Budget.h`; `cpp/src/FinanceEngine.cpp` | Each transaction, budget and finance engine is represented by a class; sample and user records are objects. |
| Encapsulation | `Transaction` and `Budget` keep data members private/protected and expose read/update operations | Domain data is not directly edited by the React application; mutation is performed through validated C++ methods. |
| Inheritance | `Income` and `Expense` derive from `Transaction` | Shared transaction fields and behavior are reused while the type is specialized. |
| Runtime polymorphism | `Transaction::getType`, `getDetails` and `clone` are virtual; `Income.cpp` and `Expense.cpp` override them | `ExpenseManager` owns base-class pointers and dynamically dispatches the actual record behavior. |
| Function overriding | `Income::getDetails` and `Expense::getDetails` | The derived types provide their own detail text while keeping the common base interface. |
| Virtual functions | `cpp/include/Transaction.h` | A virtual destructor and pure virtual type/clone interface make deletion through `unique_ptr<Transaction>` safe and behavior polymorphic. |
| Constructors and destructors | `Transaction.cpp`, `Budget.cpp`, `ExpenseManager.cpp` | Constructors initialize valid objects; the virtual transaction destructor updates the live-object counter. |
| Static data member | `Transaction::liveObjects_` in `Transaction.cpp` | One class-level counter tracks the live transaction objects across all instances. |
| Function overloading | `ExpenseManager::findById(string)` and `findById(size_t)`; `Budget::setLimit(int64_t)` and `setLimit(string)` | Related operations are offered with different argument types for ID lookup and budget-limit input. |
| Operator overloading | `Transaction::operator<` and `operator==` | Transactions compare by amount (then stable ID) or identity; generic sorting uses the comparison operator. |
| Composition | `FinanceEngine` contains `ExpenseManager`, `Analytics` and `StorageManager`; `ExpenseManager` owns transactions and budgets | Larger application objects are composed from cohesive domain collaborators. |
| Dynamic memory management | `ExpenseManager` stores `std::unique_ptr<Transaction>` and creates derived records with `std::make_unique` | Smart pointers express ownership and safely destroy polymorphic transaction objects. |
| Templates | `cpp/include/Algorithms.h` | Generic `filteredCopy` and `sortValues` algorithms work with different range value types and predicates/comparators. |
| Exception handling | `Money.cpp`, `ExpenseManager.cpp`, `Budget.cpp`, `StorageManager.cpp` | Invalid amounts, dates, types and budgets raise exceptions; WASM reports failed calls for user-visible error handling. |
| File handling | `StorageManager.cpp` | Snapshot read/write operations use C++ `ifstream`/`ofstream`; in WASM this is an in-memory file example, while durable browser storage uses the C++ serialization string in IndexedDB. |
| STL containers and algorithms | `ExpenseManager.h/.cpp`, `Algorithms.h`, `Analytics.cpp` | Transactions/budgets use vectors; `algorithm` operations filter, sort and aggregate records. The JSON library is used for a stable transfer/storage representation. |

## Source walkthrough

- **Money rules:** `cpp/src/Money.cpp` parses decimal rupees into integer paise, formats INR with Indian comma grouping and supplies calendar helpers.
- **Domain types:** `cpp/include/Transaction.h`, `cpp/include/Income.h`, `cpp/include/Expense.h`, `cpp/src/Transaction.cpp`, `cpp/src/Income.cpp`, and `cpp/src/Expense.cpp` demonstrate encapsulation and polymorphism.
- **Budgets and operations:** `cpp/include/Budget.h`, `cpp/src/Budget.cpp`, `cpp/include/ExpenseManager.h`, and `cpp/src/ExpenseManager.cpp` implement budget checks, CRUD, validation, searching, sorting and category totals.
- **Reports and persistence:** `cpp/src/Analytics.cpp` calculates dashboard/report values. `cpp/src/StorageManager.cpp` serializes the model, restores it and produces CSV/file output.
- **WASM boundary:** `cpp/include/FinanceEngine.h`, `cpp/src/FinanceEngine.cpp`, and `cpp/src/bindings.cpp` expose the operations. `frontend/src/services/finance.js` is only a loader; `frontend/src/services/storage.js` stores the serialized C++ snapshot.
- **Tests:** `cpp/tests/FinanceTests.cpp` exercises monetary parsing/formatting, totals, filters, budgets, analytics and persistence round trips. Run `npm run test:cpp`.

## Suggested in-class walkthrough

1. Show a C++-constructed `Income` and `Expense` through their shared `Transaction` interface and point out the overridden virtual methods.
2. Follow the Add Transaction action from `TransactionModal.jsx` to `FinanceEngine::addTransaction`, `ExpenseManager::addTransaction`, `parseRupeesToPaise`, and the `Income`/`Expense` object.
3. Show integer paise stored in the object and `formatINR` returning a display value such as `₹1,25,000.00`.
4. Show how `Analytics::dashboard` provides the C++ summaries used by the dashboard charts; React does not re-calculate income, expense or savings.
5. Show C++ snapshot serialization in IndexedDB and the deserialization path after reload.

## Proposed division of work between two team members

| Team member | Primary ownership | Shared review |
|---|---|---|
| Member 1 — C++ and WebAssembly | Domain classes; paise parser/formatter; `ExpenseManager`; `Budget`; analytics and storage; Embind bindings; C++ tests; explain the OOP concepts and review every financial rule. | Agree the data contract with Member 2; validate add/edit/delete, analytics and budget operations through the UI. |
| Member 2 — React interface and deployment | Screenshot-guided responsive shell; pages, charts, modal/forms and navigation; IndexedDB adapter; Vite/Tailwind setup; Vercel configuration; README/demo workflow. | Confirm the UI only renders C++-returned results; build with Member 1’s WASM artifact and review accessibility/responsive behavior. |

## Demonstration checklist

- Build the C++ and WebAssembly module with `npm run build:wasm`.
- Run native C++ checks with `npm run test:cpp`.
- Start Vite with `npm run dev`.
- Add an expense and income, edit and delete a record, and show totals updating.
- Set a budget, view C++-returned progress/warnings, and change the report month.
- Export transactions to CSV and refresh the browser to demonstrate IndexedDB persistence.

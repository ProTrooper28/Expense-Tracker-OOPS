# RupeeTracker

RupeeTracker is an expense-tracking application built using C++ and object-oriented programming concepts.

## OOP concepts used

- **Classes and objects** — represent transactions, budgets, and the finance engine.
- **Encapsulation** — protect financial data and update it through validated methods.
- **Inheritance** — `Income` and `Expense` extend the shared `Transaction` class.
- **Polymorphism and virtual functions** — handle different transaction types through a common interface.
- **Function overriding** — income and expense provide their own transaction details.
- **Constructors and destructors** — initialize and clean up objects.
- **Static data members** — track live transaction objects.
- **Function and operator overloading** — support related operations with different inputs and compare transactions.
- **Composition** — build the finance engine from managers and other components.
- **Dynamic memory management** — use smart pointers to own transaction objects.
- **Templates** — provide reusable filtering and sorting algorithms.
- **Exception handling** — report invalid financial data and operations.
- **File handling** — serialize and restore financial data and export CSV.
- **STL containers and algorithms** — store, search, sort, and analyze records.

## What the app does

- Records and manages income and expenses.
- Tracks overall and category budgets.
- Shows balances, spending summaries, and monthly analytics.
- Searches, filters, and sorts transactions.
- Exports transactions to CSV and saves data in the browser.

See [OOP_CONCEPTS.md](./OOP_CONCEPTS.md) for examples of these concepts in the project.

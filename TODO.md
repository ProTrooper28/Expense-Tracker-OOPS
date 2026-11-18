# TODO — Expense Tracker Outcomes

## Application shell, navigation and responsive dashboard
- [x] Reproduce the supplied screenshot’s dark rounded app shell and three zones: narrow icon-only left navigation, broad black center dashboard and dark charcoal right panel. The screenshot’s lavender perimeter is outside the UI and must not be rendered. Use dark charcoal cards/panels, rounded corners, soft shadows, subtle borders, minimal typography, pastel cyan/purple/light-green/yellow-orange category tiles, green income and red expenses, with purple/cyan accents.
- [x] Left navigation includes user profile/avatar, Dashboard, Transactions, Budget, Analytics and Notifications icons; show active navigation indicators and make navigation work.
- [x] Dashboard top shows “Welcome back!” and current date, with optional notification indicator. Responsive behavior rearranges the three sections vertically on smaller screens without breaking the dashboard.

## Dashboard calculations and charts
- [x] Display total balance, total income, total expenditure, savings, category-wise expenditure, monthly income vs expenses and recent transactions.
- [x] Update all dashboard components immediately after transactions change.
- [x] Insights card includes spending-distribution donut, total expenditure, short automatically generated spending insights and a working Read All button.
- [x] Total Balance card includes current balance in INR, previous-week comparison, weekly income and weekly expenditure.
- [x] Show Rent (cyan), Transport (purple), Health (light green) and Groceries (yellow/orange) category cards, each with icon, total expenditure and category name.
- [x] Monthly Overview chart shows income in green and expenses in purple, monthly labels, hover tooltips, dynamic data returned by C++, and month selection.

## Transactions and transaction history
- [x] Support adding income, adding expenses, editing transactions, deleting transactions, searching, filtering by category, transaction type and date, and sorting by amount or date.
- [x] Each transaction has unique ID, title, amount, category, Income/Expense type, date, description and payment method.
- [x] Provide a polished Add Transaction modal with proper validation.
- [x] Default expense categories: Rent, Transport, Food, Groceries, Health, Shopping, Entertainment, Education, Bills and Other. Default income categories: Salary, Freelance, Scholarship, Allowance, Investments and Other. Users can assign categories when creating transactions.
- [x] Provide a dedicated history page with search, category/date/type filters, sort options and CSV export.

## Budgets and analytics
- [x] Users can set monthly budgets and category-wise budgets, view spending progress and remaining budget, and receive visual warnings when spending approaches or exceeds a budget.
- [x] Analytics page includes monthly income vs expense chart, category-wise spending donut, savings trend, highest spending category, monthly comparison and total savings. Allow selecting a month to view the report.

## INR, C++/WASM and OOP
- [x] Every financial amount uses Indian Rupees only: ₹, INR, en-IN locale and Indian comma grouping. Examples include ₹12,530.00, ₹1,410.00, ₹325.90 and ₹1,25,000.00. Never display dollars, USD or another currency.
- [x] All calculations use integer paise in C++ to avoid floating-point financial inaccuracies.
- [x] Actual financial calculations, transaction operations, budget management, data processing and business rules execute in C++17 or newer compiled to WebAssembly using Emscripten and Embind. React handles UI rendering, input collection, presentation, UI state, storage communication and the dedicated WASM communication layer only; it must not implement financial business logic, duplicate ExpenseManager logic or use hardcoded dashboard calculations.
- [x] WASM initializes before enabling financial operations. Charts consume actual results returned by C++. Apply the flow to all financial operations: form data -> C++ WASM operation -> C++ model update/calculation -> returned financial summary -> React render.
- [x] Implement Transaction base class with id, title, amount, category, date and description, plus getDetails(), getAmount(), getCategory() and getType(). Implement derived Income and Expense classes and income/expense-specific behavior.
- [x] Implement Budget (category, monthlyLimit, amountSpent; calculateRemaining(), calculateUsagePercentage(), isExceeded()), ExpenseManager (add/update/delete/search and income/expense/savings/financial summary), Analytics (monthly summaries, category spending, income/expense comparisons and savings) and StorageManager (serialization, deserialization, importing saved records via C++ snapshot restore, exporting financial data as CSV).
- [x] Use appropriate STL containers including vector, unordered_map, string and algorithm; smart pointers where appropriate.
- [x] Naturally demonstrate and add source comments for classes/objects, encapsulation, inheritance, runtime polymorphism, function overriding, virtual functions, constructors/destructors, static data members, function overloading, operator overloading, composition, dynamic memory management, templates, exception handling, file handling, and STL containers/algorithms. Use meaningful examples: Income/Expense inheritance, virtual transaction behavior, comparison by transaction amount, templates for generic sorting/filtering, exception validation, static transaction-object tracking, and ExpenseManager composition of transactions and budgets.

## Persistence and initial demo data
- [x] Use IndexedDB for persistent browser storage. React may communicate with IndexedDB, but C++ controls the data model and serialization/deserialization.
- [x] Prepopulate realistic sample INR transactions on first launch, including monthly salary ₹35,000, Rent ₹12,000, Transport ₹1,500, Groceries ₹3,200, Health ₹800, Food ₹2,000, Shopping ₹1,500 and Freelance income ₹5,000. Include dates in the current and previous months for meaningful charts.
- [x] Allow users to clear demo data and start fresh. Do not reset user data on every page reload.

## Right-side card and latest transactions
- [x] Show an attractive virtual debit card UI with masked card number, cardholder name and expiry date. Do not implement real banking or payment integrations.
- [x] Latest Transactions shows merchant/category icon, title, date, amount, green for income and red for expenses, plus a working Show More button.

## Build, deployment, tests and documents
- [x] Use React, Vite, Tailwind CSS, Recharts, Lucide React, responsive design, C++17+, Emscripten, WebAssembly, Embind and IndexedDB.
- [x] Organize professional C++ headers/implementation, React components/pages/hooks/services, and project/build configuration.
- [x] Provide package scripts, CMakeLists.txt or equivalent build configuration, Emscripten build instructions, Vercel configuration, production build scripts, README with exact setup/deployment steps, and correct WASM paths/MIME handling. Build C++ to WASM then React. Static frontend and WASM must work on Vercel without a locally running C++ server.
- [x] Test add expense, add income, edit, delete, balance updates, category-wise calculations, monthly reports, budget alerts, persistence after refresh, production build and that financial logic is actually executing in C++ WASM.
- [ ] Test deployed Vercel application if/when deployment is explicitly requested; deployment was not requested for this task.
- [x] Deliver complete source code, functional React frontend, actual C++ OOP implementation, compiled WASM, Vercel-ready configuration, README, OOP-concept-to-source-location document, running/demonstration instructions and a clear division of work between two team members.
- [x] No fake buttons, no non-functional navigation, no placeholder pages, no success-only buttons that omit the underlying operation. Prioritize a working application over unnecessary features.

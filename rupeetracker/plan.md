# Implementation Plan — Rupee Expense Tracker

## Product and architecture

Build a static, Vercel-deployable React + Vite application whose financial domain model and every money-related operation live in C++17 compiled to WebAssembly through Emscripten and exposed using Embind. The browser UI will be a presentation/input layer only. A small WASM service adapter owns the boundary between React and the C++ module. C++ creates and owns transaction and budget records, validates operations, filters/sorts records, computes all financial totals, budget status, category/monthly aggregates, and serializes/deserializes the complete model. JavaScript may parse returned JSON for rendering and transfer C++-produced serialized strings to/from IndexedDB; it may not calculate finance or mutate financial records.

Store every amount as signed integer paise in C++. UI amount entry will be passed to a C++ decimal-string parser for INR-to-paise conversion; formatting and aggregation remain in C++ and the UI displays returned INR-formatted values. On first launch, if IndexedDB has no saved state, C++ supplies realistic demo transactions across the current and previous months. Clearing demo data is an explicit user action. On subsequent launch the persisted state is restored through the C++ deserializer rather than reset.

## Design

### Reference and design movement

Follow the supplied screenshot’s premium dark finance-dashboard aesthetic. The screenshot’s lavender perimeter is image/background outside the application and is explicitly not part of the UI; do not render lavender. Reproduce only the rounded dark application shell: compact charcoal icon rail at left, near-black center, charcoal right panel, soft borders and restrained shadows.

### Core principles

- Preserve the screenshot’s three-zone proportions and strong dark/light contrast while adapting the layout responsively.
- Use restrained accents (cyan, purple, mint, warm yellow) for category cards; green for income and red for expenses.
- Keep financial information immediately legible with meaningful typography, generous card spacing and direct labels.
- Make every visible control perform a real action and keep all money values in INR.

### Color philosophy

Near-black is the main working surface; charcoal separates navigation and supporting panels. Pastel cyan, violet, mint and amber category fills echo the reference’s bright tiles without making the overall interface noisy. Green and red carry semantic income/expense meaning, respectively. No lavender outer canvas or decorative outer backdrop is included.

### Layout paradigm

A rounded, desktop-height three-column financial workspace: narrow icon-only rail, wide primary dashboard, and fixed-width auxiliary cards/activity panel. At tablet widths the auxiliary panel drops below the main dashboard; at mobile widths all sections become a readable vertical flow and navigation remains accessible without horizontal overflow.

### Signature elements

- Four pastel category tiles with circular icon badges.
- A charcoal insights/balance pair above a large combined income/expense overview chart.
- A right-side virtual debit card, shown as illustrative only, stacked above compact merchant transaction rows.

### Interaction philosophy and motion

Actions are direct and reversible where practical: forms validate before C++ receives a transaction, edits/deletes update the WASM model and then IndexedDB, and resulting summaries refresh from C++. Navigation changes the visible module; search, filters, month selectors, sorting, show-more and CSV export operate on actual application data. Use subtle 120–180 ms opacity/translate transitions for hover/focus and mobile panel changes; avoid chart or dashboard motion that delays reading. Provide visible keyboard focus and reduced-motion respect.

### Typography and brand

Use a clean sans-serif hierarchy: compact semibold section labels, bold greeting and balance, and tabular figures for financial values. Brand essence: a calm, trustworthy INR-first expense workspace for students and personal budgeting; personality: clear, disciplined, approachable. Brand voice: concise and practical. Example lines: “Good job — your spending is on track.” “A clearer view of where your money goes.” The wordmark is a compact rupee/ledger mark paired with “Rupee”; signature brand color is bright mint used sparingly for active states and positive change.

## Functional modules

- **Dashboard:** C++-generated balance, income, expenses, savings, category spending, weekly comparison, monthly chart series, insights and recent transactions.
- **Transactions:** add/edit/delete; typed income/expense; category, date, text search, sort; validation in C++; CSV data generated from C++ serialization/export API.
- **Budgets:** monthly and category limits; usage, remaining amount, near/over-limit status and warnings computed in C++.
- **Analytics:** selected-month income/expense, category shares, savings trend, top category, comparison and total savings all returned from C++.
- **Persistence:** C++ serializes/deserializes model records; browser IndexedDB stores the returned serialized model string without applying business rules.
- **Reports and OOP guide:** README with build/run/deploy/demo instructions; separate document mapping each requested OOP concept to implementation locations and a two-member division of work.

## C++ object model and module boundaries

`Transaction` is an encapsulated polymorphic base type with `Income` and `Expense` derived classes. `Budget` implements remaining/usage/exceeded calculations. `ExpenseManager` composes owned transactions and budgets, maintains indexes, and exposes CRUD/search/summary operations. `Analytics` returns month/category aggregates from manager data. `StorageManager` uses stream-based serialization/deserialization and CSV export; its API is called through the WASM boundary. Embind exposes safe data-transfer operations rather than leaking JS-owned domain logic. Demonstrate virtual override/polymorphism, constructors/destructors, static counters, overloads, comparison operator, templates for sorting/filtering, smart-pointer/dynamic ownership, exceptions, file/stream handling and STL with explanatory comments. A native C++ test executable validates core rules alongside building the browser WASM target.

## Project structure

- `cpp/include/`: domain headers for Transaction, Income, Expense, Budget, ExpenseManager, Analytics and StorageManager.
- `cpp/src/`: domain implementation, JSON/serialization helpers, Embind bindings and native/WASM entry setup.
- `cpp/tests/`: native tests for monetary parsing, CRUD, totals, budgets, filtering, monthly analytics and persistence round trips.
- `frontend/src/components/`: dashboard cards, chart, forms, navigation, dialogs, tables and transaction rows.
- `frontend/src/pages/`: dashboard, transactions, budgets and analytics views.
- `frontend/src/services/`: WASM loading/adapter and IndexedDB string persistence; no duplicate business rules.
- `frontend/src/hooks/`: UI-facing application initialization and refresh hooks.
- Root: Vite/Tailwind, package scripts, Vercel static configuration, `README.md`, `OOP_CONCEPTS.md`, `plan.md`, and `TODO.md`.

## Build and delivery approach

Pin the Node package manager and dependencies, compile C++ into Embind/WASM with an Emscripten build script, and build the Vite frontend after the WASM target is available. Commit generated WASM loader/binary assets so Vercel can deploy a static build without a running C++ server or requiring a server-side Emscripten installation; provide a local `build:wasm`/full-build workflow and a Vercel-safe frontend build that consumes the committed generated assets. Include Vercel SPA routing and WASM MIME configuration if needed. Validate native C++ behavior, run the WASM build, run the frontend production build, and exercise the key flows in Preview where supported. Do not publish/deploy the project unless explicitly requested; provide Vercel-ready files and instructions.

## Constraints and decisions

- Use the supplied screenshot, but omit the lavender outer image canvas per the user’s clarification.
- All displayed money is INR using Indian formatting; C++ stores integer paise.
- No backend, login, real payment or bank integration.
- The sandbox initially has no Emscripten executable; set up a compatible toolchain before compiling WASM and document the build prerequisites.
- Vercel publication is not authorized; prepare a deployable project only.

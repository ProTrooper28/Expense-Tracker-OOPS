# Rupee — Expense Tracker

A responsive INR-first expense tracker for a college OOP project. The React interface is a presentation/input layer; C++17 owns the financial model and operations and is compiled to WebAssembly with Emscripten/Embind. Browser IndexedDB stores a serialized snapshot produced and restored by C++.

The visual design follows the supplied dark dashboard screenshot: a compact icon rail, black center workspace, charcoal right rail, pastel category tiles, spending donut, balance/insight cards, monthly chart and virtual card. The lavender around the screenshot was only its image background and is not rendered in the app.

## Requirements

- Node.js 20+ and npm 10 (npm is pinned in `package.json`).
- CMake 3.20+.
- Emscripten 3.1+ (`emcc`, `emcmake`).
- The nlohmann JSON header is vendored in `cpp/include/nlohmann`; there is no runtime server or separate backend.

On Ubuntu/Debian, install the build tools with:

```bash
sudo apt update
sudo apt install -y emscripten cmake
```

## Run locally

From the project root:

```bash
npm install
npm run build:wasm
npm run dev
```

Open the Vite URL printed by the dev server (the Manus Preview uses port 3000). `npm run build:wasm` compiles `cpp/` and writes `frontend/public/wasm/finance.js` and `finance.wasm`. `npm run dev` serves the React UI and generated module.

To produce the complete local production build (WASM first, React second):

```bash
npm run build
```

The static output is `dist/`. To run the built output locally:

```bash
npm run preview
```

`npm run test:cpp` configures, builds and runs native tests for paise parsing/formatting, CRUD, category filtering, balances, budgets, analytics and serialization. The tests do not need a browser.

## Architecture

`cpp/include` and `cpp/src` contain the `Transaction` base class, `Income` and `Expense` derived classes, `Budget`, `ExpenseManager`, `Analytics`, `StorageManager`, and the `FinanceEngine` façade exposed by `bindings.cpp`. Money is parsed and stored as integer paise; C++ returns the formatted INR strings and chart/report data used by React. C++ also serializes/deserializes transactions and budgets and creates CSV content.

`frontend/src/services/finance.js` loads the Emscripten-generated ES module and offers no parallel JavaScript finance model. `frontend/src/services/storage.js` stores the opaque serialized state string in IndexedDB; it does not inspect or calculate transaction data. `FinanceContext.jsx` initializes WASM, restores the snapshot or seeds demo data on first launch, and asks C++ to return dashboard, budget and analytics summaries after each operation.

The app uses React, Vite, Tailwind CSS, Recharts, Lucide React and React Router. `vercel.json` configures Vercel’s static build, SPA routes, and `application/wasm` MIME type.

## Vercel deployment

The repository includes `vercel.json` with the production output directory (`dist`) and build command (`npm run build:vercel`). Since Vercel’s standard static builder does not include Emscripten, the generated `frontend/public/wasm/finance.js` and `finance.wasm` are committed deployment assets. `npm run build:vercel` verifies these files and builds the React frontend without a server-side C++ service.

1. Build or refresh the WASM artifacts locally: `npm run build:wasm`.
2. Commit both generated files under `frontend/public/wasm/` together with the source changes.
3. Import the repository in Vercel. Keep the project root at the repository root. Vercel reads `vercel.json` and runs `npm run build:vercel`; the output is `dist`.
4. Deploy. The app is static; IndexedDB data remains in each user’s browser and is not sent to a backend.

The project has been prepared for Vercel, but no public deployment has been performed from this workspace.

## Demo flow

1. Start the app. First launch seeds realistic sample records, including the requested monthly salary ₹35,000, Rent ₹12,000, Transport ₹1,500, Groceries ₹3,200, Health ₹800, Food ₹2,000, Shopping ₹1,500 and Freelance income ₹5,000, plus earlier-month history and example budgets.
2. Open **Transactions** and add an income or expense. The modal sends raw decimal INR text to the C++ WASM engine, which validates it, converts it to integer paise, updates the model and returns the new summaries.
3. Edit a transaction, then delete one. Search, type/category/date filters and sorting are run in C++.
4. Open **Budgets** to create an overall or category limit. Usage, remaining budget and warning state come from C++.
5. Open **Analytics** and choose a month. The charts and comparison cards consume C++ summaries.
6. Use **Export CSV** on the transaction page; C++ produces the CSV content. Use **Clear all records** to remove saved transactions and budgets and begin fresh. Reloading after any other operation restores the saved IndexedDB snapshot.

Every displayed financial amount is INR (₹), formatted using Indian grouping. The virtual card is illustrative and has no payment/banking integration.

## OOP and team materials

See [`OOP_CONCEPTS.md`](./OOP_CONCEPTS.md) for a concept-by-concept map to the actual source files and a presentation/demo outline. It also includes a proposed division of work for two team members.

## Important implementation boundary

Do not add transaction totals, category spending, budget usage, month comparisons, savings, currency conversion or financial filtering in React. Extend `FinanceEngine` and its C++ collaborators, expose the operation through Embind, then render returned values. JavaScript is permitted to collect user input, route, render, manage transient UI state, parse C++-returned JSON for presentation, and put/get the C++ snapshot string in IndexedDB.

import { existsSync, statSync } from 'node:fs';
import { resolve } from 'node:path';

const required = ['frontend/public/wasm/finance.js', 'frontend/public/wasm/finance.wasm'];
const missing = required.filter((file) => !existsSync(resolve(file)) || statSync(resolve(file)).size === 0);
if (missing.length) {
  console.error(`Missing generated C++ WebAssembly asset(s): ${missing.join(', ')}. Run npm run build:wasm with Emscripten, then commit the generated assets.`);
  process.exit(1);
}
console.log('Verified checked-in C++/WASM loader and binary.');

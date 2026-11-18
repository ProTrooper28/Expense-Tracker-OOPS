let modulePromise;

export async function createFinanceEngine() {
  if (!modulePromise) {
    modulePromise = (async () => {
      const loaderUrl = new URL('/wasm/finance.js', window.location.href).href;
      const imported = await import(/* @vite-ignore */ loaderUrl);
      const createModule = imported.default;
      const wasm = await createModule({ locateFile: (file) => `/wasm/${file}` });
      return new wasm.FinanceEngine();
    })();
  }
  return modulePromise;
}

export function readCppJson(serialized) {
  return JSON.parse(serialized);
}

import path from 'node:path';
import { fileURLToPath } from 'node:url';

const frontendRoot = path.dirname(fileURLToPath(import.meta.url));

export default {
  content: [path.join(frontendRoot, 'index.html'), path.join(frontendRoot, 'src/**/*.{js,jsx}')],
  theme: {
    extend: {
      colors: {
        ink: '#090a0c',
        panel: '#202124',
        mint: '#55e0b1',
        violet: '#c88aff',
        cyan: '#83edf4',
        amber: '#f0c56e',
      },
      fontFamily: { sans: ['Inter', 'ui-sans-serif', 'system-ui', 'sans-serif'] },
    },
  },
  plugins: [],
};

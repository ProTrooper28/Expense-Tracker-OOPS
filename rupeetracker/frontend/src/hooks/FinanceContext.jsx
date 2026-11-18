import { createContext, useCallback, useContext, useEffect, useMemo, useState } from 'react';
import { createFinanceEngine, readCppJson } from '../services/finance.js';
import { loadSnapshot, saveSnapshot } from '../services/storage.js';

const FinanceContext = createContext(null);

export function FinanceProvider({ children }) {
  const [engine, setEngine] = useState(null);
  const [selectedMonth, setSelectedMonth] = useState('');
  const [dashboard, setDashboard] = useState(null);
  const [analytics, setAnalytics] = useState(null);
  const [budgets, setBudgets] = useState([]);
  const [categories, setCategories] = useState({ expenses: [], income: [] });
  const [revision, setRevision] = useState(0);
  const [status, setStatus] = useState('loading');
  const [loadError, setLoadError] = useState('');
  const [toast, setToast] = useState(null);
  const [transactionModal, setTransactionModal] = useState(null);
  const [notificationsOpen, setNotificationsOpen] = useState(false);

  useEffect(() => {
    let cancelled = false;
    async function initialize() {
      try {
        const finance = await createFinanceEngine();
        const snapshot = await loadSnapshot();
        if (snapshot) {
          finance.load(snapshot);
        } else {
          finance.seedDemoData();
          await saveSnapshot(finance.serialize());
        }
        const month = finance.currentMonth();
        if (cancelled) return;
        setEngine(finance);
        setSelectedMonth(month);
        setCategories(readCppJson(finance.categories()));
        setStatus('ready');
      } catch (error) {
        if (cancelled) return;
        setLoadError(error?.message || 'Could not initialize the finance engine.');
        setStatus('error');
      }
    }
    initialize();
    return () => { cancelled = true; };
  }, []);

  const refresh = useCallback(() => {
    if (!engine || !selectedMonth) return;
    setDashboard(readCppJson(engine.dashboard(selectedMonth)));
    setAnalytics(readCppJson(engine.analytics(selectedMonth)));
    setBudgets(readCppJson(engine.budgets(selectedMonth)));
  }, [engine, selectedMonth]);

  useEffect(() => { refresh(); }, [refresh, revision]);

  const notify = useCallback((message, kind = 'success') => {
    setToast({ message, kind, key: Date.now() });
  }, []);

  const perform = useCallback(async (operation, successMessage) => {
    if (!engine) return false;
    try {
      const result = operation(engine);
      await saveSnapshot(engine.serialize());
      setRevision((value) => value + 1);
      if (successMessage) notify(successMessage);
      return result === undefined ? true : result;
    } catch (error) {
      notify(error?.message || 'The operation could not be completed.', 'error');
      return false;
    }
  }, [engine, notify]);

  const value = useMemo(() => ({
    engine, ready: status === 'ready', loading: status === 'loading',
    loadError, selectedMonth, setSelectedMonth, dashboard, analytics, budgets,
    categories, revision, refresh, perform, toast, setToast, notify,
    transactionModal, setTransactionModal, notificationsOpen, setNotificationsOpen,
  }), [engine, status, loadError, selectedMonth, dashboard, analytics, budgets,
    categories, revision, refresh, perform, toast, notify, transactionModal, notificationsOpen]);

  return <FinanceContext.Provider value={value}>{children}</FinanceContext.Provider>;
}

export function useFinance() {
  const value = useContext(FinanceContext);
  if (!value) throw new Error('useFinance must be used within FinanceProvider.');
  return value;
}

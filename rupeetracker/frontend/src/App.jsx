import { useEffect } from 'react';
import { Bell, ChartNoAxesCombined, CreditCard, LayoutDashboard, LoaderCircle, ReceiptText } from 'lucide-react';
import { NavLink, Navigate, Route, Routes, useLocation } from 'react-router-dom';
import { FinanceProvider, useFinance } from './hooks/FinanceContext.jsx';
import DashboardPage from './pages/DashboardPage.jsx';
import TransactionsPage from './pages/TransactionsPage.jsx';
import BudgetsPage from './pages/BudgetsPage.jsx';
import AnalyticsPage from './pages/AnalyticsPage.jsx';
import RightRail from './components/RightRail.jsx';
import TransactionModal from './components/TransactionModal.jsx';

const navigation = [
  { to: '/', label: 'Dashboard', icon: LayoutDashboard },
  { to: '/transactions', label: 'Transactions', icon: ReceiptText },
  { to: '/budgets', label: 'Budget', icon: CreditCard },
  { to: '/analytics', label: 'Analytics', icon: ChartNoAxesCombined },
];

function AppShell() {
  const { loading, ready, loadError, toast, setToast, transactionModal, notificationsOpen, setNotificationsOpen } = useFinance();
  const location = useLocation();
  useEffect(() => {
    if (!toast) return undefined;
    const timeout = window.setTimeout(() => setToast(null), 3600);
    return () => window.clearTimeout(timeout);
  }, [toast, setToast]);

  if (loading) return <div className="boot-screen"><LoaderCircle className="spin" size={25} /><div><strong>Waking up the finance engine</strong><span>Loading the C++ WebAssembly module…</span></div></div>;
  if (!ready) return <div className="boot-screen boot-error"><div className="boot-error-card"><span className="eyebrow">Rupee could not start</span><h1>The finance engine is unavailable.</h1><p>{loadError || 'Check that the WebAssembly assets were built and try again.'}</p><button className="button button-primary" onClick={() => window.location.reload()}>Try again</button></div></div>;

  return (
    <div className="app-viewport">
      <div className="app-shell">
        <aside className="icon-rail" aria-label="Main navigation">
          <NavLink to="/" className="profile-avatar" aria-label="Rupee home" title="Rupee home"><span>TS</span><i /></NavLink>
          <nav className="rail-links">
            {navigation.map(({ to, label, icon: Icon }) => (
              <NavLink key={to} to={to} aria-label={label} title={label}
                className={({ isActive }) => `rail-link${isActive && (to === '/' ? location.pathname === '/' : true) ? ' active' : ''}`}>
                <Icon size={19} strokeWidth={1.8} />
              </NavLink>
            ))}
            <button className={`rail-link ${notificationsOpen ? 'active' : ''}`} aria-label="Notifications" title="Notifications" onClick={() => setNotificationsOpen((value) => !value)}>
              <Bell size={19} strokeWidth={1.8} /><span className="notification-dot" />
            </button>
          </nav>
          <div className="rail-spacer" />
          <div className="rail-mark" aria-label="Rupee">₹</div>
        </aside>

        <main className="main-panel">
          <Routes>
            <Route path="/" element={<DashboardPage />} />
            <Route path="/transactions" element={<TransactionsPage />} />
            <Route path="/budgets" element={<BudgetsPage />} />
            <Route path="/analytics" element={<AnalyticsPage />} />
            <Route path="*" element={<Navigate to="/" replace />} />
          </Routes>
        </main>

        <RightRail />
        {transactionModal && <TransactionModal />}
        {notificationsOpen && <div className="notice-popover" role="dialog" aria-label="Notifications">
          <div className="popover-heading"><strong>Notifications</strong><button className="icon-button small" onClick={() => setNotificationsOpen(false)} aria-label="Close notifications">×</button></div>
          <div className="notice-item"><span className="notice-icon"><Bell size={15} /></span><p><b>Your records stay private.</b><small>All financial calculations run in the local C++ finance engine.</small></p></div>
          <div className="notice-item"><span className="notice-icon mint"><ChartNoAxesCombined size={15} /></span><p><b>Budget check-in</b><small>Open Budgets to review this month’s progress.</small></p></div>
        </div>}
      </div>
      {toast && <div key={toast.key} className={`toast toast-${toast.kind}`} role="status">{toast.message}</div>}
      <span className="sr-only" aria-live="polite">{location.pathname}</span>
    </div>
  );
}

export default function App() {
  return <FinanceProvider><AppShell /></FinanceProvider>;
}

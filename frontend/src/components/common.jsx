import { CalendarDays, Plus } from 'lucide-react';
import { useFinance } from '../hooks/FinanceContext.jsx';

export function Amount({ children, className = '' }) {
  return <span className={`amount-tabular ${className}`}>{children || '₹0.00'}</span>;
}

export function MonthPicker({ className = '' }) {
  const { selectedMonth, setSelectedMonth } = useFinance();
  return <label className={`month-picker ${className}`}>
    <CalendarDays size={14} />
    <input aria-label="Select report month" type="month" value={selectedMonth} onChange={(event) => setSelectedMonth(event.target.value)} />
  </label>;
}

export function PageHeading({ eyebrow, title, subtitle, action = true }) {
  const { setTransactionModal } = useFinance();
  return <div className="page-heading">
    <div><span className="eyebrow">{eyebrow}</span><h1>{title}</h1>{subtitle && <p>{subtitle}</p>}</div>
    {action && <button className="button button-primary add-button" onClick={() => setTransactionModal({ mode: 'add' })}><Plus size={16} /> Add transaction</button>}
  </div>;
}

export function EmptyState({ title, detail }) {
  return <div className="empty-state"><span className="empty-mark">₹</span><strong>{title}</strong><p>{detail}</p></div>;
}

export function SectionTitle({ children, action, onAction }) {
  return <div className="section-title"><h2>{children}</h2>{action && <button className="text-link" onClick={onAction}>{action}</button>}</div>;
}

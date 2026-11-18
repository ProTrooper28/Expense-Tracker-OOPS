import { ArrowDownRight, ArrowUpRight, CarFront, ChevronRight, HeartPulse, Home, ShoppingBasket } from 'lucide-react';
import { useNavigate } from 'react-router-dom';
import { useFinance } from '../hooks/FinanceContext.jsx';
import { MonthPicker } from '../components/common.jsx';
import { OverviewChart, SpendingDonut } from '../components/DashboardCharts.jsx';

const categoryCards = [
  { name: 'Rent', icon: Home, color: 'category-cyan' },
  { name: 'Transport', icon: CarFront, color: 'category-lilac' },
  { name: 'Health', icon: HeartPulse, color: 'category-green' },
  { name: 'Groceries', icon: ShoppingBasket, color: 'category-amber' },
];
const displayDate = new Intl.DateTimeFormat('en-IN', { weekday: 'long', day: 'numeric', month: 'long', year: 'numeric' });

export default function DashboardPage() {
  const { dashboard, setTransactionModal } = useFinance();
  const navigate = useNavigate();
  if (!dashboard) return null;
  const { totals, monthTotals, weekly, categories, categoryCards: categoryValues, insight, monthlySeries } = dashboard;
  const cardValues = new Map(categoryValues.map((item) => [item.category, item]));

  return <div className="dashboard-page">
    <header className="dashboard-header">
      <div className="greeting-block"><span className="eyebrow">{displayDate.format(new Date(`${dashboard.currentDate}T12:00:00`))}</span><h1>Welcome back!</h1><p>Your money, a little clearer today.</p></div>
      <div className="dashboard-actions"><MonthPicker /><button className="button button-primary" onClick={() => setTransactionModal({ mode: 'add' })}>+ <span>Add transaction</span></button></div>
    </header>

    <section className="top-cards">
      <article className="panel-card insight-card">
        <div className="panel-card-head"><span className="card-overline">Insights</span><button className="text-link" onClick={() => navigate('/analytics')}>Read all</button></div>
        <div className="insight-content"><SpendingDonut categories={categories} total={monthTotals.expenseFormatted} />
          <div className="insight-copy"><strong>{insight.status === 'warning' || insight.status === 'exceeded' ? 'A thoughtful check-in' : 'Good job!'}</strong><p>{insight.text}</p>{insight.budgetFormatted !== '—' && <span className="budget-mini">{monthTotals.expenseFormatted} of {insight.budgetFormatted}</span>}</div>
        </div>
      </article>

      <article className="panel-card balance-card">
        <div className="panel-card-head"><span className="card-overline">Total balance</span><span className="balance-mark">₹</span></div>
        <strong className="balance-amount">{totals.balanceFormatted}</strong>
        <div className="weekly-compare">
          <div><span>This week <i className={weekly.incomeUp ? 'trend-up' : 'trend-down'}>{weekly.incomeUp ? <ArrowUpRight size={13} /> : <ArrowDownRight size={13} />}</i></span><strong className="positive">{weekly.incomeFormatted}</strong><small>Income</small></div>
          <div><span>Previous week</span><strong className="positive">{weekly.previousIncomeFormatted}</strong><small>Income</small></div>
          <div><span>This week <i className={weekly.expenseUp ? 'trend-down' : 'trend-up'}>{weekly.expenseUp ? <ArrowDownRight size={13} /> : <ArrowUpRight size={13} />}</i></span><strong className="negative">{weekly.expenseFormatted}</strong><small>Expenses</small></div>
          <div><span>Previous week</span><strong className="negative">{weekly.previousExpenseFormatted}</strong><small>Expenses</small></div>
        </div>
      </article>
    </section>

    <section className="category-grid" aria-label="Category spending">
      {categoryCards.map(({ name, icon: Icon, color }) => {
        const item = cardValues.get(name);
        return <button className={`category-tile ${color}`} key={name} onClick={() => navigate(`/transactions?category=${encodeURIComponent(name)}`)}>
          <span className="category-icon"><Icon size={17} strokeWidth={2} /></span><strong>{item?.formatted || '₹0.00'}</strong><span className="category-name">{name}<ChevronRight size={13} /></span>
        </button>;
      })}
    </section>

    <section className="panel-card overview-panel">
      <div className="overview-header"><div><span className="card-overline">Overview</span><small>Income and expenses over the last six months</small></div>
        <div className="chart-legend"><span><i className="legend-income" />Income</span><span><i className="legend-expense" />Expenses</span></div>
      </div>
      <OverviewChart series={monthlySeries} />
    </section>

    <section className="dashboard-foot-stats">
      <div><span>Income to date</span><strong className="positive">{totals.incomeFormatted}</strong></div>
      <div><span>Expenses to date</span><strong className="negative">{totals.expenseFormatted}</strong></div>
      <div><span>Net savings</span><strong>{totals.savingsFormatted}</strong></div>
    </section>
  </div>;
}

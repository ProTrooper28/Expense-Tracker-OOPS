import { ArrowUpRight, ChartNoAxesCombined, PiggyBank, ReceiptText, TrendingUp } from 'lucide-react';
import { useFinance } from '../hooks/FinanceContext.jsx';
import { CategoryDonut, OverviewChart } from '../components/DashboardCharts.jsx';
import { MonthPicker, PageHeading } from '../components/common.jsx';

export default function AnalyticsPage() {
  const { analytics, selectedMonth } = useFinance();
  if (!analytics) return null;
  const { totals, categories, monthlySeries, topCategory, comparison } = analytics;
  const stats = [
    { label: 'Income this month', amount: totals.incomeFormatted, icon: TrendingUp, kind: 'positive', note: `Previous month ${comparison.previousIncomeFormatted}` },
    { label: 'Expenses this month', amount: totals.expenseFormatted, icon: ReceiptText, kind: 'negative', note: `Previous month ${comparison.previousExpenseFormatted}` },
    { label: 'Monthly savings', amount: totals.savingsFormatted, icon: PiggyBank, kind: '', note: `All-time savings ${totals.allTimeSavingsFormatted}` },
    { label: 'Highest spend', amount: topCategory, icon: ChartNoAxesCombined, kind: '', note: 'Selected month' },
  ];
  return <div className="page-content analytics-page">
    <PageHeading eyebrow="Understand your patterns" title="Analytics" subtitle="A monthly view of income, outgoings and savings." action={false} />
    <div className="analytics-month-bar"><span>Report for</span><MonthPicker /></div>
    <section className="analytics-stats">{stats.map(({ label, amount, icon: Icon, kind, note }) => <article className="panel-card analytics-stat" key={label}><span className={`summary-icon ${kind === 'positive' ? 'mint-icon' : kind === 'negative' ? 'red-icon' : 'violet-icon'}`}><Icon size={17} /></span><span>{label}</span><strong className={kind}>{amount}</strong><small>{note}</small></article>)}</section>
    <section className="panel-card analytics-trend-card"><div className="section-title"><div><span className="card-overline">Six-month comparison</span><h2>Income vs expenses</h2></div><div className="chart-legend"><span><i className="legend-income" />Income</span><span><i className="legend-expense" />Expenses</span></div></div><OverviewChart series={monthlySeries} /></section>
    <section className="analytics-lower">
      <article className="panel-card category-report"><div className="section-title"><div><span className="card-overline">Where it goes</span><h2>Spending breakdown</h2></div></div><CategoryDonut categories={categories} />
        <div className="analytics-category-list">{categories.map((item, index) => <div key={item.category}><span><i className={`category-dot dot-${index % 6}`} />{item.category}</span><strong>{item.formatted}</strong><small>{item.share.toFixed(0)}%</small></div>)}</div>
      </article>
      <article className="panel-card savings-report"><div className="section-title"><div><span className="card-overline">Keep the direction</span><h2>Savings trend</h2></div></div>
        <div className="savings-timeline">{monthlySeries.map((item) => <div className="saving-month" key={item.month}><span className={`saving-value ${item.savingsStatus}`}>{item.savingsFormatted}</span><div className={`saving-bar ${item.savingsStatus === 'negative' ? 'bar-negative' : ''}`} style={{ height: `${item.savingsBarHeight}%` }}><i /></div><span className="saving-label">{item.label}</span></div>)}</div>
        <div className="savings-foot"><span><ArrowUpRight size={14} /> Savings are calculated by C++</span><span>{selectedMonth}</span></div>
      </article>
    </section>
  </div>;
}

import { useState } from 'react';
import { AlertTriangle, CheckCircle2, Plus, Wallet } from 'lucide-react';
import { useFinance } from '../hooks/FinanceContext.jsx';
import { EmptyState, MonthPicker, PageHeading } from '../components/common.jsx';

export default function BudgetsPage() {
  const { engine, categories, budgets, selectedMonth, perform } = useFinance();
  const [category, setCategory] = useState('__monthly__');
  const [amount, setAmount] = useState('');
  const [saving, setSaving] = useState(false);
  const expenseBudgets = budgets.filter((item) => item.category !== '__monthly__');
  const overall = budgets.find((item) => item.category === '__monthly__');

  async function save(event) {
    event.preventDefault();
    if (!amount) return;
    setSaving(true);
    const result = await perform((finance) => finance.setBudget(category, selectedMonth, amount), 'Budget saved.');
    setSaving(false);
    if (result !== false) setAmount('');
  }
  const allVisible = [overall, ...expenseBudgets].filter(Boolean);

  return <div className="page-content">
    <PageHeading eyebrow="Plan with intention" title="Budgets" subtitle="Set monthly limits and keep an eye on what remains." action={false} />
    <div className="budget-controls"><div className="budget-month"><span className="eyebrow">Budget month</span><MonthPicker /></div>
      <form className="budget-form panel-card" onSubmit={save}>
        <label className="form-field"><span>Budget category</span><select value={category} onChange={(event) => setCategory(event.target.value)}><option value="__monthly__">Monthly budget</option>{categories.expenses.map((name) => <option key={name}>{name}</option>)}</select></label>
        <label className="form-field"><span>Limit (₹)</span><input inputMode="decimal" placeholder="e.g. 25000.00" value={amount} onChange={(event) => setAmount(event.target.value)} required /></label>
        <button className="button button-primary" disabled={saving}><Plus size={15} /> {saving ? 'Saving…' : 'Set budget'}</button>
      </form>
    </div>

    <section className="budget-summary-grid">
      <article className="panel-card budget-summary-card"><div className="summary-icon violet-icon"><Wallet size={18} /></div><span>Month’s spending</span><strong>{engine ? JSON.parse(engine.dashboard(selectedMonth)).monthTotals.expenseFormatted : '₹0.00'}</strong><small>{overall ? `of ${overall.limitFormatted} planned` : 'Set an overall monthly budget to track your pace.'}</small></article>
      <article className="panel-card budget-summary-card"><div className={`summary-icon ${overall?.isExceeded ? 'red-icon' : 'mint-icon'}`}>{overall?.isExceeded ? <AlertTriangle size={18} /> : <CheckCircle2 size={18} />}</div><span>Monthly status</span><strong>{overall?.isExceeded ? 'Over budget' : overall ? `${overall.usagePercentage.toFixed(0)}% used` : 'No limit yet'}</strong><small>{overall?.remainingFormatted ? `${overall.remainingFormatted} remaining` : 'Add your first budget limit.'}</small></article>
    </section>

    <section className="panel-card budget-list-panel"><div className="section-title"><div><span className="card-overline">Your limits</span><h2>Budget progress</h2></div><span className="small-muted">{selectedMonth}</span></div>
      {allVisible.length ? <div className="budget-list">{allVisible.map((budget) => {
        const isMonthly = budget.category === '__monthly__';
        return <article className="budget-row" key={budget.category}>
          <div className="budget-row-top"><span className="budget-category-icon">{isMonthly ? '₹' : budget.category.slice(0, 1)}</span><div className="budget-name"><strong>{isMonthly ? 'Monthly budget' : budget.category}</strong><small>{budget.spentFormatted} spent of {budget.limitFormatted}</small></div><div className="budget-numbers"><strong>{budget.remainingFormatted}</strong><small>remaining</small></div></div>
          <div className="progress-track"><span className={`${budget.isExceeded ? 'progress-over' : budget.isApproaching ? 'progress-warn' : ''}`} style={{ width: `${budget.barPercentage}%` }} /></div>
          <div className="budget-row-foot"><span>{budget.isExceeded ? <><AlertTriangle size={12} /> Over limit</> : budget.isApproaching ? <><AlertTriangle size={12} /> Almost at limit</> : <><CheckCircle2 size={12} /> On track</>}</span><span>{budget.usagePercentage.toFixed(0)}% used</span></div>
        </article>;
      })}</div> : <EmptyState title="No budgets set for this month" detail="Set a monthly limit or choose a category above to start tracking progress." />}
    </section>
  </div>;
}

import { useEffect, useState } from 'react';
import { X } from 'lucide-react';
import { useFinance } from '../hooks/FinanceContext.jsx';

function todayFromEngine(engine) {
  return engine?.dashboard(engine.currentMonth()) ? JSON.parse(engine.dashboard(engine.currentMonth())).currentDate : '';
}

export default function TransactionModal() {
  const { engine, categories, transactionModal, setTransactionModal, perform } = useFinance();
  const record = transactionModal?.record;
  const [form, setForm] = useState({
    title: '', amount: '', category: 'Food', type: 'Expense', date: '', description: '', paymentMethod: 'UPI',
  });
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState('');

  useEffect(() => {
    if (record) {
      setForm({
        title: record.title || '', amount: record.amountInput || '', category: record.category || 'Other',
        type: record.type || 'Expense', date: record.date || '', description: record.description || '',
        paymentMethod: record.paymentMethod || 'Other',
      });
    } else {
      setForm((previous) => ({ ...previous, date: todayFromEngine(engine) || previous.date }));
    }
  }, [record, engine]);

  const allowedCategories = form.type === 'Income' ? categories.income : categories.expenses;
  function change(name, value) {
    setForm((previous) => ({ ...previous, [name]: value,
      ...(name === 'type' ? { category: value === 'Income' ? (categories.income[0] || 'Salary') : (categories.expenses[0] || 'Food') } : {}) }));
  }
  async function submit(event) {
    event.preventDefault();
    setError('');
    if (!form.title.trim() || !form.amount || !form.category || !form.date) {
      setError('Complete the required fields before saving.');
      return;
    }
    setBusy(true);
    const operation = record
      ? (finance) => finance.updateTransaction(record.id, form.title.trim(), form.amount, form.category, form.type, form.date, form.description, form.paymentMethod)
      : (finance) => finance.addTransaction(form.title.trim(), form.amount, form.category, form.type, form.date, form.description, form.paymentMethod);
    const result = await perform(operation, record ? 'Transaction updated.' : `${form.type} added.`);
    setBusy(false);
    if (result !== false) setTransactionModal(null);
    else setError('The C++ finance engine rejected that entry. Check the amount, date and required fields.');
  }

  return <div className="modal-backdrop" role="presentation" onMouseDown={(event) => { if (event.target === event.currentTarget) setTransactionModal(null); }}>
    <section className="transaction-modal" role="dialog" aria-modal="true" aria-labelledby="transaction-modal-title">
      <div className="modal-header"><div><span className="eyebrow">{record ? 'Update a record' : 'Add to your ledger'}</span><h2 id="transaction-modal-title">{record ? 'Edit transaction' : 'New transaction'}</h2></div><button className="icon-button" onClick={() => setTransactionModal(null)} aria-label="Close dialog"><X size={18} /></button></div>
      <form onSubmit={submit}>
        <div className="type-switch" role="group" aria-label="Transaction type">
          <button type="button" className={form.type === 'Expense' ? 'selected expense' : ''} onClick={() => change('type', 'Expense')}>Expense</button>
          <button type="button" className={form.type === 'Income' ? 'selected income' : ''} onClick={() => change('type', 'Income')}>Income</button>
        </div>
        <label className="form-field"><span>Title <b>*</b></span><input autoFocus value={form.title} onChange={(event) => change('title', event.target.value)} placeholder="e.g. Monthly rent" maxLength={80} required /></label>
        <div className="form-grid">
          <label className="form-field"><span>Amount <b>*</b></span><div className="money-input"><span>₹</span><input inputMode="decimal" value={form.amount} onChange={(event) => change('amount', event.target.value)} placeholder="0.00" required /></div></label>
          <label className="form-field"><span>Category <b>*</b></span><select value={form.category} onChange={(event) => change('category', event.target.value)}>{allowedCategories.map((name) => <option key={name}>{name}</option>)}</select></label>
          <label className="form-field"><span>Date <b>*</b></span><input type="date" value={form.date} onChange={(event) => change('date', event.target.value)} required /></label>
          <label className="form-field"><span>Payment method</span><select value={form.paymentMethod} onChange={(event) => change('paymentMethod', event.target.value)}>{['UPI', 'Card', 'Cash', 'Bank transfer', 'Other'].map((name) => <option key={name}>{name}</option>)}</select></label>
        </div>
        <label className="form-field"><span>Description</span><textarea rows="3" value={form.description} onChange={(event) => change('description', event.target.value)} placeholder="Add a note (optional)" maxLength={240} /></label>
        <p className="form-footnote">Amounts are validated and stored as integer paise by the C++ finance engine.</p>
        {error && <div className="form-error" role="alert">{error}</div>}
        <div className="modal-actions"><button type="button" className="button button-quiet" onClick={() => setTransactionModal(null)}>Cancel</button><button className="button button-primary" disabled={busy}>{busy ? 'Saving…' : record ? 'Save changes' : 'Add transaction'}</button></div>
      </form>
    </section>
  </div>;
}

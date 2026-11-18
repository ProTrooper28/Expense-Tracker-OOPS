import { useMemo, useState } from 'react';
import { Download, Pencil, Search, Trash2 } from 'lucide-react';
import { useLocation } from 'react-router-dom';
import { useFinance } from '../hooks/FinanceContext.jsx';
import { EmptyState, PageHeading } from '../components/common.jsx';

const dateLabel = (date) => new Intl.DateTimeFormat('en-IN', { day: 'numeric', month: 'short', year: 'numeric' }).format(new Date(`${date}T12:00:00`));

export default function TransactionsPage() {
  const { engine, categories, revision, perform, setTransactionModal, notify } = useFinance();
  const location = useLocation();
  const queryCategory = new URLSearchParams(location.search).get('category') || 'All';
  const [search, setSearch] = useState('');
  const [category, setCategory] = useState(queryCategory);
  const [type, setType] = useState('All');
  const [from, setFrom] = useState('');
  const [to, setTo] = useState('');
  const [sortBy, setSortBy] = useState('date');
  const [sortDirection, setSortDirection] = useState('desc');
  const items = useMemo(() => engine ? JSON.parse(engine.transactions(search, category, type, from, to, sortBy, sortDirection)) : [],
    [engine, revision, search, category, type, from, to, sortBy, sortDirection]);

  async function remove(item) {
    if (!window.confirm(`Delete “${item.title}” (${item.formatted})?`)) return;
    await perform((finance) => finance.deleteTransaction(item.id), 'Transaction deleted.');
  }
  function exportCsv() {
    const data = engine.csv();
    const blob = new Blob([data], { type: 'text/csv;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const anchor = document.createElement('a');
    anchor.href = url;
    anchor.download = 'rupee-transactions.csv';
    anchor.click();
    URL.revokeObjectURL(url);
    notify('C++ transaction export downloaded.');
  }
  async function startFresh() {
    if (!window.confirm('This clears all transactions and budgets from this browser. Continue?')) return;
    await perform((finance) => finance.clearAll(), 'All records cleared. You can start fresh.');
  }

  return <div className="page-content">
    <PageHeading eyebrow="Your ledger" title="Transactions" subtitle="Search, review and manage every record." />
    <section className="panel-card history-panel">
      <div className="history-toolbar">
        <label className="search-box"><Search size={16} /><input aria-label="Search transactions" placeholder="Search title, category, note…" value={search} onChange={(event) => setSearch(event.target.value)} /></label>
        <div className="filter-row">
          <select aria-label="Filter by category" value={category} onChange={(event) => setCategory(event.target.value)}><option value="All">All categories</option>{[...categories.expenses, ...categories.income].filter((item, index, all) => all.indexOf(item) === index).map((item) => <option key={item}>{item}</option>)}</select>
          <select aria-label="Filter by type" value={type} onChange={(event) => setType(event.target.value)}><option>All</option><option>Income</option><option>Expense</option></select>
          <label className="date-filter"><span>From</span><input aria-label="Start date" type="date" value={from} onChange={(event) => setFrom(event.target.value)} /></label>
          <label className="date-filter"><span>To</span><input aria-label="End date" type="date" value={to} onChange={(event) => setTo(event.target.value)} /></label>
        </div>
      </div>
      <div className="history-subbar"><span><b>{items.length}</b> records</span><div className="history-actions"><label className="sort-label">Sort <select value={sortBy} onChange={(event) => setSortBy(event.target.value)}><option value="date">Date</option><option value="amount">Amount</option><option value="title">Title</option></select></label><button className="icon-button text-export" onClick={() => setSortDirection((value) => value === 'desc' ? 'asc' : 'desc')} title="Reverse sort order">{sortDirection === 'desc' ? 'Newest first' : 'Oldest first'}</button><button className="button button-secondary export-button" onClick={exportCsv}><Download size={15} /> Export CSV</button></div></div>
      {items.length ? <div className="table-scroll"><table className="transaction-table"><thead><tr><th>Transaction</th><th>Category</th><th>Date</th><th>Payment</th><th>Amount</th><th><span className="sr-only">Actions</span></th></tr></thead><tbody>{items.map((item) => <tr key={item.id}>
        <td><div className="transaction-title-cell"><span className={`type-badge ${item.type.toLowerCase()}`}>{item.type === 'Income' ? '+' : '−'}</span><div><strong>{item.title}</strong><small>{item.description || item.id}</small></div></div></td>
        <td><span className="category-pill">{item.category}</span></td><td>{dateLabel(item.date)}</td><td>{item.paymentMethod}</td>
        <td className={`table-amount ${item.type === 'Income' ? 'positive' : 'negative'}`}>{item.type === 'Income' ? '+' : '−'}{item.formatted}</td>
        <td><div className="row-actions"><button aria-label={`Edit ${item.title}`} className="icon-button small" onClick={() => setTransactionModal({ mode: 'edit', record: item })}><Pencil size={14} /></button><button aria-label={`Delete ${item.title}`} className="icon-button small danger-icon" onClick={() => remove(item)}><Trash2 size={14} /></button></div></td>
      </tr>)}</tbody></table></div> : <EmptyState title="No matching transactions" detail="Change the search or filters, or add a record to your ledger." />}
    </section>
    <div className="clear-data-row"><div><strong>Want to start fresh?</strong><span>Clear the demo records and budgets from this browser.</span></div><button className="button button-danger-outline" onClick={startFresh}>Clear all records</button></div>
  </div>;
}

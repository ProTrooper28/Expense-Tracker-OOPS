import { ArrowDownLeft, ArrowUpRight, Banknote, CarFront, Coffee, HeartPulse, Home, MoreHorizontal, ShoppingBag, ShoppingBasket, Wallet } from 'lucide-react';
import { useNavigate } from 'react-router-dom';
import { useState } from 'react';
import { useFinance } from '../hooks/FinanceContext.jsx';

const iconFor = (category) => {
  const icons = { Rent: Home, Transport: CarFront, Food: Coffee, Groceries: ShoppingBasket, Health: HeartPulse, Shopping: ShoppingBag, Salary: Banknote, Freelance: Wallet };
  return icons[category] || Wallet;
};

export default function RightRail() {
  const { dashboard } = useFinance();
  const navigate = useNavigate();
  const [cardExpanded, setCardExpanded] = useState(false);
  const latest = dashboard?.latestTransactions || [];
  return <aside className="right-rail">
    <div className="right-heading"><h2>Cards</h2><button className="icon-button" title="More card options" aria-label="More card options" aria-expanded={cardExpanded} onClick={() => setCardExpanded((value) => !value)}><MoreHorizontal size={18} /></button></div>
    <div className="rail-section">
      <div className="section-title"><span>My cards</span><button className="text-link" aria-expanded={cardExpanded} onClick={() => setCardExpanded((value) => !value)}>Details</button></div>
      <div className={`card-wrap ${cardExpanded ? 'card-expanded' : ''}`}>
        <div className="virtual-card" aria-label="Illustrative virtual debit card">
          <div className="card-shine" />
          <div className="card-topline"><span>RUPEE <i>PLATINUM</i></span><span className="card-chip" /></div>
          <div className="card-number">•••• &nbsp;•••• &nbsp;•••• &nbsp;4321</div>
          <div className="card-bottom"><span>TARANPREET SINGH</span><span>EXP. 05/27</span></div>
          <span className="card-network"><i /><i /></span>
        </div>
        <div className="card-back" />
        <div className="card-detail-extra">Illustrative card preview · no real card or bank connection</div>
      </div>
      <div className="card-dots" aria-hidden="true"><i className="selected" /><i /><i /></div>
    </div>

    <div className="rail-section transaction-peek">
      <div className="section-title"><span>Latest transactions</span><button className="text-link" onClick={() => navigate('/transactions')}>Show more</button></div>
      <div className="peek-list">
        {latest.length ? latest.map((item) => {
          const Icon = iconFor(item.category);
          const income = item.type === 'Income';
          return <div className="peek-row" key={item.id}>
            <div className={`merchant-icon ${income ? 'merchant-income' : `cat-${item.category.toLowerCase()}`}`}><Icon size={15} /></div>
            <div className="peek-copy"><strong>{item.title}</strong><span>{item.category} · {item.date}</span></div>
            <span className={`peek-amount ${income ? 'positive' : 'negative'}`}>{income ? '+' : '−'}{item.formatted}</span>
          </div>;
        }) : <div className="peek-empty">Your latest activity will appear here.</div>}
      </div>
    </div>
    <div className="right-rail-foot"><span className="privacy-dot" /> Private by design <span>•</span> C++ powered</div>
  </aside>;
}

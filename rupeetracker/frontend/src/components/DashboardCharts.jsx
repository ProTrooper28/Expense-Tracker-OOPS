import { Bar, CartesianGrid, Cell, ComposedChart, Line, Pie, PieChart, ResponsiveContainer, Tooltip, XAxis, YAxis } from 'recharts';

const PALETTE = ['#76e5eb', '#cc8bea', '#b8ef84', '#edc06a', '#f27c83', '#70d3a9'];
const currencyCompact = new Intl.NumberFormat('en-IN', { style: 'currency', currency: 'INR', notation: 'compact', maximumFractionDigits: 0 });

function SpendingTooltip({ active, payload }) {
  if (!active || !payload?.length) return null;
  const datum = payload[0]?.payload;
  return <div className="chart-tooltip"><span>{datum?.category}</span><strong>{datum?.formatted}</strong><small>{Number(datum?.share || 0).toFixed(0)}% of this month’s spend</small></div>;
}

function MonthlyTooltip({ active, payload }) {
  if (!active || !payload?.length) return null;
  const datum = payload[0]?.payload;
  return <div className="chart-tooltip"><span>{datum?.month}</span><div><i className="tooltip-dot income" />Income <strong>{datum?.incomeFormatted}</strong></div><div><i className="tooltip-dot expense" />Expenses <strong>{datum?.expenseFormatted}</strong></div></div>;
}

export function SpendingDonut({ categories = [], total }) {
  return <div className="donut-wrap">
    <ResponsiveContainer width="100%" height="100%">
      <PieChart>
        <Pie data={categories} dataKey="amount" nameKey="category" innerRadius={43} outerRadius={61} paddingAngle={3} stroke="none" isAnimationActive={false}>
          {categories.map((entry, index) => <Cell key={entry.category} fill={PALETTE[index % PALETTE.length]} />)}
        </Pie>
        <Tooltip content={<SpendingTooltip />} />
      </PieChart>
    </ResponsiveContainer>
    <div className="donut-center"><strong>{total || '₹0.00'}</strong><span>spent</span></div>
  </div>;
}

export function OverviewChart({ series = [] }) {
  return <div className="overview-chart">
    {series.length ? <ResponsiveContainer width="100%" height="100%">
      <ComposedChart data={series} margin={{ top: 8, right: 8, bottom: 0, left: 2 }}>
        <defs>
          <linearGradient id="incomeGlow" x1="0" y1="0" x2="0" y2="1"><stop offset="0%" stopColor="#50dcae" stopOpacity={0.48} /><stop offset="100%" stopColor="#50dcae" stopOpacity={0.02} /></linearGradient>
        </defs>
        <CartesianGrid stroke="#343538" strokeDasharray="3 5" vertical={false} />
        <XAxis dataKey="label" axisLine={false} tickLine={false} tick={{ fill: '#a3a3aa', fontSize: 11 }} dy={8} />
        <YAxis axisLine={false} tickLine={false} tick={{ fill: '#96979d', fontSize: 10 }} tickFormatter={(value) => currencyCompact.format(value)} width={58} />
        <Tooltip content={<MonthlyTooltip />} cursor={{ fill: '#ffffff08' }} />
        <Bar dataKey="income" name="Income" fill="url(#incomeGlow)" stroke="#49d9aa" strokeWidth={1} radius={[6, 6, 0, 0]} maxBarSize={17} isAnimationActive={false} />
        <Bar dataKey="expense" name="Expenses" fill="#b488d166" stroke="#c48be7" strokeWidth={0.5} radius={[6, 6, 0, 0]} maxBarSize={17} isAnimationActive={false} />
        <Line dataKey="income" name="Income trend" type="monotone" stroke="#4fe0b3" strokeWidth={2} dot={{ r: 3, fill: '#61e2b7', stroke: '#101b18', strokeWidth: 2 }} activeDot={{ r: 5 }} isAnimationActive={false} />
      </ComposedChart>
    </ResponsiveContainer> : <div className="chart-empty">Add transactions to build your monthly overview.</div>}
  </div>;
}

export function CategoryDonut({ categories = [] }) {
  return <div className="analytics-donut">
    {categories.length ? <ResponsiveContainer width="100%" height="100%"><PieChart>
      <Pie data={categories} dataKey="amount" nameKey="category" innerRadius={67} outerRadius={96} paddingAngle={3} stroke="none" isAnimationActive={false}>
        {categories.map((entry, index) => <Cell key={entry.category} fill={PALETTE[index % PALETTE.length]} />)}
      </Pie><Tooltip content={<SpendingTooltip />} />
    </PieChart></ResponsiveContainer> : <div className="chart-empty">No expenses for this month yet.</div>}
  </div>;
}

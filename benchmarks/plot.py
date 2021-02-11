import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt
#sns.set_theme(style="whitegrid")


# Initialize the matplotlib figure
#f, ax = plt.subplots(figsize=(6, 15))

# Load the example car crash dataset
#crashes = sns.load_dataset("car_crashes").sort_values("total", ascending=False)

df = pd.read_csv('all.csv')
#column_list = ['insert','erase','find','iter','destruct']
#df['sum'] = df[column_list].sum(axis=1)
#df = df.sort_values('sum', ascending=False)

#sns.set_color_codes("pastel")
#g1 = sns.barplot(x='Method', y='Ratio', data=df, hue='Library', ci=68)

df = df[df.Method != 'total']
g = sns.catplot(data=df, x='Method', y='Seconds', hue='Library', col='C', kind='bar',
                 ci=68, legend=False, col_wrap=2, sharex=False, aspect=1.6, height=3)
g.set_xlabels('')

g.add_legend(bbox_to_anchor=(0.75, 0.2), borderaxespad=0.)

g.fig.subplots_adjust(top=0.90, left=0.06, bottom=0.07)
g.fig.suptitle('Benchmark STC vs c++ std containers', fontsize=15, y=0.98)

#a1 = g.fig.axes[1]
#a1.set_title("Custom Title")

#g.despine(right=True)
#sns.despine(left=True, bottom=True)
plt.show()

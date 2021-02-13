import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt
sns.set_theme(style="whitegrid")

df = pd.read_csv('plot_perf.csv')
df = df[df.Method != 'total']

g = sns.catplot(data=df, x='Method', y='Seconds', hue='Library', col='C', kind='bar',
                 ci=68, legend=False, col_wrap=2, sharex=False, aspect=1.6, height=3)
g.set_xlabels('')

g.add_legend(bbox_to_anchor=(0.75, 0.2), borderaxespad=0.)

g.fig.subplots_adjust(top=0.90, left=0.06, bottom=0.07)
g.fig.suptitle('Benchmark STC vs c++ std containers', fontsize=15, y=0.98)

plt.show()

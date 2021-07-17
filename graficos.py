# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gds

dados = pd.read_csv("SHBOT.txt")

# Remove 0:0:0 values - no energy in RTC
dadosR = dados.drop(dados[dados.Time == '0:0:0'].index)

#dados.plot()
time = dadosR.iloc[:,0]
battery = dadosR.iloc[:,1]
dht = dadosR.iloc[:, [2,3]]
sh = dadosR.iloc[:,4]
lumns = dadosR.iloc[:,5]

       
# Comment/Uncomment to draw different graphs
"""
# Plot Bateria com Tempo
fig, ax = plt.subplots()
ax.plot(time, battery)
ax.set(xlabel='time', ylabel='Battery', title='')


#dht.plot()
#battery.plot()
#andrews_curves(dados)
"""


# AllInOne -- Subplots 
"""
fig = plt.figure(tight_layout=True)
gs = gds.GridSpec(5,2)

ax = fig.add_subplot(gs[0,:])
ax.plot(battery)
ax.set_ylabel('Bateria')


ax = fig.add_subplot(gs[1, :])
ax.plot(lumns)
ax.set_ylabel('Lumns')


ax = fig.add_subplot(gs[2, :])
ax.plot(sh)
ax.set_ylabel('sl')


ax = fig.add_subplot(gs[3, :])
ax.plot(dht.iloc[:,0])
ax.set_ylabel('ºC')



ax = fig.add_subplot(gs[4, :])
ax.plot(dht.iloc[:,1])
ax.set_ylabel('%')
"""
# Boxplots
fig = plt.figure(tight_layout=True)
gs = gds.GridSpec(2,2)

ax = fig.add_subplot(gs[0,:])

ax.boxplot(lumns, vert=False, showfliers=False, 
            patch_artist=True)
ax.title('Intensidade Luminosa')
ax.xlabel('Lumns')



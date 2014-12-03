#!/usr/bin/env python
# a bar plot with errorbars
import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cmx
import matplotlib.colors as colors
from matplotlib.font_manager import FontProperties

fontP = FontProperties()
fontP.set_size('small')

def get_cmap(N):
    '''Returns a function that maps each index in 0, 1, ... N-1 to a distinct RGB color.'''
    color_norm  = colors.Normalize(vmin=0, vmax=N-1)
    scalar_map = cmx.ScalarMappable(norm=color_norm, cmap='hsv') 
    def map_index_to_rgb_color(index):
        return scalar_map.to_rgba(index)
    return map_index_to_rgb_color

parser = argparse.ArgumentParser()
parser.add_argument("--results", help="results file")
parser.add_argument("--params", help="params file")
args = parser.parse_args()

if args.results:
    print "results file"

if args.params:
    print "params file"

with open(args.results) as f:
    data = f.read();

data = data.split('\n')
data = filter(None, data)#.remove('')

ddata={}
k_count = 0
for k,v in (item.split(':') for item in data):
    k_count += 1
    l = ddata.get(k.strip())
    if (l is None): l=[]
    l.append(float(v.strip()))
    ddata.update([(k.strip(), l)]) 


with open(args.params) as ff:
    labs = ff.read();
labs = labs.split('\n')
labs = filter(None, labs)#.remove('')
labels=[]
fix_i=0.0
for item in labs:
    item = item.split(' ')
    item = filter(None, item)
    labels.append(item[0] + 'threads \n' + item[1] + 'ops')
    fix_i=float(item[2])
N = len(labs)


ind = np.arange(N)  # the x locations for the groups
width = 0.2 # / k_count       # the width of the bars

fig, ax = plt.subplots()

rects=[]
names=[]
colours=[]
i=0
cmap = get_cmap(5)
for k,v in ddata.iteritems():
    col = cmap(i)
    tmp_rect=ax.bar(ind+width*i, v, width, color=col)
    rects.append(tmp_rect)
    colours.append(tmp_rect[0])
    names.append(k)
    i+=1


# add some text for labels, title and axes ticks
ax.set_ylabel('Msecs')
ax.set_title('Runtime graph using ' + str(fix_i*100) + '% insertions' )
ax.set_xticks(ind+width)
ax.set_xticklabels( labels )#('1 thread\n75000ops', 'G2', 'G3', 'G4', 'G5') )
for tick in ax.xaxis.get_major_ticks():
    tick.label.set_fontsize('x-small')

lgd = ax.legend(colours, names, loc='right', bbox_to_anchor=(1.4, 0.9), prop=fontP)


def autolabel(rects):
    title_font = {'size':'9'} 
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 2+height, '%d'%int(height), ha='center', va='bottom', **title_font)

for rect in rects:
    autolabel(rect)
#autolabel(rects1)
#autolabel(rects2)

#plt.show()
plt.savefig(args.results+"_"+args.params+'.png', format='png', bbox_extra_artists=(lgd,), bbox_inches='tight')

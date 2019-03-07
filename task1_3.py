#!/usr/bin/env python3
import argparse
import os
from math import *

from datetime import datetime
from pwd import getpwuid
from grp import getgrgid

parser = argparse.ArgumentParser(prog='ls')
parser.add_argument('-a', '--all', action='store_true', default=False, help='shows files starting with .')
parser.add_argument('-A', '--almostall', action='store_true', default=False, help='hides . and .. files')
parser.add_argument('-l', '--longformat', action='store_true', default=False, help='long format')
parser.add_argument('-hu', action='store_true', default=False, help='human readable')
parser.add_argument('-H', '--si', action='store_true', default=False, help='human readable converted in si')
parser.add_argument('-S', '--sortsize', action='store_true', default=False, help='sorts by size')
parser.add_argument('-t', '--sorttime', action='store_true', default=False, help='sorts by time')
parser.add_argument('-X', '--sortextension', action='store_true', default=False, help='sorts by extention')

args = parser.parse_args()
statinfo = []
measures = ' kМГТП'
access = 'rwxrwxrwx'

if (args.hu and args.si):
	print("Conflicting formats si(H) and hu")
	quit()

def form_acc(l):
	s = ''
	for i in range(9):
		if (l%2==1):
			s = s + access[8 - i]
		else:
			s = s + '-'
		l = l//2
	l = l//32 #сдвиг на бит с флагом директории
	if (l%2==1):
		s += 'd'
	else:
		s += '-'
	return s[::-1]

def form_size(l):
	n = 0
	s = ''
	if (args.hu or args.si):
		x = float(l)
		dev = args.hu*1024 + args.si*1000
		while((x//dev)!=0 and n!=5):
			x = x/dev
			n+=1
		if (float.is_integer(x)):
			s+=str(int(x))
		else:
			s+= str(round(x, 1))
		s += measures[n]
	else:
		s = str(l)
	return s
	
def bySize_key(f):
	return f[0].st_size
def byTime_key(f):
	return f[0].st_mtime
def byExt_key(f):
	return (os.path.splitext(f[1])[1])

if args.all:
	print(".\n..")

for file in os.listdir(path='.'):
	if (file[0]=='.') and not (args.almostall or args.all):
		continue
	statinfo.append((os.stat(file), file))

if (args.sortsize):
	statinfo = reversed(sorted(statinfo, key=bySize_key))
if (args.sorttime):
	statinfo = reversed(sorted(statinfo, key=byTime_key))
if (args.sortextension):
	statinfo = sorted(statinfo, key=byExt_key)

for l in statinfo:
	if(args.longformat):
		print (form_acc(l[0].st_mode), l[0].st_nlink, getpwuid(l[0].st_uid).pw_name, getgrgid(l[0].st_gid).gr_name, "%+6s" % form_size(l[0].st_size), datetime.fromtimestamp(int(l[0].st_mtime)), l[1], sep=' ', end='\n')
	else:
		print(l[1])

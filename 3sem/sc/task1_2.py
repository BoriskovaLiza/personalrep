#!/usr/bin/env python3
import argparse
import os
import random
parser = argparse.ArgumentParser(prog='password generator')
parser.add_argument('-l', '--length', action='store_true', default=False, help='length of the password')
parser.add_argument('len', type=int, nargs = '?', default=8, help='length of the password')
parser.add_argument('-ss', '--skip_symbols', action='store_true', default=False, help='skips signs in a password')
parser.add_argument('-sn', '--skip_numbers', action='store_true', default=False, help='skips digits in a password')

def generate(l, s, n):
	st = ''
	st1 = ''
	for i in range(l):
		if n:
			st1 += str(random.randint(0,9))
		if s:
			st1 += random.choice("!_'/:;+=-$#@%")
		st1 += chr(random.randint(ord('A'),ord('Z')))
		st1 += chr(random.randint(ord('a'),ord('z')))
		st = st + random.choice(st1)
	print(st);

args = parser.parse_args()
a = args.len

generate(a, not args.skip_symbols, not args.skip_numbers)


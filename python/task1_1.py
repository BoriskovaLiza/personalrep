#!/usr/bin/env python3
import random
no_s = "_"
cross = "X"
nough = "O"
wins = 0
loses = 0
field = [no_s, no_s, no_s, no_s, no_s, no_s, ' ', ' ', ' ']
def do_beauty(f):
	for i in range(len(f)):
		if (i+1)%3==0:
			print(f[i],end='\n')
		else:
			print(f[i],end='|')
def do_keyboard():
	print("\nTap any number to hit an empty field:")
	for i in range(9):
		if (i+1)%3==0:
			print(i+1,end='\n')
		else:
			print(i+1,end='.')

def rever(role):
	if role == cross:
		return nough
	if role == nough:
		return cross

def is_winner(f,char):
	if f[0]==f[1]==f[2]==char or f[4]==f[5]==f[3]==char or f[6]==f[7]==f[8]==char or f[0]==f[3]==f[6]==char or f[1]==f[4]==f[7]==char or f[2]==f[5]==f[8]==char or f[0]==f[4]==f[8]==char or f[2]==f[4]==f[6]==char:
		return True
	else:
		return False

def field_full(f):
	return(not f[0]==no_s and not f[1]==no_s and not f[2]==no_s and not f[3]==no_s and not f[4]==no_s and not f[5]==no_s and not f[6]==' ' and not f[7]==' ' and not f[8]==' ')

def steps(f, turn, role):
	while(not is_winner(f, role) and not is_winner(f, rever(role)) and not field_full(f)):
		if turn==False:
			comp_step(f, role)
			turn = True
		elif turn==True:
			step(f, role)
			turn = False
	if is_winner(f, role):
		do_beauty(f)
		print("You won!\n")
		global wins
		wins+=1
		return
	if is_winner(f, rever(role)):
		do_beauty(f)
		print("You lost :c\n")
		global loses
		loses+=1
		return
	if field_full(f):
		print("Nice game!")	
		return
	 
def step(f, role):
	print("\nYour turn!")
	while(True):
		do_beauty(f)
		do_keyboard()
		target = int(input())
		if (target<1 or target>9):
			print("Try again")
			continue
		elif (f[target-1]==cross or f[target-1]==nough):
			print("This place is taken, try again")
			continue
		else:
			f[target-1] = role
			return
def comp_step(f, role):
	while(True):
		target = random.randint(0,8)
		if (f[target]==cross or f[target]==nough):
			continue
		else:
			f[target] = rever(role)
			return
	
def game():
	while(True):	
		print("Current tic-tac-toe score:", wins, ":", loses, "\nPress n to play by noughts and c to play by crosses.\nPress x to exit.")
		char = input()		
		if char=='x':
			return 0
		elif char=='c':
			cur_field = field[:]
			steps(cur_field, True, cross)
		elif char=='n':
			cur_field = field[:]
			steps(cur_field, False, nough)
		else:
			print("Could you repeat that please?\n")
			continue
		print("Wanna play again?[y/n]")
		if input()=='n':
			return 0
game()

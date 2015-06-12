f = open('temp.txt','r')  
for line in open('temp.txt'):  
	line = f.readline()
	a = line.split(' ')
	if (a[1][len(a[1])-1] == '\n'):
		a[1] = a[1][0:len(a[1])-1]
	if (a[0] == 'intsig'):
		print '\tint ', a[1]+';'
	else:
		print '\tbool ', a[1]+';'

f.close()                  

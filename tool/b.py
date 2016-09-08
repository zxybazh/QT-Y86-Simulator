f = open('temp.txt','r')  
for line in open('temp.txt'):  
	line = f.readline()
	a = line.split(' ')
	if (a[1][len(a[1])-1] == '\n'):
		a[1] = a[1][0:len(a[1])-1]
	if (a[0] == 'intsig'):
		print '\t'+a[1]+'= 0;'
	else:
		print '\t'+a[1]+'= 0;'

f.close()                  

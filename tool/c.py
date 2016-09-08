f = open('tt.txt','r')  
for line in open('tt.txt'):  
	line = f.readline()
	a = line.split(' ')
	print '\t'+a[2][0:len(a[2])-2]+' = 0;'

f.close()                  

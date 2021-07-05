# 
#	Christopher Fuentes
#	cfuentes@g.harvard.edu
#	HUID 60857326
#
# 	Makefile for Project 2 (pfind)
# 	Builds pfind tool, a simplified version of the find tool.
#
#	Formulas:
#	make 		=> builds pfind
#	make pfind 	=> builds pfind
#	make clean 	=> cleans pwd of object files and the pfind executable
#

pfind: pfind.c
	$(CC) -Wall pfind_utils.c pfind.c -o pfind

clean:
	rm -f *.o pfind
	
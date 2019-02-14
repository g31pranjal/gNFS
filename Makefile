
all :
	cd proto/ && make 
	cd utils/ && make
	cd server/ && make 
	cd client/ && make 

schema :
	cd proto/ && make 

utils :
	cd utils/ && make

server :
	cd server/ && make 

client :
	cd client/ && make 


clean :
	cd proto/ && make clean
	cd utils/ && make clean
	cd server/ && make clean
	cd client/ && make clean

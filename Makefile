all:
		g++ server_proba.cpp -o server
		g++ client.cpp -o client

clean: 
		rm -f client server
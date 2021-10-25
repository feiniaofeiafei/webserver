server:  
	g++ -std=c++11 -I/usr/include/mysql main.cpp ./config/config.cpp ./log/block_queue.h ./mysql/sql_connection_pool.cpp -o server -O2 -L/usr/lib/mysql -lmysqlclient
clean:
	rm -rf server
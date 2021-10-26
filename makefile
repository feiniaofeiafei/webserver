server:  
	g++ -std=c++11 -I/usr/include/mysql main.cpp ./config/config.cpp ./webserver/webserver.cpp ./http/http_conn.cpp ./threadpool/threadpool.h ./mysql/sql_connection_pool.cpp -o server -O2 -L/usr/lib/mysql -lmysqlclient -lpthread
clean:
	rm -rf server
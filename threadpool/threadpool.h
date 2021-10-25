#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include "./../mysql/sql_connection_pool.h"
using namespace std;

template <typename T>
class thread_pool{
public:
    thread_pool(int actor_model, mysql_connection_pool* conn_pool, int thread_number = 8, int max_request = 100000);
    ~thread_pool();
    bool append(T* request, int state);
    bool append_p(T* request);

private:
    // 真正工作线程运行的函数，不断从请求队列m_workqueue中取出任务并执行
    static void* worker(void* arg);
    void run();

private:
    int m_thread_number;                    // 线程数
    int m_max_request;                      // 请求队列中允许的最大请求书
    pthread_t* m_threads;                   // 线程池，本质就为一个数组
    std::list<T*> m_workqueue;              // 请求队列，存放的是一个个请求
    mysql_connection_pool* m_conn_pool;     // 数据库链接
    int m_actor_model;                      // 事件处理模型 Reactor/Proactor
                                            // TODO 锁机制
};

// 往线程池中创建thread_number个线程，默认为8
template<typename T>
thread_pool<T>::thread_pool(int actor_model, mysql_connection_pool* conn_pool, int thread_number, int max_request): m_actor_model(actor_model), m_conn_pool(conn_pool), m_thread_number(thread_number), m_max_request(max_request), m_threads(nullptr){
    if(thread_number <= 0 || max_request <= 0){
        throw exception();
    }
    m_threads = new pthread_t[m_thread_number];
    if(!m_threads){
        throw exception();
    }
    for(int i = 0; i < thread_number; i++){
        if(pthread_create(m_threads + i, nullptr, worker, this) != 0){
            delete[] m_threads;
            throw exception();
        }
        if(pthread_detach(m_threads[i])){
            delete[] m_threads;
            throw exception();
        }
    }
}

template<typename T>
thread_pool<T>::~thread_pool(){
    delete[] m_threads;
}

template<typename T>
bool thread_pool<T>::append(T* request, int state){
    // 加锁
    if(this -> m_workqueue.size() >= m_max_request){
        // 解锁
        return false;
    }
    request -> m_state = state;
    m_workqueue.push_back(request);
    return true;
}

template<typename T>
bool thread_pool<T>::append_p(T* request){
    // 加锁
    if(this -> m_workqueue.size() >= m_max_request){
        // 解锁
        return false;
    }
    m_workqueue.push_back(request);
    // 正常解锁
    return true;
}

template<typename T>
void* thread_pool<T>::worker(void* arg){
    thread_pool* pool = (thread_pool*) arg;
    pool -> run();
    return pool;
}

template<typename T>
void thread_pool<T>::run(){
    while(true){
        if(m_workqueue.empty()){
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        if(!request){
            continue;
        }
        if(1 == m_actor_model){
            // if(0 == request -> m_state)}{
            //     if(request -> read_once()){

            //     }else{

            //     }
            // }
        }else{
            connectionRAII mysqlcon(&request -> mysql, m_conn_pool);
            request -> process();
        }
    }
}

#endif

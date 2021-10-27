#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "./../mysql/sql_connection_pool.h"
#include "../lock/lock.h"

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
    sem m_queuestat;                        // 是否有任务需要处理
    locker m_queuelocker;                   // 保护请求队列的互斥锁
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
    m_queuelocker.lock();
    if(this -> m_workqueue.size() >= m_max_request){
        m_queuelocker.unlock();
        return false;
    }
    request -> m_state = state;
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
bool thread_pool<T>::append_p(T* request){
    m_queuelocker.lock();
    if(this -> m_workqueue.size() >= m_max_request){
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
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
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if(!request){
            continue;
        }
        if(1 == m_actor_model){
            // // 读
            // if(0 == request -> m_state)}{
            //     if(request -> read_once()){

            //     }else{

            //     }
            // }// 写
            // else{

            // }
        }else{
            // connectionRAII mysqlcon(&request -> sql, m_conn_pool);
            // request -> process();
        }
    }
}

#endif

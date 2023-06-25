#pragma once
#include <map>
#include <list>
#include <thread>
#include <mutex>
#include <bits/stdc++.h>

template<typename Key, typename Value>
struct IConsumer
{
    virtual void Consume(Key id, const Value &value)
    {
        id;
        value;
    }
    virtual ~IConsumer() = default;
};

#define MaxCapacity 1000

template<typename Key, typename Value>
class MultiQueueProcessor
{
public:
    MultiQueueProcessor() :
        running{ true },
        th(std::bind(&MultiQueueProcessor::Process, this)) {}

    ~MultiQueueProcessor()
    {
        StopProcessing();
        cv.notify_one();
        th.join();
    }

    void StopProcessing()
    {
        running = false;
    }

    void Subscribe(Key id, IConsumer<Key, Value> * consumer)
    {
        std::lock_guard<std::mutex> lock{ ctx };
        auto iter = consumers.find(id);
        if (iter == consumers.end())
        {
            consumers.insert(std::make_pair(id, consumer));
        }
    }

    void Unsubscribe(Key id)
    {
        std::lock_guard<std::mutex> lock{ ctx };
        auto iter = consumers.find(id);
        if (iter != consumers.end())
            consumers.erase(id);
    }

    bool Enqueue(Key id, Value value)
    {
        std::lock_guard<std::mutex> lock{ mtx };
        auto iter = queues.find(id);
        if (iter != queues.end())
        {
            if (iter->second.size() < MaxCapacity)
                iter->second.push_back(value);
            else
                return false;
        }
        else
        {
            queues.insert(std::make_pair(id, std::list<Value>()));
            iter = queues.find(id);
            if (iter != queues.end())
            {
                if (iter->second.size() < MaxCapacity)
                    iter->second.push_back(value);
                else
                    return false;
            }
        }
        cv.notify_one();

        return true;
    }

protected:
    Value Dequeue(Key id)
    {
        auto iter = queues.find(id);
        if (iter != queues.end())
        {
            if (iter->second.size() > 0)
            {
                auto front = iter->second.front();
                iter->second.pop_front();
                return front;
            }
        }
        return Value{};
    }

    void Process()
    {
        while (running)
        {
            std::unique_lock lock{ mtx };
            cv.wait(lock, [&](){
                return !queues.empty() || running;
            });

            for (auto iter = queues.begin(); iter != queues.end(); ++iter)
            {
                std::lock_guard<std::mutex> lock{ ctx };
                auto consumerIter = consumers.find(iter->first);
                if (consumerIter != consumers.end())
                {
                    Value front = Dequeue(iter->first);
                    mtx.unlock();

                    if (front != Value{})
                    {
                        consumerIter->second->Consume(iter->first, front);
                    }

                    mtx.lock();
                }
            }
        }
    }

protected:
    std::map<Key, IConsumer<Key, Value> *> consumers;
    std::map<Key, std::list<Value>> queues;

    std::atomic_bool running;
    std::mutex mtx;
    std::mutex ctx;

    std::thread th;

    std::condition_variable cv;
};
#include <iostream>
#include <memory>
#include <bits/stdc++.h>

#include "MultiQueueProcessor.h"

using namespace std;

template<typename Key, typename Value>
class Consumer : public IConsumer<Key, Value>
{
public:
    void Consume(Key id, const Value &value) override
	{
		std::cout << id << endl;
		std::cout << value << endl;
	}
};

int main(int argc, const char* argv[])
{

    auto c1 = new Consumer<std::string, std::string>();
    MultiQueueProcessor<std::string, std::string> mp;

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 100; i++)
    {
        std::string key = "MyKey" + to_string(i);
        std::string value = "MyValue" + to_string(i);

        mp.Subscribe(key, c1);
        mp.Enqueue(key, value);
    }
    this_thread::sleep_for(10us);
    for(int i = 0; i < 100; i++)
    {
        std::string key = "MyKey" + to_string(i);
        mp.Unsubscribe(key);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    //std::cout << duration.count() << endl;
}

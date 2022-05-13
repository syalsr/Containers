#include <iostream>
#include <algorithm>
#include <string>
#include <time.h>
#include <chrono>
#include <vector>

#include "vector.hpp"

using namespace std::chrono;


class LogDuration {
public:
    explicit LogDuration(const std::string& msg = "")
            : message(msg + ": ")
            , start(steady_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = steady_clock::now();
        auto dur = finish - start;
        std::cerr << message
             << duration_cast<milliseconds>(dur).count()
             << " ms" << std::endl;
    }
private:
    std::string message;
    steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};

void test()
{
    vector<int> v(10);
    for(int i = 0; i < 100; ++i)
        v.push_back(i);
    vector<int> vv{v};
    vector<int> vvv;
    vvv = v;

    vector<int> vvvv(2);
    for(int i = 50; i > 0; --i)
        v.push_back(i);
    for(auto&& i : v)
        std::cout << i;
    std::sort(v.begin(), v.end());
    for(auto&& i : v)
        std::cout << i;
}

void timt()
{
    std::size_t size = 1000000000;
    {
        LOG_DURATION("vec");
        std::vector<int> v;
        for(int i = 0; i < size; ++i)
        {
            v.push_back(i);
        }
    }
    {
        LOG_DURATION("my vec");
        vector<int> v;
        for(int i = 0; i < size; ++i)
        {
            v.push_back(i);
        }
    }
}

int main() {
    vector<int> d{2,3};
    //timt();
    vector<int> v(5);
    v[0]=0;
    v[1]=1;
    v[2]=2;
    for(auto&& i : v)
    {
        std::cout << i;
    }

    Iterator first{v.data()};
    Iterator last{v.data()+v.size()};
    vector<int> vv{first, last};
    for(auto&& i : vv)
    {
        std::cout << i;
    }
    return 0;
}

#include <map>
#include <string>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

int main( int argc, char** argv )
{
    if (argc != 3)
        return (0);
    std::map<int, std::string>  mptest;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::size_t max = strtol(argv[2], NULL, 10);
    std::uniform_int_distribution<> distr(3, max * 8);
    while (mptest.size() < max) {
        mptest.insert(std::make_pair(distr(gen), "hallo"));
    }
    std::cout << "size of map: " << mptest.size() << std::endl;
    int foundtestsize = strtol(argv[1], NULL, 10);
    // std::vector<std::chrono::nanoseconds>   nanosecvec(foundtestsize);
    std::vector<long long>   nanosecvec(foundtestsize);
    long long nanosecges = 0;
    for (int i = 0; i != foundtestsize; ++i) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::map<int, std::string>::iterator it = mptest.find(distr(gen));
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        nanosecges += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
        // nanosecvec[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    }

    std::cout << "search iterations: " << foundtestsize << std::endl;
    std::cout << "Time difference = " << nanosecges / foundtestsize << "[ns]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << "[ns]" << std::endl;
    return (0);
}
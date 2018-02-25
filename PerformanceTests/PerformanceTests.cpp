// PerformanceTests.cpp : Defines the entry point for the console application.
//

// Example program
#include <iostream>
#include <string>
#include <variant>
#include <chrono>
#include <vector>
#include <numeric>
#include <functional>
#include <iomanip>
#include <array>

#define EngineTypes int, float, std::string

template <typename TTag, typename... TSupportedTypes>
struct Backend {};

struct Tag0_t {};
struct Tag1_t {};

template <>
struct Backend<Tag0_t, EngineTypes> {
  void foo(int *p) { int k = 0; k += 23232; *p = k; }
};

template <>
struct Backend<Tag1_t, EngineTypes> {
  void foo(int *p) { int k = 0; k += 23232; *p = k; }    
};

using BackendTypes = std::variant<Backend<Tag0_t, EngineTypes>, Backend<Tag1_t, EngineTypes>>;

template <typename... TSupportedTypes>
class BaseBackend { 
public:
  virtual void foo(int *p) = 0;
};

class Backend0 : public BaseBackend<EngineTypes> {
public:
  inline void foo(int *p) { int k = 0; k += 23232; *p = k; }
};
class Backend1 : public BaseBackend<EngineTypes> {
public:
  inline void foo(int *p) { int k = 0; k += 23232; *p = k; }
};

int main()
{ 
  std::vector<double> variantCounts;
  std::vector<double> virtualCounts0;
  std::vector<double> virtualCounts1;
  std::vector<double> staticCounts;

  Backend<Tag0_t, EngineTypes> b0;
  Backend<Tag1_t, EngineTypes> b1;

  BackendTypes backends(b0);
  Backend0 be0;
  std::array<BaseBackend<EngineTypes>*, 2> p;
  p[0] = new Backend0();
  p[1] = new Backend1();

  int m = 0;

  for(unsigned int k=0; k < 1000000; ++k) {
    std::chrono::time_point<std::chrono::high_resolution_clock> first = std::chrono::high_resolution_clock::now();
    std::get<Backend<Tag0_t, EngineTypes>>(backends).foo(&m);
    std::chrono::time_point<std::chrono::high_resolution_clock> second = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = second - first;
    variantCounts.push_back(diff.count());

    std::chrono::time_point<std::chrono::high_resolution_clock> first0 = std::chrono::high_resolution_clock::now();
    p[0]->foo(&m);
    std::chrono::time_point<std::chrono::high_resolution_clock> second0 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff0 = second0 - first0;
    virtualCounts0.push_back(diff0.count());


    std::chrono::time_point<std::chrono::high_resolution_clock> first1 = std::chrono::high_resolution_clock::now();
    p[1]->foo(&m);
    std::chrono::time_point<std::chrono::high_resolution_clock> second1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff1 = second1 - first1;
    virtualCounts1.push_back(diff1.count());

    std::chrono::time_point<std::chrono::high_resolution_clock> first2 = std::chrono::high_resolution_clock::now();
    be0.foo(&m);
    std::chrono::time_point<std::chrono::high_resolution_clock> second2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff2 = second2 - first2;
    staticCounts.push_back(diff2.count());
  }

  std::function<double(std::vector<double> const&)> acc 
    = [] (std::vector<double> const&v) {
    double r = 0.0;
    for(double const& d : v)
      r += d;

    return r;
  };
  std::cout << m << std::endl;

  double value0 = std::accumulate(variantCounts.begin(), variantCounts.end(), 0.0);
  double value1 = std::accumulate(virtualCounts0.begin(), virtualCounts0.end(), 0.0);
  double value2 = std::accumulate(virtualCounts1.begin(), virtualCounts1.end(), 0.0);
  double value3 = std::accumulate(staticCounts.begin(), staticCounts.end(), 0.0);

  std::cout << "Average std::variant call took "     << std::fixed << std::setprecision(25) << (value0 / variantCounts.size()) << " seconds" << std::endl;
  std::cout << "Average virtual function call first object took " << std::fixed << std::setprecision(25) << (value1 / virtualCounts0.size()) << " seconds" << std::endl;
  std::cout << "Average virtual function call second object took " << std::fixed << std::setprecision(25) << (value2 / virtualCounts1.size()) << " seconds" << std::endl;
  std::cout << "Average static function call took "  << std::fixed << std::setprecision(25) << (value3 / staticCounts.size())  << " seconds" << std::endl;

  system("PAUSE");
}


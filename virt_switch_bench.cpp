#include <random>

struct Base {
  virtual int GetN() = 0;
};

struct Foo1 : public Base {
  int GetN() override {
    return 1;
  }
};

struct Foo2 : public Base {
  int GetN() override {
    return 2;
  }
};

struct Foo3 : public Base {
  int GetN() override {
    return 3;
  }
};


static void Virtual(benchmark::State& state) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(0,2);

  std::array<Base*, 1000> arr;
  for (size_t i = 0; i < arr.size(); i++) {
    switch(dist6(rng)) {
      case 0:
        arr[i] = new Foo1;
        break;
      case 1:
        arr[i] = new Foo2;
        break;
      case 2:
        arr[i] = new Foo3;
        break;
    }
  }

  for (auto _ : state) {
    for (size_t i = 0; i < arr.size(); i++) {
      int res = arr[i]->GetN();
      benchmark::DoNotOptimize(res);
    }
    
  }
}
BENCHMARK(Virtual);

struct TestSwitch {
  int GetN(int n) {
    switch(n) {
      case 0:
        return 1;
      case 1:
        return 2;
      case 2:
        return 3;
    }
  }
};


static void Switch(benchmark::State& state) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(0,2);

  std::array<int, 1000> arr;
  for (size_t i = 0; i < arr.size(); i++) {
    arr[i] = dist6(rng);
  }

  for (auto _ : state) {
    for (size_t i = 0; i < arr.size(); i++) {
      int res = TestSwitch().GetN(arr[i]);
      benchmark::DoNotOptimize(res);
    }
  }
}
BENCHMARK(Switch);

#include <bitset>
#include <cstdlib>
#include <iostream>
#include <ctime>

// Types for hold list of used registers:
template<size_t N>
class RegMaskImpl : public std::bitset<N> {
public:
    class RegIterator;

    using Self = RegMaskImpl<N>;
    using Base = std::bitset<N>;

    using Base::Base;

    RegMaskImpl(Base b) : Base(b) {}

    Self Union(const Self& other) {
        return Self::to_ullong() & other.to_ullong();
    }

    auto begin()
    {
        return RegIterator(*this, 0);
    }
    auto end()
    {
        return RegIterator(*this, Self::size());
    }
};

using RegMask = RegMaskImpl<32>;

template<size_t N>
class RegMaskImpl<N>::RegIterator {
public:
    RegIterator(RegMaskImpl<N> mask, size_t pos) : mask_(mask), position_(pos) {}

    bool operator==(const RegIterator &rhs) const
    {
        return mask_ == rhs.mask_ && position_ == rhs.position_;
    }

    bool operator!=(const RegIterator &rhs) const
    {
        return !(*this == rhs);
    }

    RegIterator &operator++()
    {
        for (position_++; (mask_.to_ullong() & (1UL << position_)) == 0 && position_ != N; position_++) {
        }
        return *this;
    }

    uint32_t operator*()
    {
        return position_;
    }
private:
    RegMaskImpl<N> mask_;
    size_t position_{0};
};


static void regmask1(benchmark::State& state) {
  std::srand(std::time(nullptr)); // use current time as seed for random generator
  uint32_t random_variable = std::rand();
  RegMask mask{random_variable};
  for (auto _ : state) {
    int n = 0;
    for (auto reg : mask) {
        n += reg;
    }

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(n);
  }
}
// Register the function as a benchmark
BENCHMARK(regmask1);

static void regmask2(benchmark::State& state) {
  std::srand(std::time(nullptr)); // use current time as seed for random generator
  uint32_t random_variable = std::rand();
  RegMask mask{random_variable};
  for (auto _ : state) {
    int n = 0;
    for (int i = 0; i < mask.size(); i++) {
        if (mask[i])
           n += i;
    }

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(n);
  }
}
// Register the function as a benchmark
BENCHMARK(regmask2);

/*
 * Duncan Horn
 */

#include <benchmark/benchmark.h>
#include <dhorn/math/vector.h>

#define DECLARE_TEST_SIZE_TYPE(Name, Size, Type) \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<Type, Size, dhorn::math::nointrin_traits<Type>>); \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<Type, Size, dhorn::math::sse2_traits<Type>>);

#define DECLARE_TEST_SIZE(Name, Size) \
DECLARE_TEST_SIZE_TYPE(Name, Size, char) \
DECLARE_TEST_SIZE_TYPE(Name, Size, short) \
DECLARE_TEST_SIZE_TYPE(Name, Size, std::int32_t) \
DECLARE_TEST_SIZE_TYPE(Name, Size, std::int64_t) \
DECLARE_TEST_SIZE_TYPE(Name, Size, float) \
DECLARE_TEST_SIZE_TYPE(Name, Size, double)

#define DECLARE_TEST(Name) \
DECLARE_TEST_SIZE(Name, 2) \
DECLARE_TEST_SIZE(Name, 3) \
DECLARE_TEST_SIZE(Name, 4) \
DECLARE_TEST_SIZE(Name, 5) \
DECLARE_TEST_SIZE(Name, 6) \
DECLARE_TEST_SIZE(Name, 7) \
DECLARE_TEST_SIZE(Name, 8)

#define DECLARE_TEST_SIZE_TYPE_ARGS(Name, Size, Type, ...) \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<Type, Size, dhorn::math::nointrin_traits<Type>>)->Args({ __VA_ARGS__ }); \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<Type, Size, dhorn::math::sse2_traits<Type>>)->Args({ __VA_ARGS__ });

#define DECLARE_TEST_SIZE_ARGS(Name, Size, ...) \
DECLARE_TEST_SIZE_TYPE_ARGS(Name, Size, char, __VA_ARGS__) \
DECLARE_TEST_SIZE_TYPE_ARGS(Name, Size, short, __VA_ARGS__) \
DECLARE_TEST_SIZE_TYPE_ARGS(Name, Size, std::int32_t, __VA_ARGS__) \
DECLARE_TEST_SIZE_TYPE_ARGS(Name, Size, std::int64_t, __VA_ARGS__) \
DECLARE_TEST_SIZE_TYPE_ARGS(Name, Size, float, __VA_ARGS__) \
DECLARE_TEST_SIZE_TYPE_ARGS(Name, Size, double, __VA_ARGS__)

#define DECLARE_TEST_ARGS(Name, ...) \
DECLARE_TEST_SIZE_ARGS(Name, 2, __VA_ARGS__) \
DECLARE_TEST_SIZE_ARGS(Name, 3, __VA_ARGS__) \
DECLARE_TEST_SIZE_ARGS(Name, 4, __VA_ARGS__) \
DECLARE_TEST_SIZE_ARGS(Name, 5, __VA_ARGS__) \
DECLARE_TEST_SIZE_ARGS(Name, 6, __VA_ARGS__) \
DECLARE_TEST_SIZE_ARGS(Name, 7, __VA_ARGS__) \
DECLARE_TEST_SIZE_ARGS(Name, 8, __VA_ARGS__)

template <typename VectorType>
void Vector_DefaultConstruction(benchmark::State& state)
{
    for (auto _ : state)
    {
        // Should NOP in all cases
        VectorType v;
        benchmark::DoNotOptimize(v);
    }
}
DECLARE_TEST(Vector_DefaultConstruction);



template <typename VectorType, std::size_t... Values>
VectorType Vector_ConstructConstantValues_Helper(std::index_sequence<Values...>)
{
    return VectorType(static_cast<typename VectorType::value_type>(Values)...);
}

template <typename VectorType>
void Vector_ConstructConstantValues(benchmark::State& state)
{
    for (auto _ : state)
    {
        const auto v = Vector_ConstructConstantValues_Helper<VectorType>(std::make_index_sequence<VectorType::dimensions>{});
        benchmark::DoNotOptimize(v);
    }
}
DECLARE_TEST(Vector_ConstructConstantValues);

template <typename VectorType, std::size_t... Indices>
VectorType Vector_ConstructNonConstantValues_Helper(std::index_sequence<Indices...>)
{
    static volatile typename VectorType::value_type values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    return VectorType(values[Indices]...);
}

template <typename VectorType>
void Vector_ConstructNonConstantValues(benchmark::State& state)
{
    for (auto _ : state)
    {
        const auto v = Vector_ConstructNonConstantValues_Helper<VectorType>(std::make_index_sequence<VectorType::dimensions>{});
        benchmark::DoNotOptimize(v);
    }
}
DECLARE_TEST(Vector_ConstructNonConstantValues);



template <typename VectorType>
void Vector_Zero(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(VectorType::zero());
    }
}
DECLARE_TEST(Vector_Zero);



template <typename VectorType>
void Vector_SplatConstantValue(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(VectorType::splat(1));
    }
}
DECLARE_TEST(Vector_SplatConstantValue);

template <typename VectorType>
void Vector_SplatNonConstantValue(benchmark::State& state)
{
    const auto value = static_cast<typename VectorType::value_type>(state.range(0));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(VectorType::splat(value));
    }
}
DECLARE_TEST_ARGS(Vector_SplatNonConstantValue, 42);



//template <typename VectorType>
//void Vector_Negate(benchmark::State& state)
//{
//    auto v = VectorType::splat(static_cast<typename VectorType::value_type>(1));
//    for (auto _ : state)
//    {
//        v = -v;
//    }
//
//    benchmark::DoNotOptimize(v);
//}
//DECLARE_TEST(Vector_Negate);

template <typename VectorType>
void Vector_Addition(benchmark::State& state)
{
    const auto c = VectorType::splat(static_cast<typename VectorType::value_type>(1));
    //auto v = VectorType::splat(static_cast<typename VectorType::value_type>(1));
    auto v = Vector_ConstructConstantValues_Helper<VectorType>(std::make_index_sequence<VectorType::dimensions>{});
    for (auto _ : state)
    {
        v += c;
    }

    benchmark::DoNotOptimize(v);
}
DECLARE_TEST(Vector_Addition);

template <typename VectorType>
void Vector_Subtraction(benchmark::State& state)
{
    const auto c = VectorType::splat(static_cast<typename VectorType::value_type>(1));
    auto v = VectorType::splat(static_cast<typename VectorType::value_type>(1));
    for (auto _ : state)
    {
        v -= c;
    }

    benchmark::DoNotOptimize(v);
}
DECLARE_TEST(Vector_Subtraction);

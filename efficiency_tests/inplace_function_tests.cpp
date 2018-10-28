/*
 * Duncan Horn
 */

#include <benchmark/benchmark.h>
#include <dhorn/inplace_function.h>

struct test_object
{
    int value = 42;
    int get_value() { return value; }
};



template <typename Func, typename... Args>
void TestInvoke(benchmark::State& state, Func func, Args... args)
{
    for ([[maybe_unused]] auto _ : state)
    {
        benchmark::DoNotOptimize(std::invoke(func, args...));
    }
}



void Strlen_FunctionPointerTest(benchmark::State& state)
{
    TestInvoke(state, std::strlen, "");
}
BENCHMARK(Strlen_FunctionPointerTest);

void Strlen_StdFunctionTest(benchmark::State& state)
{
    std::function<std::size_t(const char*)> func(std::strlen);
    TestInvoke(state, func, "");
}
BENCHMARK(Strlen_StdFunctionTest);

void Strlen_InplaceFunctionTest(benchmark::State& state)
{
    dhorn::inplace_function<std::size_t(const char*)> func(std::strlen);
    TestInvoke(state, func, "");
}
BENCHMARK(Strlen_InplaceFunctionTest);



void MemberFunction_FunctionPointerTest(benchmark::State& state)
{
    test_object o;
    TestInvoke(state, &test_object::get_value, o);
}
BENCHMARK(MemberFunction_FunctionPointerTest);

void MemberFunction_StdFunctionTest(benchmark::State& state)
{
    test_object o;
    std::function<int(test_object&)> func(&test_object::get_value);
    TestInvoke(state, func, o);
}
BENCHMARK(MemberFunction_StdFunctionTest);

void MemberFunction_InplaceFunctionTest(benchmark::State& state)
{
    test_object o;
    dhorn::inplace_function<int(test_object&)> func(&test_object::get_value);
    TestInvoke(state, func, o);
}
BENCHMARK(MemberFunction_InplaceFunctionTest);



void Member_PointerTest(benchmark::State& state)
{
    test_object o;
    TestInvoke(state, &test_object::value, o);
}
BENCHMARK(Member_PointerTest);

void Member_StdFunctionTest(benchmark::State& state)
{
    test_object o;
    std::function<int(test_object&)> func(&test_object::value);
    TestInvoke(state, func, o);
}
BENCHMARK(Member_StdFunctionTest);

void Member_InplaceFunctionTest(benchmark::State& state)
{
    test_object o;
    dhorn::inplace_function<int(test_object&)> func(&test_object::value);
    TestInvoke(state, func, o);
}
BENCHMARK(Member_InplaceFunctionTest);

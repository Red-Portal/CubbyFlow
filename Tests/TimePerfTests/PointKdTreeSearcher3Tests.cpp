#include "benchmark/benchmark.h"

#include <Core/Array/Array1.h>
#include <Core/Searcher/PointKdTreeSearcher3.h>
#include <Core/Vector/Vector3.h>

#include <random>

using CubbyFlow::Array1;
using CubbyFlow::Vector3D;

class PointKdTreeSearcher3 : public ::benchmark::Fixture
{
protected:
    std::mt19937 rng{ 0 };
    std::uniform_real_distribution<> dist{ 0.0, 1.0 };
    Array1<Vector3D> points;

    void SetUp(const ::benchmark::State& state)
    {
        const int N = state.range(0);

        points.Clear();
        for (int i = 0; i < N; ++i)
        {
            points.Append(MakeVec());
        }
    }

    Vector3D MakeVec()
    {
        return Vector3D(dist(rng), dist(rng), dist(rng));
    }
};

BENCHMARK_DEFINE_F(PointKdTreeSearcher3, Build)(benchmark::State& state)
{
    while (state.KeepRunning())
    {
        CubbyFlow::PointKdTreeSearcher3 tree;
        tree.Build(points);
    }
}

BENCHMARK_REGISTER_F(PointKdTreeSearcher3, Build)
    ->Arg(1 << 5)
    ->Arg(1 << 10)
    ->Arg(1 << 20);

BENCHMARK_DEFINE_F(PointKdTreeSearcher3, ForEachNearbyPoints)(benchmark::State& state)
{
    CubbyFlow::PointKdTreeSearcher3 tree;
    tree.Build(points);

    size_t cnt = 0;
    while (state.KeepRunning())
    {
        tree.ForEachNearbyPoint(MakeVec(), 1.0 / 64.0,
            [&](size_t, const Vector3D&)
        {
            ++cnt;
        });
    }
}

BENCHMARK_REGISTER_F(PointKdTreeSearcher3, ForEachNearbyPoints)
    ->Arg(1 << 5)
    ->Arg(1 << 10)
    ->Arg(1 << 20);
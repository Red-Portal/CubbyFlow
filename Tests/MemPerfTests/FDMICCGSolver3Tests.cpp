#include "MemPerfTestsUtils.h"

#include "gtest/gtest.h"

#include <FDM/FDMLinearSystem3.h>
#include <Solver/FDM/FDMICCGSolver3.h>

using namespace CubbyFlow;

TEST(FDMICCGSolver3, Memory)
{
    const size_t n = 300;

    const size_t mem0 = GetCurrentRSS();

    FDMLinearSystem3 system;
    system.A.Resize(n, n, n);
    system.x.Resize(n, n, n);
    system.b.Resize(n, n, n);

    FDMICCGSolver3 solver(1, 0.0);
    solver.Solve(&system);

    const size_t mem1 = GetCurrentRSS();

    const auto msg = MakeReadableByteSize(mem1 - mem0);

    CUBBYFLOW_PRINT_INFO("Mem usage: %f %s.\n", msg.first, msg.second.c_str());
}
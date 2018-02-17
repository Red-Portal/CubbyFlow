/*************************************************************************
> File Name: LevelSetLiquidSolver.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: LevelSetLiquidSolver functions for CubbyFlow Python API.
> Created Time: 2018/02/17
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_PYTHON_LEVEL_SET_LIQUID_SOLVER_H
#define CUBBYFLOW_PYTHON_LEVEL_SET_LIQUID_SOLVER_H

#include <pybind11/pybind11.h>

void AddLevelSetLiquidSolver2(pybind11::module& m);
void AddLevelSetLiquidSolver3(pybind11::module& m);

#endif
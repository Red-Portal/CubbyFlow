/*************************************************************************
> File Name: GridBlockedBoundaryConditionSolver.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: GridBlockedBoundaryConditionSolver functions for CubbyFlow Python API.
> Created Time: 2018/02/15
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_PYTHON_GRID_BLOCKED_BOUNDARY_CONDITION_SOLVER_H
#define CUBBYFLOW_PYTHON_GRID_BLOCKED_BOUNDARY_CONDITION_SOLVER_H

#include <pybind11/pybind11.h>

void AddGridBlockedBoundaryConditionSolver2(pybind11::module& m);
void AddGridBlockedBoundaryConditionSolver3(pybind11::module& m);


#endif
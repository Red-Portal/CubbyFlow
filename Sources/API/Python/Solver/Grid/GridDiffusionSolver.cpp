/*************************************************************************
> File Name: GridDiffusionSolver.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: GridDiffusionSolver functions for CubbyFlow Python API.
> Created Time: 2018/02/15
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <API/Python/Solver/Grid/GridDiffusionSolver.h>
#include <Core/Solver/Grid/GridDiffusionSolver2.h>
#include <Core/Solver/Grid/GridDiffusionSolver3.h>

#include <pybind11/pybind11.h>

using namespace CubbyFlow;

void AddGridDiffusionSolver2(pybind11::module& m)
{
	pybind11::class_<GridDiffusionSolver2, GridDiffusionSolver2Ptr>(static_cast<pybind11::handle>(m), "GridDiffusionSolver2",
		R"pbdoc(
			Abstract base class for 2-D grid-based diffusion equation solver.

			This class provides functions to solve the diffusion equation for different
			types of fields. The target equation can be written as
			\f$\frac{\partial f}{\partial t} = \mu\nabla^2 f\f$ where \f$\mu\f$ is
			the diffusion coefficient. The field \f$f\f$ can be either scalar or vector
			field.
		)pbdoc");
}

void AddGridDiffusionSolver3(pybind11::module& m)
{
	pybind11::class_<GridDiffusionSolver3, GridDiffusionSolver3Ptr>(static_cast<pybind11::handle>(m), "GridDiffusionSolver3",
		R"pbdoc(
			Abstract base class for 3-D grid-based diffusion equation solver.

			This class provides functions to solve the diffusion equation for different
			types of fields. The target equation can be written as
			\f$\frac{\partial f}{\partial t} = \mu\nabla^2 f\f$ where \f$\mu\f$ is
			the diffusion coefficient. The field \f$f\f$ can be either scalar or vector
			field.
		)pbdoc");
}
/*************************************************************************
> File Name: FDMGaussSeidelSolver.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: FDMGaussSeidelSolver functions for CubbyFlow Python API.
> Created Time: 2018/02/14
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <API/Python/Solver/FDM/FDMGaussSeidelSolver.h>
#include <Core/Solver/FDM/FDMGaussSeidelSolver2.h>
#include <Core/Solver/FDM/FDMGaussSeidelSolver3.h>

#include <pybind11/pybind11.h>

using namespace CubbyFlow;

void AddFDMGaussSeidelSolver2(pybind11::module& m)
{
	pybind11::class_<FDMGaussSeidelSolver2, FDMGaussSeidelSolver2Ptr, FDMLinearSystemSolver2>(m, "FDMGaussSeidelSolver2",
		R"pbdoc(
			2-D finite difference-type linear system solver using conjugate gradient.
		)pbdoc")
	.def(pybind11::init<uint32_t, uint32_t, double, double, bool>(),
		pybind11::arg("maxNumberOfIterations"),
		pybind11::arg("residualCheckInterval"),
		pybind11::arg("tolerance"),
		pybind11::arg("sorFactor") = 1.0,
		pybind11::arg("useRedBlackOrdering") = false)
	.def_property_readonly("maxNumberOfIterations", &FDMGaussSeidelSolver2::GetMaxNumberOfIterations,
		R"pbdoc(
			Max number of CG iterations.
		)pbdoc")
	.def_property_readonly("lastNumberOfIterations", &FDMGaussSeidelSolver2::GetLastNumberOfIterations,
		R"pbdoc(
			The last number of CG iterations the solver made.
		)pbdoc")
	.def_property_readonly("tolerance", &FDMGaussSeidelSolver2::GetTolerance,
		R"pbdoc(
			The max residual tolerance for the CG method.
		)pbdoc")
	.def_property_readonly("lastResidual", &FDMGaussSeidelSolver2::GetLastResidual,
		R"pbdoc(
			The last residual after the CG iterations.
		)pbdoc")
	.def_property_readonly("sorFactor", &FDMGaussSeidelSolver2::GetSORFactor,
		R"pbdoc(
			Returns the SOR (Successive Over Relaxation) factor.
		)pbdoc")
	.def_property_readonly("useRedBlackOrdering", &FDMGaussSeidelSolver2::GetUseRedBlackOrdering,
		R"pbdoc(
			Returns true if red-black ordering is enabled.
		)pbdoc");
}

void AddFDMGaussSeidelSolver3(pybind11::module& m)
{
	pybind11::class_<FDMGaussSeidelSolver3, FDMGaussSeidelSolver3Ptr, FDMLinearSystemSolver3>(m, "FDMGaussSeidelSolver3",
		R"pbdoc(
			3-D finite difference-type linear system solver using conjugate gradient.
		)pbdoc")
	.def(pybind11::init<uint32_t, uint32_t, double, double, bool>(),
		pybind11::arg("maxNumberOfIterations"),
		pybind11::arg("residualCheckInterval"),
		pybind11::arg("tolerance"),
		pybind11::arg("sorFactor") = 1.0,
		pybind11::arg("useRedBlackOrdering") = false)
	.def_property_readonly("maxNumberOfIterations", &FDMGaussSeidelSolver3::GetMaxNumberOfIterations,
		R"pbdoc(
			Max number of CG iterations.
		)pbdoc")
	.def_property_readonly("lastNumberOfIterations", &FDMGaussSeidelSolver3::GetLastNumberOfIterations,
		R"pbdoc(
			The last number of CG iterations the solver made.
		)pbdoc")
	.def_property_readonly("tolerance", &FDMGaussSeidelSolver3::GetTolerance,
		R"pbdoc(
			The max residual tolerance for the CG method.
		)pbdoc")
	.def_property_readonly("lastResidual", &FDMGaussSeidelSolver3::GetLastResidual,
		R"pbdoc(
			The last residual after the CG iterations.
		)pbdoc")
	.def_property_readonly("sorFactor", &FDMGaussSeidelSolver3::GetSORFactor,
		R"pbdoc(
			Returns the SOR (Successive Over Relaxation) factor.
		)pbdoc")
	.def_property_readonly("useRedBlackOrdering", &FDMGaussSeidelSolver3::GetUseRedBlackOrdering,
		R"pbdoc(
			Returns true if red-black ordering is enabled.
		)pbdoc");
}
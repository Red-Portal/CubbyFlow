/*************************************************************************
> File Name: GridSmokeSolver.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: GridSmokeSolver functions for CubbyFlow Python API.
> Created Time: 2018/02/17
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <API/Python/Solver/Grid/GridSmokeSolver.h>
#include <API/Python/Utils/pybind11Utils.h>
#include <Core/Solver/Grid/GridSmokeSolver2.h>
#include <Core/Solver/Grid/GridSmokeSolver3.h>

#include <pybind11/pybind11.h>

using namespace CubbyFlow;

void AddGridSmokeSolver2(pybind11::module& m)
{
	pybind11::class_<GridSmokeSolver2, GridSmokeSolver2Ptr, GridFluidSolver2>(m, "GridSmokeSolver2",
		R"pbdoc(
			2-D grid-based smoke solver.

			This class extends GridFluidSolver2 to implement smoke simulation solver.
			It adds smoke density and temperature fields to define the smoke and uses
			buoyancy force to simulate hot rising smoke.

			See Fedkiw, Ronald, Jos Stam, and Henrik Wann Jensen.
			"Visual simulation of smoke." Proceedings of the 28th annual conference
			on Computer graphics and interactive techniques. ACM, 2001.
		)pbdoc")
	.def("__init__", [](GridSmokeSolver2& instance, pybind11::args args, pybind11::kwargs kwargs)
	{
		Size2 resolution{ 1, 1 };
		Vector2D gridSpacing{ 1, 1 };
		Vector2D gridOrigin{ 0, 0 };

		ParseGridResizeParams(args, kwargs, resolution, gridSpacing, gridOrigin);

		new (&instance) GridSmokeSolver2(resolution, gridSpacing, gridOrigin);
	},
		R"pbdoc(
			Constructs GridSmokeSolver2

			This method constructs GridSmokeSolver2 with resolution, gridSpacing,
			and gridOrigin.

			Parameters
			----------
			- `*args` : resolution, gridSpacing, and gridOrigin arguments.
			- `**kwargs`
				- resolution : Grid resolution.
				- gridSpacing : Grid spacing.
				- gridOrigin : Origin point ot the grid.
				- domainSizeX : Domain size in x-direction.
		)pbdoc")
	.def_property("smokeDiffusionCoefficient", &GridSmokeSolver2::GetSmokeDiffusionCoefficient, &GridSmokeSolver2::SetSmokeDiffusionCoefficient,
		R"pbdoc(
			The smoke diffusion coefficient.
		)pbdoc")
	.def_property("temperatureDiffusionCoefficient", &GridSmokeSolver2::GetTemperatureDiffusionCoefficient, &GridSmokeSolver2::SetTemperatureDiffusionCoefficient,
		R"pbdoc(
			The temperature diffusion coefficient.
		)pbdoc")
	.def_property("buoyancySmokeDensityFactor", &GridSmokeSolver2::GetBuoyancySmokeDensityFactor, &GridSmokeSolver2::SetBuoyancySmokeDensityFactor,
		R"pbdoc(
			The buoyancy factor which will be multiplied to the smoke density.

			This class computes buoyancy by looking up the value of smoke density
			and temperature, compare them to the average values, and apply
			multiplier factor to the diff between the value and the average. That
			multiplier is defined for each smoke density and temperature separately
			For example, negative smoke density buoyancy factor means a heavier
			smoke should sink.
		)pbdoc")
	.def_property("buoyancyTemperatureFactor", &GridSmokeSolver2::GetBuoyancyTemperatureFactor, &GridSmokeSolver2::SetBuoyancyTemperatureFactor,
		R"pbdoc(
			The buoyancy factor which will be multiplied to the temperature.

			This class computes buoyancy by looking up the value of smoke density
			and temperature, compare them to the average values, and apply
			multiplier factor to the diff between the value and the average. That
			multiplier is defined for each smoke density and temperature separately
			For example, negative smoke density buoyancy factor means a heavier
			smoke should sink.
		)pbdoc")
	.def_property("smokeDecayFactor", &GridSmokeSolver2::GetSmokeDecayFactor, &GridSmokeSolver2::SetSmokeDecayFactor,
		R"pbdoc(
			The smoke decay factor.

			In addition to the diffusion, the smoke also can fade-out over time by
			setting the decay factor between 0 and 1.
		)pbdoc")
	.def_property("smokeTemperatureDecayFactor", &GridSmokeSolver2::GetTemperatureDecayFactor, &GridSmokeSolver2::SetTemperatureDecayFactor,
		R"pbdoc(
			The temperature decay factor.

			In addition to the diffusion, the temperature also can fade-out over
			time by setting the decay factor between 0 and 1.
		)pbdoc")
	.def_property_readonly("smokeDensity", &GridSmokeSolver2::GetSmokeDensity,
		R"pbdoc(
			Returns smoke density field.
		)pbdoc")
	.def_property_readonly("temperature", &GridSmokeSolver2::GetTemperature,
		R"pbdoc(
			Returns temperature field.
		)pbdoc");
}

void AddGridSmokeSolver3(pybind11::module& m)
{
	pybind11::class_<GridSmokeSolver3, GridSmokeSolver3Ptr, GridFluidSolver3>(m, "GridSmokeSolver3",
		R"pbdoc(
			3-D grid-based smoke solver.

			This class extends GridFluidSolver3 to implement smoke simulation solver.
			It adds smoke density and temperature fields to define the smoke and uses
			buoyancy force to simulate hot rising smoke.

			See Fedkiw, Ronald, Jos Stam, and Henrik Wann Jensen.
			"Visual simulation of smoke." Proceedings of the 28th annual conference
			on Computer graphics and interactive techniques. ACM, 2001.
		)pbdoc")
	.def("__init__", [](GridSmokeSolver3& instance, pybind11::args args, pybind11::kwargs kwargs)
	{
		Size3 resolution{ 1, 1, 1 };
		Vector3D gridSpacing{ 1, 1, 1 };
		Vector3D gridOrigin{ 0, 0, 0 };

		ParseGridResizeParams(args, kwargs, resolution, gridSpacing, gridOrigin);

		new (&instance) GridSmokeSolver3(resolution, gridSpacing, gridOrigin);
	},
		R"pbdoc(
			Constructs GridSmokeSolver3

			This method constructs GridSmokeSolver3 with resolution, gridSpacing,
			and gridOrigin.

			Parameters
			----------
			- `*args` : resolution, gridSpacing, and gridOrigin arguments.
			- `**kwargs`
				- resolution : Grid resolution.
				- gridSpacing : Grid spacing.
				- gridOrigin : Origin point ot the grid.
				- domainSizeX : Domain size in x-direction.
		)pbdoc")
	.def_property("smokeDiffusionCoefficient", &GridSmokeSolver3::GetSmokeDiffusionCoefficient, &GridSmokeSolver3::SetSmokeDiffusionCoefficient,
		R"pbdoc(
			The smoke diffusion coefficient.
		)pbdoc")
	.def_property("temperatureDiffusionCoefficient", &GridSmokeSolver3::GetTemperatureDiffusionCoefficient, &GridSmokeSolver3::SetTemperatureDiffusionCoefficient,
		R"pbdoc(
			The temperature diffusion coefficient.
		)pbdoc")
	.def_property("buoyancySmokeDensityFactor", &GridSmokeSolver3::GetBuoyancySmokeDensityFactor, &GridSmokeSolver3::SetBuoyancySmokeDensityFactor,
		R"pbdoc(
			The buoyancy factor which will be multiplied to the smoke density.

			This class computes buoyancy by looking up the value of smoke density
			and temperature, compare them to the average values, and apply
			multiplier factor to the diff between the value and the average. That
			multiplier is defined for each smoke density and temperature separately
			For example, negative smoke density buoyancy factor means a heavier
			smoke should sink.
		)pbdoc")
	.def_property("buoyancyTemperatureFactor", &GridSmokeSolver3::GetBuoyancyTemperatureFactor, &GridSmokeSolver3::SetBuoyancyTemperatureFactor,
		R"pbdoc(
			The buoyancy factor which will be multiplied to the temperature.

			This class computes buoyancy by looking up the value of smoke density
			and temperature, compare them to the average values, and apply
			multiplier factor to the diff between the value and the average. That
			multiplier is defined for each smoke density and temperature separately
			For example, negative smoke density buoyancy factor means a heavier
			smoke should sink.
		)pbdoc")
	.def_property("smokeDecayFactor", &GridSmokeSolver3::GetSmokeDecayFactor, &GridSmokeSolver3::SetSmokeDecayFactor,
		R"pbdoc(
			The smoke decay factor.

			In addition to the diffusion, the smoke also can fade-out over time by
			setting the decay factor between 0 and 1.
		)pbdoc")
	.def_property("smokeTemperatureDecayFactor", &GridSmokeSolver3::GetTemperatureDecayFactor, &GridSmokeSolver3::SetTemperatureDecayFactor,
		R"pbdoc(
			The temperature decay factor.

			In addition to the diffusion, the temperature also can fade-out over
			time by setting the decay factor between 0 and 1.
		)pbdoc")
	.def_property_readonly("smokeDensity", &GridSmokeSolver3::GetSmokeDensity,
		R"pbdoc(
			Returns smoke density field.
		)pbdoc")
	.def_property_readonly("temperature", &GridSmokeSolver3::GetTemperature,
		R"pbdoc(
			Returns temperature field.
		)pbdoc");
}
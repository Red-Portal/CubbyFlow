/*************************************************************************
> File Name: PointsToImplicit.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: PointsToImplicit functions for CubbyFlow Python API.
> Created Time: 2018/02/10
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <API/Python/PointsToImplicit/PointsToImplicit.h>
#include <API/Python/Utils/pybind11Utils.h>
#include <Core/PointsToImplicit/PointsToImplicit2.h>
#include <Core/PointsToImplicit/PointsToImplicit3.h>

#include <pybind11/pybind11.h>

using namespace CubbyFlow;

void AddPointsToImplicit2(pybind11::module& m)
{
	pybind11::class_<PointsToImplicit2, PointsToImplicit2Ptr>(m, "PointsToImplicit2",
		R"pbdoc(
			Abstract base class for 2-D points-to-implicit converters.
		)pbdoc")
	.def("Convert", [](const PointsToImplicit2& instance, const pybind11::list& points, ScalarGrid2Ptr output)
	{
		std::vector<Vector2D> points_;
		for (size_t i = 0; i < points.size(); ++i)
		{
			points_.push_back(ObjectToVector2D(points[i]));
		}

		ConstArrayAccessor1<Vector2D> pointsAcc(points_.size(), points_.data());
		instance.Convert(pointsAcc, output.get());
	},
		R"pbdoc(
			Converts the given points to implicit surface scalar field.

			Parameters
			----------
			- points : List of 2D vectors.
			- output : Scalar grid output.
		)pbdoc",
		pybind11::arg("points"),
		pybind11::arg("output"));
}

void AddPointsToImplicit3(pybind11::module& m)
{
	pybind11::class_<PointsToImplicit3, PointsToImplicit3Ptr>(m, "PointsToImplicit3",
		R"pbdoc(
			Abstract base class for 3-D points-to-implicit converters.
		)pbdoc")
	.def("Convert", [](const PointsToImplicit3& instance, const pybind11::list& points, ScalarGrid3Ptr output)
	{
		std::vector<Vector3D> points_;
		for (size_t i = 0; i < points.size(); ++i)
		{
			points_.push_back(ObjectToVector3D(points[i]));
		}

		ConstArrayAccessor1<Vector3D> pointsAcc(points_.size(), points_.data());
		instance.Convert(pointsAcc, output.get());
	},
		R"pbdoc(
			Converts the given points to implicit surface scalar field.

			Parameters
			----------
			- points : List of 3D vectors.
			- output : Scalar grid output.
		)pbdoc",
		pybind11::arg("points"),
		pybind11::arg("output"));
}
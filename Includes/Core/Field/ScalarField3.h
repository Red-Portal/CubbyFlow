/*************************************************************************
> File Name: ScalarField3.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Abstract base class for 3-D scalar field.
> Created Time: 2017/03/14
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_SCALAR_FIELD3_H
#define CUBBYFLOW_SCALAR_FIELD3_H

#include <Core/Field/Field3.h>
#include <Core/Vector/Vector3.h>

#include <functional>
#include <memory>

namespace CubbyFlow
{
	//! Abstract base class for 3-D scalar field.
	class ScalarField3 : public Field3
	{
	public:
		//! Default constructor.
		ScalarField3();

		//! Default destructor.
		virtual ~ScalarField3();

		//! Returns sampled value at given position \p x.
		virtual double Sample(const Vector3D& x) const = 0;

		//! Returns gradient vector at given position \p x.
		virtual Vector3D Gradient(const Vector3D& x) const;

		//! Returns Laplacian at given position \p x.
		virtual double Laplacian(const Vector3D& x) const;

		//! Returns sampler function object.
		virtual std::function<double(const Vector3D&)> Sampler() const;
	};

	//! Shared pointer for the ScalarField3 type.
	using ScalarField3Ptr = std::shared_ptr<ScalarField3>;
}

#endif
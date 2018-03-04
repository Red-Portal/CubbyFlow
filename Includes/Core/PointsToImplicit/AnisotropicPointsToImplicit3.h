/*************************************************************************
> File Name: AnisotropicPointsToImplicit3.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 3-D points-to-implicit converter using Anisotropic kernels.
> Created Time: 2017/11/19
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_ANISOTROPIC_POINTS_TO_IMPLICIT3_H
#define CUBBYFLOW_ANISOTROPIC_POINTS_TO_IMPLICIT3_H

#include <Core/PointsToImplicit/PointsToImplicit3.h>

namespace CubbyFlow
{
	//!
	//! \brief 3-D points-to-implicit converter using Anisotropic kernels.
	//!
	//! This class converts 3-D points to implicit surface using anisotropic kernels
	//! so that the kernels are oriented and stretched to reflect the point
	//! distribution more naturally (thus less bumps). The implementation is based
	//! on Yu and Turk's 2013 paper with some modifications.
	//!
	//! \see Yu, Jihun, and Greg Turk. "Reconstructing surfaces of particle-based
	//!      fluids using anisotropic kernels." ACM Transactions on Graphics (TOG)
	//!      32.1 (2013): 5.
	//!
	class AnisotropicPointsToImplicit3 final : public PointsToImplicit3
	{
	public:
		//!
		//! \brief Constructs the converter with given parameters.
		//!
		//! \param kernelRadius Kernel radius for interpolations.
		//! \param cutOffDensity Iso-contour density value.
		//! \param positionSmoothingFactor Position smoothing factor.
		//! \param minNumNeighbors Minimum number of neighbors to enable anisotropic kernel.
		//!
		AnisotropicPointsToImplicit3(
			double kernelRadius = 1.0,
			double cutOffDensity = 0.5,
			double positionSmoothingFactor = 0.5,
			size_t minNumNeighbors = 25,
			bool isOutputSDF = true);

		//! Converts the given points to implicit surface scalar field.
		void Convert(const ConstArrayAccessor1<Vector3D>& points, ScalarGrid3* output) const override;

	private:
		double m_kernelRadius = 1.0;
		double m_cutOffDensity = 0.5;
		double m_positionSmoothingFactor = 0.0;
		size_t m_minNumNeighbors = 25;
		bool m_isOutputSDF = true;
	};

	//! Shared pointer for the AnisotropicPointsToImplicit3 type.
	using AnisotropicPointsToImplicit3Ptr = std::shared_ptr<AnisotropicPointsToImplicit3>;
}

#endif
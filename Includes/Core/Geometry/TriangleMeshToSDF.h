/*************************************************************************
> File Name: TriangleMeshToSDF.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Generates signed-distance field out of given triangle mesh.
> Created Time: 2017/08/24
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
// This code is ported from Christopher Batty's SDFGen software.
// (https://github.com/christopherbatty/SDFGen)
//
// The MIT License (MIT)
//
// Copyright (c) 2015, Christopher Batty
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CUBBYFLOW_TRIANGLE_MESH_TO_SDF_H
#define CUBBYFLOW_TRIANGLE_MESH_TO_SDF_H

#include <Core/Geometry/TriangleMesh3.h>
#include <Core/Grid/ScalarGrid3.h>

namespace CubbyFlow
{
	//!
	//! \brief      Generates signed-distance field out of given triangle mesh.
	//!
	//! This function generates signed-distance field from a triangle mesh. The mesh
	//! should be water-tight, meaning there should be no gap. A gap can make the
	//! evaluation of the signs impossible. The output signed-distance field will be
	//! assigned to the scalar field, \p sdf, which can be any type of scalar field
	//! (vertex-centered vs. cell-centered). To accelerate the calculation, this
	//! function also takes extra parameter, \p exactBand, which defines the
	//! bandwidth around the mesh in a number of grid points. This bandwidth is the
	//! region where the exact distance to the mesh will be computed. Distance
	//! values of the areas that are farther from the mesh surface will be
	//! approximated using fast sweeping method. The sign of the signed-distance
	//! field is determined by assuming the bounding box of the output scalar grid
	//! is the exterior of the mesh.
	//!
	//! This function is a port of Christopher Batty's SDFGen software.
	//!
	//! \see https://github.com/christopherbatty/SDFGen
	//!
	//! \param[in]      mesh      The mesh.
	//! \param[in,out]  sdf       The output signed-distance field.
	//! \param[in]      exactBand The bandwidth for exact distance computation.
	//!
	void TriangleMeshToSDF(
		const TriangleMesh3& mesh,
		ScalarGrid3* sdf,
		const unsigned int exactBand = 1);
}

#endif
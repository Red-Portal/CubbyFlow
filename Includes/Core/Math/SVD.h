/*************************************************************************
> File Name: SVD.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Singular value decomposition (SVD).
> Created Time: 2017/11/19
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_SVD_H
#define CUBBYFLOW_SVD_H

#include <Core/Matrix/Matrix2x2.h>
#include <Core/Matrix/Matrix3x3.h>
#include <Core/Matrix/Matrix4x4.h>
#include <Core/Matrix/MatrixMxN.h>
#include <Core/Vector/VectorN.h>

namespace CubbyFlow
{
	//!
	//! \brief Singular value decomposition (SVD).
	//!
	//! This function decompose the input matrix \p a to \p u * \p w * \p v^T.
	//!
	//! \tparam T Real-value type.
	//!
	//! \param a The input matrix to decompose.
	//! \param u Left-most output matrix.
	//! \param w The vector of singular values.
	//! \param v Right-most output matrix.
	//!
	template <typename T>
	void SVD(const MatrixMxN<T>& a, MatrixMxN<T>& u, VectorN<T>& w, MatrixMxN<T>& v);

	//!
	//! \brief Singular value decomposition (SVD).
	//!
	//! This function decompose the input matrix \p a to \p u * \p w * \p v^T.
	//!
	//! \tparam T Real-value type.
	//!
	//! \param a The input matrix to decompose.
	//! \param u Left-most output matrix.
	//! \param w The vector of singular values.
	//! \param v Right-most output matrix.
	//!
	template <typename T, size_t M, size_t N>
	void SVD(const Matrix<T, M, N>& a, Matrix<T, M, N>& u, Vector<T, N>& w, Matrix<T, N, N>& v);
}

#include <Core/Math/SVD-Impl.h>

#endif
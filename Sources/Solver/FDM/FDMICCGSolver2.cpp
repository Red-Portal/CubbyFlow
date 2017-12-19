/*************************************************************************
> File Name: FDMICCGSolver2.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 2-D finite difference-type linear system solver using incomplete
>          Cholesky conjugate gradient (ICCG).
> Created Time: 2017/08/12
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Math/CG.h>
#include <Solver/FDM/FDMICCGSolver2.h>
#include <Utils/Logger.h>

namespace CubbyFlow
{
	void FDMICCGSolver2::Preconditioner::Build(const FDMMatrix2& matrix)
	{
		Size2 size = matrix.size();
		A = matrix.ConstAccessor();

		d.Resize(size, 0.0);
		y.Resize(size, 0.0);

		matrix.ForEachIndex([&](size_t i, size_t j)
		{
			double denom =
				matrix(i, j).center -
				((i > 0) ? Square(matrix(i - 1, j).right) * d(i - 1, j) : 0.0) -
				((j > 0) ? Square(matrix(i, j - 1).up)    * d(i, j - 1) : 0.0);

			if (std::fabs(denom) > 0.0)
			{
				d(i, j) = 1.0 / denom;
			}
			else
			{
				d(i, j) = 0.0;
			}
		});
	}

	void FDMICCGSolver2::Preconditioner::Solve(const FDMVector2& b, FDMVector2* x)
	{
		Size2 size = b.size();
		ssize_t sx = static_cast<ssize_t>(size.x);
		ssize_t sy = static_cast<ssize_t>(size.y);

		b.ForEachIndex([&](size_t i, size_t j)
		{
			y(i, j) =
				(b(i, j) -
				((i > 0) ? A(i - 1, j).right * y(i - 1, j) : 0.0) -
				((j > 0) ? A(i, j - 1).up    * y(i, j - 1) : 0.0)) *
				d(i, j);
		});

		for (ssize_t j = sy - 1; j >= 0; --j)
		{
			for (ssize_t i = sx - 1; i >= 0; --i)
			{
				(*x)(i, j) =
					(y(i, j) -
					((i + 1 < sx) ? A(i, j).right * (*x)(i + 1, j) : 0.0) -
					((j + 1 < sy) ? A(i, j).up    * (*x)(i, j + 1) : 0.0)) *
					d(i, j);
			}
		}
	}

	FDMICCGSolver2::FDMICCGSolver2(unsigned int maxNumberOfIterations, double tolerance) :
		m_maxNumberOfIterations(maxNumberOfIterations),
		m_lastNumberOfIterations(0),
		m_tolerance(tolerance),
		m_lastResidualNorm(std::numeric_limits<double>::max())
	{
		// Do nothing
	}

	bool FDMICCGSolver2::Solve(FDMLinearSystem2* system)
	{
		FDMMatrix2& matrix = system->A;
		FDMVector2& solution = system->x;
		FDMVector2& rhs = system->b;

		assert(matrix.size() == rhs.size());
		assert(matrix.size() == solution.size());

		Size2 size = matrix.size();
		m_r.Resize(size);
		m_d.Resize(size);
		m_q.Resize(size);
		m_s.Resize(size);

		system->x.Set(0.0);
		m_r.Set(0.0);
		m_d.Set(0.0);
		m_q.Set(0.0);
		m_s.Set(0.0);

		m_precond.Build(matrix);
		
		PCG<FDMBLAS2, Preconditioner>(
			matrix,
			rhs,
			m_maxNumberOfIterations,
			m_tolerance,
			&m_precond,
			&solution,
			&m_r,
			&m_d,
			&m_q,
			&m_s,
			&m_lastNumberOfIterations,
			&m_lastResidualNorm);

		CUBBYFLOW_INFO << "Residual after solving ICCG: " << m_lastResidualNorm
			<< " Number of ICCG iterations: " << m_lastNumberOfIterations;

		return (m_lastResidualNorm <= m_tolerance) || (m_lastNumberOfIterations < m_maxNumberOfIterations);
	}

	unsigned int FDMICCGSolver2::GetMaxNumberOfIterations() const
	{
		return m_maxNumberOfIterations;
	}

	unsigned int FDMICCGSolver2::GetLastNumberOfIterations() const
	{
		return m_lastNumberOfIterations;
	}

	double FDMICCGSolver2::GetTolerance() const
	{
		return m_tolerance;
	}

	double FDMICCGSolver2::GetLastResidual() const
	{
		return m_lastResidualNorm;
	}
}

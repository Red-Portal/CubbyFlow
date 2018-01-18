/*************************************************************************
> File Name: Factory.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Factory methods for CubbyFlow.
> Created Time: 2017/05/01
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_FACTORY_H
#define CUBBYFLOW_FACTORY_H

#include <Core/Grid/ScalarGrid2.h>
#include <Core/Grid/ScalarGrid3.h>
#include <Core/Grid/VectorGrid2.h>
#include <Core/Grid/VectorGrid3.h>
#include <Core/Searcher/PointNeighborSearcher2.h>
#include <Core/Searcher/PointNeighborSearcher3.h>

namespace CubbyFlow
{
	class Factory
	{
	public:
		static ScalarGrid2Ptr BuildScalarGrid2(const std::string& name);

		static ScalarGrid3Ptr BuildScalarGrid3(const std::string& name);

		static VectorGrid2Ptr BuildVectorGrid2(const std::string& name);

		static VectorGrid3Ptr BuildVectorGrid3(const std::string& name);

		static PointNeighborSearcher2Ptr BuildPointNeighborSearcher2(const std::string& name);

		static PointNeighborSearcher3Ptr BuildPointNeighborSearcher3(const std::string& name);
	};
}

#endif
/*************************************************************************
> File Name: IntersectionQueryEngine3.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Abstract base class for 3-D intersection test query engine.
> Created Time: 2017/10/14
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_INTERSECTION_QUERY_ENGINE3_H
#define CUBBYFLOW_INTERSECTION_QUERY_ENGINE3_H

#include <Core/BoundingBox/BoundingBox3.h>
#include <Core/Vector/Vector3.h>

namespace CubbyFlow
{
	//! Closest intersection query result.
	template <typename T>
	struct ClosestIntersectionQueryResult3
	{
		const T* item = nullptr;
		double distance = std::numeric_limits<double>::max();
	};

	//! Closest intersection distance measure function.
	template <typename T>
	using ClosestIntersectionDistanceFunc3 = std::function<double(const T&, const Vector3D&)>;

	//! Box-item intersection test function.
	template <typename T>
	using BoxIntersectionTestFunc3 = std::function<bool(const T&, const BoundingBox3D&)>;

	//! Ray-item intersection test function.
	template <typename T>
	using RayIntersectionTestFunc3 = std::function<bool(const T&, const Ray3D&)>;

	//! Ray-item closest intersection evaluation function.
	template <typename T>
	using GetRayIntersectionFunc3 = std::function<double(const T&, const Ray3D&)>;

	//! Visitor function which is invoked for each intersecting item.
	template <typename T>
	using IntersectionVisitorFunc3 = std::function<void(const T&)>;

	//! Abstract base class for 3-D intersection test query engine.
	template <typename T>
	class IntersectionQueryEngine3
	{
	public:
		//! Returns true if given \p box intersects with any of the stored items.
		virtual bool IsIntersects(
			const BoundingBox3D& box,
			const BoxIntersectionTestFunc3<T>& testFunc) const = 0;

		//! Returns true if given \p ray intersects with any of the stored items.
		virtual bool IsIntersects(
			const Ray3D& ray,
			const RayIntersectionTestFunc3<T>& testFunc) const = 0;

		//! Invokes \p visitorFunc for every intersecting items.
		virtual void ForEachIntersectingItem(
			const BoundingBox3D& box, const BoxIntersectionTestFunc3<T>& testFunc,
			const IntersectionVisitorFunc3<T>& visitorFunc) const = 0;

		//! Invokes \p visitorFunc for every intersecting items.
		virtual void ForEachIntersectingItem(
			const Ray3D& ray, const RayIntersectionTestFunc3<T>& testFunc,
			const IntersectionVisitorFunc3<T>& visitorFunc) const = 0;

		//! Returns the closest intersection for given \p ray.
		virtual ClosestIntersectionQueryResult3<T> GetClosestIntersection(
			const Ray3D& ray, const GetRayIntersectionFunc3<T>& testFunc) const = 0;
	};
}

#endif
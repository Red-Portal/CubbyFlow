/*************************************************************************
> File Name: BoundingBox2-Impl.h
> Project Name: CubbyFlow
> Author: Dongmin Kim
> Purpose: 2-D axis-aligned bounding box class.
> Created Time: 2017/03/31
> Copyright (c) 2018, Dongmin Kim
*************************************************************************/
#ifndef CUBBYFLOW_BOUNDING_BOX2_IMPL_H
#define CUBBYFLOW_BOUNDING_BOX2_IMPL_H

namespace CubbyFlow
{
	template <typename T>
	BoundingBox<T, 2>::BoundingBox()
	{
		Reset();
	}

	template <typename T>
	BoundingBox<T, 2>::BoundingBox(const Vector2<T>& point1, const Vector2<T>& point2)
	{
		lowerCorner.x = std::min(point1.x, point2.x);
		lowerCorner.y = std::min(point1.y, point2.y);
		upperCorner.x = std::max(point1.x, point2.x);
		upperCorner.y = std::max(point1.y, point2.y);
	}

	template <typename T>
	BoundingBox<T, 2>::BoundingBox(const BoundingBox& other) :
		lowerCorner(other.lowerCorner), upperCorner(other.upperCorner)
	{
		// Do nothing
	}

	template <typename T>
	T BoundingBox<T, 2>::GetWidth() const
	{
		return upperCorner.x - lowerCorner.x;
	}

	template <typename T>
	T BoundingBox<T, 2>::GetHeight() const
	{
		return upperCorner.y - lowerCorner.y;
	}

	template <typename T>
	T BoundingBox<T, 2>::Length(size_t axis)
	{
		return upperCorner[axis] - lowerCorner[axis];
	}

	template <typename T>
	bool BoundingBox<T, 2>::Overlaps(const BoundingBox& other) const
	{
		if (upperCorner.x < other.lowerCorner.x || lowerCorner.x > other.upperCorner.x)
		{
			return false;
		}

		if (upperCorner.y < other.lowerCorner.y || lowerCorner.y > other.upperCorner.y)
		{
			return false;
		}

		return true;
	}

	template <typename T>
	bool BoundingBox<T, 2>::Contains(const Vector2<T>& point) const
	{
		if (upperCorner.x < point.x || lowerCorner.x > point.x)
		{
			return false;
		}

		if (upperCorner.y < point.y || lowerCorner.y > point.y)
		{
			return false;
		}

		return true;
	}

	template <typename T>
	bool BoundingBox<T, 2>::Intersects(const Ray2<T>& ray) const
	{
		T min = 0;
		T max = std::numeric_limits<T>::max();

		const Vector2<T>& rayInvDir = ray.direction.RDiv(1);

		for (size_t i = 0; i < 2; ++i)
		{
			T near = (lowerCorner[i] - ray.origin[i]) * rayInvDir[i];
			T far = (upperCorner[i] - ray.origin[i]) * rayInvDir[i];

			if (near > far)
			{
				std::swap(near, far);
			}

			min = std::max(near, min);
			max = std::min(far, max);

			if (min > max)
			{
				return false;
			}
		}

		return true;
	}

	template <typename T>
	BoundingBoxRayIntersection2<T> BoundingBox<T, 2>::ClosestIntersection(const Ray2<T>& ray) const
	{
		BoundingBoxRayIntersection2<T> intersection;

		T min = 0;
		T max = std::numeric_limits<T>::max();

		const Vector2<T>& rayInvDir = ray.direction.RDiv(1);

		for (size_t i = 0; i < 2; ++i)
		{
			T near = (lowerCorner[i] - ray.origin[i]) * rayInvDir[i];
			T far = (upperCorner[i] - ray.origin[i]) * rayInvDir[i];

			if (near > far)
			{
				std::swap(near, far);
			}

			min = std::max(near, min);
			max = std::min(far, max);

			if (min > max)
			{
				intersection.isIntersecting = false;
				return intersection;
			}
		}

		intersection.isIntersecting = true;

		if (Contains(ray.origin))
		{
			intersection.near = max;
			intersection.far = std::numeric_limits<T>::max();
		}
		else
		{
			intersection.near = min;
			intersection.far = max;
		}

		return intersection;
	}

	template <typename T>
	Vector2<T> BoundingBox<T, 2>::MidPoint() const
	{
		return (upperCorner + lowerCorner) / static_cast<T>(2);
	}

	template <typename T>
	T BoundingBox<T, 2>::DiagonalLength() const
	{
		return (upperCorner - lowerCorner).Length();
	}

	template <typename T>
	T BoundingBox<T, 2>::DiagonalLengthSquared() const
	{
		return (upperCorner - lowerCorner).LengthSquared();
	}

	template <typename T>
	void BoundingBox<T, 2>::Reset()
	{
		lowerCorner.x = std::numeric_limits<T>::max();
		lowerCorner.y = std::numeric_limits<T>::max();
		upperCorner.x = -std::numeric_limits<T>::max();
		upperCorner.y = -std::numeric_limits<T>::max();
	}

	template <typename T>
	void BoundingBox<T, 2>::Merge(const Vector2<T>& point)
	{
		lowerCorner.x = std::min(lowerCorner.x, point.x);
		lowerCorner.y = std::min(lowerCorner.y, point.y);
		upperCorner.x = std::max(upperCorner.x, point.x);
		upperCorner.y = std::max(upperCorner.y, point.y);
	}

	template <typename T>
	void BoundingBox<T, 2>::Merge(const BoundingBox& other)
	{
		lowerCorner.x = std::min(lowerCorner.x, other.lowerCorner.x);
		lowerCorner.y = std::min(lowerCorner.y, other.lowerCorner.y);
		upperCorner.x = std::max(upperCorner.x, other.upperCorner.x);
		upperCorner.y = std::max(upperCorner.y, other.upperCorner.y);
	}

	template <typename T>
	void BoundingBox<T, 2>::Expand(T delta)
	{
		lowerCorner -= delta;
		upperCorner += delta;
	}

	template <typename T>
	Vector2<T> BoundingBox<T, 2>::Corner(size_t idx) const
	{
		static const T h = static_cast<T>(1) / 2;
		static const Vector2<T> offset[4] =
		{
			{ -h, -h }, { +h, -h }, { -h, +h }, { +h, +h }
		};

		return Vector2<T>(GetWidth(), GetHeight()) * offset[idx] + MidPoint();
	}

	template <typename T>
	Vector2<T> BoundingBox<T, 2>::Clamp(const Vector2<T>& pt) const
	{
		return CubbyFlow::Clamp(pt, lowerCorner, upperCorner);
	}

	template <typename T>
	bool BoundingBox<T, 2>::IsEmpty() const
	{
		return (lowerCorner.x >= upperCorner.x || lowerCorner.y >= upperCorner.y);
	}
}

#endif
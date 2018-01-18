/*************************************************************************
> File Name: Surface2.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Abstract base class for 2-D surface.
> Created Time: 2017/03/20
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_SURFACE2_H
#define CUBBYFLOW_SURFACE2_H

#include <Core/BoundingBox/BoundingBox2.h>
#include <Core/Ray/Ray2.h>
#include <Core/Transform/Transform2.h>
#include <Core/Vector/Vector2.h>

#include <limits>
#include <memory>

namespace CubbyFlow
{
	//! Structure that represents ray-surface intersection point.
	struct SurfaceRayIntersection2
	{
		bool isIntersecting = false;
		double distance = std::numeric_limits<double>::max();
		Vector2D point;
		Vector2D normal;
	};

	//! Abstract base class for 2-D surface.
	class Surface2
	{
	public:
		//! Local-to-world transform.
		Transform2 transform;

		//! Flips normal.
		bool isNormalFlipped = false;

		//! Constructs a surface with normal direction.
		Surface2(
			const Transform2& transform = Transform2(),
			bool isNormalFlipped = false);

		//! Copy constructor.
		Surface2(const Surface2& other);

		//! Default destructor.
		virtual ~Surface2();

		//! Returns the closest point from the given point \p otherPoint to the surface.
		Vector2D ClosestPoint(const Vector2D& otherPoint) const;

		//! Returns the bounding box of this surface object.
		BoundingBox2D BoundingBox() const;

		//! Returns true if the given \p ray intersects with this surface object.
		bool Intersects(const Ray2D& ray) const;

		//! Returns the closest distance from the given point \p otherPoint to the
		//! point on the surface.
		double ClosestDistance(const Vector2D& otherPoint) const;

		//! Returns the closest intersection point for given \p ray.
		SurfaceRayIntersection2 ClosestIntersection(const Ray2D& ray) const;

		//! Returns the normal to the closest point on the surface from the given
		//! point \p otherPoint.
		Vector2D ClosestNormal(const Vector2D& otherPoint) const;

		//! Updates internal spatial query engine.
		virtual void UpdateQueryEngine();

	protected:
		//! Returns the closest point from the given point \p otherPoint to the
		//! surface in local frame.
		virtual Vector2D ClosestPointLocal(const Vector2D& otherPoint) const = 0;

		//! Returns the bounding box of this surface object in local frame.
		virtual BoundingBox2D BoundingBoxLocal() const = 0;

		//! Returns the closest intersection point for given \p ray in local frame.
		virtual SurfaceRayIntersection2 ClosestIntersectionLocal(const Ray2D& ray) const = 0;

		//! Returns the normal to the closest point on the surface from the given
		//! point \p otherPoint in local frame.
		virtual Vector2D ClosestNormalLocal(const Vector2D& otherPoint) const = 0;

		//! Returns true if the given \p ray intersects with this surface object
		//! in local frame.
		virtual bool IntersectsLocal(const Ray2D& ray) const;

		//! Returns the closest distance from the given point \p otherPoint to the
		//! point on the surface in local frame.
		virtual double ClosestDistanceLocal(const Vector2D& otherPoint) const;
	};

	//! Shared pointer for the Surface2 type.
	using Surface2Ptr = std::shared_ptr<Surface2>;

	//!
	//! \brief Base class for 2-D surface builder.
	//!
	template <typename DerivedBuilder>
	class SurfaceBuilderBase2
	{
	public:
		//! Returns builder with flipped normal flag.
		DerivedBuilder& WithIsNormalFlipped(bool isNormalFlipped);

		//! Returns builder with translation.
		DerivedBuilder& WithTranslation(const Vector2D& translation);

		//! Returns builder with orientation.
		DerivedBuilder& WithOrientation(double orientation);

		//! Returns builder with transform.
		DerivedBuilder& WithTransform(const Transform2& transform);

	protected:
		bool m_isNormalFlipped = false;
		Transform2 m_transform;
	};

	template <typename T>
	T& SurfaceBuilderBase2<T>::WithIsNormalFlipped(bool isNormalFlipped)
	{
		m_isNormalFlipped = isNormalFlipped;
		return static_cast<T&>(*this);
	}

	template <typename T>
	T& SurfaceBuilderBase2<T>::WithTranslation(const Vector2D& translation)
	{
		m_transform.SetTranslation(translation);
		return static_cast<T&>(*this);
	}

	template <typename T>
	T& SurfaceBuilderBase2<T>::WithOrientation(double orientation)
	{
		m_transform.SetOrientation(orientation);
		return static_cast<T&>(*this);
	}

	template <typename T>
	T& SurfaceBuilderBase2<T>::WithTransform(const Transform2& transform)
	{
		m_transform = transform;
		return static_cast<T&>(*this);
	}
}

#endif
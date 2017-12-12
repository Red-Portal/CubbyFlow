/*************************************************************************
> File Name: Cylinder3.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 3-D cylinder geometry.
> Created Time: 2017/06/24
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Geometry/Box2.h>
#include <Geometry/Cylinder3.h>
#include <Geometry/Plane3.h>

namespace CubbyFlow
{
	Cylinder3::Cylinder3(const Transform3& transform, bool isNormalFlipped) :
		Surface3(transform, isNormalFlipped)
	{
		// Do nothing
	}

	Cylinder3::Cylinder3(const Vector3D& center_, double radius_, double height_,
		const Transform3& transform, bool isNormalFlipped) :
		Surface3(transform, isNormalFlipped), center(center_), radius(radius_), height(height_)
	{
		// Do nothing
	}

	Cylinder3::Cylinder3(const Cylinder3& other) :
		Surface3(other), center(other.center), radius(other.radius), height(other.height)
	{
		// Do nothing
	}

	Vector3D Cylinder3::ClosestPointLocal(const Vector3D& otherPoint) const
	{
		Vector3D r = otherPoint - center;
		Vector2D rr(std::sqrt(r.x * r.x + r.z * r.z), r.y);
		Box2 box(Vector2D(-radius, -0.5 * height), Vector2D(radius, 0.5 * height));

		Vector2D cp = box.ClosestPoint(rr);
		double angle = std::atan2(r.z, r.x);
		
		return Vector3D(cp.x * std::cos(angle), cp.y, cp.x * std::sin(angle)) + center;
	}

	double Cylinder3::ClosestDistanceLocal(const Vector3D& otherPoint) const
	{
		Vector3D r = otherPoint - center;
		Vector2D rr(std::sqrt(r.x * r.x + r.z * r.z), r.y);
		Box2 box(Vector2D(-radius, -0.5 * height), Vector2D(radius, 0.5 * height));

		return box.ClosestDistance(rr);
	}

	Vector3D Cylinder3::ClosestNormalLocal(const Vector3D& otherPoint) const
	{
		Vector3D r = otherPoint - center;
		Vector2D rr(std::sqrt(r.x * r.x + r.z * r.z), r.y);
		Box2 box(Vector2D(-radius, -0.5 * height), Vector2D(radius, 0.5 * height));

		Vector2D cn = box.ClosestNormal(rr);
		if (cn.y > 0)
		{
			return Vector3D(0, 1, 0);
		}
		if (cn.y < 0)
		{
			return Vector3D(0, -1, 0);
		}
		
		return Vector3D(r.x, 0, r.z).Normalized();
	}

	bool Cylinder3::IntersectsLocal(const Ray3D& ray) const
	{
		// Calculate intersection with infinite cylinder
		// (dx^2 + dz^2)t^2 + 2(ox.dx + oz.dz)t + ox^2 + oz^2 - r^2 = 0
		Vector3D d = ray.direction;
		d.y = 0.0;
		Vector3D o = ray.origin - center;
		o.y = 0.0;
		double A = d.LengthSquared();
		double B = d.Dot(o);
		double C = o.LengthSquared() - Square(radius);

		BoundingBox3D bbox = BoundingBox();
		Plane3 upperPlane(Vector3D(0, 1, 0), bbox.upperCorner);
		Plane3 lowerPlane(Vector3D(0, -1, 0), bbox.lowerCorner);

		SurfaceRayIntersection3 upperIntersection = upperPlane.ClosestIntersection(ray);
		SurfaceRayIntersection3 lowerIntersection = lowerPlane.ClosestIntersection(ray);

		// In case the ray does not intersect with infinite cylinder
		if (A < std::numeric_limits<double>::epsilon() || B * B - A * C < 0.0)
		{
			// Check if the ray is inside the infinite cylinder
			Vector3D r = ray.origin - center;
			Vector2D rr(r.x, r.z);
			
			if (rr.LengthSquared() <= Square(radius))
			{
				if (upperIntersection.isIntersecting || lowerIntersection.isIntersecting)
				{
					return true;
				}
			}

			return false;
		}

		double t1 = (-B + std::sqrt(B * B - A * C)) / A;
		double t2 = (-B - std::sqrt(B * B - A * C)) / A;
		double tCylinder = t2;

		if (t2 < 0.0)
		{
			tCylinder = t1;
		}

		Vector3D pointOnCylinder = ray.PointAt(tCylinder);

		if (pointOnCylinder.y >= center.y - 0.5 * height ||
			pointOnCylinder.y <= center.y + 0.5 * height)
		{
			return true;
		}

		if (upperIntersection.isIntersecting)
		{
			Vector3D r = upperIntersection.point - center;
			r.y = 0.0;

			if (r.LengthSquared() <= Square(radius))
			{
				return true;
			}
		}

		if (lowerIntersection.isIntersecting) {
			Vector3D r = lowerIntersection.point - center;
			r.y = 0.0;

			if (r.LengthSquared() <= Square(radius))
			{
				return true;
			}
		}

		return false;
	}

	SurfaceRayIntersection3 Cylinder3::ClosestIntersectionLocal(const Ray3D& ray) const
	{
		SurfaceRayIntersection3 intersection;

		// Calculate intersection with infinite cylinder
		// (dx^2 + dz^2)t^2 + 2(ox.dx + oz.dz)t + ox^2 + oz^2 - r^2 = 0
		Vector3D d = ray.direction;
		d.y = 0.0;
		Vector3D o = ray.origin - center;
		o.y = 0.0;
		double A = d.LengthSquared();
		double B = d.Dot(o);
		double C = o.LengthSquared() - Square(radius);

		BoundingBox3D bbox = BoundingBox();
		Plane3 upperPlane(Vector3D(0, 1, 0), bbox.upperCorner);
		Plane3 lowerPlane(Vector3D(0, -1, 0), bbox.lowerCorner);

		SurfaceRayIntersection3 upperIntersection = upperPlane.ClosestIntersection(ray);
		SurfaceRayIntersection3 lowerIntersection =	lowerPlane.ClosestIntersection(ray);

		intersection.distance = std::numeric_limits<double>::max();
		intersection.isIntersecting = false;

		// In case the ray does not intersect with infinite cylinder
		if (A < std::numeric_limits<double>::epsilon() || B * B - A * C < 0.0)
		{
			// Check if the ray is inside the infinite cylinder
			Vector3D r = ray.origin - center;
			Vector2D rr(r.x, r.z);
			
			if (rr.LengthSquared() <= Square(radius))
			{
				if (upperIntersection.isIntersecting)
				{
					intersection = upperIntersection;
				}
				if (lowerIntersection.isIntersecting && lowerIntersection.distance < intersection.distance)
				{
					intersection = lowerIntersection;
				}
			}

			return intersection;
		}

		double t1 = (-B + std::sqrt(B * B - A * C)) / A;
		double t2 = (-B - std::sqrt(B * B - A * C)) / A;
		double tCylinder = t2;

		if (t2 < 0.0)
		{
			tCylinder = t1;
		}

		Vector3D pointOnCylinder = ray.PointAt(tCylinder);

		if (pointOnCylinder.y >= center.y - 0.5 * height ||
			pointOnCylinder.y <= center.y + 0.5 * height)
		{
			intersection.isIntersecting = true;
			intersection.distance = tCylinder;
			intersection.point = pointOnCylinder;
			intersection.normal = pointOnCylinder - center;
			intersection.normal.y = 0.0;
			intersection.normal.Normalize();
		}

		if (upperIntersection.isIntersecting)
		{
			Vector3D r = upperIntersection.point - center;
			r.y = 0.0;

			if (r.LengthSquared() > Square(radius))
			{
				// Do nothing
			}
			else if (upperIntersection.distance < intersection.distance)
			{
				intersection = upperIntersection;
			}
		}

		if (lowerIntersection.isIntersecting)
		{
			Vector3D r = lowerIntersection.point - center;
			r.y = 0.0;
			
			if (r.LengthSquared() > Square(radius))
			{
                // Do nothing
			}
			else if (lowerIntersection.distance < intersection.distance)
			{
				intersection = lowerIntersection;
			}
		}

		return intersection;
	}

	BoundingBox3D Cylinder3::BoundingBoxLocal() const
	{
		return BoundingBox3D(
			center - Vector3D(radius, 0.5 * height, radius),
			center + Vector3D(radius, 0.5 * height, radius));
	}

	Cylinder3::Builder Cylinder3::GetBuilder()
	{
		return Builder();
	}

	Cylinder3::Builder& Cylinder3::Builder::WithCenter(const Vector3D& center)
	{
		m_center = center;
		return *this;
	}

	Cylinder3::Builder& Cylinder3::Builder::WithRadius(double radius)
	{
		m_radius = radius;
		return *this;
	}

	Cylinder3::Builder& Cylinder3::Builder::WithHeight(double height)
	{
		m_height = height;
		return *this;
	}

	Cylinder3 Cylinder3::Builder::Build() const
	{
		return Cylinder3(m_center, m_radius, m_height, m_transform, m_isNormalFlipped);
	}

	Cylinder3Ptr Cylinder3::Builder::MakeShared() const
	{
		return std::shared_ptr<Cylinder3>(
			new Cylinder3(m_center, m_radius, m_height, m_transform, m_isNormalFlipped),
			[](Cylinder3* obj)
		{
			delete obj;
		});
	}
}
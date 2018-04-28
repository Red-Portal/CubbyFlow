#include "pch.h"
#include "UnitTestsUtils.h"

#include <Core/Geometry/Sphere3.h>
#include <Core/Surface/SurfaceSet3.h>

using namespace CubbyFlow;

TEST(SurfaceSet3, Constructors)
{
	SurfaceSet3 sset1;
	EXPECT_EQ(0u, sset1.NumberOfSurfaces());

	auto sph1 = Sphere3::Builder().WithRadius(1.0).WithCenter({ 0, 0, 0 }).MakeShared();
	auto sph2 = Sphere3::Builder().WithRadius(0.5).WithCenter({ 0, 3, 2 }).MakeShared();
	auto sph3 = Sphere3::Builder().WithRadius(0.25).WithCenter({ -2, 0, 0 }).MakeShared();
	SurfaceSet3 sset2({ sph1, sph2, sph3 }, Transform3(), false);
	EXPECT_EQ(3u, sset2.NumberOfSurfaces());
	EXPECT_EQ(sph1->radius, std::dynamic_pointer_cast<Sphere3>(sset2.SurfaceAt(0))->radius);
	EXPECT_EQ(sph2->radius, std::dynamic_pointer_cast<Sphere3>(sset2.SurfaceAt(1))->radius);
	EXPECT_EQ(sph3->radius, std::dynamic_pointer_cast<Sphere3>(sset2.SurfaceAt(2))->radius);
	EXPECT_EQ(Vector3D(), sset2.transform.GetTranslation());
	EXPECT_EQ(QuaternionD(), sset2.transform.GetOrientation());

	SurfaceSet3 sset3({ sph1, sph2, sph3 }, Transform3(Vector3D(1, 2, 3), QuaternionD({ 1, 0, 0 }, 0.5)), false);
	EXPECT_EQ(Vector3D(1, 2, 3), sset3.transform.GetTranslation());
	EXPECT_EQ(QuaternionD({ 1, 0, 0 }, 0.5), sset3.transform.GetOrientation());
}

TEST(SurfaceSet3, AddSurface)
{
	SurfaceSet3 sset1;
	EXPECT_EQ(0u, sset1.NumberOfSurfaces());

	auto sph1 = Sphere3::Builder().WithRadius(1.0).WithCenter({ 0, 0, 0 }).MakeShared();
	auto sph2 = Sphere3::Builder().WithRadius(0.5).WithCenter({ 0, 3, 2 }).MakeShared();
	auto sph3 = Sphere3::Builder().WithRadius(0.25).WithCenter({ -2, 0, 0 }).MakeShared();

	sset1.AddSurface(sph1);
	sset1.AddSurface(sph2);
	sset1.AddSurface(sph3);

	EXPECT_EQ(3u, sset1.NumberOfSurfaces());
	EXPECT_EQ(sph1->radius, std::dynamic_pointer_cast<Sphere3>(sset1.SurfaceAt(0))->radius);
	EXPECT_EQ(sph2->radius, std::dynamic_pointer_cast<Sphere3>(sset1.SurfaceAt(1))->radius);
	EXPECT_EQ(sph3->radius, std::dynamic_pointer_cast<Sphere3>(sset1.SurfaceAt(2))->radius);
	EXPECT_EQ(Vector3D(), sset1.transform.GetTranslation());
	EXPECT_EQ(QuaternionD(), sset1.transform.GetOrientation());
}

TEST(SurfaceSet3, ClosestPoint)
{
	SurfaceSet3 sset1;

	// Test empty set
	Vector3D emptyPoint = sset1.ClosestPoint({ 1.0, 2.0, 3.0 });
	EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), emptyPoint.x);
	EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), emptyPoint.y);
	EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), emptyPoint.z);

	size_t numSamples = GetNumberOfSamplePoints3();

	// Use first half of the samples as the centers of the spheres
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter(GetSamplePoints3()[i])
			.MakeShared();
		sset1.AddSurface(sph);
	}

	const auto bruteForceSearch = [&](const Vector3D& pt)
	{
		double minDist2 = std::numeric_limits<double>::max();
		Vector3D result;

		for (size_t i = 0; i < numSamples / 2; ++i)
		{
			auto localResult = sset1.SurfaceAt(i)->ClosestPoint(pt);
			double localDist2 = pt.DistanceSquaredTo(localResult);
			
			if (localDist2 < minDist2)
			{
				minDist2 = localDist2;
				result = localResult;
			}
		}

		return result;
	};

	// Use second half of the samples as the query points
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		auto actual = sset1.ClosestPoint(GetSamplePoints3()[i]);
		auto expected = bruteForceSearch(GetSamplePoints3()[i]);
		EXPECT_VECTOR3_EQ(expected, actual);
	}

	// Now With GetTranslation instead of center
	SurfaceSet3 sset2;
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter({ 0, 0, 0 })
			.WithTranslation(GetSamplePoints3()[i])
			.MakeShared();
		sset2.AddSurface(sph);
	}
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		auto actual = sset2.ClosestPoint(GetSamplePoints3()[i]);
		auto expected = bruteForceSearch(GetSamplePoints3()[i]);
		EXPECT_VECTOR3_EQ(expected, actual);
	}
}

TEST(SurfaceSet3, ClosestNormal)
{
	SurfaceSet3 sset1;

	// Test empty set
	Vector3D emptyNormal = sset1.ClosestNormal({ 1.0, 2.0, 3.0 });
	// No excepted value -- just see if it doesn't crash
	(void)emptyNormal;

	size_t numSamples = GetNumberOfSamplePoints3();

	// Use first half of the samples as the centers of the spheres
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter(GetSamplePoints3()[i])
			.MakeShared();
		sset1.AddSurface(sph);
	}

	const auto bruteForceSearch = [&](const Vector3D& pt)
	{
		double minDist2 = std::numeric_limits<double>::max();
		Vector3D result;

		for (size_t i = 0; i < numSamples / 2; ++i)
		{
			auto localResult = sset1.SurfaceAt(i)->ClosestNormal(pt);
			auto ClosestPt = sset1.SurfaceAt(i)->ClosestPoint(pt);
			double localDist2 = pt.DistanceSquaredTo(ClosestPt);
			
			if (localDist2 < minDist2)
			{
				minDist2 = localDist2;
				result = localResult;
			}
		}

		return result;
	};

	// Use second half of the samples as the query points
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		auto actual = sset1.ClosestNormal(GetSamplePoints3()[i]);
		auto expected = bruteForceSearch(GetSamplePoints3()[i]);
		EXPECT_VECTOR3_EQ(expected, actual);
	}

	// Now With GetTranslation instead of center
	SurfaceSet3 sset2;
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter({ 0, 0, 0 })
			.WithTranslation(GetSamplePoints3()[i])
			.MakeShared();
		sset2.AddSurface(sph);
	}
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		auto actual = sset2.ClosestNormal(GetSamplePoints3()[i]);
		auto expected = bruteForceSearch(GetSamplePoints3()[i]);
		EXPECT_VECTOR3_EQ(expected, actual);
	}
}

TEST(SurfaceSet3, ClosestDistance)
{
	SurfaceSet3 sset1;

	size_t numSamples = GetNumberOfSamplePoints3();

	// Use first half of the samples as the centers of the spheres
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter(GetSamplePoints3()[i])
			.MakeShared();
		sset1.AddSurface(sph);
	}

	const auto bruteForceSearch = [&](const Vector3D& pt)
	{
		double minDist = std::numeric_limits<double>::max();
		
		for (size_t i = 0; i < numSamples / 2; ++i)
		{
			double localDist = sset1.SurfaceAt(i)->ClosestDistance(pt);
			if (localDist < minDist)
			{
				minDist = localDist;
			}
		}

		return minDist;
	};

	// Use second half of the samples as the query points
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		auto actual = sset1.ClosestDistance(GetSamplePoints3()[i]);
		auto expected = bruteForceSearch(GetSamplePoints3()[i]);
		EXPECT_DOUBLE_EQ(expected, actual);
	}

	// Now With GetTranslation instead of center
	SurfaceSet3 sset2;
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter({ 0, 0, 0 })
			.WithTranslation(GetSamplePoints3()[i])
			.MakeShared();
		sset2.AddSurface(sph);
	}
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		auto actual = sset2.ClosestDistance(GetSamplePoints3()[i]);
		auto expected = bruteForceSearch(GetSamplePoints3()[i]);
		EXPECT_DOUBLE_EQ(expected, actual);
	}
}

TEST(SurfaceSet3, Intersects)
{
	SurfaceSet3 sset1;

	size_t numSamples = GetNumberOfSamplePoints3();

	// Use first half of the samples as the centers of the spheres
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter(GetSamplePoints3()[i])
			.MakeShared();
		sset1.AddSurface(sph);
	}

	const auto bruteForceTest = [&](const Ray3D& ray)
	{
		for (size_t i = 0; i < numSamples / 2; ++i)
		{
			if (sset1.SurfaceAt(i)->Intersects(ray))
			{
				return true;
			}
		}

		return false;
	};

	// Use second half of the samples as the query points
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		Ray3D ray(GetSamplePoints3()[i], GetSampleDirs3()[i]);
		bool actual = sset1.Intersects(ray);
		bool expected = bruteForceTest(ray);
		EXPECT_EQ(expected, actual);
	}

	// Now With GetTranslation instead of center
	SurfaceSet3 sset2;
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter({ 0, 0, 0 })
			.WithTranslation(GetSamplePoints3()[i])
			.MakeShared();
		sset2.AddSurface(sph);
	}
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		Ray3D ray(GetSamplePoints3()[i], GetSampleDirs3()[i]);
		bool actual = sset2.Intersects(ray);
		bool expected = bruteForceTest(ray);
		EXPECT_EQ(expected, actual);
	}
}

TEST(SurfaceSet3, ClosestIntersection)
{
	SurfaceSet3 sset1;

	size_t numSamples = GetNumberOfSamplePoints3();

	// Use first half of the samples as the centers of the spheres
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter(GetSamplePoints3()[i])
			.MakeShared();
		sset1.AddSurface(sph);
	}

	const auto bruteForceTest = [&](const Ray3D& ray)
	{
		SurfaceRayIntersection3 result{};
		
		for (size_t i = 0; i < numSamples / 2; ++i)
		{
			auto localResult = sset1.SurfaceAt(i)->ClosestIntersection(ray);
			if (localResult.distance < result.distance)
			{
				result = localResult;
			}
		}

		return result;
	};

	// Use second half of the samples as the query points
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		Ray3D ray(GetSamplePoints3()[i], GetSampleDirs3()[i]);
		auto actual = sset1.ClosestIntersection(ray);
		auto expected = bruteForceTest(ray);
		EXPECT_DOUBLE_EQ(expected.distance, actual.distance);
		EXPECT_VECTOR3_EQ(expected.point, actual.point);
		EXPECT_VECTOR3_EQ(expected.normal, actual.normal);
		EXPECT_EQ(expected.isIntersecting, actual.isIntersecting);
	}

	// Now With GetTranslation instead of center
	SurfaceSet3 sset2;
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter({ 0, 0, 0 })
			.WithTranslation(GetSamplePoints3()[i])
			.MakeShared();
		sset2.AddSurface(sph);
	}
	for (size_t i = numSamples / 2; i < numSamples; ++i)
	{
		Ray3D ray(GetSamplePoints3()[i], GetSampleDirs3()[i]);
		auto actual = sset2.ClosestIntersection(ray);
		auto expected = bruteForceTest(ray);
		EXPECT_DOUBLE_EQ(expected.distance, actual.distance);
		EXPECT_VECTOR3_EQ(expected.point, actual.point);
		EXPECT_VECTOR3_EQ(expected.normal, actual.normal);
		EXPECT_EQ(expected.isIntersecting, actual.isIntersecting);
	}
}

TEST(SurfaceSet3, BoundingBox)
{
	SurfaceSet3 sset1;

	size_t numSamples = GetNumberOfSamplePoints3();

	// Use first half of the samples as the centers of the spheres
	BoundingBox3D answer;
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter(GetSamplePoints3()[i])
			.MakeShared();
		sset1.AddSurface(sph);

		answer.Merge(sph->BoundingBox());
	}

	EXPECT_BOUNDING_BOX3_NEAR(answer, sset1.BoundingBox(), 1e-9);

	// Now With GetTranslation instead of center
	SurfaceSet3 sset2;
	BoundingBox3D debug;
	for (size_t i = 0; i < numSamples / 2; ++i)
	{
		auto sph = Sphere3::Builder()
			.WithRadius(0.01)
			.WithCenter({ 0, 0, 0 })
			.WithTranslation(GetSamplePoints3()[i])
			.MakeShared();
		sset2.AddSurface(sph);

		debug.Merge(sph->BoundingBox());
	}

	EXPECT_BOUNDING_BOX3_NEAR(answer, debug, 1e-9);
	EXPECT_BOUNDING_BOX3_NEAR(answer, sset2.BoundingBox(), 1e-9);
}
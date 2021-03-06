/*************************************************************************
> File Name: TriangleMesh3.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 3-D triangle mesh geometry.
> Created Time: 2017/04/15
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <Core/Geometry/TriangleMesh3.h>
#include <Core/Math/MathUtils.h>
#include <Core/Utils/Parallel.h>

#include <obj/obj_parser.hpp>

#include <cassert>
#include <iostream>

namespace CubbyFlow
{
	inline std::ostream& operator<<(std::ostream& stream, const Vector2D& v)
	{
		stream << v.x << ' ' << v.y;
		return stream;
	}

	inline std::ostream& operator<<(std::ostream& stream, const Vector3D& v)
	{
		stream << v.x << ' ' << v.y << ' ' << v.z;
		return stream;
	}

	TriangleMesh3::TriangleMesh3(const Transform3& transform_, bool isNormalFlipped_) :
		Surface3(transform_, isNormalFlipped_)
	{
		// Do nothing
	}

	TriangleMesh3::TriangleMesh3(
		const PointArray& points,
		const NormalArray& normals,
		const UVArray& uvs,
		const IndexArray& pointIndices,
		const IndexArray& normalIndices,
		const IndexArray& uvIndices,
		const Transform3& transform_,
		bool isNormalFlipped_) :
		Surface3(transform_, isNormalFlipped_), m_points(points), m_normals(normals), m_uvs(uvs),
		m_pointIndices(pointIndices), m_normalIndices(normalIndices), m_uvIndices(uvIndices)
	{
		// Do nothing
	}

	TriangleMesh3::TriangleMesh3(const TriangleMesh3& other) :
		Surface3(other)
	{
		Set(other);
	}

	void TriangleMesh3::UpdateQueryEngine()
	{
		BuildBVH();
	}

	Vector3D TriangleMesh3::ClosestPointLocal(const Vector3D& otherPoint) const
	{
		BuildBVH();

		const auto distanceFunc = [this](const size_t& triIdx, const Vector3D& pt)
		{
			Triangle3 tri = Triangle(triIdx);
			return tri.ClosestDistance(pt);
		};

		const auto queryResult = m_bvh.GetNearestNeighbor(otherPoint, distanceFunc);
		return Triangle(*queryResult.item).ClosestPoint(otherPoint);
	}

	Vector3D TriangleMesh3::ClosestNormalLocal(const Vector3D& otherPoint) const
	{
		BuildBVH();

		const auto distanceFunc = [this](const size_t& triIdx, const Vector3D& pt)
		{
			Triangle3 tri = Triangle(triIdx);
			return tri.ClosestDistance(pt);
		};

		const auto queryResult = m_bvh.GetNearestNeighbor(otherPoint, distanceFunc);
		return Triangle(*queryResult.item).ClosestNormal(otherPoint);
	}

	SurfaceRayIntersection3 TriangleMesh3::ClosestIntersectionLocal(const Ray3D& ray) const
	{
		BuildBVH();

		const auto testFunc = [this](const size_t& triIdx, const Ray3D& ray)
		{
			Triangle3 tri = Triangle(triIdx);
			SurfaceRayIntersection3 result = tri.ClosestIntersection(ray);

			return result.distance;
		};

		const auto queryResult = m_bvh.GetClosestIntersection(ray, testFunc);

		SurfaceRayIntersection3 result;
		result.distance = queryResult.distance;
		result.isIntersecting = queryResult.item != nullptr;

		if (queryResult.item != nullptr)
		{
			result.point = ray.PointAt(queryResult.distance);
			result.normal = Triangle(*queryResult.item).ClosestNormal(result.point);
		}

		return result;
	}

	BoundingBox3D TriangleMesh3::BoundingBoxLocal() const
	{
		BuildBVH();

		return m_bvh.GetBoundingBox();
	}

	bool TriangleMesh3::IntersectsLocal(const Ray3D& ray) const
	{
		BuildBVH();

		const auto testFunc = [this](const size_t& triIdx, const Ray3D& ray)
		{
			Triangle3 tri = Triangle(triIdx);
			return tri.Intersects(ray);
		};

		return m_bvh.IsIntersects(ray, testFunc);
	}

	double TriangleMesh3::ClosestDistanceLocal(const Vector3D& otherPoint) const
	{
		BuildBVH();

		const auto distanceFunc = [this](const size_t& triIdx, const Vector3D& pt)
		{
			Triangle3 tri = Triangle(triIdx);
			return tri.ClosestDistance(pt);
		};

		const auto queryResult = m_bvh.GetNearestNeighbor(otherPoint, distanceFunc);
		return queryResult.distance;
	}

	void TriangleMesh3::Clear()
	{
		m_points.Clear();
		m_normals.Clear();
		m_uvs.Clear();
		m_pointIndices.Clear();
		m_normalIndices.Clear();
		m_uvIndices.Clear();

		InvalidateBVH();
	}

	void TriangleMesh3::Set(const TriangleMesh3& other)
	{
		m_points.Set(other.m_points);
		m_normals.Set(other.m_normals);
		m_uvs.Set(other.m_uvs);
		m_pointIndices.Set(other.m_pointIndices);
		m_normalIndices.Set(other.m_normalIndices);
		m_uvIndices.Set(other.m_uvIndices);

		InvalidateBVH();
	}

	void TriangleMesh3::Swap(TriangleMesh3& other)
	{
		m_points.Swap(other.m_points);
		m_normals.Swap(other.m_normals);
		m_uvs.Swap(other.m_uvs);
		m_pointIndices.Swap(other.m_pointIndices);
		m_normalIndices.Swap(other.m_normalIndices);
		m_uvIndices.Swap(other.m_uvIndices);
	}

	double TriangleMesh3::Area() const
	{
		double a = 0;

		for (size_t i = 0; i < NumberOfTriangles(); ++i)
		{
			Triangle3 tri = Triangle(i);
			a += tri.Area();
		}
		
		return a;
	}

	double TriangleMesh3::Volume() const
	{
		double vol = 0;

		for (size_t i = 0; i < NumberOfTriangles(); ++i)
		{
			Triangle3 tri = Triangle(i);
			vol += tri.points[0].Dot(tri.points[1].Cross(tri.points[2])) / 6.f;
		}

		return vol;
	}

	const Vector3D& TriangleMesh3::Point(size_t i) const
	{
		return m_points[i];
	}

	Vector3D& TriangleMesh3::Point(size_t i)
	{
		InvalidateBVH();
		return m_points[i];
	}

	const Vector3D& TriangleMesh3::Normal(size_t i) const
	{
		return m_normals[i];
	}

	Vector3D& TriangleMesh3::Normal(size_t i)
	{
		return m_normals[i];
	}

	const Vector2D& TriangleMesh3::UV(size_t i) const
	{
		return m_uvs[i];
	}

	Vector2D& TriangleMesh3::UV(size_t i)
	{
		return m_uvs[i];
	}

	const Point3UI& TriangleMesh3::PointIndex(size_t i) const
	{
		return m_pointIndices[i];
	}

	Point3UI& TriangleMesh3::PointIndex(size_t i)
	{
		return m_pointIndices[i];
	}

	const Point3UI& TriangleMesh3::NormalIndex(size_t i) const
	{
		return m_normalIndices[i];
	}

	Point3UI& TriangleMesh3::NormalIndex(size_t i)
	{
		return m_normalIndices[i];
	}

	const Point3UI& TriangleMesh3::UVIndex(size_t i) const
	{
		return m_uvIndices[i];
	}

	Point3UI& TriangleMesh3::UVIndex(size_t i)
	{
		return m_uvIndices[i];
	}

	Triangle3 TriangleMesh3::Triangle(size_t i) const
	{
		Triangle3 tri;

		for (int j = 0; j < 3; ++j)
		{
			tri.points[j] = m_points[m_pointIndices[i][j]];
			
			if (HasUVs())
			{
				tri.uvs[j] = m_uvs[m_uvIndices[i][j]];
			}
		}

		Vector3D n = tri.FaceNormal();

		for (int j = 0; j < 3; ++j)
		{
			if (HasNormals())
			{
				tri.normals[j] = m_normals[m_normalIndices[i][j]];
			}
			else
			{
				tri.normals[j] = n;
			}
		}

		return tri;
	}

	size_t TriangleMesh3::NumberOfPoints() const
	{
		return m_points.size();
	}

	size_t TriangleMesh3::NumberOfNormals() const
	{
		return m_normals.size();
	}

	size_t TriangleMesh3::NumberOfUVs() const
	{
		return m_uvs.size();
	}

	size_t TriangleMesh3::NumberOfTriangles() const
	{
		return m_pointIndices.size();
	}

	bool TriangleMesh3::HasNormals() const
	{
		return m_normals.size() > 0;
	}

	bool TriangleMesh3::HasUVs() const
	{
		return m_uvs.size() > 0;
	}

	void TriangleMesh3::AddPoint(const Vector3D& pt)
	{
		m_points.Append(pt);
	}

	void TriangleMesh3::AddNormal(const Vector3D& n)
	{
		m_normals.Append(n);
	}

	void TriangleMesh3::AddUV(const Vector2D& t)
	{
		m_uvs.Append(t);
	}

	void TriangleMesh3::AddPointTriangle(const Point3UI& newPointIndices)
	{
		m_pointIndices.Append(newPointIndices);
		InvalidateBVH();
	}

	void TriangleMesh3::AddPointNormalTriangle(
		const Point3UI& newPointIndices,
		const Point3UI& newNormalIndices)
	{
		// Number of normal indices must match with number of point indices once
		// you decided to add normal indices. Same for the uvs as well.
		assert(m_pointIndices.size() == m_normalIndices.size());

		m_pointIndices.Append(newPointIndices);
		m_normalIndices.Append(newNormalIndices);

		InvalidateBVH();
	}

	void TriangleMesh3::AddPointUVNormalTriangle(
		const Point3UI& newPointIndices,
		const Point3UI& newUVIndices,
		const Point3UI& newNormalIndices)
	{
		// Number of normal indices must match with number of point indices once
		// you decided to add normal indices. Same for the uvs as well.
		assert(m_pointIndices.size() == m_normalIndices.size());
		assert(m_pointIndices.size() == m_uvIndices.size());
		m_pointIndices.Append(newPointIndices);
		m_normalIndices.Append(newNormalIndices);
		m_uvIndices.Append(newUVIndices);

		InvalidateBVH();
	}

	void TriangleMesh3::AddPointUVTriangle(
		const Point3UI& newPointIndices,
		const Point3UI& newUVIndices)
	{
		// Number of normal indices must match with number of point indices once
		// you decided to add normal indices. Same for the uvs as well.
		assert(m_pointIndices.size() == m_uvs.size());
		m_pointIndices.Append(newPointIndices);
		m_uvIndices.Append(newUVIndices);

		InvalidateBVH();
	}

	void TriangleMesh3::AddTriangle(const Triangle3& tri)
	{
		size_t vStart = m_points.size();
		size_t nStart = m_normals.size();
		size_t tStart = m_uvs.size();
		Point3UI newPointIndices;
		Point3UI newNormalIndices;
		Point3UI newUvIndices;

		for (size_t i = 0; i < 3; ++i)
		{
			m_points.Append(tri.points[i]);
			m_normals.Append(tri.normals[i]);
			m_uvs.Append(tri.uvs[i]);

			newPointIndices[i] = vStart + i;
			newNormalIndices[i] = nStart + i;
			newUvIndices[i] = tStart + i;
		}

		m_pointIndices.Append(newPointIndices);
		m_normalIndices.Append(newNormalIndices);
		m_uvIndices.Append(newUvIndices);

		InvalidateBVH();
	}

	void TriangleMesh3::SetFaceNormal()
	{
		m_normals.Resize(m_points.size());
		m_normalIndices.Set(m_pointIndices);

		for (size_t i = 0; i < NumberOfTriangles(); ++i)
		{
			Triangle3 tri = Triangle(i);
			Vector3D n = tri.FaceNormal();
			Point3UI f = m_pointIndices[i];

			m_normals[f.x] = n;
			m_normals[f.y] = n;
			m_normals[f.z] = n;
		}
	}

	void TriangleMesh3::SetAngleWeightedVertexNormal()
	{
		m_normals.Clear();
		m_normalIndices.Clear();

		Array1<double> angleWeights(m_points.size());
		Vector3DArray pseudoNormals(m_points.size());

		for (size_t i = 0; i < m_points.size(); ++i)
		{
			angleWeights[i] = 0;
			pseudoNormals[i] = Vector3D();
		}

		for (size_t i = 0; i < NumberOfTriangles(); ++i)
		{
			Vector3D pts[3];
			size_t idx[3];

			// Quick references
			for (int j = 0; j < 3; ++j)
			{
				idx[j] = m_pointIndices[i][j];
				pts[j] = m_points[idx[j]];
			}

			// Angle for point 0
			Vector3D e0 = pts[1] - pts[0];
			Vector3D e1 = pts[2] - pts[0];
			e0.Normalize();
			e1.Normalize();
			Vector3D normal = e0.Cross(e1);
			normal.Normalize();
			double cosAngle = Clamp(e0.Dot(e1), -1.0, 1.0);
			double angle = std::acos(cosAngle);
			angleWeights[idx[0]] += angle;
			pseudoNormals[idx[0]] += angle*normal;

			// Angle for point 1
			e0 = pts[2] - pts[1];
			e1 = pts[0] - pts[1];
			e0.Normalize();
			e1.Normalize();
			normal = e0.Cross(e1);
			normal.Normalize();
			cosAngle = Clamp(e0.Dot(e1), -1.0, 1.0);
			angle = std::acos(cosAngle);
			angleWeights[idx[1]] += angle;
			pseudoNormals[idx[1]] += angle*normal;

			// Angle for point 2
			e0 = pts[0] - pts[2];
			e1 = pts[1] - pts[2];
			e0.Normalize();
			e1.Normalize();
			normal = e0.Cross(e1);
			normal.Normalize();
			cosAngle = Clamp(e0.Dot(e1), -1.0, 1.0);
			angle = std::acos(cosAngle);
			angleWeights[idx[2]] += angle;
			pseudoNormals[idx[2]] += angle*normal;
		}

		for (size_t i = 0; i < m_points.size(); ++i)
		{
			if (angleWeights[i] > 0)
			{
				pseudoNormals[i] /= angleWeights[i];
			}
		}

		std::swap(pseudoNormals, m_normals);
		m_normalIndices.Set(m_pointIndices);
	}

	void TriangleMesh3::Scale(double factor)
	{
		ParallelFor(ZERO_SIZE, NumberOfPoints(),
			[this, factor](size_t i)
		{
			m_points[i] *= factor;
		});

		InvalidateBVH();
	}

	void TriangleMesh3::Translate(const Vector3D& t)
	{
		ParallelFor(ZERO_SIZE, NumberOfPoints(),
			[this, t](size_t i)
		{
			m_points[i] += t;
		});

		InvalidateBVH();
	}

	void TriangleMesh3::Rotate(const QuaternionD& q)
	{
		ParallelFor(ZERO_SIZE, NumberOfPoints(),
			[this, q](size_t i)
		{
			m_points[i] = q * m_points[i];
		});

		ParallelFor(ZERO_SIZE, NumberOfNormals(),
			[this, q](size_t i)
		{
			m_normals[i] = q * m_normals[i];
		});

		InvalidateBVH();
	}

	void TriangleMesh3::WriteObj(std::ostream* stream) const
	{
		// vertex
		for (const auto& pt : m_points)
		{
			*stream << "v " << pt << std::endl;
		}

		// UV coordinates
		for (const auto& uv : m_uvs)
		{
			*stream << "vt " << uv << std::endl;
		}

		// normals
		for (const auto& n : m_normals)
		{
			*stream << "vn " << n << std::endl;
		}

		// faces
		bool hasUVs = HasUVs();
		bool hasNormals = HasNormals();
		for (size_t i = 0; i < NumberOfTriangles(); ++i)
		{
			*stream << "f ";

			for (int j = 0; j < 3; ++j)
			{
				*stream << m_pointIndices[i][j] + 1;
				
				if (hasNormals || hasUVs)
				{
					*stream << '/';
				}
				
				if (hasUVs)
				{
					*stream << m_uvIndices[i][j] + 1;
				}

				if (hasNormals)
				{
					*stream << '/' << m_normalIndices[i][j] + 1;
				}
				*stream << ' ';
			}

			*stream << std::endl;
		}
	}

	bool TriangleMesh3::WriteObj(const std::string& fileName) const
	{
		std::ofstream file(fileName.c_str());

		if (file)
		{
			WriteObj(&file);
			file.close();

			return true;
		}

		return false;
	}

	bool TriangleMesh3::ReadObj(std::istream* stream)
	{
		obj::obj_parser parser(obj::obj_parser::triangulate_faces | obj::obj_parser::translate_negative_indices);

		parser.info_callback([](size_t lineNumber, const std::string& message)
		{
			std::cout << lineNumber << " " << message << "\n";
		});
		
		parser.warning_callback([](size_t lineNumber, const std::string& message)
		{
			std::cerr << lineNumber << " " << message << "\n";
		});
		
		parser.error_callback([](size_t lineNumber, const std::string& message)
		{
			std::cerr << lineNumber << " " << message << "\n";
		});

		parser.geometric_vertex_callback([this](obj::float_type x, obj::float_type y, obj::float_type z)
		{
			AddPoint({ x, y, z });
		});

		parser.texture_vertex_callback([this](obj::float_type u, obj::float_type v)
		{
			AddUV({ u, v });
		});

		parser.vertex_normal_callback([this](obj::float_type nx, obj::float_type ny, obj::float_type nz)
		{
			AddNormal({ nx, ny, nz });
		});

		parser.face_callbacks(
			// triangular_face_geometric_vertices_callback_type
			[this](obj::index_type v0, obj::index_type v1, obj::index_type v2)
		{
			AddPointTriangle({ v0 - 1, v1 - 1, v2 - 1 });
		},
			// triangular_face_geometric_vertices_texture_vertices_callback_type
			[this](const obj::index_2_tuple_type& v0_vt0,
				const obj::index_2_tuple_type& v1_vt1,
				const obj::index_2_tuple_type& v2_vt2)
		{
			AddPointUVTriangle(
			{
				std::get<0>(v0_vt0) - 1,
				std::get<0>(v1_vt1) - 1,
				std::get<0>(v2_vt2) - 1
			},
			{
				std::get<1>(v0_vt0) - 1,
				std::get<1>(v1_vt1) - 1,
				std::get<1>(v2_vt2) - 1
			});
		},
			// triangular_face_geometric_vertices_vertex_normals_callback_type
			[this](const obj::index_2_tuple_type& v0_vn0,
				const obj::index_2_tuple_type& v1_vn1,
				const obj::index_2_tuple_type& v2_vn2)
		{
			AddPointNormalTriangle(
			{
				std::get<0>(v0_vn0) - 1,
				std::get<0>(v1_vn1) - 1,
				std::get<0>(v2_vn2) - 1
			},
			{
				std::get<1>(v0_vn0) - 1,
				std::get<1>(v1_vn1) - 1,
				std::get<1>(v2_vn2) - 1
			});
		},
			// triangular_face_geometric_vertices_texture_vertices_vertex_normals...
			[this](const obj::index_3_tuple_type& v0_vt0_vn0,
				const obj::index_3_tuple_type& v1_vt1_vn1,
				const obj::index_3_tuple_type& v2_vt2_vn2)
		{
			AddPointUVNormalTriangle(
			{
				std::get<0>(v0_vt0_vn0) - 1,
				std::get<0>(v1_vt1_vn1) - 1,
				std::get<0>(v2_vt2_vn2) - 1
			},
			{
				std::get<1>(v0_vt0_vn0) - 1,
				std::get<1>(v1_vt1_vn1) - 1,
				std::get<1>(v2_vt2_vn2) - 1
			},
			{
				std::get<2>(v0_vt0_vn0) - 1,
				std::get<2>(v1_vt1_vn1) - 1,
				std::get<2>(v2_vt2_vn2) - 1
			});
		},
			// quadrilateral_face_geometric_vertices_callback_type
			[](obj::index_type, obj::index_type, obj::index_type, obj::index_type)
		{
			// Do nothing
		},
			// quadrilateral_face_geometric_vertices_texture_vertices_callback_type
			[](const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&)
		{
			// Do nothing
		},
			// quadrilateral_face_geometric_vertices_vertex_normals_callback_type
			[](const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&)
		{
			// Do nothing
		},
			// quadrilateral_face_geometric_vertices_texture_vertices_vertex_norm...
			[](const obj::index_3_tuple_type&,
				const obj::index_3_tuple_type&,
				const obj::index_3_tuple_type&,
				const obj::index_3_tuple_type&)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_begin_callback_type
			[](obj::index_type, obj::index_type, obj::index_type)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_vertex_callback_type
			[](obj::index_type)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_end_callback_type
			[]()
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_texture_vertices_begin_callback_...
			[](const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_texture_vertices_vertex_callback...
			[](const obj::index_2_tuple_type&)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_texture_vertices_end_callback_type
			[]()
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_vertex_normals_begin_callback_type
			[](const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&,
				const obj::index_2_tuple_type&)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_vertex_normals_vertex_callback_type
			[](const obj::index_2_tuple_type&)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_vertex_normals_end_callback_type
			[]()
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_texture_vertices_vertex_normals_...
			[](const obj::index_3_tuple_type&,
				const obj::index_3_tuple_type&,
				const obj::index_3_tuple_type&)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_texture_vertices_vertex_normals_...
			[](const obj::index_3_tuple_type&)
		{
			// Do nothing
		},
			// polygonal_face_geometric_vertices_texture_vertices_vertex_normals_...
			[]()
		{
			// Do nothing
		});

		parser.group_name_callback([](const std::string&)
		{
			// Do nothing
		});
		parser.smoothing_group_callback([](obj::size_type)
		{
			// Do nothing
		});
		parser.object_name_callback([](const std::string&)
		{
			// Do nothing
		});
		parser.material_library_callback([](const std::string&)
		{
			// Do nothing
		});
		parser.material_name_callback([](const std::string&)
		{
			// Do nothing
		});
		parser.comment_callback([](const std::string&)
		{
			// Do nothing
		});

		InvalidateBVH();

		return parser.parse(*stream);
	}

	bool TriangleMesh3::ReadObj(const std::string& fileName)
	{
		std::ifstream file(fileName.c_str());

		if (file)
		{
			bool result = ReadObj(&file);
			file.close();

			return result;
		}

		return false;
	}

	TriangleMesh3& TriangleMesh3::operator=(const TriangleMesh3& other)
	{
		Set(other);
		return *this;
	}

	TriangleMesh3::Builder TriangleMesh3::GetBuilder()
	{
		return Builder();
	}

	void TriangleMesh3::InvalidateBVH() const
	{
		m_bvhInvalidated = true;
	}

	void TriangleMesh3::BuildBVH() const
	{
		if (m_bvhInvalidated)
		{
			size_t nTris = NumberOfTriangles();

			std::vector<size_t> ids(nTris);
			std::vector<BoundingBox3D> bounds(nTris);
			for (size_t i = 0; i < nTris; ++i)
			{
				ids[i] = i;
				bounds[i] = Triangle(i).BoundingBox();
			}

			m_bvh.Build(ids, bounds);
			m_bvhInvalidated = false;
		}
	}
	
	TriangleMesh3::Builder& TriangleMesh3::Builder::WithPoints(const PointArray& points)
	{
		m_points = points;
		return *this;
	}

	TriangleMesh3::Builder& TriangleMesh3::Builder::WithNormals(const NormalArray& normals)
	{
		m_normals = normals;
		return *this;
	}

	TriangleMesh3::Builder& TriangleMesh3::Builder::WithUVs(const UVArray& uvs)
	{
		m_uvs = uvs;
		return *this;
	}

	TriangleMesh3::Builder& TriangleMesh3::Builder::WithPointIndices(const IndexArray& pointIndices)
	{
		m_pointIndices = pointIndices;
		return *this;
	}

	TriangleMesh3::Builder& TriangleMesh3::Builder::WithNormalIndices(const IndexArray& normalIndices)
	{
		m_normalIndices = normalIndices;
		return *this;
	}

	TriangleMesh3::Builder& TriangleMesh3::Builder::WithUVIndices(const IndexArray& uvIndices)
	{
		m_uvIndices = uvIndices;
		return *this;
	}

	TriangleMesh3 TriangleMesh3::Builder::Build() const
	{
		return TriangleMesh3(
			m_points,
			m_normals,
			m_uvs,
			m_pointIndices,
			m_normalIndices,
			m_uvIndices,
			m_transform,
			m_isNormalFlipped);
	}

	TriangleMesh3Ptr TriangleMesh3::Builder::MakeShared() const
	{
		return std::shared_ptr<TriangleMesh3>(
			new TriangleMesh3(
				m_points,
				m_normals,
				m_uvs,
				m_pointIndices,
				m_normalIndices,
				m_uvIndices,
				m_transform,
				m_isNormalFlipped),
			[](TriangleMesh3* obj)
		{
			delete obj;
		});
	}
}
/*************************************************************************
> File Name: ColliderSet3.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Collection of 3-D colliders.
> Created Time: 2017/05/09
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_COLLIDER_SET3_H
#define CUBBYFLOW_COLLIDER_SET3_H

#include <Core/Collider/Collider3.h>

#include <vector>

namespace CubbyFlow
{
	//! Collection of 3-D colliders
	class ColliderSet3 final : public Collider3
	{
	public:
		class Builder;

		//! Default constructor.
		ColliderSet3();

		//! Constructs with other colliders.
		explicit ColliderSet3(const std::vector<Collider3Ptr>& others);

		//! Returns the velocity of the collider at given \p point.
		Vector3D VelocityAt(const Vector3D& point) const override;

		//! Adds a collider to the set.
		void AddCollider(const Collider3Ptr& collider);

		//! Returns number of colliders.
		size_t NumberOfColliders() const;

		//! Returns collider at index \p i.
		Collider3Ptr Collider(size_t i) const;

		//! Returns builder fox ColliderSet3.
		static Builder GetBuilder();

	private:
		std::vector<Collider3Ptr> m_colliders;
	};

	//! Shared pointer for the ColliderSet3 type.
	using ColliderSet3Ptr = std::shared_ptr<ColliderSet3>;

	//!
	//! \brief Front-end to create ColliderSet3 objects step by step.
	//!
	class ColliderSet3::Builder final
	{
	public:
		//! Returns builder with other colliders.
		Builder& WithColliders(const std::vector<Collider3Ptr>& others);

		//! Builds ColliderSet3.
		ColliderSet3 Build() const;

		//! Builds shared pointer of ColliderSet3 instance.
		ColliderSet3Ptr MakeShared() const;

	private:
		std::vector<Collider3Ptr> m_colliders;
	};
}

#endif
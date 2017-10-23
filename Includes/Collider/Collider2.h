/*************************************************************************
> File Name: Collider2.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Abstract base class for generic collider object.
> Created Time: 2017/05/09
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_COLLIDER2_H
#define CUBBYFLOW_COLLIDER2_H

#include <Surface/Surface2.h>

#include <functional>

namespace CubbyFlow
{
	//!
	//! \brief Abstract base class for generic collider object.
	//!
	//! This class contains basic interfaces for colliders. Most of the
	//! functionalities are implemented within this class, except the member
	//! function Collider2::VelocityAt. This class also let the subclasses to
	//! provide a Surface2 instance to define collider surface using
	//! Collider2::SetSurface function.
	//!
	class Collider2
	{
	public:
		//!
		//! \brief Callback function type for update calls.
		//!
		//! This type of callback function will take the collider pointer, current
		//! time, and time interval in seconds.
		//!
		using OnBeginUpdateCallback = std::function<void(Collider2*, double, double)>;

		//! Default constructor.
		Collider2();

		//! Default destructor.
		virtual ~Collider2();

		//! Returns the velocity of the collider at given \p point.
		virtual Vector2D VelocityAt(const Vector2D& point) const = 0;

		//!
		//! Resolves collision for given point.
		//!
		//! \param radius Radius of the colliding point.
		//! \param restitutionCoefficient Defines the restitution effect.
		//! \param position Input and output position of the point.
		//! \param velocity Input and output velocity of the point.
		//!
		void ResolveCollision(double radius, double restitutionCoefficient, Vector2D* position, Vector2D* velocity);

		//! Returns friction coefficient.
		double FrictionCoefficient() const;

		//!
		//! \brief Sets the friction coefficient.
		//!
		//! This function assigns the friction coefficient to the collider. Any
		//! negative inputs will be clamped to zero.
		//!
		void SetFrictionCoefficient(double newFrictionCoeffient);

		//! Returns the surface instance.
		const Surface2Ptr& Surface() const;

		//! Updates the collider state.
		void Update(double currentTimeInSeconds, double timeIntervalInSeconds);

		//!
		//! \brief      Sets the callback function to be called when
		//!             Collider2::update function is invoked.
		//!
		//! The callback function takes current simulation time in seconds unit. Use
		//! this callback to track any motion or state changes related to this
		//! collider.
		//!
		//! \param[in]  callback The callback function.
		//!
		void SetOnBeginUpdateCallback(const OnBeginUpdateCallback& callback);

	protected:
		//! Internal query result structure.
		struct ColliderQueryResult final
		{
			double distance;
			Vector2D point;
			Vector2D normal;
			Vector2D velocity;
		};

		//! Assigns the surface instance from the subclass.
		void SetSurface(const Surface2Ptr& newSurface);

		//! Outputs closest point's information.
		void GetClosestPoint(const Surface2Ptr& surface, const Vector2D& queryPoint, ColliderQueryResult* result) const;

		//! Returns true if given point is in the opposite side of the surface.
		bool IsPenetrating(const ColliderQueryResult& colliderPoint, const Vector2D& position, double radius);

	private:
		Surface2Ptr m_surface;
		double m_frictionCoeffient = 0.0;
		OnBeginUpdateCallback m_onUpdateCallback;
	};

	//! Shared pointer type for the Collider2.
	using Collider2Ptr = std::shared_ptr<Collider2>;
}

#endif
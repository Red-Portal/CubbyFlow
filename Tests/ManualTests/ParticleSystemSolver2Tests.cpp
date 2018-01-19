#include "pch.h"

#include <ManualTests.h>

#include <Core/Collider/RigidBodyCollider2.h>
#include <Core/Emitter/PointParticleEmitter2.h>
#include <Core/Field/ConstantVectorField2.h>
#include <Core/Geometry/Plane2.h>
#include <Core/Solver/Particle/ParticleSystemSolver2.h>

using namespace CubbyFlow;

CUBBYFLOW_TESTS(ParticleSystemSolver2);

CUBBYFLOW_BEGIN_TEST_F(ParticleSystemSolver2, Update)
{
	Plane2Ptr plane = std::make_shared<Plane2>(Vector2D(0, 1), Vector2D());
	RigidBodyCollider2Ptr collider = std::make_shared<RigidBodyCollider2>(plane);
	ConstantVectorField2Ptr wind = std::make_shared<ConstantVectorField2>(Vector2D(1, 0));

	ParticleSystemSolver2 solver;
	solver.SetCollider(collider);
	solver.SetWind(wind);

	ParticleSystemData2Ptr particles = solver.GetParticleSystemData();
	PointParticleEmitter2Ptr emitter = std::make_shared<PointParticleEmitter2>(Vector2D(0, 3), Vector2D(0, 1), 5.0, 45.0);
	emitter->SetMaxNumberOfNewParticlesPerSecond(100);
	solver.SetEmitter(emitter);

	SaveParticleDataXY(particles, 0);

	for (Frame frame(0, 1.0 / 60.0); frame.index < 360; frame.Advance())
	{
		solver.Update(frame);

		SaveParticleDataXY(particles, frame.index);
	}
}
CUBBYFLOW_END_TEST_F
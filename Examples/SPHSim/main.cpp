/*************************************************************************
> File Name: main.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: SPH Simulator
> Created Time: 2017/06/18
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Array/ArrayUtils.h>
#include <Collider/RigidBodyCollider3.h>
#include <Emitter/VolumeParticleEmitter3.h>
#include <Geometry/Box3.h>
#include <Geometry/Cylinder3.h>
#include <Geometry/Plane3.h>
#include <Geometry/Sphere3.h>
#include <Particle/ParticleSystemData3.h>
#include <Solver/PCISPH/PCISPHSolver3.h>
#include <Solver/SPH/SPHSolver3.h>
#include <Surface/Implicit/ImplicitSurfaceSet3.h>
#include <Utils/Logger.h>

#include <pystring/pystring.h>

#ifdef CUBBYFLOW_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <getopt.h>

#include <fstream>
#include <string>
#include <vector>

#define APP_NAME "SPHSim"

using namespace CubbyFlow;

void SaveParticleAsPos(const ParticleSystemData3Ptr& particles, const std::string& rootDir, int frameCnt)
{
	Array1<Vector3D> positions(particles->NumberOfParticles());
	CopyRange1(particles->GetPositions(), particles->NumberOfParticles(), &positions);
	char baseName[256];
	snprintf(baseName, sizeof(baseName), "frame_%06d.pos", frameCnt);
	std::string fileName = pystring::os::path::join(rootDir, baseName);
	std::ofstream file(fileName.c_str(), std::ios::binary);
	if (file)
	{
		printf("Writing %s...\n", fileName.c_str());
		std::vector<uint8_t> buffer;
		Serialize(positions.ConstAccessor(), &buffer);
		file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
		file.close();
	}
}

void SaveParticleAsXYZ(const ParticleSystemData3Ptr& particles, const std::string& rootDir, int frameCnt)
{
	Array1<Vector3D> positions(particles->NumberOfParticles());
	CopyRange1(particles->GetPositions(), particles->NumberOfParticles(), &positions);
	char baseName[256];
	snprintf(baseName, sizeof(baseName), "frame_%06d.xyz", frameCnt);
	std::string filename = pystring::os::path::join(rootDir, baseName);
	std::ofstream file(filename.c_str());
	if (file)
	{
		printf("Writing %s...\n", filename.c_str());
		for (const auto& pt : positions)
		{
			file << pt.x << ' ' << pt.y << ' ' << pt.z << std::endl;
		}
		file.close();
	}
}

void PrintUsage()
{
	printf(
		"Usage: " APP_NAME " "
		"-s spacing -l length -f frames -e example_num\n"
		"   -s, --spacing: target particle spacing (default is 0.02)\n"
		"   -f, --frames: total number of frames (default is 100)\n"
		"   -p, --fps: frames per second (default is 60.0)\n"
		"   -l, --log: log filename (default is " APP_NAME ".log)\n"
		"   -o, --output: output directory name "
		"(default is " APP_NAME "_output)\n"
		"   -m, --format: particle output format (xyz or pos. default is xyz)\n"
		"   -e, --example: example number (between 1 and 3, default is 1)\n"
		"   -h, --help: print this message\n");
}

void PrintInfo(const SPHSolver3Ptr& solver)
{
	const auto particles = solver->GetSPHSystemData();
	printf("Number of particles: %zu\n", particles->NumberOfParticles());
}

void RunSimulation(const std::string& rootDir, const SPHSolver3Ptr& solver, int numberOfFrames, const std::string& format, double fps)
{
	const auto particles = solver->GetSPHSystemData();

	for (Frame frame(0, 1.0 / fps); frame.index < numberOfFrames; ++frame)
	{
		solver->Update(frame);

		if (format == "xyz")
		{
			SaveParticleAsXYZ(particles, rootDir, frame.index);
		}
		else if (format == "pos")
		{
			SaveParticleAsPos(particles, rootDir, frame.index);
		}
	}
}

// Water-drop example (PCISPH)
void RunExample1(const std::string& rootDir, double targetSpacing, int numberOfFrames, const std::string& format, double fps)
{
	BoundingBox3D domain(Vector3D(), Vector3D(1, 2, 1));

	// Build solver
	auto solver = PCISPHSolver3::GetBuilder()
		.WithTargetDensity(1000.0)
		.WithTargetSpacing(targetSpacing)
		.MakeShared();

	solver->SetPseudoViscosityCoefficient(0.0);

	// Build emitter
	BoundingBox3D sourceBound(domain);
	sourceBound.Expand(-targetSpacing);

	const auto plane = Plane3::GetBuilder()
		.WithNormal({0, 1, 0})
		.WithPoint({0, 0.25 * domain.Height(), 0})
		.MakeShared();

	const auto sphere = Sphere3::GetBuilder()
		.WithCenter(domain.MidPoint())
		.WithRadius(0.15 * domain.Width())
		.MakeShared();

	const auto surfaceSet = ImplicitSurfaceSet3::GetBuilder()
		.WithExplicitSurfaces({plane, sphere})
		.MakeShared();

	const auto emitter = VolumeParticleEmitter3::GetBuilder()
		.WithSurface(surfaceSet)
		.WithSpacing(targetSpacing)
		.WithMaxRegion(sourceBound)
		.WithIsOneShot(true)
		.MakeShared();

	solver->SetEmitter(emitter);

	// Build collider
	const auto box = Box3::GetBuilder()
		.WithIsNormalFlipped(true)
		.WithBoundingBox(domain)
		.MakeShared();

	const auto collider = RigidBodyCollider3::GetBuilder()
		.WithSurface(box)
		.MakeShared();

	solver->SetCollider(collider);

	// Print simulation info
	printf("Running example 1 (water-drop with PCISPH)\n");
	PrintInfo(solver);

	// Run simulation
	RunSimulation(rootDir, solver, numberOfFrames, format, fps);
}

// Water-drop example (SPH)
void RunExample2(const std::string& rootDir, double targetSpacing, int numberOfFrames, const std::string& format, double fps)
{
	BoundingBox3D domain(Vector3D(), Vector3D(1, 2, 1));

	auto solver = SPHSolver3::GetBuilder()
		.WithTargetDensity(1000.0)
		.WithTargetSpacing(targetSpacing)
		.MakeShared();

	solver->SetPseudoViscosityCoefficient(0.0);

	// Build emitter
	BoundingBox3D sourceBound(domain);
	sourceBound.Expand(-targetSpacing);

	const auto plane = Plane3::GetBuilder()
		.WithNormal({0, 1, 0})
		.WithPoint({0, 0.25 * domain.Height(), 0})
		.MakeShared();

	const auto sphere = Sphere3::GetBuilder()
		.WithCenter(domain.MidPoint())
		.WithRadius(0.15 * domain.Width())
		.MakeShared();

	const auto surfaceSet = ImplicitSurfaceSet3::GetBuilder()
		.WithExplicitSurfaces({plane, sphere})
		.MakeShared();

	const auto emitter = VolumeParticleEmitter3::GetBuilder()
		.WithSurface(surfaceSet)
		.WithSpacing(targetSpacing)
		.WithMaxRegion(sourceBound)
		.WithIsOneShot(true)
		.MakeShared();

	solver->SetEmitter(emitter);

	// Build collider
	const auto box = Box3::GetBuilder()
		.WithIsNormalFlipped(true)
		.WithBoundingBox(domain)
		.MakeShared();

	const auto collider = RigidBodyCollider3::GetBuilder()
		.WithSurface(box)
		.MakeShared();

	solver->SetCollider(collider);

	// Print simulation info
	printf("Running example 2 (water-drop with SPH)\n");
	PrintInfo(solver);

	// Run simulation
	RunSimulation(rootDir, solver, numberOfFrames, format, fps);
}

// Dam-breaking example
void RunExample3(const std::string& rootDir, double targetSpacing, int numberOfFrames, const std::string& format, double fps)
{
	BoundingBox3D domain(Vector3D(), Vector3D(3, 2, 1.5));
	const double lz = domain.Depth();

	// Build solver
	auto solver = PCISPHSolver3::GetBuilder()
		.WithTargetDensity(1000.0)
		.WithTargetSpacing(targetSpacing)
		.MakeShared();

	solver->SetPseudoViscosityCoefficient(0.0);
	solver->SetTimeStepLimitScale(10.0);

	// Build emitter
	BoundingBox3D sourceBound(domain);
	sourceBound.Expand(-targetSpacing);

	const auto box1 = Box3::GetBuilder()
		.WithLowerCorner({0, 0, 0})
		.WithUpperCorner({0.5 + 0.001, 0.75 + 0.001, 0.75 * lz + 0.001})
		.MakeShared();

	const auto box2 = Box3::GetBuilder()
		.WithLowerCorner({2.5 - 0.001, 0, 0.25 * lz - 0.001})
		.WithUpperCorner({3.5 + 0.001, 0.75 + 0.001, 1.5 * lz + 0.001})
		.MakeShared();

	const auto boxSet = ImplicitSurfaceSet3::GetBuilder()
		.WithExplicitSurfaces({box1, box2})
		.MakeShared();

	const auto emitter = VolumeParticleEmitter3::GetBuilder()
		.WithSurface(boxSet)
		.WithMaxRegion(sourceBound)
		.WithSpacing(targetSpacing)
		.MakeShared();

	solver->SetEmitter(emitter);

	// Build collider
	const auto cyl1 = Cylinder3::GetBuilder()
		.WithCenter({1, 0.375, 0.375})
		.WithRadius(0.1)
		.WithHeight(0.75)
		.MakeShared();

	const auto cyl2 = Cylinder3::GetBuilder()
		.WithCenter({1.5, 0.375, 0.75})
		.WithRadius(0.1)
		.WithHeight(0.75)
		.MakeShared();

	const auto cyl3 = Cylinder3::GetBuilder()
		.WithCenter({2, 0.375, 1.125})
		.WithRadius(0.1)
		.WithHeight(0.75)
		.MakeShared();

	const auto box = Box3::GetBuilder()
		.WithIsNormalFlipped(true)
		.WithBoundingBox(domain)
		.MakeShared();

	const auto surfaceSet = ImplicitSurfaceSet3::GetBuilder()
		.WithExplicitSurfaces({ cyl1, cyl2, cyl3, box })
		.MakeShared();

	const auto collider = RigidBodyCollider3::GetBuilder()
		.WithSurface(surfaceSet)
		.MakeShared();

	solver->SetCollider(collider);

	// Print simulation info
	printf("Running example 3 (dam-breaking with PCISPH)\n");
	PrintInfo(solver);

	// Run simulation
	RunSimulation(rootDir, solver, numberOfFrames, format, fps);
}

int main(int argc, char* argv[])
{
	double targetSpacing = 0.02;
	int numberOfFrames = 100;
	double fps = 60.0;
	int exampleNum = 1;
	std::string logFileName = APP_NAME ".log";
	std::string outputDir = APP_NAME "_output";
	std::string format = "xyz";

	// Parse options
	static struct option longOptions[] =
	{
		{"spacing",   optional_argument, nullptr, 's'},
		{"frames",    optional_argument, nullptr, 'f'},
		{"fps",       optional_argument, nullptr, 'p'},
		{"example",   optional_argument, nullptr, 'e'},
		{"log",       optional_argument, nullptr, 'l'},
		{"outputDir", optional_argument, nullptr, 'o'},
		{"format",    optional_argument, nullptr, 'm'},
		{"help",      optional_argument, nullptr, 'h'},
		{nullptr,     0,                 nullptr,  0 }
	};

	int opt;
	int long_index = 0;
	while ((opt = getopt_long(argc, argv, "s:f:p:e:l:o:m:h", longOptions, &long_index)) != -1)
	{
		switch (opt)
		{
			case 's':
				targetSpacing = atof(optarg);
				break;
			case 'f':
				numberOfFrames = atoi(optarg);
				break;
			case 'p':
				fps = atof(optarg);
				break;
			case 'e':
				exampleNum = atoi(optarg);
				break;
			case 'l':
				logFileName = optarg;
				break;
			case 'o':
				outputDir = optarg;
				break;
			case 'm':
				format = optarg;
				if (format != "pos" && format != "xyz")
				{
					PrintUsage();
					exit(EXIT_FAILURE);
				}
				break;
			case 'h':
				PrintUsage();
				exit(EXIT_SUCCESS);
			default:
				PrintUsage();
				exit(EXIT_FAILURE);
		}
	}

#ifdef CUBBYFLOW_WINDOWS
	_mkdir(outputDir.c_str());
#else
	mkdir(outputDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif

	std::ofstream logFile(logFileName.c_str());
	if (logFile)
	{
		Logging::SetAllStream(&logFile);
	}

	switch (exampleNum)
	{
		case 1:
			RunExample1(outputDir, targetSpacing, numberOfFrames, format, fps);
			break;
		case 2:
			RunExample2(outputDir, targetSpacing, numberOfFrames, format, fps);
			break;
		case 3:
			RunExample3(outputDir, targetSpacing, numberOfFrames, format, fps);
			break;
		default:
			PrintUsage();
			exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

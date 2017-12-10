/*************************************************************************
> File Name: main.cpp
> Project Name: CubbyFlow
> Author: Dongmin Kim
> Purpose: Level-set Liquid Simulator
> Created Time: 2017/09/09
> Copyright (c) 2017, Dongmin Kim
*************************************************************************/
#include <Array/Array2.h>
#include <Collider/RigidBodyCollider3.h>
#include <Emitter/VolumeGridEmitter3.h>
#include <Geometry/Box3.h>
#include <Geometry/Cylinder3.h>
#include <Geometry/ImplicitTriangleMesh3.h>
#include <Geometry/Plane3.h>
#include <Geometry/Sphere3.h>
#include <Geometry/TriangleMesh3.h>
#include <Grid/ScalarGrid3.h>
#include <Math/MathUtils.h>
#include <MarchingCubes/MarchingCubes.h>
#include <Solver/LevelSet/LevelSetLiquidSolver3.h>
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

#define APP_NAME "LevelSetLiquidSim"

using namespace CubbyFlow;

void SaveTriangleMesh(const TriangleMesh3& mesh, const std::string& rootDir, int frameCnt)
{
	char baseName[256];
	snprintf(baseName, sizeof(baseName), "frame_%06d.obj", frameCnt);
	std::string fileName = pystring::os::path::join(rootDir, baseName);
	std::ofstream file(fileName.c_str());
	if (file)
	{
		printf("Writing %s...\n", fileName.c_str());
		mesh.WriteObj(&file);
		file.close();
	}
}

void TriangulateAndSave(const ScalarGrid3Ptr& sdf, const std::string& rootDir, int frameCnt)
{
	TriangleMesh3 mesh;
	const int flag = DIRECTION_ALL & ~DIRECTION_DOWN;
	MarchingCubes(
		sdf->GetConstDataAccessor(),
		sdf->GridSpacing(),
		sdf->GetDataOrigin(),
		&mesh,
		0.0,
		flag);
	SaveTriangleMesh(mesh, rootDir, frameCnt);
}

void PrintUsage()
{
	printf(
		"Usage: " APP_NAME " "
		"-r resolution -l length -f frames -e example_num\n"
		"   -r, --resx: grid resolution in x-axis (default is 50)\n"
		"   -f, --frames: total number of frames (default is 100)\n"
		"   -p, --fps: frames per second (default is 60.0)\n"
		"   -l, --log: log filename (default is " APP_NAME ".log)\n"
		"   -o, --output: output directory name (default is " APP_NAME "_output)\n"
		"   -e, --example: example number (between 1 and 4, default is 1)\n"
		"   -h, --help: print this message\n");
}

void PrintInfo(const LevelSetLiquidSolver3Ptr& solver)
{
	const auto grids = solver->GetGridSystemData();
	const Size3 resolution = grids->GetResolution();
	const BoundingBox3D domain = grids->GetBoundingBox();
	const Vector3D gridSpacing = grids->GetGridSpacing();

	printf(
		"Resolution: %zu x %zu x %zu\n",
		resolution.x, resolution.y, resolution.z);
	printf(
		"Domain: [%f, %f, %f] x [%f, %f, %f]\n",
		domain.lowerCorner.x, domain.lowerCorner.y, domain.lowerCorner.z,
		domain.upperCorner.x, domain.upperCorner.y, domain.upperCorner.z);
	printf(
		"Grid spacing: [%f, %f, %f]\n",
		gridSpacing.x, gridSpacing.y, gridSpacing.z);
}

void RunSimulation(const std::string& rootDir, const LevelSetLiquidSolver3Ptr& solver, unsigned int numberOfFrames, double fps)
{
	const auto sdf = solver->GetSignedDistanceField();

	for (Frame frame(0, 1.0 / fps); frame.index < numberOfFrames; ++frame)
	{
		solver->Update(frame);

		TriangulateAndSave(sdf, rootDir, frame.index);
	}
}

// Water-drop example
void RunExample1(const std::string& rootDir, size_t resX, int numberOfFrames, double fps)
{
	// Build solver
	auto solver = LevelSetLiquidSolver3::Builder()
		.WithResolution({ resX, 2 * resX, resX })
		.WithDomainSizeX(1.0)
		.MakeShared();

	const auto grids = solver->GetGridSystemData();
	BoundingBox3D domain = grids->GetBoundingBox();

	// Build emitter
	const auto plane = Plane3::Builder()
		.WithNormal({ 0, 1, 0 })
		.WithPoint({ 0, 0.25 * domain.Height(), 0 })
		.MakeShared();

	const auto sphere = Sphere3::Builder()
		.WithCenter(domain.MidPoint())
		.WithRadius(0.15 * domain.Width())
		.MakeShared();

	const auto surfaceSet = ImplicitSurfaceSet3::Builder()
		.WithExplicitSurfaces({ plane, sphere })
		.MakeShared();

	auto emitter = VolumeGridEmitter3::Builder()
		.WithSourceRegion(surfaceSet)
		.MakeShared();

	solver->SetEmitter(emitter);
	emitter->AddSignedDistanceTarget(solver->GetSignedDistanceField());

	// Print simulation info
	printf("Running example 1 (water-drop)\n");
	PrintInfo(solver);

	// Run simulation
	RunSimulation(rootDir, solver, numberOfFrames, fps);
}

// Dam-breaking example
void RunExample2(const std::string& rootDir, size_t resX, int numberOfFrames, double fps)
{
	// Build solver
	auto solver = LevelSetLiquidSolver3::Builder()
		.WithResolution({ 3 * resX, 2 * resX, (3 * resX) / 2 })
		.WithDomainSizeX(3.0)
		.MakeShared();

	const auto grids = solver->GetGridSystemData();
	BoundingBox3D domain = grids->GetBoundingBox();
	const double lz = domain.Depth();

	// Build emitter
	const auto box1 = Box3::Builder()
		.WithLowerCorner({ -0.5, -0.5, -0.5 * lz })
		.WithUpperCorner({ 0.5, 0.75, 0.75 * lz })
		.MakeShared();

	const auto box2 = Box3::Builder()
		.WithLowerCorner({ 2.5, -0.5, 0.25 * lz })
		.WithUpperCorner({ 3.5, 0.75, 1.5 * lz })
		.MakeShared();

	const auto boxSet = ImplicitSurfaceSet3::Builder()
		.WithExplicitSurfaces({ box1, box2 })
		.MakeShared();

	auto emitter = VolumeGridEmitter3::Builder()
		.WithSourceRegion(boxSet)
		.MakeShared();

	solver->SetEmitter(emitter);
	emitter->AddSignedDistanceTarget(solver->GetSignedDistanceField());

	// Build collider
	const auto cyl1 = Cylinder3::Builder()
		.WithCenter({ 1, 0.375, 0.375 })
		.WithRadius(0.1)
		.WithHeight(0.75)
		.MakeShared();

	const auto cyl2 = Cylinder3::Builder()
		.WithCenter({ 1.5, 0.375, 0.75 })
		.WithRadius(0.1)
		.WithHeight(0.75)
		.MakeShared();

	const auto cyl3 = Cylinder3::Builder()
		.WithCenter({ 2, 0.375, 1.125 })
		.WithRadius(0.1)
		.WithHeight(0.75)
		.MakeShared();

	const auto cylSet = ImplicitSurfaceSet3::Builder()
		.WithExplicitSurfaces({ cyl1, cyl2, cyl3 })
		.MakeShared();

	const auto collider = RigidBodyCollider3::Builder()
		.WithSurface(cylSet)
		.MakeShared();

	solver->SetCollider(collider);

	// Print simulation info
	printf("Running example 2 (dam-breaking)\n");
	PrintInfo(solver);

	// Run simulation
	RunSimulation(rootDir, solver, numberOfFrames, fps);
}

// High-viscosity example (bunny-drop)
void RunExample3(const std::string& rootDir, size_t resX, int numberOfFrames, double fps)
{
	// Build solver
	auto solver = LevelSetLiquidSolver3::Builder()
		.WithResolution({ resX, resX, resX })
		.WithDomainSizeX(1.0)
		.MakeShared();

	solver->SetViscosityCoefficient(1.0);
	solver->SetIsGlobalCompensationEnabled(true);

	auto grids = solver->GetGridSystemData();

	// Build emitters
	auto bunnyMesh = TriangleMesh3::Builder().MakeShared();
	std::ifstream objFile("Resources/bunny.obj");
	if (objFile)
	{
		bunnyMesh->ReadObj(&objFile);
	}
	else
	{
		fprintf(stderr, "Cannot open Resources/bunny.obj\n");
		exit(EXIT_FAILURE);
	}

	const auto bunny = ImplicitTriangleMesh3::Builder()
		.WithTriangleMesh(bunnyMesh)
		.WithResolutionX(resX)
		.MakeShared();

	auto emitter = VolumeGridEmitter3::Builder()
		.WithSourceRegion(bunny)
		.MakeShared();

	solver->SetEmitter(emitter);
	emitter->AddSignedDistanceTarget(solver->GetSignedDistanceField());

	// Print simulation info
	printf("Running example 3 (high-viscosity)\n");
	PrintInfo(solver);

	// Run simulation
	RunSimulation(rootDir, solver, numberOfFrames, fps);
}

// Low-viscosity example (bunny-drop)
void RunExample4(const std::string& rootDir, size_t resX, int numberOfFrames, double fps)
{
	// Build solver
	auto solver = LevelSetLiquidSolver3::Builder()
		.WithResolution({ resX, resX, resX })
		.WithDomainSizeX(1.0)
		.MakeShared();

	solver->SetViscosityCoefficient(0.0);
	solver->SetIsGlobalCompensationEnabled(true);

	auto grids = solver->GetGridSystemData();

	// Build emitters
	auto bunnyMesh = TriangleMesh3::Builder().MakeShared();
	std::ifstream objFile("Resources/bunny.obj");
	if (objFile)
	{
		bunnyMesh->ReadObj(&objFile);
	}
	else
	{
		fprintf(stderr, "Cannot open Resources/bunny.obj\n");
		exit(EXIT_FAILURE);
	}

	const auto bunny = ImplicitTriangleMesh3::Builder()
		.WithTriangleMesh(bunnyMesh)
		.WithResolutionX(resX)
		.MakeShared();

	auto emitter = VolumeGridEmitter3::Builder()
		.WithSourceRegion(bunny)
		.MakeShared();

	solver->SetEmitter(emitter);
	emitter->AddSignedDistanceTarget(solver->GetSignedDistanceField());

	// Print simulation info
	printf("Running example 4 (low-viscosity)\n");
	PrintInfo(solver);

	// Run simulation
	RunSimulation(rootDir, solver, numberOfFrames, fps);
}

int main(int argc, char* argv[])
{
	size_t resX = 50;
	int numberOfFrames = 100;
	double fps = 60.0;
	int exampleNum = 1;
	std::string logFileName = APP_NAME ".log";
	std::string outputDir = APP_NAME "_output";

	// Parse options
	static struct option longOptions[] =
	{
		{ "resx",      optional_argument, nullptr, 'r' },
		{ "frames",    optional_argument, nullptr, 'f' },
		{ "fps",       optional_argument, nullptr, 'p' },
		{ "example",   optional_argument, nullptr, 'e' },
		{ "log",       optional_argument, nullptr, 'l' },
		{ "outputDir", optional_argument, nullptr, 'o' },
		{ "help",      optional_argument, nullptr, 'h' },
		{ nullptr,     0,                 nullptr,  0 }
	};

	int opt;
	int long_index = 0;
	while ((opt = getopt_long(argc, argv, "r:f:p:e:l:o:h", longOptions, &long_index)) != -1)
	{
		switch (opt)
		{
		case 'r':
			resX = static_cast<size_t>(atoi(optarg));
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
		RunExample1(outputDir, resX, numberOfFrames, fps);
		break;
	case 2:
		RunExample2(outputDir, resX, numberOfFrames, fps);
		break;
	case 3:
		RunExample3(outputDir, resX, numberOfFrames, fps);
		break;
	case 4:
		RunExample4(outputDir, resX, numberOfFrames, fps);
		break;
	default:
		PrintUsage();
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
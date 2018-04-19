#
# Find TBB
#

set(TBB_VERSION_REQUIRED "2.0")

if(NOT TBB_ROOT)
	set(TBB_ROOT $ENV{TBB_ROOT})
endif()
if(NOT TBB_ROOT)
	set(TBB_ROOT $ENV{TBBROOT})
endif()

# Detect changed TBB_ROOT
if(NOT TBB_ROOT STREQUAL TBB_ROOT_LAST)
	unset(TBB_INCLUDE_DIR			CACHE)
	unset(TBB_LIBRARY				CACHE)
	unset(TBB_LIBRARY_DEBUG			CACHE)
	unset(TBB_LIBRARY_MALLOC		CACHE)
	unset(TBB_LIBRARY_MALLOC_DEBUG	CACHE)
	unset(TBB_INCLUDE_DIR_MIC		CACHE)
	unset(TBB_LIBRARY_MIC			CACHE)
	unset(TBB_LIBRARY_MALLOC_MIC	CACHE)
endif()

if(WIN32)
	# Workaround for parentheses in variable name / CMP0053
	set(PROGRAMFILESx86 "PROGRAMFILES(x86)")
	set(PROGRAMFILES32 "$ENV{${PROGRAMFILESx86}}")
	if(NOT PROGRAMFILES32)
		set(PROGRAMFILES32 "$ENV{PROGRAMFILES}")
	endif()
	if(NOT PROGRAMFILES32)
		set(PROGRAMFILES32 "C:/Program Files (x86)")
	endif()
	find_path(TBB_ROOT include/tbb/task_scheduler_init.h
		DOC "Root of TBB installation"
		HINTS ${TBB_ROOT}
		PATHS
			${PROJECT_SOURCE_DIR}/tbb
			${PROJECT_SOURCE_DIR}/../tbb
			"${PROGRAMFILES32}/IntelSWTools/compilers_and_libraries/windows/tbb"
			"${PROGRAMFILES32}/Intel/Composer XE/tbb"
			"${PROGRAMFILES32}/Intel/compilers_and_libraries/windows/tbb"
	)

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(TBB_ARCH intel64)
	else()
		set(TBB_ARCH ia32)
	endif()

	if(MSVC10)
		set(TBB_VCVER vc10)
	elseif(MSVC11)
		set(TBB_VCVER vc11)
	elseif(MSVC12)
		set(TBB_VCVER vc12)
	else()
		set(TBB_VCVER vc14)
	endif()

	set(TBB_LIBDIR ${TBB_ROOT}/lib/${TBB_ARCH}/${TBB_VCVER})
	set(TBB_BINDIR ${TBB_ROOT}/bin/${TBB_ARCH}/${TBB_VCVER})

	find_path(TBB_INCLUDE_DIR tbb/task_scheduler_init.h PATHS ${TBB_ROOT}/include NO_DEFAULT_PATH)
	find_library(TBB_LIBRARY tbb PATHS ${TBB_LIBDIR} NO_DEFAULT_PATH)
	find_library(TBB_LIBRARY_DEBUG tbb_debug PATHS ${TBB_LIBDIR} NO_DEFAULT_PATH)
	find_library(TBB_LIBRARY_MALLOC tbbmalloc PATHS ${TBB_LIBDIR} NO_DEFAULT_PATH)
	find_library(TBB_LIBRARY_MALLOC_DEBUG tbbmalloc_debug PATHS ${TBB_LIBDIR} NO_DEFAULT_PATH)
else()
	find_path(TBB_ROOT include/tbb/task_scheduler_init.h
		DOC "Root of TBB installation"
		HINTS ${TBB_ROOT}
		PATHS
			${PROJECT_SOURCE_DIR}/tbb
			/opt/intel/composerxe/tbb
			/opt/intel/compilers_and_libraries/tbb
	)

	if(APPLE)
		find_path(TBB_INCLUDE_DIR tbb/task_scheduler_init.h PATHS ${TBB_ROOT}/include NO_DEFAULT_PATH)
		find_library(TBB_LIBRARY tbb PATHS ${TBB_ROOT}/lib NO_DEFAULT_PATH)
		find_library(TBB_LIBRARY_DEBUG tbb_debug PATHS ${TBB_ROOT}/lib NO_DEFAULT_PATH)
		find_library(TBB_LIBRARY_MALLOC tbbmalloc PATHS ${TBB_ROOT}/lib NO_DEFAULT_PATH)
		find_library(TBB_LIBRARY_MALLOC_DEBUG tbbmalloc_debug PATHS ${TBB_ROOT}/lib NO_DEFAULT_PATH)
	else()
		find_path(TBB_INCLUDE_DIR tbb/task_scheduler_init.h PATHS ${TBB_ROOT}/include NO_DEFAULT_PATH)
		find_library(TBB_LIBRARY libtbb.so.2 HINTS ${TBB_ROOT}/lib/intel64/gcc4.4)
		find_library(TBB_LIBRARY_DEBUG libtbb_debug.so.2 HINTS ${TBB_ROOT}/lib/intel64/gcc4.4)
		find_library(TBB_LIBRARY_MALLOC libtbbmalloc.so.2 HINTS ${TBB_ROOT}/lib/intel64/gcc4.4)
		find_library(TBB_LIBRARY_MALLOC_DEBUG libtbbmalloc_debug.so.2 HINTS ${TBB_ROOT}/lib/intel64/gcc4.4)
	endif()

	find_path(TBB_INCLUDE_DIR_MIC tbb/task_scheduler_init.h PATHS ${TBB_ROOT}/include NO_DEFAULT_PATH)
	find_library(TBB_LIBRARY_MIC libtbb.so.2 PATHS ${TBB_ROOT}/lib/mic NO_DEFAULT_PATH)
	find_library(TBB_LIBRARY_MALLOC_MIC libtbbmalloc.so.2 PATHS ${TBB_ROOT}/lib/mic NO_DEFAULT_PATH)

	mark_as_advanced(TBB_INCLUDE_DIR_MIC)
	mark_as_advanced(TBB_LIBRARY_MIC)
	mark_as_advanced(TBB_LIBRARY_MALLOC_MIC)
endif()
/*************************************************************************
> File Name: Logger.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Logging functions for CubbyFlow Python API.
> Created Time: 2018/01/23
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <API/Python/Utils/Logger.h>
#include <Core/Utils/Logger.h>

using namespace CubbyFlow;

void AddLogging(pybind11::module& m)
{
	pybind11::enum_<LogLevel>(m, "LoggingLevel")
		.value("ALL", LogLevel::All)
		.value("DEBUG", LogLevel::Debug)
		.value("INFO", LogLevel::Info)
		.value("WARN", LogLevel::Warn)
		.value("ERROR", LogLevel::Error)
		.value("OFF", LogLevel::Off)
		.export_values();

	pybind11::class_<Logging>(m, "Logging")
		.def_static("SetLevel", &Logging::SetLevel)
		.def_static("Mute",		&Logging::Mute)
		.def_static("Unmute",	&Logging::Unmute);
}
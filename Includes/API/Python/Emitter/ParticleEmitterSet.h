/*************************************************************************
> File Name: ParticleEmitterSet.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: ParticleEmitterSet functions for CubbyFlow Python API.
> Created Time: 2018/02/11
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_PYTHON_PARTICLE_EMITTER_SET_H
#define CUBBYFLOW_PYTHON_PARTICLE_EMITTER_SET_H

#include <pybind11/pybind11.h>

void AddParticleEmitterSet2(pybind11::module& m);
void AddParticleEmitterSet3(pybind11::module& m);

#endif
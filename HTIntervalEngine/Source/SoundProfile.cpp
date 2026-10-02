/*
  ==============================================================================

    SoundProfil.e.cpp
    Created: 30 Oct 2025 2:40:48pm
    Author:  Ian Hanley

  ==============================================================================
*/

#include "SoundProfile.h"

SoundProfile::SoundProfile(SynthEngine& engine, unsigned int i) : synthEngine(engine), profileId(i)
{
    jassert(profileId > 0);
}

SoundProfile::~SoundProfile() {}

SynthEngine& SoundProfile::getSynthEngine()
{
    return synthEngine;
}

unsigned int SoundProfile::getProfileId()
{
    return profileId;
}

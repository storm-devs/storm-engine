//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#pragma once

#include "../service.h"

class IParticleManager;

#define CreateManager(FileName) CreateManagerEx(FileName, __FILE__, __LINE__)

// Particle service, creates particle managers
class IParticleService : public SERVICE
{
  public:
    // Constructor / destructor
    IParticleService(){};

    virtual ~IParticleService(){};

    // As a file name, you need to pass the name of the project
    // in which we want to work at the moment
    //
    // If filename = NULL, there will be no systems available to load
    virtual IParticleManager *CreateManagerEx(const char *ProjectName, const char *File, int Line) = 0;

    virtual uint32_t GetManagersCount() = 0;
    virtual IParticleManager *GetManagerByIndex(uint32_t Index) = 0;

    virtual IParticleManager *DefManager() = 0;
};

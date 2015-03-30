/*
* Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MANGOS_FLEEINGMOVEMENTGENERATOR_H
#define MANGOS_FLEEINGMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "ObjectGuid.h"

template<class T>
class MANGOS_DLL_SPEC FleeingMovementGenerator
    : public MovementGeneratorMedium< T, FleeingMovementGenerator<T> >
{
    public:
        FleeingMovementGenerator(ObjectGuid fright) : i_frightGuid(fright), i_nextCheckTime(0) {}

        MovementGeneratorType GetMovementGeneratorType() const { return FLEEING_MOTION_TYPE; }
        const char* Name() const { return "<Fleeing>"; }

        virtual void initialize(T&);
        virtual void finalize(T&);
        virtual void interrupt(T&);
        virtual void reset(T&);
        virtual bool update(T&, const uint32&);

    private:
        void _setTargetLocation(T& owner);
        bool _getPoint(T& owner, float& x, float& y, float& z);

        ObjectGuid i_frightGuid;
        TimeTracker i_nextCheckTime;
};

class MANGOS_DLL_SPEC TimedFleeingMovementGenerator
    : public FleeingMovementGenerator<Creature>
{
    public:
        TimedFleeingMovementGenerator(ObjectGuid fright, uint32 time) :
            FleeingMovementGenerator<Creature>(fright),
            i_totalFleeTime(time) {}

        MovementGeneratorType GetMovementGeneratorType() const { return TIMED_FLEEING_MOTION_TYPE; }
        const char* Name() const { return "<TimedFleeing>"; }

        virtual bool update(Creature&, const uint32&);
        virtual void finalize(Creature&);

    private:
        TimeTracker i_totalFleeTime;
};

#endif

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

#include "HomeMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "movement/MoveSplineInit.h"
#include "movement/MoveSpline.h"

void HomeMovementGenerator<Creature>::initialize(Creature& owner)
{
    _setTargetLocation(owner);
}

void HomeMovementGenerator<Creature>::reset(Creature&){}

void HomeMovementGenerator<Creature>::_setTargetLocation(Creature& owner)
{
    if (owner.hasUnitState(UNIT_STAT_NOT_MOVE))
        return;

    Movement::MoveSplineInit<Unit*> init(owner);
    float x, y, z, o;

    // at apply we can select more nice return points base at current movegen
    if (owner.GetMotionMaster()->empty() || !owner.GetMotionMaster()->CurrentMovementGenerator()->GetResetPosition(owner, x, y, z, o))
        owner.GetRespawnCoord(x, y, z, &o);

    init.SetFacing(o);
    init.MoveTo(x, y, z, true);
    init.SetSmooth(); // fix broken fly movement for old creatures
    //init.SetWalk(false);
    // hack for old creatures with bugged fly animation
    bool bSetWalk = (owner.GetTypeId() == TYPEID_UNIT && owner.IsLevitating() && owner.GetFloatValue(UNIT_FIELD_HOVERHEIGHT) == 0.0f);
    init.SetWalk(bSetWalk);
    init.Launch();

    m_arrived = false;
    owner.clearUnitState(UNIT_STAT_ALL_DYN_STATES);
}

bool HomeMovementGenerator<Creature>::update(Creature& owner, const uint32& /*time_diff*/)
{
    m_arrived = owner.movespline->Finalized();
    return !m_arrived;
}

void HomeMovementGenerator<Creature>::finalize(Creature& owner)
{
    if (m_arrived)
    {
        if (owner.GetTemporaryFactionFlags() & TEMPFACTION_RESTORE_REACH_HOME)
            owner.ClearTemporaryFaction();

        owner.SetWalk(!owner.hasUnitState(UNIT_STAT_RUNNING_STATE) && !owner.IsLevitating(), false);
        owner.SetHealthPercent(100.0f);
        owner.LoadCreatureAddon(true);
        owner.AI()->JustReachedHome();
    }
}

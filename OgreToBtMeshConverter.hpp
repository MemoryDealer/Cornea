//================================================//

#ifndef __OGRETOBTMESHCONVERTER_HPP__
#define __OGRETOBTMESHCONVERTER_HPP__

//================================================//

#include "stdafx.h"
#include "EventManager.hpp"

//================================================//

void registerEntityAsCollider(Ogre::Entity* entity, btCollisionWorld* colWorld, EventManager* pEventManager);
void registerAllEntitiesAsColliders(Ogre::SceneManager* sceneMgr, btCollisionWorld* colWorld, EventManager* pEventManager);

//================================================//

#endif

//================================================//
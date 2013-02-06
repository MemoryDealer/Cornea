//================================================//

#ifndef __NPCMANAGER_HPP__
#define __NPCMANAGER_HPP__

//================================================//

#include "stdafx.h"
#include "NPC.hpp"

//================================================//

class NPCManager
{
public:
	NPCManager(Ogre::SceneManager* mgr, Sparks::Camera* camera);
	~NPCManager(void);

	void addNPC(Ogre::SceneNode* node, btCollisionObject* obj);

	// Getter functions
	std::vector<NPC*> getNPCs(void) const;
	NPC* getNPC(Ogre::String name);
	Physics* getPhysics(void) const;

	void update(double timeSinceLastFrame);

private:
	Ogre::SceneManager*		m_pSceneMgr;

	Sparks::Camera*			m_pCamera;

	std::vector<NPC*>		m_NPCs;

	Physics*				m_physics;
};

//================================================//

inline std::vector<NPC*> NPCManager::getNPCs(void) const
{ return m_NPCs; }

inline Physics* NPCManager::getPhysics(void) const
{ return m_physics; }

//================================================//

#endif

//================================================//
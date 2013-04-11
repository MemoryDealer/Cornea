//================================================//

#ifndef __DYNAMICOBJECTMANAGER_HPP__
#define __DYNAMICOBJECTMANAGER_HPP__

//================================================//

#include "stdafx.h"
#include "DynamicObject.hpp"
#include "RetrievableItems.hpp"
#include "Doors.hpp"
#include "Light.hpp"
#include "Trigger.hpp"
#include "NPCManager.hpp" 
#include "Base.hpp"

//================================================//

class DynamicObjectManager
{
public:
	DynamicObjectManager(Ogre::SceneManager* mgr, Sparks::Camera* camera);
	~DynamicObjectManager(void);

	bool addObject(Ogre::SceneNode* node, btCollisionObject* obj, int tier);
	void registerAllObjectsInScene(void);
	void registerTriggerAction(DynamicObject::DYNAMIC_OBJECT_DATA* data);

	// Getter functions
	std::vector<DynamicObject*>& getObjects(void);
	DynamicObject* getObject(Ogre::String& name);
	
	// Setter functions
	void setNPCManager(NPCManager* mgr);

	void update(double timeSinceLastFrame);

	std::vector<Ogre::String>	firstTierObjects;
	std::vector<Ogre::String>	secondTierObjects;
	std::vector<Ogre::String>	thirdTierObjects;
	
private:
	Ogre::SceneManager*			m_pSceneMgr;

	Sparks::Camera*				m_pCamera;

	NPCManager*					m_pNPCManager;

	std::vector<DynamicObject*> m_objects;

	Physics*					m_physics;
};

//================================================//

inline std::vector<DynamicObject*>& DynamicObjectManager::getObjects(void)
{ return m_objects; }

inline void DynamicObjectManager::setNPCManager(NPCManager* mgr)
{ m_pNPCManager = mgr; }

//================================================//

#endif

//================================================//
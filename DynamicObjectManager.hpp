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

typedef std::list<DynamicObject*> DynamicObjectList;
typedef DynamicObjectList::iterator DynamicObjectListIterator;

//================================================//

class DynamicObjectManager
{
public:
	DynamicObjectManager(Ogre::SceneManager* mgr, Sparks::Camera* camera);
	~DynamicObjectManager(void);

	bool addObject(Ogre::SceneNode* node, btCollisionObject* obj, int tier);
	void registerAllObjectsInScene(void);
	void registerTriggerAction(DynamicObject::DYNAMIC_OBJECT_DATA* data);
	void registerTriggerChains(void);

	// Getter functions
	DynamicObjectList& getObjects(void);
	DynamicObject* getObject(Ogre::String& name);
	DynamicObject* getObject(Ogre::SceneNode* node);
	
	// Setter functions
	void setNPCManager(NPCManager* mgr);

	void update(double timeSinceLastFrame);
	
private:
	Ogre::SceneManager*			m_pSceneMgr;

	Sparks::Camera*				m_pCamera;

	NPCManager*					m_pNPCManager;

	DynamicObjectList		    m_objects;

	Physics*					m_physics;
};

//================================================//

inline DynamicObjectList& DynamicObjectManager::getObjects(void)
{ return m_objects; }

inline void DynamicObjectManager::setNPCManager(NPCManager* mgr)
{ m_pNPCManager = mgr; }

//================================================//

#endif

//================================================//
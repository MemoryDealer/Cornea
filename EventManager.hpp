//================================================//

#ifndef __EVENTMANAGER_HPP__
#define __EVENTMANAGER_HPP__

//================================================//

#include "DynamicObjectManager.hpp"
#include "NPCManager.hpp"
#include "Weapon.hpp"
#include "Camera.hpp"

//================================================//

class EventManager
{
public:
	EventManager(Ogre::SceneManager* mgr, Sparks::Camera* camera);
	~EventManager(void);

	// Getter functions
	DynamicObjectManager* getDynamicObjectManager(void) const;
	ProjectileManager* getProjectileManager(void) const;
	NPCManager* getNPCManager(void) const;

	// Setter functions
	void setCamera(Sparks::Camera* camera);

	void update(double timeSinceLastFrame);

protected:
	Ogre::SceneManager*		m_pSceneMgr;

	DynamicObjectManager*	m_pDynamicObjectManager;
	ProjectileManager*		m_pProjectileManager;
	NPCManager*				m_pNPCManager;

	Sparks::Camera*			m_pCamera;
};

//================================================//

// Getter functions
inline DynamicObjectManager* EventManager::getDynamicObjectManager(void) const
{ return m_pDynamicObjectManager; }

inline ProjectileManager* EventManager::getProjectileManager(void) const
{ return m_pProjectileManager; }

inline NPCManager* EventManager::getNPCManager(void) const
{ return m_pNPCManager; }

// Setter functions
inline void EventManager::setCamera(Sparks::Camera* camera)
{ m_pCamera = camera; }

//================================================//

#endif

//================================================//
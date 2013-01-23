//================================================//

#include "EventManager.hpp"

//================================================//

EventManager::EventManager(Ogre::SceneManager* mgr, Sparks::Camera* camera)
{
	m_pSceneMgr = mgr;
	m_pCamera = camera;

	// Init dynamic object manager
	m_pDynamicObjectManager = new DynamicObjectManager(m_pSceneMgr, m_pCamera);

	// Init projectile manager
	m_pProjectileManager = new ProjectileManager(m_pSceneMgr);

	// Init NPC Manager
	m_pNPCManager = new NPCManager(m_pSceneMgr, m_pCamera);
	m_pDynamicObjectManager->setNPCManager(m_pNPCManager);
	m_pProjectileManager->setNPCManager(m_pNPCManager);

	// Init Effect Manager
	m_pEffectManager = new EffectManager(m_pSceneMgr, 10);
}

//================================================//

EventManager::~EventManager(void)
{
	delete m_pDynamicObjectManager;
	delete m_pProjectileManager;
	delete m_pNPCManager;
}

//================================================//

void EventManager::update(double timeSinceLastFrame)
{
	// Update world objects
	m_pDynamicObjectManager->update(timeSinceLastFrame);

	// Update projectiles
	m_pProjectileManager->update(timeSinceLastFrame);

	// Update NPCs
	m_pNPCManager->update(timeSinceLastFrame);

	// Update Effect Manager
	m_pEffectManager->update(timeSinceLastFrame);
}

//================================================//


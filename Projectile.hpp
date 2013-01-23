//================================================//

#ifndef __PROJECTILE_HPP__
#define __PROJECTILE_HPP__

//================================================//

#include "stdafx.h"
#include "Base.hpp"
#include "DotSceneLoader.hpp"
#include "Sparks.hpp"
#include "Camera.hpp"
#include "Sound.hpp"
#include "NPCManager.hpp"
#include "WeaponEffects.hpp"

//================================================//

/* Class for projectiles fired from weapons */
class Projectile
{
public:
	Projectile(void){}
	~Projectile(void);

	/* This is projectile data that will be passed into the environment function to add a projectile (called from game state) */
	typedef struct projectile_data{
		Ogre::Vector3 origin;
		Ogre::Vector3 direction;
		Ogre::Real  range;
		Ogre::Real	speed;
		Ogre::Real  incrementor;
		Ogre::Real  damage;
		Ogre::Real  collisionDistance;
		Ogre::ColourValue colour;
	} PROJECTILE_DATA, *PPROJECTILE_DATA;

	void init(Ogre::SceneManager* mgr, PROJECTILE_DATA& data, NPCManager* npcManager);
	void cleanup(void);

	// Getter functions
	const bool isAlive(void) const;
	const Ogre::Real getVelocity(void) const;
	const Ogre::Real getIncrementor(void) const;
	const Ogre::Vector3 getPosition(void) const;
	

	// Update functions
	bool isColliding(void);
	void update(double timeSinceLastFrame);

private:
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::Light*			m_pLight;
	Ogre::SceneNode*		m_pSceneNode;
	Ogre::BillboardSet*		m_pBbSet;
	Ogre::Billboard*		m_pBb;
	Ogre::ColourValue		m_colour;

	Ogre::Vector3			m_origin;
	Ogre::Vector3			m_direction;
	Ogre::Vector3			m_lastPosition;
	Ogre::Real				m_speed;
	Ogre::Real				m_incrementor;
	Ogre::Real				m_range;
	Ogre::Real				m_damage;
	Ogre::Real				m_distanceMoved;
	Ogre::Real				m_collisionDistance;

	bool					m_alive;

	NPCManager*				m_pNPCManager;
};

//================================================//

// Getter functions
inline const bool Projectile::isAlive(void) const
{ return m_alive; }

inline const Ogre::Real Projectile::getVelocity(void) const // probably not needed
{ return m_speed; }

inline const Ogre::Real Projectile::getIncrementor(void) const
{ return m_incrementor; }

inline const Ogre::Vector3 Projectile::getPosition(void) const
{ return m_pSceneNode->getPosition(); }

//================================================//

// *** //

//================================================//

class ProjectileManager
{
public:
	ProjectileManager(Ogre::SceneManager* mgr);
	~ProjectileManager(void);

	void addProjectile(Projectile::PROJECTILE_DATA& data);
	void clearAll(void);

	// Getter functions
	const int getCount(void) const;
	const Ogre::Vector3 getProjectilePosition(int element) const;

	// Setter functions
	void setNPCManager(NPCManager* npcManager);

	void update(double timeSinceLastFrame);

private:
	Ogre::SceneManager*			m_pSceneMgr;

	NPCManager*					m_pNPCManager;

	std::vector<Projectile*>	m_projectiles;
};

//================================================//

inline const int ProjectileManager::getCount(void) const
{ return m_projectiles.size(); }

inline const Ogre::Vector3 ProjectileManager::getProjectilePosition(int element) const
{ //return m_projectiles.at(element).m_pSceneNode->getPosition(); }
	Projectile* proj = m_projectiles.at(element);
	return proj->getPosition();
}

inline void ProjectileManager::setNPCManager(NPCManager* npcManager)
{ m_pNPCManager = npcManager; }

//================================================//

#endif

//================================================//
//================================================//

#ifndef __WEAPONEFFECTS_HPP__
#define __WEAPONEFFECTS_HPP__

//================================================//

#include "stdafx.h"
#include "Settings.hpp"

//================================================//

/* For impact effects where a bullet hits */
class Impact
{
public:
	Impact(Ogre::SceneManager* mgr, unsigned type, Ogre::Vector3 pos);
	~Impact(void){}

	void clear(void);
	const bool isAlive(void) const;

	void update(double timeSinceLastFrame);

	// --- //

	enum{
		TYPE_NULL = 0,
		TYPE_CLOUD1
	};

protected:
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::SceneNode*		m_pSceneNode;
	Ogre::BillboardSet*		m_pBbSet;
	Ogre::Billboard*		m_pBb;

	Ogre::Timer*			m_pTimer;
	Ogre::Real				m_life;
	bool					m_alive;
};

//================================================//

inline const bool Impact::isAlive(void) const
{ return m_alive; }

//================================================//
//================================================//

class ImpactManager
{
public:
	ImpactManager(void);
	~ImpactManager(void){}

	void addImpact(Impact* impact);

	void update(double timeSinceLastFrame);

private:
	std::vector<Impact*>	m_impacts;
};

//================================================//
//================================================//

class MuzzleFlash
{
public:
	MuzzleFlash(Ogre::SceneManager* mgr);
	~MuzzleFlash(void){}

	void create(Ogre::SceneNode* parent, unsigned type, Ogre::Vector3 pos);
	void clear(void);

	// Getter functions
	const bool isAlive(void) const;

	void update(double timeSinceLastFrame);

	// --- //

	enum{
		TYPE_NULL = 0,
		TYPE_FLASH1,
		TYPE_FLASH2
	};

protected:
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::SceneNode*		m_pSceneNode;
	Ogre::BillboardSet*		m_pBbSet;
	Ogre::Billboard*		m_pBb;
	Ogre::Light*			m_pLight;
	Ogre::SceneNode*		m_pLightNode;

	Ogre::Timer*			m_pTimer;
	Ogre::Real				m_life;
	Ogre::Real				m_growthRate;
	bool					m_alive;
};

//================================================//

inline const bool MuzzleFlash::isAlive(void) const
{ return m_alive; }

//================================================//

#endif

//================================================//
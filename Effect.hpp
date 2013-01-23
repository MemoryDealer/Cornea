//================================================//

#ifndef __EFFECT_HPP__
#define __EFFECT_HPP__

//================================================//

#include "stdafx.h"

//================================================//

class Effect
{
public:
	Effect(void){}
	~Effect(void){}

	enum{
		TYPE_NULL = 0,

		// Weapon hits
		TYPE_EXPLOSION1
	};

	void init(Ogre::SceneManager* mgr);

	// Getter functions
	const bool isActive(void) const;

	// Setter functions
	void setType(unsigned type);
	void activate(Ogre::Vector3 pos);

protected:
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::ParticleSystem*	m_pSystem;
	Ogre::SceneNode*		m_pSceneNode;

	bool					m_active;
};

//================================================//

inline const bool Effect::isActive(void) const
{ return m_active; }

//================================================//

#endif

//================================================//
//================================================//

#ifndef __EFFECTMANAGER_HPP__
#define __EFFECTMANAGER_HPP__

//================================================//

#include "stdafx.h"
#include "Effect.hpp"

//================================================//

/* This will hold particle systems which will be reused throughout the game state */
class EffectManager
{
public:
	EffectManager(Ogre::SceneManager* mgr, int maxEffects);
	~EffectManager(void){}

	void activateEffect(unsigned type, Ogre::Vector3 pos);

	void update(double timeSinceLastFrame);

private:
	Ogre::SceneManager*		m_pSceneMgr;

	std::vector<Effect*>	m_effects;
	int						m_numEffects;
};

//================================================//

#endif

//================================================//
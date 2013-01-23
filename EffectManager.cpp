//================================================//

#include "EffectManager.hpp"

//================================================//

EffectManager::EffectManager(Ogre::SceneManager* mgr, int maxEffects)
{
	m_pSceneMgr = mgr;

	m_numEffects = maxEffects;

	// Allocate a certain number of effects since they will be reused
	for(int i=0; i<m_numEffects; ++i){
		m_effects.push_back(new Effect());
		m_effects.back()->init(m_pSceneMgr);
	}
}

//================================================//

void EffectManager::activateEffect(unsigned type, Ogre::Vector3 pos)
{
	bool activated = false;

	// Find the first inactive effect and activate it
	for(std::vector<Effect*>::iterator itr = m_effects.begin();
		itr != m_effects.end();
		++itr){
			if(!((*itr)->isActive())){
				(*itr)->activate(pos);
				activated = true;
			}
	}
}

//================================================//

void EffectManager::update(double timeSinceLastFrame)
{

}

//================================================//
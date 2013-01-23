//================================================//

#include "Effect.hpp"

//================================================//

void Effect::init(Ogre::SceneManager* mgr)
{
	m_pSceneMgr = mgr;

	m_pSystem = m_pSceneMgr->createParticleSystem("Smoke" + Ogre::StringConverter::toString(Ogre::Math::RangeRandom(0, 25000)), "Examples/Smoke"); // change?
	m_pSystem->setEmitting(false);
	m_active = false;
}

//================================================//

void Effect::setType(unsigned type)
{
	
}

//================================================//

void Effect::activate(Ogre::Vector3 pos)
{
	m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(pos);
	m_pSceneNode->attachObject(m_pSystem);

	m_pSystem->setEmitting(true);
	m_active = true;
}

//================================================//
//================================================//

#include "WeaponEffects.hpp"

//================================================//

Impact::Impact(Ogre::SceneManager* mgr, unsigned type, Ogre::Vector3 pos)
{
	m_pSceneMgr = mgr;
	m_alive = true;
	m_life = 50.0;

	// Create the billboard effect
	m_pBbSet = m_pSceneMgr->createBillboardSet(1);

	switch(type){
	case TYPE_NULL:
	default:
		break;

	case TYPE_CLOUD1:

		m_pBbSet->setMaterialName("Examples/Flare");
		break;

	}

	m_pBb = m_pBbSet->createBillboard(Ogre::Vector3::ZERO);

	m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pSceneNode->attachObject(m_pBbSet);
	m_pSceneNode->setPosition(pos);

	// Initiate timer
	m_pTimer = new Ogre::Timer();
}

//================================================//

void Impact::clear(void)
{
	m_pSceneNode->detachAllObjects();

	m_pBbSet->removeBillboard(m_pBb);
	m_pSceneMgr->destroyBillboardSet(m_pBbSet);

	delete m_pTimer;
}

//================================================//

void Impact::update(double timeSinceLastFrame)
{
	Ogre::Real scale = 1.1;
	m_pSceneNode->scale(scale, scale, scale);


	//m_pBbSet->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_TEXTURE, Ogre::LBS_MANUAL, 0.0);
	//m_pBb->setColour(Ogre::ColourValue(1.0, 1.0, 1.0, 0.5));
	// another pass in the material script? 

	unsigned long time = m_pTimer->getMillisecondsCPU();

	m_pBb->setColour(Ogre::ColourValue(1.0, 1.0, 1.0, 0.01 * (100.0 - time)));

	if(time >= m_life){
		m_alive = false;
	}
}

//================================================//
//================================================//

ImpactManager::ImpactManager(void)
{

}

//================================================//

void ImpactManager::addImpact(Impact* impact)
{
	m_impacts.push_back(impact);
}

//================================================//

void ImpactManager::update(double timeSinceLastFrame)
{
	for(std::vector<Impact*>::iterator itr = m_impacts.begin();
		itr != m_impacts.end();){
			if((*itr)->isAlive()){
				(*itr)->update(timeSinceLastFrame);

				++itr;
			}
			else{
				(*itr)->clear();
				delete *itr;
				itr = m_impacts.erase(itr);
			}
	}
}

//================================================//
//================================================//

MuzzleFlash::MuzzleFlash(Ogre::SceneManager* mgr)
{
	m_pSceneMgr = mgr;
	m_alive = false;
}

//================================================//

void MuzzleFlash::create(Ogre::SceneNode* parent, unsigned type, Ogre::Vector3 pos)
{
	Ogre::Vector3 scale = Ogre::Vector3::ZERO;
	Ogre::Real randomModifier = 0.2;

	// Clear if alive before creating a new one
	if(m_alive){
		clear();
	}

	m_alive = true;
	m_life = 50.0;
	m_growthRate = 0.01;

	// Create billboard set
	m_pBbSet = m_pSceneMgr->createBillboardSet(1);

	// Light
	m_pLight = m_pSceneMgr->createLight("muzzlelight");
	m_pLight->setType(Ogre::Light::LT_POINT);

	// settings for type
	switch(type){
	case TYPE_NULL:
	default:
		break;

	case TYPE_FLASH1:
		m_pBbSet->setMaterialName("Examples/Flame");
		scale = 0.03;
		randomModifier = 1.0;
		m_growthRate = 0.02;
		break;

	case TYPE_FLASH2:
		m_pBbSet->setMaterialName("Examples/Flame");
		scale = 0.03;
		randomModifier = 1.0;
		m_growthRate = 0.02;

		m_pLight->setDiffuseColour(1.0, 1.0, 0.5);
		m_pLight->setSpecularColour(1.0, 1.0, 0.5);
		m_pLight->setCastShadows(Settings::getSingletonPtr()->graphics.shadows.enabled);
		m_pLight->setAttenuation(300.0, 1, 1, 0);
		m_pLightNode = parent->createChildSceneNode();
		m_pLightNode->attachObject(m_pLight);
		break;
	}

	m_pBb = m_pBbSet->createBillboard(Ogre::Vector3::ZERO);

	// Create node as a child of the parent
	m_pSceneNode = parent->createChildSceneNode();
	m_pSceneNode->attachObject(m_pBbSet);

	// Randomize the position a little
	//pos += Ogre::Vector3(Ogre::Math::RangeRandom(0.0, randomModifier), Ogre::Math::RangeRandom(0.0, randomModifier), Ogre::Math::RangeRandom(0.0, randomModifier));
	m_pSceneNode->setPosition(pos);
	//m_pSceneNode->roll(Ogre::Radian(Ogre::Math::RangeRandom(0.0, Ogre::Math::PI * 2.0)));

	// Scale the billboard itself
	m_pSceneNode->setScale(scale);

	// Initiate timer
	m_pTimer = new Ogre::Timer();
}

//================================================//

void MuzzleFlash::clear(void)
{
	m_pSceneNode->detachAllObjects();

	m_pBbSet->removeBillboard(m_pBb);
	m_pSceneMgr->destroyBillboardSet(m_pBbSet);

	m_pLight->detachFromParent();
	m_pSceneMgr->destroyLight(m_pLight);

	delete m_pTimer;

	m_alive = false;
}

//================================================//

void MuzzleFlash::update(double timeSinceLastFrame)
{
	// "Grow" the billboard texture over time
	Ogre::Vector3 growth(1.0, 1.0, 1.0);
	growth += (m_growthRate * timeSinceLastFrame);
	m_pSceneNode->scale(growth);
	
	m_pLight->setAttenuation(m_pLight->getAttenuationRange() + (m_growthRate * timeSinceLastFrame), 1, 1, 0);


	//m_pBbSet->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_TEXTURE, Ogre::LBS_MANUAL, 0.0);
	//m_pBb->setColour(Ogre::ColourValue(1.0, 1.0, 1.0, 0.5));
	// another pass in the material script? 

	unsigned long time = m_pTimer->getMillisecondsCPU();

	//m_pBb->setColour(Ogre::ColourValue(1.0, 1.0, 1.0, 0.01 * (100.0 - time)));
	//m_pBb->setColour(Ogre::ColourValue(1.0, 1.0, 1.0, time * 0.01));

	if(time >= m_life){
		clear();
	}
}

//================================================//
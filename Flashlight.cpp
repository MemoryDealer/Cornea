//================================================//

#include "Flashlight.hpp"

//================================================//

Flashlight::Flashlight(Ogre::SceneManager* mgr, Ogre::SceneNode* parent)
{
	m_pSceneMgr = mgr;

	m_attenuationRange = 1000.0;
	m_innerAngle = Ogre::Degree(5);
	m_outerAngle = Ogre::Degree(21);

	// Flashlight settings
	m_pLight = m_pSceneMgr->createLight("Flashlight");
	m_pLight->setType(Ogre::Light::LT_SPOTLIGHT);
	m_pLight->setDiffuseColour(1.0, 1.0, 1.0);
	m_pLight->setSpecularColour(0.8, 0.8, 0.8);
	m_pLight->setCastShadows(Settings::getSingletonPtr()->graphics.shadows.enabled);
	m_pLight->setSpotlightInnerAngle(m_innerAngle);
	//m_pLight->setSpotlightOuterAngle(m_innerAngle + Ogre::Degree(1));
	m_pLight->setSpotlightOuterAngle(m_outerAngle);
	m_pLight->setSpotlightFalloff(5.0);
	m_pLight->setAttenuation(m_attenuationRange, 0.5, 0, 0);
	m_pLight->setDirection(Ogre::Vector3(0.0, 0.0, -1.0));

	Ogre::ConfigFile file;
	file.loadDirect("C:/cam.cfg");
	// Set up scene node
	parent->createChildSceneNode("Flashlight")->attachObject(m_pLight);
	Ogre::Real x = Ogre::StringConverter::parseReal(file.getSetting("x", "flashlight"));
	Ogre::Real y = Ogre::StringConverter::parseReal(file.getSetting("y", "flashlight"));
	m_pSceneMgr->getSceneNode("Flashlight")->translate(x, y, 0.0, Ogre::Node::TS_LOCAL);

	// Set default setting to off
	m_pLight->setVisible(false);
	m_state = STATE_IDLE;
}

//================================================//

Flashlight::~Flashlight(void)
{

}

//================================================//

void Flashlight::switchState(void)
{
	m_pLight->setVisible(!m_pLight->isVisible());
}

//================================================//

void Flashlight::update(double timeSinceLastFrame)
{
	
}

//================================================//
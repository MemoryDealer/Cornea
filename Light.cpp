//================================================//

#include "Light.hpp"

//================================================//

Light::Light(void)
	: DynamicObject()
{
	m_dataLoaded = false;
}

//================================================//

void Light::initLight(Ogre::SceneManager* mgr, Ogre::SceneNode* node)
{
	m_pSceneMgr = mgr;
	m_pSceneNode = node;

	m_data = nullptr;

	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty()){
		if(any.getType() == typeid(LightData*)){
			m_data = Ogre::any_cast<LightData*>(any);
			m_dataLoaded = true;
		}
	}

	if(!m_dataLoaded){
		// Setup default light settings
		printf("No light data found on %s\n", m_pSceneNode->getName().c_str());
		m_data = new LightData();
		m_data->type = TYPE_SPOT;
		m_data->range = 600.0;
		m_data->inner = 5.0;
		m_data->outer = 21.0;
		m_data->shadows = false;
	}

	m_pLight = m_pSceneMgr->createLight("Dyn_" + m_pSceneNode->getName());
	m_pLight->setType((m_data->type == TYPE_SPOT) ? Ogre::Light::LT_SPOTLIGHT : Ogre::Light::LT_POINT);
	m_pLight->setPosition(m_pSceneNode->_getDerivedPosition());
	
	Ogre::Vector3 dir = m_pSceneNode->_getDerivedOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Y;
	m_pLight->setDirection(dir);
	//m_pLight->setCustomShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup()));
	
	// Light colour
	m_pLight->setDiffuseColour(m_data->colour);
	m_pLight->setSpecularColour(m_data->colour);
	
	// Type settings
	if(m_data->type == TYPE_SPOT){
		m_pLight->setAttenuation(m_data->range, 0.5, 0.0, 0.0);

		m_pLight->setSpotlightInnerAngle(Ogre::Degree(m_data->inner));
		m_pLight->setSpotlightOuterAngle(Ogre::Degree(m_data->outer));

		m_pLight->setSpotlightFalloff(1.0);
	}

	m_pLight->setCastShadows(m_data->shadows);

	if(m_data->hideNode)
		m_pSceneNode->setVisible(false);
}

//================================================//

void Light::deleteData(void)
{
	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty()){
		if(any.getType() == typeid(LightData*))
			delete Ogre::any_cast<LightData*>(any);
	}
}

//================================================//
//================================================//

StaticLight::StaticLight(void)
	: Light()
{

}

//================================================//

void StaticLight::update(double timeSinceLastFrame)
{

}

//================================================//
//================================================//

PulseLight::PulseLight(void)
	: Light()
{

}

//================================================//

void PulseLight::initLight(Ogre::SceneManager* mgr, Ogre::SceneNode* node)
{
	Light::initLight(mgr, node);

	if(m_dataLoaded){
		m_speed = m_data->buffer;
	}
}

//================================================//

void PulseLight::update(double timeSinceLastFrame)
{
	static Ogre::Real sin = 0.0;
	static Ogre::ColourValue colour = m_pLight->getDiffuseColour();

	Ogre::ColourValue newColour = colour * Ogre::Math::Abs(0.5 * Ogre::Math::Sin(sin));
	m_pLight->setDiffuseColour(newColour);
	m_pLight->setSpecularColour(newColour);

	sin += 0.002 * m_speed * timeSinceLastFrame;
}

//================================================//

//================================================//
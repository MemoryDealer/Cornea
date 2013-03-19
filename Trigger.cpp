//================================================//

#include "Trigger.hpp"

//================================================//

Trigger::Trigger(void)
{
	m_loop = false;
	m_triggered = false;
	m_timeout = 10000;

	m_pTimeout = new Ogre::Timer();
}

//================================================//

Trigger::~Trigger(void)
{

}

//================================================//

void Trigger::initTrigger(Ogre::SceneManager* mgr, Ogre::SceneNode* node, Sparks::Camera* camera)
{
	m_pSceneMgr = mgr;
	m_pSceneNode = node;
	m_pCamera = camera;

	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty()){
		DynamicObjectData* data = Ogre::any_cast<DynamicObjectData*>(any);

		// Trigger data
		m_actionCode	= data->trigger.actionCode;
		m_loop			= data->trigger.loop;
		m_timeout		= data->trigger.timeout;
		m_range			= data->trigger.range;
	}

	printf("Trigger %s initialised!\n", node->getName().c_str());
}

//================================================//

void Trigger::trigger(void)
{
	switch(m_actionCode){
	default:

		break;

	case TRIGGER_ACTION_CODE::ACTION_AMBIENT_LIGHT_ENABLE: // test
		m_pSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
		break;

	case TRIGGER_ACTION_CODE::ACTION_NPC_ENABLE:
		static_cast<NPC*>(m_pActionPointer)->setState(NPC::STATE_ACTIVE);
		break;

	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_ACTIVATE:
		if(static_cast<DynamicObject*>(m_pActionPointer)->getState() == DynamicObject::STATE_ACTIVATED){
			return;
		}
		static_cast<DynamicObject*>(m_pActionPointer)->DynamicObject::send(DynamicObject::ARG_ACTIVATE);
		break;

	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_DEACTIVATE:
		static_cast<DynamicObject*>(m_pActionPointer)->DynamicObject::send(DynamicObject::ARG_DEACTIVATE);
		break;
	}

	// Set trigger to true for update loop
	m_triggered = true;

	if(m_loop){
		m_pTimeout->reset();
	}
}

//================================================//

void Trigger::reset(void)
{
	m_triggered = false;
	m_pTimeout->reset();
}

//================================================//

void Trigger::deleteData(void)
{
	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty())
		delete Ogre::any_cast<DynamicObjectData*>(any);
}

//================================================//

void Trigger::update(double timeSinceLastFrame)
{
	if(m_triggered){
		if(m_loop){
			if(m_pTimeout->getMillisecondsCPU() >= m_timeout){
				m_triggered = false;
			}
		}
	}
}

//================================================//
//================================================//

TriggerWalkOver::TriggerWalkOver(void) : Trigger()
{

}

//================================================//

TriggerWalkOver::~TriggerWalkOver(void)
{

}

//================================================//

void TriggerWalkOver::update(double timeSinceLastFrame)
{
	Trigger::update(timeSinceLastFrame);

	if(!m_triggered){
		Rayhit* rayhit = m_pCamera->getNegativeYRayhit();
		if(rayhit->hasHit){
			if(rayhit->node == m_pSceneNode){
				this->trigger();
			}
		}
	}
}

//================================================//
//================================================//

TriggerLookAt::TriggerLookAt(void) : Trigger()
{

}

//================================================//

TriggerLookAt::~TriggerLookAt(void)
{

}

//================================================//

void TriggerLookAt::update(double timeSinceLastFrame)
{
	Trigger::update(timeSinceLastFrame);

	if(!m_triggered){
		Rayhit* rayhit = m_pCamera->getNegativeZRayhit();
		if(rayhit->hasHit){
			if(rayhit->node == m_pSceneNode){
				if(rayhit->distance <= m_range){
					this->trigger();
				}
			}
		}
	}
}

//================================================//

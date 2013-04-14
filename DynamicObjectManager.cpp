//================================================//

#include "DynamicObjectManager.hpp"

//================================================//

DynamicObjectManager::DynamicObjectManager(Ogre::SceneManager* mgr, Sparks::Camera* camera)
{
	m_pSceneMgr = mgr;
	m_pCamera = camera;

	m_physics = m_pCamera->getPhysics();
}

//================================================//

DynamicObjectManager::~DynamicObjectManager(void)
{
	for(std::vector<DynamicObject*>::iterator itr = m_objects.begin();
		itr != m_objects.end();){
		// Delete dynamic object data stored on the heap
		(*itr)->deleteData();
		(*itr)->freeUserData();

		++itr;
	}
}

//================================================//

bool DynamicObjectManager::addObject(Ogre::SceneNode* node, btCollisionObject* obj, int tier)
{
	const Ogre::Any& any = node->getUserAny();

	switch(tier){
	case 1:
		switch(DynamicObject::findType(node)){
		default:
			return false;

		case DynamicObject::TYPE_MOVING_OBJECT:
			m_objects.push_back(new MovingObject());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);

			m_objects.back()->setupAnimation();

			return true;
			
		case DynamicObject::TYPE_MOVING_KINEMATIC_OBJECT:
			m_objects.push_back(new MovingKinematicObject());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);

			m_objects.back()->setupAnimation();

			return true;
			
		case DynamicObject::TYPE_ELEVATOR:
			m_objects.push_back(new Elevator());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);
			m_objects.back()->setState(DynamicObject::STATE_IDLE);

			m_objects.back()->setupAnimation();

			return true;

		case DynamicObject::TYPE_ROTATING_DOOR:
			m_objects.push_back(new RotatingDoor());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);
			m_objects.back()->setState(DynamicObject::STATE_IDLE);

			
			return true;

		case DynamicObject::TYPE_SLIDING_DOOR:
			m_objects.push_back(new SlidingDoor());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);
			m_objects.back()->setState(DynamicObject::STATE_IDLE);

			
			return true;

		case DynamicObject::TYPE_STATIC_LIGHT:
			m_objects.push_back(new StaticLight());
			m_objects.back()->initLight(m_pSceneMgr, node);

			return true;

		case DynamicObject::TYPE_PULSE_LIGHT:
			m_objects.push_back(new PulseLight());
			m_objects.back()->initLight(m_pSceneMgr, node);

			return true;

		case DynamicObject::TYPE_MAGIC_CUBE:
			m_objects.push_back(new MagicCube());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);

			return true;
		}
		break;

	case 2:
		switch(DynamicObject::findType(node)){
		default:
			return false;

		case DynamicObject::TYPE_SWITCH:
			m_objects.push_back(new Switch());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);

			// Find the dynamic object it should be linked to
			if(!any.isEmpty()){
				if(any.getType() == typeid(Ogre::String)){
					Ogre::String name = Ogre::any_cast<Ogre::String>(any); // get the string

					// Acquire the link pointer
					DynamicObject* link = this->getObject(name);
					if(link != nullptr){
						m_objects.back()->setLinkedObject(link);
						link->attachSwitch(static_cast<Switch*>(m_objects.back()));
					}
				}
			}
			return true;
		
		case DynamicObject::TYPE_NPC:
			m_pNPCManager->addNPC(node, obj);
			return true;
		}
		break;

	// Triggers
	case 3:
		{
			if(!any.isEmpty()){
				// A trigger must have DynamicObjectData*
				if(any.getType() != typeid(DynamicObjectData*))
					return false;

				DynamicObjectData* data = Ogre::any_cast<DynamicObjectData*>(any);

				if(data->trigger.enabled){
					switch(data->trigger.type){
					default:

						return false;

					case Trigger::TRIGGER_WALK_OVER:
						m_objects.push_back(new TriggerWalkOver());
						break;

					case Trigger::TRIGGER_LOOK_AT:
						m_objects.push_back(new TriggerLookAt());
						break;
					}

					m_objects.back()->initTrigger(m_pSceneMgr, node, m_pCamera);
					this->registerTriggerAction(data);

					return true;
				} // trigger.enabled
			}
		}
		break;
		
	default:
		break;
	}

	return false;
}

//================================================//

void DynamicObjectManager::registerAllObjectsInScene(void)
{
	int tier = 1;
	const int MAX_TIER = 3;

	// Iterate through collision world objects and add them to the manager based on tier
	for(;tier<=MAX_TIER; ++tier){
		btCollisionObjectArray& objects = m_physics->getWorld()->getCollisionObjectArray();
		int size = objects.size();
		for(int i=(size - 1); i>=0; --i){

			if(addObject(static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer()), objects.at(i), tier)){
				printf("Added object: %s\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
			}
			else{
				printf("Skipped object: %s\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
			}
		}
	}
	// I have no way of retrieving the Bullet collision object from the scene nodes when going through that iterator, so all dynamic
	// objects will need to be collision objects as well. Exceptions can be made, of course.

	// Then remove all extra garbage from the BtOgre function
}

//================================================//

void DynamicObjectManager::registerTriggerAction(DynamicObjectData* data)
{
	// Since this class has access to almost everything, register this data here

	switch(data->trigger.actionCode){
	default:
		break;

	case TRIGGER_ACTION_CODE::ACTION_NPC_ENABLE:
		m_objects.back()->setLinkedObject(m_pNPCManager->getNPC(data->trigger.str));
		break;

	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_ACTIVATE:
	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_DEACTIVATE:
		m_objects.back()->setLinkedObject(this->getObject(data->trigger.str));
		break;

	case TRIGGER_ACTION_CODE::ACTION_DISPLAY_TEXT:
		m_objects.back()->setLinkedObject((void*)data->trigger.str.c_str());
		m_objects.back()->setTimeout(data->trigger.timeout);	
		m_objects.back()->setUserData(0, (void*)(new int(data->trigger.x)));
		m_objects.back()->setUserData(1, (void*)(new Ogre::ColourValue(data->colour)));
		break;
	}
}

//================================================//

DynamicObject* DynamicObjectManager::getObject(Ogre::String& name)
{
	Ogre::StringUtil strUtil;

	for(std::vector<DynamicObject*>::iterator itr = m_objects.begin();
		itr != m_objects.end();
		++itr){
		Ogre::String str = static_cast<Ogre::String>((*itr)->getSceneNode()->getName());
		
		if(strUtil.match(name, str, true)){
			return *itr;
		}
	}

	return nullptr;
}

//================================================//

DynamicObject* DynamicObjectManager::getObject(Ogre::SceneNode* node)
{
	for(std::vector<DynamicObject*>::iterator itr = m_objects.begin();
		itr != m_objects.end();
		++itr){
		if((*itr)->getSceneNode() == node)
			return *itr;
	}

	return nullptr;
}

//================================================//

void DynamicObjectManager::update(double timeSinceLastFrame)
{
	for(std::vector<DynamicObject*>::iterator itr = m_objects.begin();
		itr != m_objects.end();){

		if((*itr)->isRetrievable()){
			if((*itr)->isRetrieved()){
				// Delete item from list
				delete *itr;
				itr = m_objects.erase(itr);
				continue;
			}
		}

		if((*itr)->needsUpdate()){
			(*itr)->update(timeSinceLastFrame);
		}

		++itr;
	}
}

//================================================//
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
	for(DynamicObjectListIterator itr = m_objects.begin();
		itr != m_objects.end();){
		// Delete dynamic object data stored on the heap
		(*itr)->deleteData();
		(*itr)->freeUserData();

		++itr;
	}
}

//================================================//

bool DynamicObjectManager::addObject(Ogre::SceneNode* node, btCollisionObject* obj, int type, int tier)
{
	const Ogre::Any& any = node->getUserAny();

	switch(tier){
	case 1:
		switch(type){
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
		switch(type){
		default:
			return false;

		case DynamicObject::TYPE_SWITCH:
			m_objects.push_back(new Switch());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);
			
			return true;
		
		case DynamicObject::TYPE_NPC:
			//m_pNPCManager->addNPC(node, obj);
			return true;
		}
		break;

	// Triggers
	case 3:
		{
			if(type == DynamicObject::TYPE_TRIGGER){
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

						case Trigger::TRIGGER_CHAIN_NODE:
							m_objects.push_back(new TriggerChainNode());
							break;
						}

						m_objects.back()->initTrigger(m_pSceneMgr, node, m_pCamera);
						
						return true;
					} // trigger.enabled
				}
			} // TYPE_TRIGGER
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
	int tier = 2;
	const int MAX_TIER = 3;

	// Iterate through collision world objects and add them to the manager based on tier
	for(;tier<=MAX_TIER; ++tier){
		btCollisionObjectArray& objects = m_physics->getWorld()->getCollisionObjectArray();
		int size = objects.size();
		for(int i=(size - 1); i>=0; --i){
			Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer());
			if(node != nullptr){
				if(addObject(node, objects.at(i), DynamicObject::findType(node), tier)){
					printf("Added object: %s\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
				}
				else{
					printf("Skipped object: %s\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
				}
			}
		}
	}
	// I have no way of retrieving the Bullet collision object from the scene nodes when going through that iterator, so all dynamic
	// objects will need to be collision objects as well. Exceptions can be made, of course.

	// Do this after all triggers are registered
	this->registerTriggerChains();
}

//================================================//

void DynamicObjectManager::registerUpperTierObjects(void)
{
	DynamicObjectData* data = nullptr;
	DynamicObject* object = nullptr;

	// Iterate through all dynamic objects and determine if it needs to be registered
	for(DynamicObjectListIterator itr = m_objects.begin();
		itr != m_objects.end();
		++itr){

		// Trigger
		if(strstr(typeid(**itr).name(), "Trigger")){
			object = *itr;
			data = object->getData();
			if(data != nullptr){
				this->registerTriggerAction(object, data);
				printf("Trigger registered: %s\n", object->getSceneNode()->getName().c_str());
				// Test for chain node
				if(data->trigger.hasNext){
					Trigger* trigger = dynamic_cast<Trigger*>(object);
					if(trigger){
						DynamicObject* next = this->getObject(data->trigger.next);
						if(next != nullptr)
							trigger->setNextTrigger(static_cast<Trigger*>(next));
						else
							data->trigger.hasNext = false;
					}
				}
			}
		}

		// Switch
		else if(typeid(**itr) == typeid(Switch)){
			const Ogre::Any& any = (*itr)->getSceneNode()->getUserAny();
			if(!any.isEmpty()){
				if(any.getType() == typeid(Ogre::String)){
					Ogre::String name = Ogre::any_cast<Ogre::String>(any);

					// Link found for switch, now set it up
					DynamicObject* link = this->getObject(name);
					if(link != nullptr){
						(*itr)->setLinkedObject(link);
						link->attachSwitch(static_cast<Switch*>(*itr));
					}
				}
			}
		}

	}

	//this->registerTriggerChains();
}

//================================================//

void DynamicObjectManager::registerTriggerAction(DynamicObject* object, DynamicObjectData* data)
{
	// Since this class has access to almost everything, register this data here

	switch(data->trigger.actionCode){
	default:
		break;

	case TRIGGER_ACTION_CODE::ACTION_NPC_ENABLE:
		object->setLinkedObject(m_pNPCManager->getNPC(data->trigger.str));
		break;

	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_ACTIVATE:
	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_DEACTIVATE:
		object->setLinkedObject(this->getObject(data->trigger.str));
		break;

	case TRIGGER_ACTION_CODE::ACTION_DISPLAY_TEXT:
		object->setLinkedObject((void*)data->trigger.str.c_str());
		object->setTimeout(data->trigger.timeout);	
		object->setUserData(0, (void*)(new int(data->trigger.x)));
		object->setUserData(1, (void*)(new Ogre::ColourValue(data->colour)));
		break;
	}
}

//================================================//

void DynamicObjectManager::registerTriggerChains(void)
{
	for(DynamicObjectListIterator itr = m_objects.begin();
		itr != m_objects.end();
		++itr){
		// Determine if this object is derived from Trigger
		Trigger* trigger = dynamic_cast<Trigger*>(*itr);
		if(trigger){
			DynamicObjectData* data = trigger->getData();
			if(data->trigger.hasNext){
				DynamicObject* next = this->getObject(data->trigger.next);
				if(next != nullptr){
					trigger->setNextTrigger(static_cast<Trigger*>(next));
				}
				else{
					data->trigger.hasNext = false;
				}
			}
		}
	}
}

//================================================//

DynamicObject* DynamicObjectManager::getObject(Ogre::String& name)
{
	Ogre::StringUtil strUtil;

	for(DynamicObjectListIterator itr = m_objects.begin();
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
	for(DynamicObjectListIterator itr = m_objects.begin();
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
	for(DynamicObjectListIterator itr = m_objects.begin();
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
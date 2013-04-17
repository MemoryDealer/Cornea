//================================================//

#include "DynamicObject.hpp"

//================================================//

DynamicObject::DynamicObject(void)
{
	m_state = STATE_IDLE;
	m_needsUpdate = true;
	m_updateRange = 20000.0;

	m_retrievable = m_retrieved = false;
	
	m_hasSound = false;

	for(int i=0; i<USERDATA_SIZE; ++i){
		m_userData.flags[i] = false;
		m_userData.data[i] = nullptr;
	}
}

//================================================//

DynamicObject::~DynamicObject(void)
{
	printf("Base Destructor called on %s\n", m_pSceneNode->getName().c_str());

	if(m_hasSound){
		delete m_pSound;
	}
}

//================================================//

void DynamicObject::init(Ogre::SceneNode* node, btCollisionObject* colObj)
{
	m_pSceneNode = node;
	m_collisionObject = colObj;
}

//================================================//

void DynamicObject::init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj)
{
	m_pSceneMgr = mgr;
	m_physics = physics;
	m_pSceneNode = node;
	m_collisionObject = colObj;
}

//================================================//

void DynamicObject::initSound(const char* file, bool loop)
{
	
}

//================================================//

unsigned DynamicObject::send(unsigned arg)
{
	switch(arg){
	case ARG_ACTION:
		m_state = !m_state;
		break;

	case ARG_ACTIVATE:
		m_state = STATE_ACTIVATED;
		break;

	case ARG_DEACTIVATE:
		m_state = STATE_IDLE;
		break;
	}

	return 0;
}

//================================================//

unsigned DynamicObject::recv(void)
{

	return 0;
}

//================================================//

unsigned DynamicObject::recv(unsigned arg)
{

	return 0;
}

//================================================//

bool DynamicObject::needsUpdate(void)
{
	// check distance

	return m_needsUpdate;
}

//================================================//

DynamicObjectData* DynamicObject::getData(void) const
{
	const Ogre::Any& any = m_pSceneNode->getUserAny();
	DynamicObjectData* data = nullptr;

	if(!any.isEmpty()){
		if(any.getType() == typeid(DynamicObjectData*))
			data = Ogre::any_cast<DynamicObjectData*>(any);
	}

	return data;
}

//================================================//

void DynamicObject::deleteData(void)
{
	const Ogre::Any& any = m_pSceneNode->getUserAny();

	if(!any.isEmpty()){
		if(any.getType() == typeid(DynamicObjectData*)){
			delete Ogre::any_cast<DynamicObjectData*>(any);
			printf("%s data deleted\n", m_pSceneNode->getName().c_str());
		}
	}
}

//================================================//

void DynamicObject::setUserData(int n, void* data)
{
	m_userData.data[n] = data;
	m_userData.flags[n] = true;
}

//================================================//

void DynamicObject::freeUserData(void)
{
	for(int i=0; i<USERDATA_SIZE; ++i){
		if(m_userData.flags[i])
			if(m_userData.data[i] != nullptr)
				delete m_userData.data[i];
	}
}

//================================================//

void DynamicObject::retrieve(void)
{
	// Mark object for removal and delete the scene node
	m_retrieved = true;

	m_physics->getWorld()->removeCollisionObject(m_collisionObject);
	m_pSceneNode->detachAllObjects();
}

//================================================//

int DynamicObject::findType(Ogre::SceneNode* node)
{
	const char* name = node->getName().c_str();

	if(strstr(name, "_MovingObject_"))
		return TYPE_MOVING_OBJECT;
	if(strstr(name, "_MovingKinematicObject_"))
		return TYPE_MOVING_KINEMATIC_OBJECT;
	if(strstr(name, "_Elevator_"))
		return TYPE_ELEVATOR;
	if(strstr(name, "_RDoor_") || strstr(name, "_Door_"))
		return TYPE_ROTATING_DOOR;
	if(strstr(name, "_SDoor_"))
		return TYPE_SLIDING_DOOR;
	if(strstr(name, "_Switch_"))
		return TYPE_SWITCH;
	if(strstr(name, "_NPC_"))
		return TYPE_NPC;

	if(strstr(name, "_Light_") || strstr(name, "_SLight"))
		return TYPE_STATIC_LIGHT;
	if(strstr(name, "_PLight_"))
		return TYPE_PULSE_LIGHT;
	if(strstr(name, "_FLight_"))
		return TYPE_FLICKER_LIGHT;

	if(strstr(name, "_MagicCube_"))
		return TYPE_MAGIC_CUBE;

	if(strstr(name, "Trigger"))
		return TYPE_TRIGGER;

	return -1;
}

//================================================//

unsigned int DynamicObject::getTier(int type)
{
	switch(type){
	default:
		return 0;

	// Tier 1 means it doesn't depend on another DynamicObject
	case TYPE_MOVING_OBJECT:
	case TYPE_MOVING_KINEMATIC_OBJECT:
	case TYPE_ELEVATOR:
	case TYPE_ROTATING_DOOR:
	case TYPE_SLIDING_DOOR:
	case TYPE_STATIC_LIGHT:
	case TYPE_PULSE_LIGHT:
	case TYPE_FLICKER_LIGHT:
	case TYPE_MAGIC_CUBE:
		return 1;

	case TYPE_SWITCH:
		return 2;

	case TYPE_TRIGGER:
		return 3;
	}
}

//================================================//

void DynamicObject::update(double timeSinceLastFrame)
{
	// Update the world transform for the Bullet collision object
	btTransform transform(btQuaternion(m_pSceneNode->_getDerivedOrientation().x, m_pSceneNode->_getDerivedOrientation().y, m_pSceneNode->_getDerivedOrientation().z,
		m_pSceneNode->_getDerivedOrientation().w),
		btVector3(m_pSceneNode->_getDerivedPosition().x, m_pSceneNode->_getDerivedPosition().y, m_pSceneNode->_getDerivedPosition().z));
	m_collisionObject->setWorldTransform(transform);

	//Base::getSingletonPtr()->m_btWorld->updateSingleAabb(m_collisionObject);  // needed?
}

//================================================//
//================================================//

/* Moving Object */

MovingObject::MovingObject(void)
	: DynamicObject()
{

}

//================================================//

MovingObject::~MovingObject(void)
{

}

//================================================//

void MovingObject::setupAnimation(void)
{
	DynamicObjectData* data = this->getData();
	if(data == nullptr)
		return;

	m_loop = data->animation.loop;

	// Create animation
	Ogre::Animation* anim = m_pSceneMgr->createAnimation(m_pSceneNode->getName() + "_Animation", data->animation.length);
	anim->setInterpolationMode((data->animation.spline == true) ? Ogre::Animation::IM_SPLINE : Ogre::Animation::IM_LINEAR);

	// Create node track
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	Ogre::Real timePos = 0.0;
	for(std::vector<Ogre::Vector3>::iterator itr = data->animation.vectors.begin();
		itr != data->animation.vectors.end();
		++itr, timePos += data->animation.step){
			/*(*itr).x /= m_pSceneNode->_getDerivedScale().x;
			(*itr).y /= m_pSceneNode->_getDerivedScale().y;
			(*itr).z /= m_pSceneNode->_getDerivedScale().z;*/
			track->createNodeKeyFrame(timePos)->setTranslate((*itr));
	}

	m_pSceneNode->setInitialState();

	// Set up animation state for updating
	m_pAnimState = m_pSceneMgr->createAnimationState(m_pSceneNode->getName() + "_Animation");
	m_pAnimState->setEnabled(true);

	if(data->animation.active)
		m_state = STATE_ACTIVATED;
}

//================================================//

void MovingObject::update(double timeSinceLastFrame)
{
	if(m_state == STATE_ACTIVATED){
		DynamicObject::update(timeSinceLastFrame);

		m_pAnimState->addTime(timeSinceLastFrame);
	}
}

//================================================//
//================================================//

/* Moving Kinematic Object */

MovingKinematicObject::MovingKinematicObject(void)
	: MovingObject()
{

}

//================================================//

void MovingKinematicObject::setupAnimation(void)
{
	DynamicObjectData* data = this->getData();
	if(data == nullptr)
		return;

	// Remove the collision object
	m_physics->getWorld()->removeCollisionObject(m_collisionObject);

	// Add rigid body
	const float modifier = 0.98f; // Ogre's AABBs are slightly larger than Bullet's
	Ogre::AxisAlignedBox bb = m_pSceneNode->getAttachedObject(0)->getBoundingBox();
	Ogre::Vector3 bbSize;
	Ogre::Vector3 pos = m_pSceneNode->_getDerivedPosition();
	Ogre::Vector3 scale = m_pSceneNode->_getDerivedScale();
	bbSize.x = bb.getSize().x * scale.x * modifier;
	bbSize.y = bb.getSize().y * scale.y * modifier;
	bbSize.z = bb.getSize().z * scale.z * modifier;

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btVector3 halfExtents(bbSize.x * 0.5f, bbSize.y * 0.5f, bbSize.z * 0.5f);
	btCollisionShape* shape = new btBoxShape(halfExtents);

	btVector3 inertia;
	shape->calculateLocalInertia(0.0, inertia);
	//inertia.setValue(0.7f, 0.7f, 0.7f);
	
	m_rigidBody = new btRigidBody(0.0, motionState, shape, inertia);
	m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	if(data->animation.friction)
		m_rigidBody->setFriction(data->animation.friction);
	m_rigidBody->setUserPointer((void*)m_pSceneNode);

	/*m_rigidBody->setHitFraction(0.0);
	m_rigidBody->setContactProcessingThreshold(0.0);*/

	m_physics->getWorld()->addRigidBody(m_rigidBody);

	// Now set the animation track
	MovingObject::setupAnimation();
}

//================================================//

void MovingKinematicObject::update(double timeSinceLastFrame)
{
	if(m_state == STATE_ACTIVATED){
		// Must use the derived position here due to the initial state being set in setupAnimation()
		Ogre::Vector3 pos = m_pSceneNode->_getDerivedPosition();

		btTransform trans = m_rigidBody->getWorldTransform();
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
		m_rigidBody->setWorldTransform(trans);
		btMotionState* motion = m_rigidBody->getMotionState();
		motion->getWorldTransform(trans);
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
		motion->setWorldTransform(trans);

		m_pAnimState->addTime(timeSinceLastFrame);

		/*m_rigidBody->setInterpolationWorldTransform(m_rigidBody->getWorldTransform());
		m_rigidBody->setInterpolationLinearVelocity(btVector3(0, 0, 0));
		m_rigidBody->setInterpolationAngularVelocity(btVector3(0, 0, 0));*/
	}
}

//================================================//
//================================================//

/* Elevator */

Elevator::Elevator(void) : 
	MovingObject()
{

}

//================================================//

unsigned Elevator::send(unsigned arg)
{
	switch(arg){
	case ARG_OVERRIDE: // If player activated switch to the elevator
		DynamicObject::send(ARG_ACTION);
		break;

	case ARG_ACTION: // If player activated elevator itself
	default:
		break;
	}

	return 0;
}

//================================================//

void Elevator::update(double timeSinceLastFrame)
{
	MovingObject::update(timeSinceLastFrame);
}

//================================================//
//================================================//

/* Switch */

Switch::Switch(void)
	: DynamicObject()
{
	
}

//================================================//

void Switch::init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj)
{
	DynamicObject::init(mgr, physics, node, colObj);

	//DynamicObject::initSound("C:/test.wav");

	m_linked = false;
}

//================================================//

unsigned Switch::send(unsigned arg)
{
	if(m_linked){
		m_linkedObject->send(ARG_OVERRIDE); 

		if(m_hasSound)
			m_pSound->play();
	}

	return 0;
}

//================================================//

void Switch::deleteData(void)
{
	// no data on the heap here
	return;
}

//================================================//

void Switch::update(double timeSinceLastFrame)
{

}

//================================================//
//================================================//



//================================================//
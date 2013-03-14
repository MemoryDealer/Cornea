//================================================//

#include "Doors.hpp"

//================================================//

const Ogre::String Door::OPENING_ANIMATION_NAME = "_OpeningAnimation";
const Ogre::String Door::CLOSING_ANIMATION_NAME = "_ClosingAnimation";
const Ogre::Real Door::OPENING_LENGTH = 1000.0;
const Ogre::Real Door::CLOSING_LENGTH = 1000.0;

//================================================//

Door::Door(void) : MovingObject()
{
	// Set to closed and unlocked by default
	m_opening = true;
	m_locked = false;
}

//================================================//

Door::~Door(void)
{
	m_pSceneMgr->destroyAnimation(m_pSceneNode->getName() + OPENING_ANIMATION_NAME);
	m_pSceneMgr->destroyAnimation(m_pSceneNode->getName() + CLOSING_ANIMATION_NAME);
}

//================================================//

unsigned Door::send(unsigned arg)
{
	switch(arg){
	default:
	case ARG_ACTION:
		// This overcomplicated!!!
		if(m_locked){
			
			// Play locked sound
			return 0;
		}
		if(m_state == STATE_IDLE){
			m_state = STATE_ACTIVATED;

			if(m_pOpeningState->hasEnded()){
				m_pOpeningState->setEnabled(false);
				m_pClosingState->setEnabled(true);
				m_pOpeningState->setTimePosition(0.0);
				m_opening = false;
			}
			else if(m_pClosingState->hasEnded()){
				m_pClosingState->setEnabled(false);
				m_pOpeningState->setEnabled(true);
				m_pClosingState->setTimePosition(0.0);
				m_opening = true;
			}
		}
		break;

	case ARG_OVERRIDE:
		if(m_locked){
			// A switch activated door
			m_locked = false;
			this->send(ARG_ACTION);
			m_locked = true;
		}
		else{
			this->send(ARG_ACTION);
		}
		break;
	}

	return 0;
}

//================================================//

void Door::update(double timeSinceLastFrame)
{
	if(m_state == STATE_ACTIVATED){	
		DynamicObject::update(timeSinceLastFrame);

		// Open/Close logic
		if(m_opening){
			m_pOpeningState->addTime(timeSinceLastFrame);
			
			if(m_pOpeningState->hasEnded()){
				m_state = STATE_IDLE;
			}
		}
		else{
			m_pClosingState->addTime(timeSinceLastFrame);

			if(m_pClosingState->hasEnded()){
				m_state = STATE_IDLE;
			}
		}
	}
}

//================================================//

void Door::calculateOffset(void)
{
	m_offset = Ogre::Vector3::ZERO;
	m_rotationOffset = Ogre::Quaternion::ZERO;

	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty()){
		DynamicObject::DYNAMIC_OBJECT_DATA* data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);

		m_offset = data->offset;
		
		// The offset vector should only have one non-zero value, so multiply the entire thing by
		// the distance from the center of the door to the edge to find the amount to move
		m_offset *= m_pSceneNode->getAttachedObject(0)->getBoundingBox().getCenter().distance(
			m_pSceneNode->getAttachedObject(0)->getBoundingBox().getCorner(Ogre::AxisAlignedBox::CornerEnum::FAR_RIGHT_TOP).midPoint(
			m_pSceneNode->getAttachedObject(0)->getBoundingBox().getCorner(Ogre::AxisAlignedBox::CornerEnum::FAR_RIGHT_BOTTOM))) * 1.96;

		m_locked = data->buffer;
	}
}

//================================================//
//================================================//

void RotatingDoor::init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj)
{
	DynamicObject::init(mgr, physics, node, colObj);

	this->calculateOffset();

	// Setup animations
	// Opening
	Ogre::Animation* anim = m_pSceneMgr->createAnimation(m_pSceneNode->getName() + OPENING_ANIMATION_NAME, OPENING_LENGTH);
	anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);

	// Track
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	track->createNodeKeyFrame(0.0)->setRotation(Ogre::Quaternion::IDENTITY);
	track->createNodeKeyFrame(OPENING_LENGTH)->setRotation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5), 0.0, -Ogre::Math::Sqrt(0.5), 0.0));

	m_pOpeningState = m_pSceneMgr->createAnimationState(m_pSceneNode->getName() + OPENING_ANIMATION_NAME);
	m_pOpeningState->setEnabled(true);
	m_pOpeningState->setLoop(false);

	// Closing
	anim = m_pSceneMgr->createAnimation(m_pSceneNode->getName() + CLOSING_ANIMATION_NAME, CLOSING_LENGTH);
	anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);

	// Track
	track = anim->createNodeTrack(0, m_pSceneNode);
	track->createNodeKeyFrame(0.0)->setRotation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5), 0.0, -Ogre::Math::Sqrt(0.5), 0.0));
	track->createNodeKeyFrame(CLOSING_LENGTH)->setRotation(Ogre::Quaternion::IDENTITY);

	m_pClosingState = m_pSceneMgr->createAnimationState(m_pSceneNode->getName() + CLOSING_ANIMATION_NAME);
	m_pClosingState->setEnabled(false);
	m_pClosingState->setLoop(false);

	m_pSceneNode->setInitialState();
}

//================================================//
//================================================//

void SlidingDoor::init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj)
{
	DynamicObject::init(mgr, physics, node, colObj);

	this->calculateOffset();

	// Setup animations
	// Opening
	Ogre::Animation* anim = m_pSceneMgr->createAnimation(m_pSceneNode->getName() + OPENING_ANIMATION_NAME, OPENING_LENGTH);
	anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);
	Ogre::TransformKeyFrame* key;

	// Track
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	key = track->createNodeKeyFrame(0.0);
	key->setRotation(Ogre::Quaternion::IDENTITY);
	key->setTranslate(Ogre::Vector3::ZERO);

	track->createNodeKeyFrame(OPENING_LENGTH)->setTranslate(m_offset);

	m_pOpeningState = m_pSceneMgr->createAnimationState(m_pSceneNode->getName() + OPENING_ANIMATION_NAME);
	m_pOpeningState->setEnabled(true);
	m_pOpeningState->setLoop(false);

	// Closing
	anim = m_pSceneMgr->createAnimation(m_pSceneNode->getName() + CLOSING_ANIMATION_NAME, CLOSING_LENGTH);
	anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);

	// Track
	track = anim->createNodeTrack(0, m_pSceneNode);
	key = track->createNodeKeyFrame(0.0);
	key->setRotation(Ogre::Quaternion::IDENTITY);
	key->setTranslate(m_offset);
	
	track->createNodeKeyFrame(CLOSING_LENGTH)->setTranslate(Ogre::Vector3::ZERO);

	m_pClosingState = m_pSceneMgr->createAnimationState(m_pSceneNode->getName() + CLOSING_ANIMATION_NAME);
	m_pClosingState->setEnabled(false);
	m_pClosingState->setLoop(false);

	m_pSceneNode->setInitialState();
}

//================================================//
//================================================//
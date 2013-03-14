//================================================//

#ifndef __DOORS_HPP__
#define __DOORS_HPP__

//================================================//

#include "DynamicObject.hpp"

//================================================//

/* A simple rotating door */
class Door : public MovingObject
{
public:
	Door(void);
	~Door(void);

	virtual void init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj) = 0;

	unsigned send(unsigned arg);
	void lock(void);
	void unlock(void);

	void update(double timeSinceLastFrame);

protected:
	static const Ogre::String	OPENING_ANIMATION_NAME;
	static const Ogre::String	CLOSING_ANIMATION_NAME;
	static const Ogre::Real		OPENING_LENGTH;
	static const Ogre::Real		CLOSING_LENGTH;

	void calculateOffset(void);

	Ogre::AnimationState*	m_pOpeningState;
	Ogre::AnimationState*	m_pClosingState;
	Ogre::Vector3			m_offset;
	Ogre::Quaternion		m_rotationOffset;
	bool					m_opening;
	bool					m_locked;
};

//================================================//

inline void Door::lock(void)
{ m_locked = true; }

inline void Door::unlock(void)
{ m_locked = false; }

//================================================//
//================================================//

class RotatingDoor : public Door
{
public:
	RotatingDoor(void) : Door(){}

	virtual void init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj);

private:

};

//================================================//
//================================================//

class SlidingDoor : public Door
{
public:
	SlidingDoor(void) : Door(){}

	virtual void init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj);

private:

};

//================================================//
//================================================//

#endif

//================================================//
//================================================//

#ifndef __WORLDOBJECT_HPP__
#define __WORLDOBJECT_HPP__

#define USE_KINEMATIC_GROUND 1

//================================================//

#include "stdafx.h"
#include "Base.hpp"
#include "Sparks.hpp"
#include "Camera.hpp"
	
//================================================//

class Switch;

//================================================//

/* A class for interactive/moving objects in the world */
class DynamicObject{
public:
	typedef struct{

		struct{
			Ogre::Real		step;
			Ogre::Real		length;
			btScalar		friction;
			bool			spline;
			bool			loop;
			std::vector<Ogre::Vector3> vectors;
		} animation;

		struct{
			unsigned int	actionCode;
			bool			loop;
			Ogre::Real		timeout;
			Ogre::String	buffer;

			Ogre::Real		range;
		} trigger;

	} DYNAMIC_OBJECT_DATA, *PDYNAMIC_OBJECT_DATA;

	enum{
		STATE_IDLE = 0,
		STATE_ACTIVATED = 1
	};

	enum{
		ARG_NULL = 0,
		ARG_ACTION = 1, 
		ARG_ACTIVATE,
		ARG_DEACTIVATE,
		ARG_OVERRIDE
	};

	// --- //

	DynamicObject(void);
	virtual ~DynamicObject(void);

	virtual void init(Ogre::SceneNode* node, btCollisionObject* colObj);
	virtual void init(Ogre::SceneManager* mgr, Ogre::SceneNode* node, btCollisionObject* colObj);
	virtual void initTrigger(Ogre::SceneManager* mgr, Ogre::SceneNode* node, Sparks::Camera* camera){} 
	virtual void initSound(const char* file, bool loop = false);

	virtual unsigned send(unsigned arg);		// send a command
	virtual unsigned recv(void);
	virtual unsigned recv(unsigned arg);

	// Some virtual functions to be used by children
	virtual void setupAnimation(DYNAMIC_OBJECT_DATA* data){}
	virtual void setTriggerData(DYNAMIC_OBJECT_DATA* data){}
	virtual void setLinkedObject(DynamicObject* obj){}
	virtual void setLinkedObject(void* obj){}
	virtual void attachSwitch(Switch* _switch){}

	// Getter functions
	const bool isActive(void) const;
	const unsigned getState(void) const;
	Ogre::SceneNode* getSceneNode(void) const;

	// Setter functions
	void setState(const unsigned state){ m_state = state; }
	void activate(void);

	bool needsUpdate(void);

	virtual void update(double timeSinceLastFrame) = 0;

protected:
	Ogre::SceneManager* m_pSceneMgr;
	Ogre::SceneNode*	m_pSceneNode;
	btCollisionObject*  m_collisionObject;
	Ogre::Vector3		m_position;

	bool				m_needsUpdate;
	Ogre::Real			m_updateRange;
	unsigned			m_state;
	
	FMOD::Sound*		m_sound;
	FMOD::Channel*		m_soundChannel;
};

//================================================//

inline const bool DynamicObject::isActive(void) const
{ return (m_state > STATE_IDLE) ? true : false; }

inline const unsigned DynamicObject::getState(void) const
{ return m_state; }

inline Ogre::SceneNode* DynamicObject::getSceneNode(void) const
{ return m_pSceneNode; }

inline void DynamicObject::activate(void)
{ m_state = STATE_ACTIVATED; }

//================================================//

/* Here the inherited classes will be defined, such as door, elevator, lever, etc. */

//================================================//
//================================================//

/* Moving object, moves along a track of points in an infinite loop */
class MovingObject : public DynamicObject{
public:
	MovingObject(void);
	virtual ~MovingObject(void);

	virtual void setupAnimation(DYNAMIC_OBJECT_DATA* data);

	virtual void update(double timeSinceLastFrame);

protected:
	Ogre::AnimationState*		m_animState;
	bool						m_loop;
};

//================================================//
//================================================//

/* Moving kinematic object, same as MovingObject, but applies friction to rigid bodies, e.g. a moving platform that holds the player */
class MovingKinematicObject : public MovingObject{
public:
	MovingKinematicObject(void);

	void setupAnimation(DYNAMIC_OBJECT_DATA* data);

	void update(double timeSinceLastFrame);

protected:
	btRigidBody*		m_rigidBody;
};

//================================================//
//================================================//

class Elevator : public MovingObject{
public:
	Elevator(void);

	unsigned send(unsigned arg); 

	void update(double timeSinceLastFrame);

protected:

};

//================================================//
//================================================//

/* Switch that can be turned on or off */

class Switch : public DynamicObject{
public:
	Switch(void);

	void init(Ogre::SceneManager* mgr, Ogre::SceneNode* node, btCollisionObject* colObj);

	unsigned send(unsigned arg);
	void setLinkedObject(DynamicObject* obj);

	void update(double timeSinceLastFrame);

protected:
	DynamicObject*	m_linkedObject;
	bool			m_linked;
};

//================================================//

inline void Switch::setLinkedObject(DynamicObject* obj)
{ m_linkedObject = obj; m_linked = true; }

//================================================//
//================================================//

#endif

//================================================//
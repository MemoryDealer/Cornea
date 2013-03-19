//================================================//

#ifndef __TRIGGER_HPP__
#define __TRIGGER_HPP__

//================================================//

#include "stdafx.h"
#include "DynamicObject.hpp"
#include "NPC.hpp"
#include "Camera.hpp"
#include "TriggerCode.hpp"

//================================================//

class Trigger : public DynamicObject
{
public:
	Trigger(void);
	~Trigger(void);

	enum{
		TRIGGER_WALK_OVER = 0,
		TRIGGER_LOOK_AT,

		END
	};

	void initTrigger(Ogre::SceneManager* mgr, Ogre::SceneNode* node, Sparks::Camera* camera);
	
	virtual void trigger(void);
	virtual void reset(void);

	void deleteData(void);

	// Setter functions
	void setLinkedObject(DynamicObject* object);
	void setLinkedObject(void* object);

	void update(double timeSinceLastFrame);

protected:
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::SceneNode*		m_pSceneNode;
	Ogre::Timer*			m_pTimeout;

	Sparks::Camera*			m_pCamera;

	bool					m_loop;			// keep triggering after triggered once
	bool					m_triggered;	// has it been triggered?
	Ogre::Real				m_timeout;		// how long to wait before being triggered again
	Ogre::Real				m_range;

	// Actions
	unsigned int			m_actionCode;
	void*					m_pActionPointer;
	Ogre::SceneNode*		m_pActionNode;
};

//================================================//

inline void Trigger::setLinkedObject(DynamicObject* object)
{ m_pActionPointer = (void*)object; }

inline void Trigger::setLinkedObject(void* object)
{ m_pActionPointer = object; }

//================================================//
//================================================//

class TriggerWalkOver : public Trigger
{
public:
	TriggerWalkOver(void);
	~TriggerWalkOver(void);

	void update(double timeSinceLastFrame);

protected:
	
};

//================================================//
//================================================//

class TriggerLookAt : public Trigger
{
public:
	TriggerLookAt(void);
	~TriggerLookAt(void);

	void update(double timeSinceLastFrame);

protected:

};

//================================================//

#endif

//================================================//
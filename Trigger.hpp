//================================================//

#ifndef __TRIGGER_HPP__
#define __TRIGGER_HPP__

//================================================//

#include "stdafx.h"
#include "DynamicObject.hpp"
#include "NPC.hpp"
#include "Camera.hpp"
#include "TriggerCode.hpp"
#include "TextRenderer.hpp"

//================================================//

class Trigger : public DynamicObject
{
public:
	Trigger(void);
	~Trigger(void);

	enum{
		TRIGGER_WALK_OVER = 0,
		TRIGGER_LOOK_AT,

		TRIGGER_CHAIN_NODE,

		END
	};

	void initTrigger(Ogre::SceneManager* mgr, Ogre::SceneNode* node, Sparks::Camera* camera);
	
	virtual void trigger(void);
	virtual void reset(void);

	void deleteData(void);

	// Setter functions
	void setLinkedObject(DynamicObject* object);
	void setLinkedObject(void* object);
	void setTimeout(Ogre::Real timeout);
	void setNextTrigger(Trigger* next);

	void update(double timeSinceLastFrame);

protected:
	Ogre::Timer*			m_pTimeout;

	Sparks::Camera*			m_pCamera;

	Trigger*				m_pNext;		// for multiple trigger chains

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

inline void Trigger::setTimeout(Ogre::Real timeout)
{ m_timeout = timeout; }

inline void Trigger::setNextTrigger(Trigger* next)
{ m_pNext = next; }

//================================================//
//================================================//

class TriggerChainNode : public Trigger
{
public:
	TriggerChainNode(void);

	void update(double timeSinceLastFrame);

protected:

};

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
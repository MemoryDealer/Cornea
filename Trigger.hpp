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

	void initTrigger(Ogre::SceneManager* mgr, Ogre::SceneNode* node, Sparks::Camera* camera);
	void setTriggerData(DynamicObject::DYNAMIC_OBJECT_DATA* data);
	
	virtual void trigger(void);
	virtual void reset(void);

	// Setter functions
	void setLinkedObject(DynamicObject* object);
	void setLinkedObject(void* object);

	void update(double timeSinceLastFrame);

protected:
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::SceneNode*		m_pSceneNode;
	Ogre::Timer*			m_pTimeout;

	Sparks::Camera*			m_pCamera;

	bool					m_loop;		// keep triggering after triggered once
	bool					m_triggered;	// has it been triggered?
	Ogre::Real				m_timeout;	// how long to wait before being triggered again
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
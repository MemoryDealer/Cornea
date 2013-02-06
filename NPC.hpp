//================================================//

#ifndef __NPC_HPP__
#define __NPC_HPP__

//================================================//

#include "stdafx.h"
#include "Base.hpp"
#include "Camera.hpp"

//================================================//

class NPC
{
public:
	NPC(void);
	virtual ~NPC(void){}

	typedef struct{
		btScalar mass;
		struct shape_t{
			btScalar radius;
			btScalar height;
		} shape;
		btVector3 gravity;
		int						activationMethod;
		Ogre::Real				activationRange;
		Ogre::Real				health;
		Ogre::Real				speed;
		bool					animate;
		Ogre::Real				animationSpeed;
		int						forwards;
	} NPC_DATA, *PNPC_DATA;

	enum{
		ACTIVATE_BY_RANGE = 0,
		ACTIVATE_BY_LOOKAT
	};

	void takeDamage(Ogre::Real amount);
	void die(void);
	void setAnimationState(Ogre::String str);

	// Getter functions
	const Ogre::SceneNode* getSceneNode(void) const;
	const int getState(void) const;
	const bool isActive(void) const;
	const bool isAlive(void) const;
	const bool isFriendly(void) const;
	btCollisionObject* getCollisionObject(void) const;

	// Setter functions
	void setState(int state);

	virtual void init(Ogre::SceneManager* mgr, Ogre::SceneNode* node, btCollisionObject* obj, Sparks::Camera* camera) = 0;

	virtual void update(double timeSinceLastFrame);
	virtual void updateAI(double timeSinceLastFrame){}

	// --- //

	enum{
		STATE_DEAD = -1,
		STATE_IDLE = 0,
		STATE_ACTIVE
	};

	enum{
		FORWARDS_NEGATIVE_UNIT_Z = 0,
		FORWARDS_NEGATIVE_UNIT_Y = 1,
		FORWARDS_NEGATIVE_UNIT_X = 2,
		FORWARDS_UNIT_Z = 3,
		FORWARDS_UNIT_Y = 4,
		FORWARDS_UNIT_X = 5
	};

protected:
	// Ogre
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::SceneNode*		m_pSceneNode;
	Ogre::Entity*			m_pEntity;
	Ogre::SceneNode*		m_pPlayerNode;
	Ogre::Vector3			m_forwards;
	Ogre::Timer*			m_pHitTimer;

	Sparks::Camera*			m_pCamera;

	// Animation
	Ogre::AnimationState*	m_pAnimationState;

	// Bullet
	btCollisionObject*		m_collisionObject;
	btVector3				m_gravity;

	// Gameplay
	NPC_DATA				m_settings;
	int						m_state;
	bool					m_friendly;
	bool					m_hit;
};

//================================================//

// Getter functions
inline const Ogre::SceneNode* NPC::getSceneNode(void) const
{ return m_pSceneNode; }

inline const int NPC::getState(void) const
{ return m_state; }

inline const bool NPC::isActive(void) const
{ return (m_state > STATE_IDLE) ? true : false; }

inline const bool NPC::isAlive(void) const
{ return (m_state == STATE_DEAD) ? false : true; }

inline const bool NPC::isFriendly(void) const
{ return m_friendly; }

inline btCollisionObject* NPC::getCollisionObject(void) const
{ return m_collisionObject; }

// Setter functions
inline void NPC::setState(int state)
{ m_state = state; }

//================================================//
//================================================//

class Monster : public NPC
{
public:
	Monster(void);
	~Monster(void){}

	void init(Ogre::SceneManager* mgr, Ogre::SceneNode* node, btCollisionObject* obj, Sparks::Camera* camera);

	void update(double timeSinceLastFrame);
	void updateAI(double timeSinceLastFrame);

protected:

};

//================================================//

#endif

//================================================//
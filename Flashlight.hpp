//================================================//

#ifndef __FLASHLIGHT_HPP__
#define __FLASHLIGHT_HPP__

//================================================//

#include "stdafx.h"

//================================================//

class Flashlight
{
public:
	Flashlight(Ogre::SceneManager* mgr, Ogre::SceneNode* parent);
	~Flashlight(void);

	void switchState(void);

	void update(double timeSinceLastFrame);

private:
	Ogre::SceneManager*		m_pSceneMgr;

	Ogre::Light*			m_pLight;

	Ogre::Real				m_attenuationRange;
	Ogre::Degree			m_outerAngle;
	int						m_state;

	// --- //

	enum FLASHLIGHT_STATE{
		STATE_IDLE = 0,
		STATE_TURNING_ON,
		STATE_TURNING_OFF
	};
};

//================================================//

#endif

//================================================//
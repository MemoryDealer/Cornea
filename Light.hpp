//================================================//

#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

//================================================//

#include "DynamicObject.hpp"

//================================================//

class Light : public DynamicObject
{
public:
	Light(void);

	void initLight(Ogre::SceneManager* mgr, Ogre::SceneNode* node);
	void deleteData(void);

	enum{
		TYPE_SPOT = 0,
		TYPE_POINT
	};

	void update(double timeSinceLastFrame) = 0;

protected:
	Ogre::Light*	m_pLight;
	LightData*		m_data;
	bool			m_dataLoaded;
};

//================================================//

class StaticLight : public Light
{
public:
	StaticLight(void);

	void update(double timeSinceLastFrame);

private:

};

//================================================//

class PulseLight : public Light
{
public:
	PulseLight(void);

	void initLight(Ogre::SceneManager* mgr, Ogre::SceneNode* node);

	void update(double timeSinceLastFrame);

private:
	Ogre::Real	m_speed;
};

//================================================//

class FlickerLight: public Light
{
public:
	FlickerLight(void);

	void initLight(Ogre::SceneManager* mgr, Ogre::SceneNode* node);

	void update(double timeSinceLastFrame);

	enum{
		TYPE_FLICKER_NORMAL = 0,
		TYPE_FLICKER_SLOW,
		TYPE_FLICKER_FAST
	};

private:
	Ogre::Real		m_speed;
	Ogre::Timer*	m_pTimer;
	int				m_type;
};

//================================================//

#endif

//================================================//
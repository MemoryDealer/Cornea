//================================================//

#ifndef __MLP_HPP__
#define __MLP_HPP__

#include "Weapon.hpp"

//================================================//

class MLP : public Weapon
{
public:
	MLP(Ogre::SceneManager* mgr);
	~MLP(void);

	void init(Ogre::SceneNode* node);
	void createAnimations(void);
	void attack(Sparks::Camera* camera);
	void reload(void);

	void update(double timeSinceLastFrame);

private:

};

//================================================//

class TestGun : public Weapon
{
public:
	TestGun(Ogre::SceneManager* mgr);
	~TestGun(void);

	void init(Ogre::SceneNode* node);
	void createAnimations(void);
	void attack(Sparks::Camera* camera);
	void reload(void){}
	

	void update(double timeSinceLastFrame);

private:
};

//================================================//

#endif

//================================================//
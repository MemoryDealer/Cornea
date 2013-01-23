//================================================//

#ifndef __WARPGUN_HPP__
#define __WARPGUN_HPP__

//================================================//

#include "Weapon.hpp"

//================================================//

class WarpGun : public Weapon
{
public:
	WarpGun(Ogre::SceneManager* mgr);
	~WarpGun(void);

	void init(Ogre::SceneNode* node);
	void createAnimations(void);
	void attack(Sparks::Camera* camera);
	void attackAlt(Sparks::Camera* camera);
	void reload(void){}

	void update(double timeSinceLastFrame);

private:

};

//================================================//

#endif

//================================================//
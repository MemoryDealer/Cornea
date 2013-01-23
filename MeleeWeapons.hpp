//================================================//

#ifndef __MELEEWEAPON_HPP__
#define __MELEEWEAPON_HPP__

//================================================//

#define MELEE_ANIMATION_SWING	"Melee_Animation_Swing"
#define MELEE_ANIMATION_STAB	"Melee_Animation_Stab"

//================================================//

#include "Weapon.hpp"

//================================================//

class Blade : public Weapon
{
public:
	Blade(Ogre::SceneManager* mgr);
	~Blade(void);

	void init(Ogre::SceneNode* node);
	void attack(Sparks::Camera* camera);

	void update(double timeSinceLastFrame);

protected:
	
};

//================================================//

class Sword : public Blade
{
public:
	Sword(Ogre::SceneManager* mgr);
	~Sword(void);

	void init(Ogre::SceneNode* node);
	void createAnimations(void);
	void attack(Sparks::Camera* camera);

	void update(double timeSinceLastFrame);

protected:
	Ogre::AnimationState*	m_pAnimState_Swing;
	Ogre::AnimationState*	m_pAnimState_Stab;
};

//================================================//

#endif

//================================================//
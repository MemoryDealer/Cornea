//================================================//

#include "MeleeWeapons.hpp"

//================================================//

Blade::Blade(Ogre::SceneManager* mgr) : Weapon(mgr)
{
	m_pSceneMgr = mgr;

	m_offset = Ogre::Vector3(2.50, -2.10, -7.20);

	m_type = TYPE_MELEE;
	m_range = 50.0;

	// Reticule
	m_reticuleDefault = "Examples/Crosshair";
	m_reticuleEnemy = "Examples/CrosshairRed";
	m_reticuleFriendly = "Examples/Flare";
}

//================================================//

Blade::~Blade(void)
{
	
}

//================================================//

void Blade::init(Ogre::SceneNode* node)
{
	Weapon::init(node);
}

//================================================//

void Blade::attack(Sparks::Camera* camera)
{
	// choose animation...

	if(!m_pAnimState->getEnabled()){
		m_pAnimState->setEnabled(true);

		Weapon::meleeAttack(camera);
	}
}

//================================================//

void Blade::update(double timeSinceLastFrame)
{
	Weapon::update(timeSinceLastFrame);
}

//================================================//
//================================================//

Sword::Sword(Ogre::SceneManager* mgr) : Blade(mgr)
{
	m_filename = "sword.mesh";

	m_damage = 20.0;

	m_offset = Ogre::Vector3(2.50, -3.90, -7.80);
}

//================================================//

Sword::~Sword(void)
{
	m_pSceneMgr->destroyAnimation(MELEE_ANIMATION_SWING);
	m_pSceneMgr->destroyAnimation(MELEE_ANIMATION_STAB);
}

//================================================//

void Sword::init(Ogre::SceneNode* node)
{
	Blade::init(node);

	m_pEntity->setCastShadows(false);

	m_pSceneNode->yaw(Ogre::Radian(Ogre::Math::DegreesToRadians(-70.0)));
	m_pSceneNode->roll(Ogre::Radian(Ogre::Math::DegreesToRadians(-15.0)));
	m_pSceneNode->scale(0.5, 0.5, 0.5);

	// Set up animations
	createAnimations();
}

//================================================//

void Sword::createAnimations(void)
{
	m_pSceneNode->setInitialState();

	// Swing
	Ogre::Animation* anim = m_pSceneMgr->createAnimation(MELEE_ANIMATION_SWING, 500.0);
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	
	Ogre::TransformKeyFrame* key;

	key = track->createNodeKeyFrame(0.0);
	key->setRotation(Ogre::Quaternion::IDENTITY);
	
	key = track->createNodeKeyFrame(250.0);
	key->setRotation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5), 0.0, 0.0, Ogre::Math::Sqrt(0.5)));

	key = track->createNodeKeyFrame(500.0);
	key->setRotation(Ogre::Quaternion::IDENTITY);

	m_pAnimState_Swing = m_pSceneMgr->createAnimationState(MELEE_ANIMATION_SWING);
	m_pAnimState_Swing->setLoop(false);


	// Stab
	anim = m_pSceneMgr->createAnimation(MELEE_ANIMATION_STAB, 500.0);
	track = anim->createNodeTrack(0, m_pSceneNode);
	anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);

	key = track->createNodeKeyFrame(0.0);
	key->setRotation(Ogre::Quaternion::IDENTITY);

	key = track->createNodeKeyFrame(150.0);
	key->setRotation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5), 0, 0, Ogre::Math::Sqrt(0.5)));
	
	key = track->createNodeKeyFrame(250.0);
	key->setTranslate(Ogre::Vector3(0, -5.0, -5.0));

	key = track->createNodeKeyFrame(500.0);
	key->setRotation(Ogre::Quaternion::IDENTITY);
	key->setTranslate(Ogre::Vector3::ZERO);

	m_pAnimState_Stab = m_pSceneMgr->createAnimationState(MELEE_ANIMATION_STAB);
	m_pAnimState_Stab->setLoop(false);


	// Set default animation
	m_pAnimState = m_pAnimState_Swing;
}

//================================================//

void Sword::attack(Sparks::Camera* camera)
{
	Blade::attack(camera);
}

//================================================//

void Sword::update(double timeSinceLastFrame)
{
	Blade::update(timeSinceLastFrame);
}

//================================================//
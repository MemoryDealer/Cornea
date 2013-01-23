//================================================//

#include "MLP.hpp"

//================================================//

MLP::MLP(Ogre::SceneManager* mgr) : Weapon(mgr)
{
	m_pSceneMgr = mgr;

	// Set filename
	m_filename = "br.mesh";

	m_damage = 10.0;

	// Set the offset here and the Weapon init() function will translate it
	m_offset = Ogre::Vector3(2.50, -2.10, -7.20);

	// Set behaviour elements
	m_type = TYPE_PROJECTILE;
	m_projectileOffset = Ogre::Vector3(0, 0, -1.0);

	// Set projectile data
	m_projectileData.colour = Ogre::ColourValue(1.0, 1.0, 1.0);
	m_projectileData.speed = 7.0;
	m_projectileData.damage = m_damage;
	m_projectileData.range = 10000.0;

	// Sound
	//m_pSoundAttack->init(Sound::TYPE_FIRE1);
}

//================================================//

MLP::~MLP(void)
{
	m_pSceneMgr->destroyAnimation(ATTACK_ANIMATION);
}

//================================================//

void MLP::init(Ogre::SceneNode* node)
{
	Weapon::init(node);

	m_pSceneNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian((Ogre::Math::PI / 2.0)), Ogre::Node::TS_LOCAL);
	m_pSceneNode->scale(1.6, 1.6, 1.6);

	m_pEntity->setMaterialName("mainbody");
	m_pEntity->setCastShadows(false);

	m_muzzleFlashPos = Ogre::Vector3(6.0, 0.0, 0.0);
	m_muzzleFlashType = MuzzleFlash::TYPE_FLASH2;

	createAnimations();
}

//================================================//

void MLP::createAnimations(void)
{
	m_pSceneNode->setInitialState();

	Ogre::Animation* anim = m_pSceneMgr->createAnimation(ATTACK_ANIMATION, 250.0);
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	
	anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);

	Ogre::TransformKeyFrame* key;
	key = track->createNodeKeyFrame(0.0);

	key = track->createNodeKeyFrame(125.0);
	key->setTranslate(Ogre::Vector3(0.0, 0.0, 4.0));

	key = track->createNodeKeyFrame(250.0);
	key->setTranslate(Ogre::Vector3::ZERO);

	m_pAnimState = m_pSceneMgr->createAnimationState(ATTACK_ANIMATION);
	m_pAnimState->setLoop(false);
}

//================================================//

void MLP::attack(Sparks::Camera* camera)
{
	if(!m_pAnimState->getEnabled()){
		m_pAnimState->setEnabled(true);

		Weapon::projectileAttack(camera);
	}
}

//================================================//

void MLP::reload(void)
{

}

//================================================//

void MLP::update(double timeSinceLastFrame)
{
	Weapon::update(timeSinceLastFrame);
}

//================================================//
//================================================//

TestGun::TestGun(Ogre::SceneManager* mgr) : Weapon(mgr)
{
	m_pSceneMgr = mgr;

	// set filename
	m_filename = "br.mesh";
	m_damage = 10.0;

	// set the offset here and the Weapon constructor will translate it
	m_offset = Ogre::Vector3(2.50, -2.10, -7.20);

	// set behaviour elements
	m_type = TYPE_RAY;
	m_projectileOffset = Ogre::Vector3(0, 0, -1.0);
	m_range = 6000.0;

	m_debugRays = false;

	// Sound
	m_pSoundAttack->init(Sound::TYPE_FIRE1);
}

//================================================//

TestGun::~TestGun(void)
{
	m_pSceneMgr->destroyAnimation(ATTACK_ANIMATION);
}

//================================================//

void TestGun::init(Ogre::SceneNode* node)
{
	Weapon::init(node);

	m_pSceneNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian((Ogre::Math::PI / 2.0)), Ogre::Node::TS_LOCAL);
	m_pSceneNode->scale(1.6, 1.6, 1.6);

	m_pEntity->setMaterialName("mainbody");
	m_pEntity->setCastShadows(false);

	m_muzzleFlashPos = Ogre::Vector3(6.0, 0.0, 0.0);
	m_muzzleFlashType = MuzzleFlash::TYPE_FLASH2;

	createAnimations();
}

//================================================//

void TestGun::createAnimations(void)
{
	m_pSceneNode->setInitialState();

	Ogre::Animation* anim = m_pSceneMgr->createAnimation(ATTACK_ANIMATION, 250);
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	
	anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);

	Ogre::TransformKeyFrame* key;
	key = track->createNodeKeyFrame(0.0);

	key = track->createNodeKeyFrame(125.0);
	key->setTranslate(Ogre::Vector3(0.0, 0.0, 3.0));

	key = track->createNodeKeyFrame(250.0);
	key->setTranslate(Ogre::Vector3::ZERO);

	m_pAnimState = m_pSceneMgr->createAnimationState(ATTACK_ANIMATION);
	m_pAnimState->setLoop(false);
}

//================================================//

void TestGun::attack(Sparks::Camera* camera)
{
	if(!m_pAnimState->getEnabled()){
		m_pAnimState->setEnabled(true);

		Weapon::rayAttack(camera);
	}
}

//================================================//

void TestGun::update(double timeSinceLastFrame)
{
	Weapon::update(timeSinceLastFrame);
}
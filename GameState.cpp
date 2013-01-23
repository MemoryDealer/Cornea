//================================================//

#include "GameState.hpp"

struct shadowListener: public Ogre::SceneManager::Listener
{
    // this is a callback we'll be using to set up our shadow camera
    void shadowTextureCasterPreViewProj(Ogre::Light *light, Ogre::Camera *cam, size_t)
    {
        // basically, here we do some forceful camera near/far clip attenuation
        // yeah.  simplistic, but it works nicely.  this is the function I was talking
        // about you ignoring above in the Mgr declaration.
        float range = light->getAttenuationRange();
        cam->setNearClipDistance(0.01);
        cam->setFarClipDistance(range);
        // we just use a small near clip so that the light doesn't "miss" anything
        // that can shadow stuff.  and the far clip is equal to the lights' range.
        // (thus, if the light only covers 15 units of objects, it can only
        // shadow 15 units - the rest of it should be attenuated away, and not rendered)
    }

    // these are pure virtual but we don't need them...  so just make them empty
    // otherwise we get "cannot declare of type Mgr due to missing abstract
    // functions" and so on
    void shadowTexturesUpdated(size_t) {}
    void shadowTextureReceiverPreViewProj(Ogre::Light*, Ogre::Frustum*) {}
    void preFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
    void postFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
} shadowCameraUpdater;

//================================================//

GameState::GameState(void)
{
	m_bQuit = false;
}

//================================================//

void GameState::enter(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Entering GameState...");
	m_bQuit = false;

	// Create Ogre scene manager
	m_pSceneMgr = Base::getSingletonPtr()->m_pRoot->createSceneManager("OctreeSceneManager", "GameSceneMgr");

	// Physics
	Base::getSingletonPtr()->createPhysicsWorld();

	// Initialise the player object
	m_player = new Player(m_pSceneMgr);

	// Create environment
	m_pEventManager = new EventManager(m_pSceneMgr, m_player->getCamera());

	// Pre-load mesh data
	preloadMeshData();

	// Add weapon to the player
	m_player->setWeapon(Player::WEAPON_WARPGUN, m_pEventManager);

	// Setup the player's boots
	new Boots();

	// Create the scene
	createScene();

	// Create the GUI
	createGUI();
}

//================================================//

void GameState::exit(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Leaving GameState...");

	destroyGUI();

	// Destroy overlay
	m_overlay->remove2D(m_overlayReticule);
	Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(m_overlayReticule);
	Ogre::OverlayManager::getSingletonPtr()->destroy(m_overlay);

	// Free environment
	delete m_pEventManager;

	// Free the player's boots
	delete Boots::getSingletonPtr();
	delete m_player;
	
	// Clear physics
	Base::getSingletonPtr()->destroyPhysicsWorld();

	// Cleanup scene manager
	/*m_pSceneMgr->getSceneNode("ogrenode")->detachAllObjects();
	m_pSceneMgr->getEntity("ogre")->getMesh()->unload();
	m_pSceneMgr->destroyEntity("ogre");*/
	
	m_pSceneMgr->clearScene();
	m_pSceneMgr->destroyAllCameras();

	//Ogre::MeshManager::getSingleton().removeAll();
	//Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("Popular");
	
	//Ogre::MaterialManager::getSingleton().unloadAll();
	//Ogre::TextureManager::getSingleton().unloadAll();
	//Ogre::GpuProgramManager::getSingleton().unloadAll();

	Base::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

	
}

//================================================//

bool GameState::pause(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Pausing GameState...");

	destroyGUI();

	return true;
}

//================================================//

void GameState::resume(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Resuming GameState...");
	m_bQuit = false;

	createGUI();

	// set the camera again
	Base::getSingletonPtr()->m_pViewport->setCamera(m_player->getCamera()->getOgreCamera());
}

//================================================//

void GameState::createScene(void)
{
	Settings& settings = Settings::getSingleton();

	// Environment
	m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	// Soft shadows
	if(settings.graphics.shadows.enabled){
		m_pSceneMgr->setShadowTextureSelfShadow(true);
		m_pSceneMgr->setShadowTextureCasterMaterial("Sparks/shadow_caster");
		m_pSceneMgr->setShadowTextureCount(settings.graphics.shadows.textureCount);
		m_pSceneMgr->setShadowTextureSize(settings.graphics.shadows.textureSize);
		m_pSceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGB);
		m_pSceneMgr->setShadowCasterRenderBackFaces(false);
		m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		m_pSceneMgr->setShadowTextureFSAA(settings.graphics.shadows.fsaa);
	}
	//m_pSceneMgr->addListener(&shadowCameraUpdater);
	
	// light
	//Ogre::Light* light = m_pSceneMgr->createLight("MainLight");
	//Ogre::Vector3 dir(0.2, -0.3, -0.75);
	//dir.normalise();

	///*light->setType(Ogre::Light::LT_SPOTLIGHT);
	//light->setDirection(dir);
	//light->setDiffuseColour(1.0, 1.0, 1.0);
	//light->setSpecularColour(0.8, 0.8, 0.8);
	//light->setCastShadows(true);
	//light->setPosition( Ogre::Vector3::ZERO + (-dir * 500000.0));
	//light->setSpotlightInnerAngle(Ogre::Degree(2000));
	//light->setSpotlightOuterAngle(Ogre::Degree(500000));
	//light->setAttenuation(500000, 0.5, 0, 0);*/

	//light->setType(Ogre::Light::LT_DIRECTIONAL);
	//light->setDirection(dir);
	//light->setPowerScale(0.2);
	//light->setDiffuseColour(1.0, 1.0, 1.0);
	//light->setSpecularColour(0.8, 0.8, 0.8);
	//light->setCastShadows(true);
	//light->setShadowNearClipDistance(1);
	//light->setShadowFarClipDistance(30000);

	m_pFlashlight = m_pSceneMgr->createLight("Flashlight");
	m_pFlashlight->setType(Ogre::Light::LT_SPOTLIGHT);
	m_pFlashlight->setDiffuseColour(1.0, 1.0, 1.0);
	m_pFlashlight->setSpecularColour(0.8, 0.8, 0.8);
	m_pFlashlight->setCastShadows(true);
	m_pFlashlight->setSpotlightInnerAngle(Ogre::Degree(5));
	m_pFlashlight->setSpotlightOuterAngle(Ogre::Degree(20));
	m_pFlashlight->setSpotlightFalloff(5.0);
	m_pFlashlight->setAttenuation(1000, 0.5, 0, 0);
	m_pFlashlight->setDirection(Ogre::Vector3(0.0, 0.0, -1.0));
	//m_pSceneMgr->getRootSceneNode()->createChildSceneNode("Flashlight")->attachObject(m_pFlashlight);
	m_player->getCamera()->getRollNode()->createChildSceneNode("Flashlight")->attachObject(m_pFlashlight);
	m_pSceneMgr->getSceneNode("Flashlight")->translate(5.0, -2.5, 0.0, Ogre::Node::TS_LOCAL);
	//m_pSceneMgr->getRootSceneNode()->createChildSceneNode("FL")->attachObject(m_pFlashlight);
	//m_pSceneMgr->getSceneNode("FL")->setPosition(40.0, 60.0, 0.0);

	// plane
	Ogre::Entity* e;
	Ogre::Plane p;

	p.normal = Ogre::Vector3(0, 1, 0); 
	p.d = 0;

	Ogre::MeshManager::getSingleton().createPlane("FloorPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		p, 200000, 200000, 50, 50, true, 1, 200, 200, Ogre::Vector3::UNIT_Z);
	e = m_pSceneMgr->createEntity("Floor", "FloorPlane");
	e->setMaterialName("metal");
	Ogre::SceneNode* node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("MofoPlane");
	node->attachObject(e);
	//e->getMesh()->buildEdgeList();
	e->setCastShadows(false);

	// Scene
	DotSceneLoader* loader = new DotSceneLoader();
	Ogre::SceneNode* scene = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("Scene");
	loader->parseDotScene("level-test6.scene", "General", m_pSceneMgr, scene);
	delete loader;

	// Scale it and all child nodes
	scene->setInheritScale(true);
	scene->scale(20.0, 20.0, 20.0);

	/*Ogre::Entity* entity;
	entity = m_pSceneMgr->createEntity("ogre", "ogrehead.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::SceneNode* node2 = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("ogrenode");
	node2->attachObject(entity);
	entity->setCastShadows(true);

	node2->setPosition(50, 100, 0);*/

	// Add entities to the collision world
	BtOgre::registerAllEntitiesAsColliders(m_pSceneMgr, Base::getSingletonPtr()->m_btWorld);

	// Add dynamic objects
	m_pEventManager->getDynamicObjectManager()->registerAllObjectsInScene();

	// Debug drawer for Bullet
	m_debugDrawer = new BtOgre::DebugDrawer(m_pSceneMgr->getRootSceneNode(), Base::getSingletonPtr()->m_btWorld);
	Base::getSingletonPtr()->m_btWorld->setDebugDrawer(m_debugDrawer);
	m_debugDrawer->setDebugMode(0);

	// Crosshair overlay
	Ogre::OverlayManager& manager = Ogre::OverlayManager::getSingleton();
	m_overlay = manager.create("Overlay");
	m_overlayReticule = static_cast<Ogre::OverlayContainer*>(manager.createOverlayElement("Panel", "ReticulePanel"));
	m_overlayReticule->setPosition(0.48, 0.48);
	m_overlayReticule->setDimensions(0.04, 0.04);
	m_overlayReticule->setMaterialName(m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_DEFAULT));
	m_overlay->add2D(m_overlayReticule);
	m_overlay->show();

	// Reset ambient light
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.005, 0.005, 0.005));
}

//================================================//

void GameState::createGUI(void)
{
	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);

	int buttonWidth = 800, buttonHeight = 26;
	int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();
	m_button = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button", 
		screenWidth / 2.0, 0, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main");

	MyGUI::PointerManager::getInstancePtr()->setVisible(false);

	// Crosshair
	/*const MyGUI::IntSize size(80, 26);
	m_crosshairs = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::ImageBox>("ImageBox", (screenWidth / 2) - (size.width / 2), 
		(screenHeight / 2) - (size.height / 2), size.width, size.height, MyGUI::Align::Default, "Main");
	m_crosshairs->setImageTexture("D:/OgreSDK/media/materials/textures/Crosshairs.png");
	m_crosshairs->setVisible(true);*/
}

//================================================//

void GameState::destroyGUI(void)
{
	m_button->detachFromLayer();

	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(nullptr);
}

//================================================//

void GameState::preloadMeshData(void)
{
	// Load certain mesh data before scene is created to avoid lag spikes during gameplay
	Ogre::Entity* temp;
	
	temp = m_pSceneMgr->createEntity("temp", "br.mesh");
	m_pSceneMgr->destroyEntity(temp);

	temp = m_pSceneMgr->createEntity("temp", "sword.mesh");
	m_pSceneMgr->destroyEntity(temp);
}

//================================================//

bool GameState::keyPressed(const OIS::KeyEvent& arg)
{
	switch(arg.key){
	case OIS::KC_W:
		if(!m_player->getCamera()->m_moveForwardsPressed){
			m_player->getCamera()->m_moveForwardsPressed = true;
		}
		break;

	case OIS::KC_S:
		if(!m_player->getCamera()->m_moveBackwardsPressed){
			m_player->getCamera()->m_moveBackwardsPressed = true;
		}
		break;

	case OIS::KC_A:
		if(!m_player->getCamera()->m_moveLeftPressed){
			m_player->getCamera()->m_moveLeftPressed = true;
		}
		break;

	case OIS::KC_D:
		if(!m_player->getCamera()->m_moveRightPressed){
			m_player->getCamera()->m_moveRightPressed = true;
		}
		break;

	case OIS::KC_SPACE:
		if(!m_player->getCamera()->m_spacePressed){
			m_player->getCamera()->m_spacePressed = true;
		}
		break;

	// Action key
	case OIS::KC_E:
		m_player->action(m_pEventManager);
		break;

	// Keys for switching boots
	case OIS::KC_1:
		Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_NORMAL);
		break;

	case OIS::KC_2:
		Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_VELOCITY);
		break;

	case OIS::KC_3:
		Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_HIGH);
		break;

	case OIS::KC_TAB:

		break;

	case OIS::KC_CAPITAL:
		//m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
		break;

	// NOTE: Below are debugging keys only, delete later

	case OIS::KC_G:
		{
			if(m_button->isVisible())
				m_button->setVisible(false);
			else
				m_button->setVisible(true);
		}
		break;

	case OIS::KC_B:
		{
			static bool bb = false;

			bb = !bb;

			Ogre::SceneNode::ChildNodeIterator itr = m_pSceneMgr->getSceneNode("Scene")->getChildIterator();
			for(;itr.hasMoreElements();){
				Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itr.getNext());
				child->showBoundingBox(bb);
			}
		}
		break;

	case OIS::KC_P:
		{
			m_debugDrawer->setDebugMode(!m_debugDrawer->getDebugMode());
		}
		break;

	case OIS::KC_T:
		{
			static bool wireframe = false;

			if(wireframe)
				m_player->getCamera()->getOgreCamera()->setPolygonMode(Ogre::PM_SOLID);
			else
				m_player->getCamera()->getOgreCamera()->setPolygonMode(Ogre::PM_WIREFRAME);

			wireframe = !wireframe;
		}
		break;

	case OIS::KC_F:
		m_pFlashlight->setVisible(!m_pFlashlight->getVisible());
		break;

	case OIS::KC_Q:
		m_player->setWeapon(Player::WEAPON_DEFAULT, m_pEventManager);
		break;

	case OIS::KC_Z:
		m_player->setWeapon(Player::WEAPON_WARPGUN, m_pEventManager);
		break;

	case OIS::KC_L:
		{
			static bool lighting = false;
			if(lighting){
				m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.005, 0.005, 0.005));
				lighting = false;
			}
			else{
				m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8));
				lighting = true;
			}
		}
		break;

	case OIS::KC_V:
		Base::getSingletonPtr()->m_pRenderWindow->setVSyncEnabled(!Base::getSingletonPtr()->m_pRenderWindow->isVSyncEnabled());
		break;

	case OIS::KC_F1:
		m_player->getCamera()->setMode(Sparks::Camera::MODE_FIRST_PERSON);
		break;

	case OIS::KC_F2:
		m_player->getCamera()->setMode(Sparks::Camera::MODE_SPECTATOR);
		break;

	case OIS::KC_UP:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.z -= 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_DOWN:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.z += 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_RIGHT:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.x += 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_LEFT:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.x -= 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_ADD:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.y += 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_SUBTRACT:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.y -= 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_ESCAPE:
		m_bQuit = true;
		break;

	default:
		break;
	}

	Base::getSingletonPtr()->keyPressed(arg);

	return true;
}

//================================================//

bool GameState::keyReleased(const OIS::KeyEvent& arg)
{
	switch(arg.key){
		case OIS::KC_W:
		if(m_player->getCamera()->m_moveForwardsPressed){
			m_player->getCamera()->m_moveForwardsPressed = false;
		}
		break;

	case OIS::KC_S:
		if(m_player->getCamera()->m_moveBackwardsPressed){
			m_player->getCamera()->m_moveBackwardsPressed = false;
		}
		break;

	case OIS::KC_A:
		if(m_player->getCamera()->m_moveLeftPressed){
			m_player->getCamera()->m_moveLeftPressed = false;
		}
		break;

	case OIS::KC_D:
		if(m_player->getCamera()->m_moveRightPressed){
			m_player->getCamera()->m_moveRightPressed = false;
		}
		break;

	case OIS::KC_SPACE:
		if(m_player->getCamera()->m_spacePressed){
			m_player->getCamera()->m_spacePressed = false;
		}
		break;

	// NOTE: Below are only debugging keys, delete later
	case OIS::KC_3:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			Boots::getSingletonPtr()->setXMultiplier(Boots::getSingletonPtr()->getXMultiplier() - 0.1);
			Boots::getSingletonPtr()->setZMultiplier(Boots::getSingletonPtr()->getZMultiplier() - 0.1);
		}
		break;

	case OIS::KC_4:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			Boots::getSingletonPtr()->setXMultiplier(Boots::getSingletonPtr()->getXMultiplier() + 0.1);
			Boots::getSingletonPtr()->setZMultiplier(Boots::getSingletonPtr()->getZMultiplier() + 0.1);
		}
		break;

	case OIS::KC_5:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			Boots::getSingletonPtr()->setYMultiplier(Boots::getSingletonPtr()->getYMultiplier() - 10.0);
		}
		break;

	case OIS::KC_6:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			Boots::getSingletonPtr()->setYMultiplier(Boots::getSingletonPtr()->getYMultiplier() + 10.0);
		}
		break;

	case OIS::KC_7:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			Boots::getSingletonPtr()->setXAirMultiplier(Boots::getSingletonPtr()->getXAirMultiplier() - 0.005);
			Boots::getSingletonPtr()->setZAirMultiplier(Boots::getSingletonPtr()->getZAirMultiplier() - 0.005);
		}
		break;

	case OIS::KC_8:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			Boots::getSingletonPtr()->setXAirMultiplier(Boots::getSingletonPtr()->getXAirMultiplier() + 0.005);
			Boots::getSingletonPtr()->setZAirMultiplier(Boots::getSingletonPtr()->getZAirMultiplier() + 0.005);
		}
		break;

	case OIS::KC_9:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			btVector3 gravity = Boots::getSingletonPtr()->getGravity();

			gravity.setY(gravity.getY() - 0.1);
			Boots::getSingletonPtr()->setGravity(gravity);
		}
		break;

	case OIS::KC_0:
		if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
			btVector3 gravity = Boots::getSingletonPtr()->getGravity();

			gravity.setY(gravity.getY() + 0.1);
			Boots::getSingletonPtr()->setGravity(gravity);
		}
		break;

	default:
		break;
	}

	Base::getSingletonPtr()->keyReleased(arg);

	return true;
}

//================================================//

bool GameState::mouseMoved(const OIS::MouseEvent& arg)
{
	m_player->getCamera()->pitch(Ogre::Degree(arg.state.Y.rel));
	m_player->getCamera()->yaw(Ogre::Degree(arg.state.X.rel));
	return true;
}

//================================================//

bool GameState::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left){
		m_player->getWeapon()->attack(m_player->getCamera());
	}
	else if(id == OIS::MB_Right){
		m_player->getWeapon()->attackAlt(m_player->getCamera());
	}

	return true;
}

//================================================//

bool GameState::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{

	return true;
}

//================================================//

void GameState::update(double timeSinceLastFrame)
{
	if(m_bQuit){
		this->popAppState();
		return;
	}

	// Update player
	//printf("Player...\n");
	m_player->update(timeSinceLastFrame);

	// Update events
	//printf("Events...\n");
	m_pEventManager->update(timeSinceLastFrame);

	// Update UI
	//printf("UI...\n");
	updateUI();

	// Update physics
	//printf("Bullet...\n");
	updateBullet(timeSinceLastFrame);

	// Display boot info (DEBUG)
	char buf[1024];
	sprintf(buf, "XZ: %.2f // Y:%.2f // A: %.2f // G: %.2f // Offset: <%.2f, %.2f, %.2f>",
		Boots::getSingletonPtr()->getXMultiplier(), Boots::getSingletonPtr()->getYMultiplier(),
		Boots::getSingletonPtr()->getXAirMultiplier(), Boots::getSingletonPtr()->getGravity().getY(),
		m_player->getWeapon()->getOffset().x, m_player->getWeapon()->getOffset().y, m_player->getWeapon()->getOffset().z);
	MyGUI::UString str((const char*)buf);
	m_button->setCaption(str);
}

//================================================//

void GameState::updateUI(void)
{
	Rayhit* rayhit = m_player->getCamera()->getNegativeZRayhit();
	bool npcFound = false;
	Ogre::String name = rayhit->node->getName();
	Ogre::StringUtil strUtil;

	if(rayhit->hasHit){
		// Check distance
		if(rayhit->distance <= m_player->getWeapon()->getReticuleRange()){
			// Check if hit point is an NPC
			if(strUtil.startsWith(name, "NPC_", false)){
				std::vector<NPC*> NPCs = m_pEventManager->getNPCManager()->getNPCs();

				if(NPCs.size() > 0){
					// Find the NPC's scene node
					for(std::vector<NPC*>::iterator itr = NPCs.begin();
						itr != NPCs.end();
						++itr){
						Ogre::String npcName = static_cast<Ogre::String>((*itr)->getSceneNode()->getName());

						// Check the name for a match
						if(strUtil.match(name, npcName, true)){
							if((*itr)->isFriendly()){
								m_overlayReticule->setMaterialName(m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_FRIENDLY));
							}
							else{
								m_overlayReticule->setMaterialName(m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_ENEMY));
							}

							npcFound = true;
							break;
						}
					}
				}
			}
		}
	}

	if(!npcFound){
		m_overlayReticule->setMaterialName(m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_DEFAULT));
	}
}

//================================================//

void GameState::updateBullet(double timeSinceLastFrame)
{
	Base::getSingletonPtr()->m_btWorld->stepSimulation(timeSinceLastFrame * 0.015f, 60);
	//Base::getSingletonPtr()->m_btWorld->stepSimulation(1.0f / 60.0f, 60);

	m_debugDrawer->step();

	// Update all rigid bodies
	btRigidBody* body;

	for(std::vector<btRigidBody*>::iterator itr = Base::getSingletonPtr()->m_btObjects.begin(); itr!=Base::getSingletonPtr()->m_btObjects.end(); ++itr){
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>((*itr)->getUserPointer());
		body = *itr;

		btVector3 pos = body->getCenterOfMassPosition();
		node->setPosition(Ogre::Vector3((float)pos[0], (float)pos[1], (float)pos[2]));

		btQuaternion orientation = body->getOrientation();
		node->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}

	// Update the camera's rigid body
	if(m_player->getCamera()->getMode() == Sparks::Camera::MODE_FIRST_PERSON){
		body = m_player->getCamera()->getRigidBody();

		btVector3 pos = body->getCenterOfMassPosition();
		m_player->getCamera()->getSceneNode()->setPosition(Ogre::Vector3((float)pos[0], (float)pos[1], (float)pos[2]));

		btQuaternion orientation = body->getOrientation();
		m_player->getCamera()->getSceneNode()->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}
}

//================================================//
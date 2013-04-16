//================================================//

#include "GameState.hpp"

//================================================//

GameState::GameState(void)
{
	m_bQuit = false;
	m_GUIEventId = 0;
	m_skyXInitialised = false;
	m_hydraxInitialised = false;
}

//================================================//

void GameState::enter(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Entering GameState...");
	m_bQuit = false;

	// Init profile
	m_profile = new Profile();
	if(SharedData::getSingletonPtr()->action == 0){
		m_profile->create(SharedData::getSingletonPtr()->buffer);
		printf("%s created!\n", SharedData::getSingletonPtr()->buffer.c_str());
	}
	else{
		m_profile->load(SharedData::getSingletonPtr()->buffer);
		printf("%s loaded!\n", SharedData::getSingletonPtr()->buffer.c_str());
	}

	TextRenderer::getSingletonPtr()->clearAll();

	this->createSceneManager();

	m_pCompositor = new Sparks::Compositor(m_pSceneMgr, Base::getSingletonPtr()->m_pViewport);

	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);

	// Allow update loop to load the game
	m_state = STATE_LOADING_FIRST_ENTRY;
	m_loadingStep = STEP_FIRST;

	new Boots();
}

//================================================//

void GameState::exit(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Leaving GameState...");

	// Save the profile
	m_profile->save();
	delete m_profile;

	this->destroyScene();

	delete m_pCompositor;

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

	this->destroyGUI();

	return true;
}

//================================================//

void GameState::resume(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Resuming GameState...");
	m_bQuit = false;

	this->createGUI();

	// set the camera again
	Base::getSingletonPtr()->m_pViewport->setCamera(m_player->getCamera()->getOgreCamera());
}

//================================================//

void GameState::createSceneManager(void)
{
	// Determine the proper scene manager by the stage
	switch(m_profile->getStage()){
	case Profile::STAGE::DEV:
	default:
		m_pSceneMgr = Base::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_INTERIOR, SCENE_MGR_NAME);
	}
}

//================================================//

void GameState::createScene(void)
{
	// Initialise all graphics settings
	Settings& settings = Settings::getSingleton();

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

		const unsigned numShadowRTTs = m_pSceneMgr->getShadowTextureCount();
		for(unsigned i=0; i<numShadowRTTs; ++i){
			Ogre::TexturePtr tex = m_pSceneMgr->getShadowTexture(i);
			Ogre::Viewport* vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
			vp->setBackgroundColour(Ogre::ColourValue::White);
			vp->setClearEveryFrame(true);
		}

		m_pSceneMgr->addListener(&shadowCameraUpdater);
	}

	g_pListenerCamera = m_player->getCamera()->getOgreCamera();

	// Load the physical contents of the scene
	this->loadStage();
}

//================================================//

void GameState::destroyScene(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Destroying Scene...");

	destroyGUI();

	// Free environment
	delete m_pEventManager;

	// Free the player's boots
	delete Boots::getSingletonPtr();
	delete m_player;
	
	// Free physics world
	m_physics->free();

	// Clear SkyX
	if(m_skyXInitialised){
		Base::getSingletonPtr()->m_pRoot->removeFrameListener(m_skyX);
		Base::getSingletonPtr()->m_pRenderWindow->removeListener(m_skyX);

		if(m_skyXController->getDeleteBySkyX()){
			//delete m_skyXController; ?
			m_skyX->remove();
		}
	}

	if(m_hydraxInitialised){
		m_hydrax->remove();
	}
	
	// ...
	//m_pSceneMgr->destroyLight(m_pSunlight);

	TextRenderer::getSingletonPtr()->clearAll();

	m_pSceneMgr->clearScene();
	m_pSceneMgr->destroyAllCameras();
}

//================================================//

void GameState::createGUI(void)
{
	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);

	// HUD
	m_GUIHudLayer = new GUIGameState::GUIHudLayer(&m_GUIEventId);
	m_GUIHudLayer->create();


	/**/

	MyGUI::PointerManager::getInstancePtr()->setVisible(false);
}

//================================================//

void GameState::createLoadingGUI(void)
{
	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);

	// Loading Layer
	m_GUILoadingLayer = new GUIGameState::GUILoadingLayer(&m_GUIEventId);
	m_GUILoadingLayer->create();


	Base::getSingletonPtr()->m_pViewport->setBackgroundColour(Ogre::ColourValue::Black);
}

//================================================//

void GameState::destroyGUI(void)
{
	m_GUIHudLayer->destroy();
	m_GUILoadingLayer->destroy();

	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(nullptr);
}

//================================================//

void GameState::preloadMeshData(void)
{
	// Load certain mesh data before scene is created to avoid lag spikes during gameplay
	// (once a mesh is loaded it's stored in memory and is retrieved when an entity uses it)
	Ogre::Entity* temp;

	// Load specific meshes
	switch(m_profile->getStage()){
	
	}
	
	// Load general meshes
	temp = m_pSceneMgr->createEntity("temp", "br.mesh");
	m_pSceneMgr->destroyEntity(temp);

	temp = m_pSceneMgr->createEntity("temp", "sword.mesh");
	m_pSceneMgr->destroyEntity(temp);
}

//================================================//

bool GameState::keyPressed(const OIS::KeyEvent& arg)
{
	Settings& settings = Settings::getSingleton();

	if(m_state != STATE_ACTIVE){
		return true;
	}

	// Test for GUI injection and return???

	// Switch cases can't use run-time values since the compiler generates a jump table
	// Custom controls
	
	// Movement
	if(arg.key == settings.controls.keys[Settings::KEY::MOVE_FORWARD]){
		if(!m_player->getCamera()->m_moveForwardsPressed){
			m_player->getCamera()->m_moveForwardsPressed = true;
		}
	}
	else if(arg.key == settings.controls.keys[Settings::KEY::MOVE_BACKWARD]){
		if(!m_player->getCamera()->m_moveBackwardsPressed){
			m_player->getCamera()->m_moveBackwardsPressed = true;
		}
	}
	else if(arg.key == settings.controls.keys[Settings::KEY::MOVE_LEFT]){
		if(!m_player->getCamera()->m_moveLeftPressed){
			m_player->getCamera()->m_moveLeftPressed = true;
		}
	}
	else if(arg.key == settings.controls.keys[Settings::KEY::MOVE_RIGHT]){
		if(!m_player->getCamera()->m_moveRightPressed){
			m_player->getCamera()->m_moveRightPressed = true;
		}
	}

	// Actions
	else if(arg.key == settings.controls.keys[Settings::KEY::ACTION]){
		m_player->action(m_pEventManager);
	}

	switch(arg.key){
	case OIS::KC_X:
		printf("Magic Cube Count: %d\n", m_profile->getInventory().getCount(Inventory::MAGIC_CUBES));
		break;

	case OIS::KC_LSHIFT:
	case OIS::KC_RSHIFT:
		m_player->getCamera()->m_shiftPressed = true;
		break;

	case OIS::KC_SPACE:
		if(!m_player->getCamera()->m_spacePressed){
			m_player->getCamera()->m_spacePressed = true;
		}
		break;

	// Action key
	/*case OIS::KC_E:
		m_player->action(m_pEventManager);
		break;*/

	// Keys for switching boots
	case OIS::KC_1:
		//Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_NORMAL);
		m_player->setWeapon(m_profile->getInventory().getWeapon(0), m_pEventManager);
		break;

	case OIS::KC_2:
		//Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_VELOCITY);
		m_player->setWeapon(m_profile->getInventory().getWeapon(1), m_pEventManager);
		break;

	case OIS::KC_3:
		//Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_HIGH);
		m_player->setWeapon(m_profile->getInventory().getWeapon(2), m_pEventManager);
		break;

	case OIS::KC_4:
		m_player->setWeapon(m_profile->getInventory().getWeapon(3), m_pEventManager);
		break;

	case OIS::KC_TAB:

		break;

	case OIS::KC_CAPITAL:

		break;

	// NOTE: Below are debugging keys only, delete later

	case OIS::KC_G:
		{
			Text* text = new Text();

			text->text = "Find a way out.";
			text->style = Text::FADE_SINE;
			text->font = "StarWars";
			text->pos = TextRenderer::UPPER;
			text->timeout = 5000;

			TextRenderer::getSingletonPtr()->setText(text);
		}
		break;

	case OIS::KC_C:
		{
			m_player->getGlasses()->flipState();
		}
		break;

	case OIS::KC_B:
		{
			static bool bb = false;

			bb = !bb;

			Ogre::SceneNode::ChildNodeIterator itr = m_pSceneMgr->getSceneNode("MainScene")->getChildIterator();
			for(;itr.hasMoreElements();){
				Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itr.getNext());
				child->showBoundingBox(bb);
			}
		}
		break;

	case OIS::KC_P:
		{
			m_physics->flipDebugDrawer();
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
		m_player->getFlashlight()->switchState();
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

	case OIS::KC_N:
		m_profile->nextStage();
		m_state = STATE_LOADING_NEXT_STAGE;
		break;

	case OIS::KC_O:
		{
			
		}
		break;

	case OIS::KC_J:
		{
			static int index = 0;

			std::string name = "light" + Ogre::StringConverter::toString(++index);
            Ogre::Light *light = m_pSceneMgr->createLight(name);

            light->setDiffuseColour(Ogre::ColourValue::White);//Ogre::ColourValue(r, g, b));
            light->setSpecularColour(Ogre::ColourValue::White);//Ogre::ColourValue(r, g, b));

            light->setDirection(Ogre::Vector3(0, 0, -1));

            light->setType(Ogre::Light::LT_SPOTLIGHT);
            light->setSpotlightInnerAngle(Ogre::Degree(5));
            light->setSpotlightOuterAngle(Ogre::Degree(20));
			light->setSpotlightFalloff(1.0);
			//light->setCustomShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup()));

            light->setAttenuation(500, 0.5, 0, 0); // meter range.  the others our shader ignores

            Ogre::SceneNode *node =
               m_pSceneMgr->getRootSceneNode()->createChildSceneNode(name + "Node");
            node->setPosition(m_player->getPosition());
            node->setOrientation(m_player->getCamera()->getYawNode()->getOrientation() * m_player->getCamera()->getPitchNode()->getOrientation());
            node->attachObject(light);
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

	case OIS::KC_F5:
		m_profile->save();
		break;

	case OIS::KC_F9:
		
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
	Settings& settings = Settings::getSingleton();

	if(m_state != STATE_ACTIVE){
		return true;
	}

	// Custom controls

	// Movement
	if(arg.key == settings.controls.keys[Settings::KEY::MOVE_FORWARD]){
		if(m_player->getCamera()->m_moveForwardsPressed){
			m_player->getCamera()->m_moveForwardsPressed = false;
		}
	}
	else if(arg.key == settings.controls.keys[Settings::KEY::MOVE_BACKWARD]){
		if(m_player->getCamera()->m_moveBackwardsPressed){
			m_player->getCamera()->m_moveBackwardsPressed = false;
		}
	}
	else if(arg.key == settings.controls.keys[Settings::KEY::MOVE_LEFT]){
		if(m_player->getCamera()->m_moveLeftPressed){
			m_player->getCamera()->m_moveLeftPressed = false;
		}
	}
	else if(arg.key == settings.controls.keys[Settings::KEY::MOVE_RIGHT]){
		if(m_player->getCamera()->m_moveRightPressed){
			m_player->getCamera()->m_moveRightPressed = false;
		}
	}

	switch(arg.key){
	case OIS::KC_LSHIFT:
	case OIS::KC_RSHIFT:
		m_player->getCamera()->m_shiftPressed = false;
		break;

	case OIS::KC_SPACE:
		if(m_player->getCamera()->m_spacePressed){
			m_player->getCamera()->m_spacePressed = false;
		}
		break;
	}

	Base::getSingletonPtr()->keyReleased(arg);

	return true;
}

//================================================//

bool GameState::mouseMoved(const OIS::MouseEvent& arg)
{
	if(m_state != STATE_ACTIVE){
		return false;
	}

	m_player->getCamera()->pitch(Ogre::Degree(arg.state.Y.rel)); 
	m_player->getCamera()->yaw(Ogre::Degree(arg.state.X.rel));
	return true;
}

//================================================//

bool GameState::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(m_state != STATE_ACTIVE){
		return false;
	}

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
	if(m_state != STATE_ACTIVE){
		return false;
	}

	return true;
}

//================================================//

void GameState::update(double timeSinceLastFrame)
{
	if(m_bQuit){
		this->popAppState();
		return;
	}

	// Update according to sub-state
	switch(m_state){
	default:
		break;

	case STATE_ACTIVE:
		// Update physics
		this->updateBullet(timeSinceLastFrame);

		// Update player
		m_player->update(timeSinceLastFrame);

		// skyx...
		if(m_skyXInitialised){
			Ogre::Real time = m_skyXController->getTime().x;

			if(time > 21.00 || time < 7.30){
				m_pSunlight->setVisible(false);
			}
			else{
				m_pSunlight->setVisible(true);
				m_pSunlight->setDirection(-m_skyXController->getSunDirection());
			}
		}

		// Hydrax
		if(m_hydraxInitialised){
			m_hydraxCamera->setPosition(m_player->getPosition());
			m_hydraxCamera->setOrientation(m_player->getCamera()->getYawNode()->getOrientation() * m_player->getCamera()->getPitchNode()->getOrientation());

			m_hydrax->update(timeSinceLastFrame / 1024.0);

			if(m_skyXInitialised){
				m_hydrax->setSunPosition(m_player->getPosition() + m_skyXController->getSunDirection() * m_skyX->getMeshManager()->getSkydomeRadius(m_hydraxCamera) * 0.5);
				printf("Time %.2f\n", m_skyXController->getTime().x);
				// 21:30 = sunset
			}
		}

		// Update events
		m_pEventManager->update(timeSinceLastFrame);

		// Update UI
		this->updateUI();

		// Update TextRenderer
		TextRenderer::getSingletonPtr()->update(timeSinceLastFrame);

		break;

	case STATE_LOADING_FIRST_ENTRY:
		switch(m_loadingStep){
		default:
			
			break;

		case STEP_FIRST:
			// Setup loading screen
			this->createLoadingGUI();

			// Init player so we can use the camera
			m_player = new Player(m_pSceneMgr, m_profile);
			m_player->init();
			m_player->initGlasses(m_pCompositor);
			break;

		case STEP_CREATE_PHYSICS_WORLD:
			m_physics = new Physics();
			m_physics->init();

			m_player->getCamera()->init(m_physics); // must have physics world to init
			Base::getSingletonPtr()->m_pViewport->setCamera(m_player->getCamera()->getOgreCamera());
			break;

		case STEP_INIT_EVENT_MANAGER:
			m_pEventManager = new EventManager(m_pSceneMgr, m_player->getCamera());
			break;

		case STEP_PRELOAD_MESH_DATA:
			this->preloadMeshData();
			break;

		case STEP_CREATE_SCENE:
			this->createScene();
			break;

		case STEP_SETUP_COLLISION_WORLD:
			// Weird gray screen here after moving this code from createScene()
			// Add entities to the collision world
			m_physics->registerAllEntitiesInScene(m_pSceneMgr);

			// Add dynamic objects
			m_pEventManager->getDynamicObjectManager()->registerAllObjectsInScene();

			// Debug drawer for Bullet
			m_physics->initDebugDrawer(m_pSceneMgr->getRootSceneNode());
			break;

		case STEP_CREATE_GUI:
			this->createGUI();
			Base::getSingletonPtr()->m_injectGUI = false;
			break;

		case STEP_FINAL:
			if(m_pSceneMgr->hasSceneNode("PlayerSpawn")){
				m_player->spawn(m_pSceneMgr->getSceneNode("PlayerSpawn"));
			}

			// Final steps before game begins
			m_player->setWeapon(m_profile->getInventory().getWeapon(0), m_pEventManager);
			m_GUILoadingLayer->setVisible(false);
			m_state = STATE_ACTIVE;
			m_loadingStep = 0;
			return;
		}

		// Increment loading step
		++m_loadingStep;

		// Update progress bar
		m_GUILoadingLayer->incrementProgressBar(GUIGameState::GUILoadingLayer::PROGRESSBAR_STATUS, 100);

		break; // STATE_LOADING_FIRST_ENTRY

	case STATE_LOADING_NEXT_STAGE:
		switch(m_loadingStep){
		default:

			break;

		case STEP_FIRST:
			// Clear the current scene and prepare for loading of next scene
			this->destroyScene();

			m_skyXInitialised = false;
			m_hydraxInitialised = false;

			Base::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
			this->createSceneManager();

			this->createLoadingGUI();

			m_player = new Player(m_pSceneMgr, m_profile);
			m_player->init();
			m_player->initGlasses(m_pCompositor);

			m_GUILoadingLayer->setProgressBarPosition(GUIGameState::GUILoadingLayer::PROGRESSBAR_STATUS, 0);
			break;

		case STEP_CREATE_PHYSICS_WORLD:
			m_physics->init();

			m_player->getCamera()->init(m_physics);
			Base::getSingletonPtr()->m_pViewport->setCamera(m_player->getCamera()->getOgreCamera());
			break;

		case STEP_INIT_EVENT_MANAGER:
			m_pEventManager = new EventManager(m_pSceneMgr, m_player->getCamera());
			break;

		case STEP_PRELOAD_MESH_DATA:
			this->preloadMeshData();
			break;

		case STEP_CREATE_SCENE:
			this->createScene();
			break;

		case STEP_SETUP_COLLISION_WORLD:
			m_physics->registerAllEntitiesInScene(m_pSceneMgr);

			m_pEventManager->getDynamicObjectManager()->registerAllObjectsInScene();

			m_physics->initDebugDrawer(m_pSceneMgr->getRootSceneNode());
			break;

		case STEP_CREATE_GUI:
			this->createGUI();
			break;

		case STEP_FINAL:
			m_player->setWeapon(m_profile->getInventory().getWeapon(0), m_pEventManager);
			m_GUILoadingLayer->setVisible(false);
			m_state = STATE_ACTIVE;
			m_loadingStep = 0;
			return;
		}

		// Increment loading step
		++m_loadingStep;

		// Update progress bar
		m_GUILoadingLayer->incrementProgressBar(GUIGameState::GUILoadingLayer::PROGRESSBAR_STATUS, 100);

		break; // STATE_LOADING_NEXT_STAGE
	}
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
								m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_FRIENDLY));
							}
							else{
								m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_ENEMY));
							}

							npcFound = true;
							break;
						}
					}
				}
			}
			// Check for retrievable object
			else{
				DynamicObject* object = m_pEventManager->getDynamicObjectManager()->getObject(rayhit->node);
				if(object != nullptr){
					if(object->isRetrievable()){
						m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_FRIENDLY));
						npcFound = true;
					}
					else{
						m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_DEFAULT));
					}
				}
			}
		}
	}

	if(!npcFound){
		m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_DEFAULT));
	}
}

//================================================//

void GameState::updateBullet(double timeSinceLastFrame)
{
	m_physics->update(timeSinceLastFrame);
}

//================================================//
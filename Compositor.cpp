//================================================//

#include "Compositor.hpp"

//================================================//

namespace Sparks
{

//================================================//

Compositor::Compositor(void)
{

}

//================================================//

Compositor::Compositor(Ogre::SceneManager* mgr, Ogre::Viewport* vp)
{
	m_pSceneMgr = mgr;
	m_pViewport = vp;

	this->setup();
}

//================================================//

Compositor::~Compositor(void)
{
	// Clean up compositor logics
	//! Using this causes an exception when deleting Ogre::Root on shutdown for some reason
	/*Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
	CompositorLogicMap::const_iterator itr = m_compositorLogics.begin();
	CompositorLogicMap::const_iterator end = m_compositorLogics.end();

	for(;itr != end; ++itr){
		compMgr.unregisterCompositorLogic(itr->first);
		delete itr->second;
	}

	m_compositorLogics.clear();*/
}

//================================================//

void Compositor::setup(void)
{
	Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();

	static bool logicRegistered = false;
	if(!logicRegistered){
		m_compositorLogics[COMPOSITOR_GAUSSIAN_BLUR]	= new GaussianBlurLogic;
		m_compositorLogics[COMPOSITOR_HDR]				= new HDRLogic;
		m_compositorLogics[COMPOSITOR_HEAT_VISION]		= new HeatVisionLogic;

		compMgr.registerCompositorLogic(COMPOSITOR_GAUSSIAN_BLUR, m_compositorLogics[COMPOSITOR_GAUSSIAN_BLUR]);
		compMgr.registerCompositorLogic(COMPOSITOR_HDR, m_compositorLogics[COMPOSITOR_HDR]);
		compMgr.registerCompositorLogic(COMPOSITOR_HEAT_VISION, m_compositorLogics[COMPOSITOR_HEAT_VISION]);

		logicRegistered = true;
	}

	// SSAO
	//m_pSSAO = compMgr.addCompositor(m_pViewport, COMPOSITOR_SSAO);
	//m_pSSAO->setEnabled(true);
	//m_pSSAO->addListener(&ssaoParamUpdater);

	this->createEffects();
	this->registerCompositors();
}

//================================================//

void Compositor::setEnabled(const Ogre::String& name, bool enabled)
{
	if(name == COMPOSITOR_SSAO)
		m_pSSAO->setEnabled(enabled);
	else
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, name, enabled);
}

//================================================//

void Compositor::registerCompositors(void)
{
	Ogre::CompositorManager::ResourceMapIterator resourceIterator =
        Ogre::CompositorManager::getSingleton().getResourceIterator();

	for(;resourceIterator.hasMoreElements();){
		Ogre::ResourcePtr resource = resourceIterator.getNext();
		const Ogre::String& compositorName = resource->getName();

		// Don't add base Ogre/Scene compositor to view
		//if(Ogre::StringUtil::startsWith(compositorName, "Ogre/Scene/", false)){
		//	continue;
		//}
		//// Don't add the deferred shading compositors
		//if(Ogre::StringUtil::startsWith(compositorName, "DeferredShading", false)){
		//	continue;
		//}
		//// Don't add the SSAO compositors
		//if(Ogre::StringUtil::startsWith(compositorName, "SSAO", false)){
		//	continue;
		//}
		//// Don't add the TestMRT compositor
		//if(Ogre::StringUtil::startsWith(compositorName, "TestMRT", false)){
		//	continue;
		//}

		m_compositorNames.push_back(compositorName);
		int addPosition = -1;
		if(compositorName == "HDR"){
			addPosition = 0;
		}
		try{
			Ogre::CompositorManager::getSingleton().addCompositor(m_pViewport, compositorName, addPosition);
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, compositorName, false);
		} catch(...){
			Ogre::LogManager::getSingleton().logMessage("Failed to load compositor " + compositorName);
		}
	}
}

//================================================//

/* Hard coded postfilter effects */
void Compositor::createEffects(void)
{
	/// Motion blur effect
	Ogre::CompositorPtr compPtr = Ogre::CompositorManager::getSingleton().create(
		COMPOSITOR_MOTION_BLUR, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	{
		Ogre::CompositionTechnique* t = compPtr->createTechnique();
		//t->setCompositorLogicName(COMPOSITOR_MOTION_BLUR);
		{
			Ogre::CompositionTechnique::TextureDefinition* def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition* def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition* def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass* tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		// Do the motion blur
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ 
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Ogre/Compositor/Combine");
				pass->setInput(0, "scene");
				pass->setInput(1, "sum");
			}
		}
		// Copy back sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ 
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Ogre/Compositor/Copyback");
				pass->setInput(0, "temp");
			}
		}
		// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ 
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Ogre/Compositor/MotionBlur");
				pass->setInput(0, "sum");
			}
		}
	}

	/// Heat vision effect
	Ogre::CompositorPtr comp4 = Ogre::CompositorManager::getSingleton().create(
			COMPOSITOR_HEAT_VISION, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		);
	{
		Ogre::CompositionTechnique *t = comp4->createTechnique();
		t->setCompositorLogicName(COMPOSITOR_HEAT_VISION);
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Light to heat pass
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setIdentifier(0xDEADBABE); /// Identify pass for use in listener
				pass->setMaterialName("Fury/HeatVision/LightToHeat");
				pass->setInput(0, "scene");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Fury/HeatVision/Blur");
				pass->setInput(0, "temp");
			}
		}
	}
}

//================================================//

}

//================================================//
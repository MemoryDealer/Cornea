//================================================//

#include "TextRenderer.hpp"

//================================================//

template<> TextRenderer* Ogre::Singleton<TextRenderer>::msSingleton = 0;

//================================================//

TextRenderer::TextRenderer(void)
{
	m_pOverlayMgr = Ogre::OverlayManager::getSingletonPtr();

	m_pOverlay = m_pOverlayMgr->create("TextRendererOverlay");
	m_panel = static_cast<Ogre::OverlayContainer*>(m_pOverlayMgr->createOverlayElement("Panel", "TextRendererContainer"));
	m_panel->setDimensions(1, 1);
	m_panel->setPosition(0, 0);

	m_pOverlay->add2D(m_panel);

	m_pOverlay->show();
	m_panel->show();

	m_upperText = m_lowerText = nullptr;

	// This is just a hack, for some reason the first text item added to the overlay panel isn't showing up
	Text* text = new Text();
	text->text = "";
	this->setText(FILLER, text);
}

//================================================//

TextRenderer::~TextRenderer(void)
{

}

//================================================//

void TextRenderer::setText(int position, Text* text)
{
	switch(position){
	default:
	case UPPER:
		text->id = "UpperText";
		if(m_upperText != nullptr)
			this->clearText(UPPER);

		m_upperText = text;
		break;

	case LOWER:
		text->id = "LowerText";
		if(m_lowerText != nullptr)
			this->clearText(LOWER);

		m_lowerText = text;
		break;

	case FILLER:
		text->id = "FillerText";
		m_fillerText = text;
		break;
	}

	Ogre::Real width = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	Ogre::Real height = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	text->element = m_pOverlayMgr->createOverlayElement("TextArea", text->id);
	text->element->setDimensions(width, height);
	text->element->setMetricsMode(Ogre::GMM_PIXELS);
	
	text->element->setWidth(width);
	text->element->setHeight(height);
	text->element->setParameter("font_name", text->font);
	text->element->setParameter("char_height", text->size);
	text->element->setColour(text->colour);

	text->element->setCaption(text->text);

	// Set alignment
	switch(position){
	default:
	case UPPER:
		text->element->setPosition(0, 20);
		text->element->setParameter("alignment", "center");
		text->element->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_CENTER);
		break;
		
	case LOWER:
		text->element->setPosition(0, Base::getSingletonPtr()->m_pViewport->getActualHeight() - 100);
		text->element->setParameter("alignment", "center");
		text->element->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_CENTER);
		break;

	case FILLER:
		text->element->setPosition(0, 0);
		break;
	}

	m_panel->addChild(text->element);

	// Start timer
	if(position != FILLER)
		text->timer = new Ogre::Timer();
}

//================================================//

void TextRenderer::clearText(int position)
{
	Text* text = nullptr;
	switch(position){
	default:
		return;

	case UPPER:
		text = m_upperText;
		break;

	case LOWER:
		text = m_lowerText;
		break;
	}

	if(text == nullptr)
		return;

	m_panel->removeChild(text->id);
	m_pOverlayMgr->destroyOverlayElement(text->id);
	delete text;
	text = nullptr;
}

//================================================//

void TextRenderer::update(double timeSinceLastFrame)
{
	if(m_upperText != nullptr)
		m_upperText->update(timeSinceLastFrame);

	if(m_lowerText != nullptr)
		m_lowerText->update(timeSinceLastFrame);
}

//================================================//
//================================================//

Text::Text(void)
{
	text = "Default Text";
	font = "Font2A";
	size = "32";
	timeout = 20000.0;
	colour = Ogre::ColourValue::White;
	style = STATIC;
	sin = 0.0;
}

//================================================//

Text::~Text(void)
{
	delete timer;
}

//================================================//

void Text::update(double timeSinceLastFrame)
{
	// Update timer...

	switch(style){
	default:
		break;

	case FADE_SINE:
		{
			colour.a = Ogre::Math::Abs(Ogre::Math::Sin(sin));
			element->setColour(colour);

			sin += 0.002 * timeSinceLastFrame;
		}
		break;
	}
}

//================================================//
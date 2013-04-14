//================================================//

#include "TextRenderer.hpp"

//================================================//

template<> TextRenderer* Ogre::Singleton<TextRenderer>::msSingleton = 0;

//================================================//

const std::string TextRenderer::DEFAULT_FONT = "StarWars";

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

	m_upperText = nullptr;
	m_lowerText = nullptr;
	m_fillerText = nullptr;

	// This is just a hack, for some reason the first text item added to the overlay panel isn't showing up
	Text* text = new Text();
	text->text = "";
	text->pos = TextRenderer::FILLER;
	this->setText(text);
}

//================================================//

TextRenderer::~TextRenderer(void)
{

}

//================================================//

void TextRenderer::setText(Text* text)
{
	switch(text->pos){
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
	switch(text->pos){
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
	if(text->pos != FILLER)
		text->timer = new Ogre::Timer();
}

//================================================//

void TextRenderer::clearText(int position)
{
	switch(position){
	default:
		return;

	case UPPER:
		if(m_upperText == nullptr)
			return;

		m_panel->removeChild(m_upperText->id);
		m_pOverlayMgr->destroyOverlayElement(m_upperText->id);
		delete m_upperText;
		m_upperText = nullptr;
		break;

	case LOWER:
		if(m_lowerText == nullptr)
			return;

		m_panel->removeChild(m_lowerText->id);
		m_pOverlayMgr->destroyOverlayElement(m_lowerText->id);
		delete m_lowerText;
		m_lowerText = nullptr;
		break;
	}
}

//================================================//

void TextRenderer::clearAll(void)
{
	this->clearText(UPPER);
	this->clearText(LOWER);
}

//================================================//

void TextRenderer::update(double timeSinceLastFrame)
{
	if(m_upperText != nullptr)
		if(!m_upperText->update(timeSinceLastFrame))
			this->clearText(UPPER);

	if(m_lowerText != nullptr)
		if(!m_lowerText->update(timeSinceLastFrame))
			this->clearText(LOWER);
}

//================================================//
//================================================//

Text::Text(void)
{
	text = "Default Text";
	font = TextRenderer::DEFAULT_FONT;
	size = "24";
	timeout = 20000.0;
	colour = Ogre::ColourValue::White;
	style = STATIC;
	sin = 0.0;
	timer = nullptr;
}

//================================================//

Text::~Text(void)
{
	if(timer != nullptr)
		delete timer;
}

//================================================//

bool Text::update(double timeSinceLastFrame)
{
	// Update timer...
	if(timer->getMillisecondsCPU() >= timeout){
		return false;
	}

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

	return true;
}

//================================================//
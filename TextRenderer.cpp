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

	m_numTexts = 0;

	// This is just a hack, for some reason the first text item added to the overlay panel isn't showing up
	Text* text = new Text();
	text->text = "";
	this->addText(text);
}

//================================================//

TextRenderer::~TextRenderer(void)
{

}

//================================================//

void TextRenderer::addText(Text* text)
{
	std::ostringstream stream;
	stream << "textID" << m_numTexts + 1;
	text->id = stream.str();

	text->element = m_pOverlayMgr->createOverlayElement("TextArea", text->id);
	text->element->setDimensions(text->width, text->height);
	text->element->setMetricsMode(Ogre::GMM_PIXELS);
	text->element->setPosition(text->x, text->y);
	text->element->setWidth(text->width);
	text->element->setHeight(text->height);
	text->element->setParameter("font_name", text->font);
	text->element->setParameter("char_height", text->size);
	text->element->setColour(text->colour);

	text->element->setCaption(text->text);

	m_panel->addChild(text->element);

	m_texts.push_back(text);
	m_numTexts++;
}

//================================================//

void TextRenderer::removeText(Text* text)
{
	m_panel->removeChild(text->id);
	m_pOverlayMgr->destroyOverlayElement(text->id);
}

//================================================//

void TextRenderer::setText(const int index, const std::string& text)
{
	//m_texts[index]->element->setCaption(text);
}

//================================================//
//================================================//

Text::Text(void)
{
	text = "Default Text";
	font = "Font2A";
	size = "32";
	timeout = 10000.0;
	width = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	height = 20.0;
	//x = y = 10.0;
	x = Base::getSingletonPtr()->m_pViewport->getActualWidth() / 2.0;
	y = 15.0;
	colour = Ogre::ColourValue::White;
}

//================================================//

Text::~Text(void)
{

}

//================================================//

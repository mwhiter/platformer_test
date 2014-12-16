#include "PCH.h"

Renderer::Renderer(sf::RenderWindow *pWindow):
	pWindow(pWindow),
	pView(new sf::View())
{
}


Renderer::~Renderer(void)
{
}

// Draw the world.
void Renderer::draw()
{
	// Do nothing if the window was closed for some reason
	if(pWindow == NULL) return;
	if(!pWindow->isOpen()) return;

	pWindow->clear();

	// Draw the screen
	CORE.GetScreenManager()->GetActiveScreen()->draw();

	// Set the viewpoint of the game
	updateView();
	CORE.GetRenderWindow()->setView(*pView);

	if (CORE.GetScreenManager()->GetActiveScreen()->GetType() == SCREEN_TYPE_INGAME)
	{
		// Draw the level
		if (CORE.GetCurrentLevel() != NULL)
		{
			CORE.GetCurrentLevel()->draw();
		}

		drawUI();
	}

	pWindow->display();
}

// Draw the game's UI
void Renderer::drawUI()
{
	sf::Font font;
	font.loadFromFile("../assets/fonts/Harabara Mais.otf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(10);
	text.setColor(sf::Color::White);

	sf::RectangleShape rect(sf::Vector2f(pView->getSize().x, 16));
	rect.setPosition(sf::Vector2f(pView->getCenter().x - pView->getSize().x/2, pView->getCenter().y + pView->getSize().y / 2 - 16.0f));
	rect.setFillColor(sf::Color::Black);
	pWindow->draw(rect);

	// change the rectangle and render it again
	rect.setSize(sf::Vector2f(pView->getSize().x, 2));
	rect.setFillColor(sf::Color::White);
	pWindow->draw(rect);

	// Health info
	HealthController* pHealth = core.GetCurrentLevel()->GetPlayer()->GetHealthController();
	if (pHealth != NULL)
	{
		int health = pHealth->GetHealth();

		sf::String szHealth = "Health: " + std::to_string(health);

		float x = pView->getCenter().x - pView->getSize().x / 2 + 10.0f;
		float y = pView->getCenter().y + pView->getSize().y / 2 - 13.0f;

		text.setString(szHealth);
		text.setPosition(sf::Vector2f(x, y));

		pWindow->draw(text);
	}

	// Weapon info
	Weapon* pWeapon = core.GetCurrentLevel()->GetPlayer()->GetWeaponController()->GetActiveWeapon();
	if (pWeapon != NULL)
	{
		int magAmmo = pWeapon->GetCurrentMagazineAmmo();
		int curAmmo = pWeapon->GetCurrentGunAmmo();

		sf::String szWeaponAmmo = pWeapon->GetName() + ": " + std::to_string(magAmmo) + "/" + std::to_string(curAmmo);

		float x = pView->getCenter().x - pView->getSize().x / 2 + 80.0f;
		float y = pView->getCenter().y + pView->getSize().y / 2 - 13.0f;

		text.setString(szWeaponAmmo);
		text.setPosition(sf::Vector2f(x, y));

		pWindow->draw(text);
	}
}

void Renderer::updateView()
{
	// In-Game viewport
	if (CORE.GetScreenManager()->GetActiveScreen()->GetType() == SCREEN_TYPE_INGAME)
	{
		float pCenterX = 0.0, pCenterY = 0.0;

		const int TILES_ON_SCREEN = 16;

		Entity* pPlayer = CORE.GetCurrentLevel()->GET_PLAYER();

		// Position the viewport around the player...
		// commented out if-statement because I want to crash if the player doesn't have a displacement controller
		//if (pPlayer->GetDisplacementController() != NULL)
		//{
		pCenterX = pPlayer->GetDisplacementController()->GetPosition()->x;
		pCenterY = pPlayer->GetDisplacementController()->GetPosition()->y;

		pCenterX = std::max(float(TILE_SIZE * TILES_ON_SCREEN / 2), pCenterX);
		pCenterX = std::min(float(CORE.GetCurrentLevel()->GetTileWidth() * TILE_SIZE) - float(TILE_SIZE * TILES_ON_SCREEN / 2), pCenterX);

		pCenterY = std::max(float(TILE_SIZE * TILES_ON_SCREEN / 2), pCenterY);
		pCenterY = std::min(float(CORE.GetCurrentLevel()->GetTileHeight() * TILE_SIZE) - float(TILE_SIZE * TILES_ON_SCREEN / 2), pCenterY);
		//}

		// Create the actual viewport
		pView->setCenter(sf::Vector2f(pCenterX, pCenterY));
		pView->setSize(sf::Vector2f(float(TILE_SIZE * TILES_ON_SCREEN), float(TILE_SIZE * TILES_ON_SCREEN)));
	}
	// Menu viewport
	else
	{
		pView->setCenter((float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2);
		pView->setSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	}
}

// Is this position currently on the visible screen?
bool Renderer::IsVisibleOnScreen(const sf::Vector2f& pos)
{
	float fScreenLeftX		= pView->getCenter().x - pView->getSize().x / 2;
	float fScreenRightX		= pView->getCenter().x + pView->getSize().x / 2;
	float fScreenBottomY	= pView->getCenter().y + pView->getSize().y / 2;
	float fScreenTopY		= pView->getCenter().y - pView->getSize().y / 2;

	return (
		pos.x > fScreenLeftX &&
		pos.x < fScreenRightX &&
		pos.y > fScreenTopY &&
		pos.y < fScreenBottomY
		);
}

sf::View* Renderer::GetView() const
{
	return pView;
}
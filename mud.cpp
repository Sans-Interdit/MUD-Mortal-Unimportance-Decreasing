#include "classes.h"
#include <array>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1250, 750), "SFML works!");
    window.setFramerateLimit(60);

    sf::View cam{ sf::FloatRect(0, 0, 1250, 750) };
    sf::View HUD{ sf::FloatRect(0, 0, 1250, 750) };//-------------------------------------------------------------------------------------------------------------------

    std::unordered_set<sf::RectangleShape*> lstR;
    std::unordered_set<Entite*> lstProj;
    std::unordered_set<Attaque*> lstAtt;
    std::unordered_set<Plateforme*> lstPF;
    std::unordered_set<Ennemie*> lstEnn;
    EntityLists drawable{ &lstR, &lstPF, &lstEnn, &lstProj, &lstAtt };
    
    int level[20][20]{//la carte du niveau dans un tableau en 2 dimensions
        { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2 },
        { 2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,2 },
        { 2,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,1,2 },
        { 2,0,0,0,0,0,1,0,0,0,0,1,1,0,1,0,1,1,0,2 },
        { 2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
        { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 }
    };
    
    for (int y{ 0 }; y < sizeof(level)/sizeof(level[0]) ; y++)
    {
        for (int x{ 0 }; x < sizeof(level[0]) / sizeof(int); x++)
        {
            if (level[y][x]==1)
            {
                drawable.ptrPF->insert(new Plateforme (static_cast<float>(x * 200), static_cast<float>(y * 200)));
            }
            else if (level[y][x] == 2)
            {
                drawable.ptrPF->insert(new Plateforme(static_cast<float>(x * 200), static_cast<float>(y * 200), "mur"));
            }
        }
    }

    int persoIndex = 0;
    std::array<PJ, 3> persos{PJ(& drawable, 1), PJ(&drawable, 2), PJ(&drawable, 3)};
    drawable.perso = &persos[persoIndex];
    //Ennemie mechant(&drawable, 800, 600);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::Tab)
                {
                    jointVar vars = drawable.perso->resetVar();
                    ++persoIndex;
                    if (persoIndex == 3) { persoIndex = 0; }
                    drawable.perso = &persos[persoIndex];
                    drawable.perso->recoverVar(vars);
                }
            }
        }
        //Actualisation et dessin des Entités
        drawable.perso->update();
        window.clear();
        window.setView(HUD);
        for (sf::RectangleShape* r : *drawable.hud)
        {
            window.draw(*r);
        }
        window.setView(cam);
        cam.setCenter(drawable.perso->getPosition());

        for (Entite* Proj : *drawable.ptrProj)
        {
            window.draw(*Proj);
        }
        for (Attaque* att : *drawable.ptrAtt)
        {
            window.draw(*att);
        }
        for (Ennemie* enn : *drawable.ptrEnn)
        {
            size_t t = drawable.ptrEnn->size();
            enn->update();
            if (drawable.ptrEnn->size() != t)
            {
                break;
            }
            window.draw(*enn);
        }
        window.draw(*drawable.perso);
        for (Plateforme* PF : *drawable.ptrPF)
        {
            window.draw(*PF);
        }
        window.display();
    }
    return 0;
}
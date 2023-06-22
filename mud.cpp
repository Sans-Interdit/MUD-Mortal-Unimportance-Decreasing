#include "classes.h"
#include <array>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1250, 750), "SFML works!");
    window.setFramerateLimit(60);

    sf::View cam{ sf::FloatRect(0,0,1250,750) };
    sf::View HUD{ sf::FloatRect(0, 0, 1250, 750) };//-------------------------------------------------------------------------------------------------------------------

    std::unordered_set<sf::RectangleShape*> lstR;
    std::unordered_set<Entite*> lstProj;
    std::unordered_set<Attaque*> lstAtt;
    std::unordered_set<Plateforme*> lstPF;
    std::unordered_set<Ennemie*> lstEnn;
    EntityLists drawable{&lstR, &lstPF, &lstEnn, &lstProj, &lstAtt };
    Plateforme plateforme1(0, 800);
    Plateforme plateforme2(300, 600);
    Plateforme plateforme3(300, 600, "mur");
    Plateforme plateforme4(0, 1100);


    drawable.ptrPF->insert(&plateforme1);
    drawable.ptrPF->insert(&plateforme2);
    drawable.ptrPF->insert(&plateforme3);
    drawable.ptrPF->insert( &plateforme4);

    int persoIndex = 0;
    std::array<PJ, 3> persos{PJ(& drawable, 1), PJ(&drawable, 2), PJ(&drawable, 3)};
    drawable.perso = &persos[persoIndex];
    Ennemie mechant(&drawable, 300, 485);

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
                    std::cout << "f";
                    jointVar vars = drawable.perso->resetVar();
                    ++persoIndex;
                    if (persoIndex == 3) { persoIndex = 0; }
                    drawable.perso = &persos[persoIndex];
                    drawable.perso->recoverVar(vars);
                }
            }
        }

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
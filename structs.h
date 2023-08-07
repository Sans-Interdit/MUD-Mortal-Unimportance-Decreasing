#ifndef ENTITYLISTS_H
#define ENTITYLISTS_H

#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <typeinfo>

class Entite;
class Ennemie;
class Plateforme;
class PJ;
class Attaque;

struct EntityLists
{
    std::unordered_set<sf::RectangleShape*>* hud;
    std::unordered_set<Plateforme*>* ptrPF;
    std::unordered_set<Ennemie*>* ptrEnn;
    std::unordered_set<Entite*>* ptrProj;
    std::unordered_set<Attaque*>* ptrAtt;
    PJ* perso;
};

struct Stats
{
    int maxHP{ 0 };
    int AD{ 0 };
    double vit{ 0 };
    std::vector<double> AS; // attack speed en fonction des attaques
};

struct JointVar
{
    sf::Vector2f pos;
    double vecX;
    double vecY;
    double tmpSaut;
    bool aDroite;
};

struct frameAtt
{
    frameAtt(float b, float a);
    int before;
    int after;
};

#endif

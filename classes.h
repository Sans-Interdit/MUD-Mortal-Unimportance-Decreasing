#ifndef CLASSES_H
#define CLASSES_H
#include "structs.h"

struct EntityLists;
struct Stats;

class Entite : public sf::Sprite
{
public:
    Entite() = default;
    Entite(std::string filePath, bool const giveHitBox);
    void setHitbox();
    sf::FloatRect getHitbox();
    void setImg(std::string const& filepath);
    sf::Texture getImg();
    std::string getImgName();
protected:
    sf::Texture m_img;
    std::string m_imgName;
    sf::FloatRect m_hitbox;
    sf::Vector2i m_imgCoord{ 0, 0 };
};


class Plateforme : public Entite
{
public:
    Plateforme(float const x, float const y, std::string type = "sol");
};

class Unit : public Entite
{
public:
    EntityLists* getPtrGroup();
    bool getADroite();//const
    void setADroite(bool d);
    Stats getStat();
    int getNumAtt();
    void setNumAtt(int val);
    void hit(int const dmg, double push);
    virtual const type_info& getType();//probablement améliorable
protected:
    int m_hp{ 0 };
    std::vector<Attaque*> m_attaque;
    int m_hitFrames{ 0 };
    sf::Vector2f m_avPos = getPosition();
    bool m_aDroite{ true };
    double m_attAnimTmp{ 0 };
    EntityLists* m_ptrGroup{ nullptr };
    Stats m_stat;
    int m_numAtt{ 0 };
    double m_vecteurX{ 0 };
};

class SolUnit : public Unit
{
protected:
    void physique();
    bool m_auSol{ false };
    double m_vecteurY{ 0 };
    double m_tmpSaut{ 0 };
};

class Attaque : public Entite
{
public:
    Attaque() = default;
    Attaque(Unit* joueur, std::string filepath, double delay, double multiplier, double knockback, double avTmp = 0);
    //void next();
    void reset();
    double getDelay();
    double getDelayStatic();//const
    double getAvTmp();
    void setDelay(double val);
    void virtual spawn();
    void virtual update() = 0;
    const virtual type_info& getType() = 0;

protected:
    std::unordered_set<Unit*> m_lstHit;
    Unit* m_ptrPerso;
    double m_delay{ 0 };
    double m_delayStatic{ 0 };
    double m_multiplier{ 1 };
    double m_knockback{ 0 };
    double m_avTmp{ 0 };
};

class CacAtt : public Attaque
{
    using Attaque::Attaque;
public:
    void update();
    const type_info& getType();
};

class DistAtt : public Attaque
{
    using Attaque::Attaque;
public:
    void spawn();
    void update();
    const type_info& getType();
};

class Ennemie : public SolUnit
{
public:
    Ennemie(EntityLists* drawable, float x, float y);
    void update();
    void attack();
    void run();
    const type_info& getType();
protected:
    double m_tmp{ 0 };
};

class PJ : public SolUnit
    /*
    les perso seraient :
    gros qui tir de loin(charge : gros tir ? / spe : upercut qui leve / switch : caintlyn)
    bombardier qui zone(ziggs devant / explo qui repousse / tp en haut)
    rapide (enchainement att genre ww / dash ou parrade(att qui dash si touche) / un truc horizontal ptet devant derière)
    */
{
public:
    PJ(EntityLists* drawable, int p);
    void update();
    void attack();
    void run();
    void saut();
    void spe();
    jointVar resetVar();
    void recoverVar(jointVar vars);
    int getHP();
    const type_info& getType();
private:
    bool m_doubleSaut{ true };
    bool m_sbMaintenue{ false };
    bool m_attChain{ false };
    double m_speTmp{ 10 };
    bool m_attHold{ false };
    sf::RectangleShape m_dmgRect {sf::Vector2f{1220, 720}};
};

//DIFFERENCIER AVEC DES POINTEUR (pas le meme emplacement memoire)

#endif // !CLASSES_H
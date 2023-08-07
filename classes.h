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
    void hit(int const dmg, double push, double kbY);
    void attTrigger(Attaque* att, int& tmp, bool condition);
    void nextAtt();
    EntityLists* getPtrGroup();
    bool getADroite();//const
    void setADroite(bool d);
    Stats getStat();
    int getNumAtt();
    frameAtt getTime();
    virtual const type_info& getType();//probablement améliorable
protected:
    int m_hp{ 0 };
    void attack(Attaque* att, int& tmp);
    std::vector<Attaque*> m_attaque;
    int m_hitFrames{ 0 };
    sf::Vector2f m_avPos = getPosition();
    bool m_aDroite{ true };
    int m_attTmp{ 0 };
    EntityLists* m_ptrGroup{ nullptr };
    Stats m_stat{ 0, 0, 0, {0} };
    int m_numAtt{ 0 };
    double m_vecteurX{ 0 };
    double m_vecteurY{ 0 };
    frameAtt m_timeAtt{ -1, -1 };
};

class SolUnit : public Unit
{
protected:
    void physique();
    bool m_auSol{ false };
    double m_tmpSaut{ 0 };
};

class Attaque : public Entite
{
public:
    Attaque() = default;
    Attaque(Unit* joueur, std::string filepath, frameAtt time, double duration,double multiplier, double knockback);
    //void next();
    void virtual reset();
    double getDelay();
    frameAtt getTime();
    //void setDelay(int val);
    void setKB(std::array<double, 2> kb);
    void virtual spawn();
    void virtual update();
    const virtual type_info& getType();

protected:
    Unit* m_ptrPerso{ nullptr };
    int m_delay{ 0 };
    int m_duration;
    frameAtt m_time;
    double m_multiplier;
    std::array<double, 2> m_kb{ 0, 0 };
};

class CacAtt : public Attaque
{
    using Attaque::Attaque;
public:
    void update();
    void spawn();
    void reset();
    const type_info& getType();
private:
    std::unordered_set<Unit*> m_lstHit;
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
    void run();
    const type_info& getType();
protected:
    double m_tmp{ 0 };
};


//fontion pour PJ::m_speType ---------> faudra l'améliorer
void dash(PJ* player, int&  tmp);
void shoryuken(PJ *player, int& tmp);
void bomb(PJ* player, int& tmp);

class PJ : public SolUnit
    /*
    les perso seraient :
    gros avec un tir de loin(charge : gros tir ? / spe : upercut qui leve / switch : caintlyn)
    bombardier qui zone(ziggs devant / explo qui repousse / tp en haut)
    rapide (enchainement att genre ww / dash ou parrade(att qui dash si touche) / un truc horizontal ptet devant derière)
    */
{
public:
    PJ(EntityLists* drawable, int p);
    void update();
    void run();
    void saut();
    JointVar resetVar();
    void recoverVar(JointVar vars);
    void setVecteurX(int x);
    CacAtt* getSpeAtt();
    int getHP();
    const type_info& getType();
private:
    double m_rAttDelay{ 0 };
    bool m_doubleSaut{ true };
    bool m_sbMaintenue{ false };
    bool m_attChain{ false };
    int m_speTmp{ -21 };
    CacAtt* m_speAtt;
    void(*m_speType)(PJ*, int &);//destructeur ptet nécéssaire / surement améliorable
    bool m_attHold{ false };
    sf::RectangleShape m_dmgRect {sf::Vector2f{1220, 720}};
};
//DIFFERENCIER AVEC DES POINTEUR (pas le meme emplacement memoire)

#endif // !CLASSES_H
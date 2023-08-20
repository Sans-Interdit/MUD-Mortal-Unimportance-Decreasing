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

class Unit : public Entite//class de gestion qui herite pas de sprite pour desengorger?
{
public:
    void hit(int const dmg, float push, float kbY);
    void attTrigger(Attaque* att, bool condition);
    EntityLists* getPtrGroup();
    bool getADroite();//const
    void setADroite(bool d);
    Stats getStat();
    frameAtt getTime();
    virtual const type_info& getType() = 0;//probablement améliorable
protected:
    int m_hp{ 0 };
    int m_hitFrames{ 0 };
    void attack();
    Attaque* m_attChosen { nullptr };
    sf::Vector2f m_avPos = getPosition();
    bool m_aDroite{ true };
    int m_attTmp{ 0 };
    EntityLists* m_ptrGroup{ nullptr };
    Stats m_stat{ 0, 0, 0 };
    float m_vecteurX{ 0 };
    float m_vecteurY{ 0 };
    frameAtt m_timeAtt{ -1, -1 };
};

class SolUnit : public Unit
{
protected:
    void physique();
    bool m_auSol{ false };
    float m_tmpSaut{ 0 };
};

class Attaque : public Entite
{
public:
    Attaque() = default;
    Attaque(Unit* joueur, std::string filepath, frameAtt time, float duration,float multiplier, float knockback);
    //void next();
    void virtual reset();
    float getDelay();
    frameAtt getTime();
    //void setDelay(int val);
    void setKB(std::array<float, 2> kb);
    void virtual spawn();
    void virtual update();
    const virtual type_info& getType();

protected:
    Unit* m_ptrPerso{ nullptr };
    int m_delay{ 0 };
    int m_duration;
    frameAtt m_time;
    float m_multiplier;
    std::array<float, 2> m_kb{ 0, 0 };
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
    unsigned int m_numAtt{ 0 };
    float m_tmp{ 0 };
    std::vector<Attaque*> m_attaques;
};


/*
void dash(PJ* player, int&  tmp);
void shoryuken(PJ *player, int& tmp);
void bomb(PJ* player, int& tmp);
*/

class PJ : public SolUnit // rendu abstraite de façon tres/trop simple - faire attention
    /*
    les perso seraient :
    gros avec un tir de loin(charge : gros tir ? / spe : upercut qui leve / switch : caintlyn)
    bombardier qui zone(ziggs devant / explo qui repousse / tp en haut)
    rapide (enchainement att genre ww / dash ou parrade(att qui dash si touche (genre E leona?)) / un truc horizontal ptet devant derière)
    */
{
public:
    PJ() = default;
    PJ(EntityLists* drawable);
    void update();
    JointVar resetVar();
    void recoverVar(JointVar vars);
    void setVecteurX(int x);
    int getHP();
    void nextAtt();
    const type_info& getType();
protected:
    void run();
    void saut();
    float m_rAttDelay{ 0 };
    bool m_doubleSaut{ true };
    bool m_sbMaintenue{ false };
    bool m_attChain{ false };
    unsigned int m_numAtt{ 0 };
    int m_speTmp{ -21 };
    std::vector<Attaque*> m_attaques;
    static const auto m_speVal{ 0 };
    virtual void special() = 0;
    //void(*m_speType)(PJ*, int &);//destructeur ptet nécéssaire / surement améliorable
    bool m_attHold{ false };
    sf::RectangleShape m_dmgRect {sf::Vector2f{1220, 720}};
};

class P1 : public PJ
{
public:
    P1(EntityLists* drawable);
private:
    void special();
};

class P2 : public PJ
{
public:
    P2(EntityLists* drawable);
private:
    CacAtt* m_speAtt;
    void special();
};

class P3 : public PJ
{
public:
    P3(EntityLists* drawable);
private:
    DistAtt* m_speAtt;
    void special();
};

//DIFFERENCIER AVEC DES POINTEUR (pas le meme emplacement memoire)

#endif // !CLASSES_H
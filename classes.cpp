#include "classes.h"

Entite::Entite(std::string filePath, bool const giveHitBox = true)
{
	m_img.loadFromFile(filePath);
	this->setTexture(m_img);
	if (giveHitBox)
	{
		setHitbox();
	}
}

sf::FloatRect Entite::getHitbox()
{
	return m_hitbox;
}

void Entite::setImg(std::string const& filepath)
{
	m_imgName = filepath;
	m_img.loadFromFile(m_imgName);
	setTexture(m_img);
}

sf::Texture Entite::getImg() { return m_img; }

std::string Entite::getImgName() { return m_imgName; }

void Entite::setHitbox() { m_hitbox = getGlobalBounds(); }

Plateforme::Plateforme(float const x, float const y, std::string type)
{
	this->m_img.loadFromFile("Sprites/" + type + ".png");
	this->setTexture(m_img);
	this->setPosition(x, y);
	setHitbox();
}

void Unit::hit(int const dmg, float push, float kbY = 0)
{
	m_hp -= dmg;
	m_vecteurX = push;
	if (kbY != 0) { m_vecteurY = kbY; }
	m_hitFrames = static_cast<int>(abs(push) > abs(kbY) ? abs(push)*2 : abs(kbY));
	m_attTmp = 0;
}

void Unit::attTrigger(Attaque* att, bool condition)
{
	if (condition && m_attTmp < 0)
	{
		m_timeAtt = att->getTime();
		m_attTmp = m_timeAtt.before + m_timeAtt.after;
		m_attChosen = att;
	}
}

void Unit::attack()
{
	if (m_attTmp >= 0)
	{
		if (m_attTmp == m_timeAtt.after)
		{
			m_attChosen->spawn();
		}
		else if (m_attTmp > m_timeAtt.after)
		{
			if (m_imgCoord.x != 1) { m_imgCoord.x = 1; }
		}
		else
		{
			if (m_imgCoord.x != 0) { m_imgCoord.x = 0; }
		}
		m_attTmp -= 1;
	}
}

EntityLists* Unit::getPtrGroup() { return m_ptrGroup; }

frameAtt Unit::getTime() { return m_timeAtt; }

bool Unit::getADroite() { return m_aDroite; }

void Unit::setADroite(bool d) { m_aDroite = d; }

Stats Unit::getStat() { return m_stat; }

void SolUnit::hit(int const dmg, float const push, float const kbY)
{
	m_tmpSaut = 0;
	m_vecteurY = 0;
	Unit::hit(dmg, push, kbY);
}

void SolUnit::physique()
{
	m_avPos = getPosition();

	float posY{ (float)(-0.0045 * (m_tmpSaut * m_tmpSaut) + m_vecteurY) };
	move(static_cast<float>(m_vecteurX), static_cast<float>(-posY));
	m_tmpSaut += 1;
	if (m_vecteurX > 0)
	{
		m_vecteurX -= 0.5;
	}
	else if (m_vecteurX < 0)
	{
		m_vecteurX += 0.5;
	}
	setHitbox();

	sf::FloatRect zonePied(m_hitbox.left, m_hitbox.top + m_hitbox.height - 5, m_hitbox.width, 5);
	sf::Vector2f diffPos { m_avPos - getPosition() };
	for (Entite* plateforme : *m_ptrGroup->ptrPF)
	{
		sf::FloatRect const hitboxPF{ plateforme->getHitbox() };
		bool axePF{ m_avPos.x + m_hitbox.width > plateforme->getPosition().x && m_avPos.x < plateforme->getPosition().x + hitboxPF.width };
		if (hitboxPF.intersects(zonePied) && (axePF || (abs(plateforme->getPosition().y - (getPosition().y + m_hitbox.height)) <= 10) && m_auSol))
		{
			/*while (hitboxPF.top < getPosition().y + m_hitbox.height - 5)
			{
				move(0, -abs(diffPos.y / 5));
			}*/
			setPosition(getPosition().x, hitboxPF.top - m_hitbox.height + 5);
			m_auSol = true;
			m_tmpSaut = 0;
			m_vecteurY = -3;
		}
		else if (m_hitbox.intersects(hitboxPF))
		{
			if (!axePF)
			{
				setPosition(m_avPos.x, getPosition().y);
			}
			else
			{
				setPosition(getPosition().x, m_avPos.y);
				m_vecteurY = 0;
			}
		}
	}
	if (m_tmpSaut > 0)
	{
		m_auSol = false;
	}
}

Ennemie::Ennemie(EntityLists* drawable, float x, float y)
{
	setImg("Sprites/testBad.png");
	setPosition(x, y);
	m_ptrGroup = drawable;
	m_ptrGroup->ptrEnn->insert(this);
	m_hp = 500;
	m_stat = { 150, 20, 5 };
	m_attaques = { new CacAtt(this, "Sprites/Attaque1.png", {1,1.3}, 0.3, 1.0, 10) };
}

void Ennemie::run()
{
	if (m_aDroite)
	{
		m_vecteurX = 2;
	}
	else
	{
		m_vecteurX = -2;
	}
}

void Ennemie::update()
{
	if (m_hp <= 0)
	{
		m_ptrGroup->ptrAtt->erase(m_attaques[m_numAtt]);
		m_ptrGroup->ptrEnn->erase(this);
	}
	if (m_hitFrames <= 0)
	{
		sf::FloatRect HBAtt {m_attaques[m_numAtt]->getHitbox()};
		sf::FloatRect zoneAtt = sf::FloatRect{ getPosition().x, getPosition().y, HBAtt.width * 3 / 5, m_hitbox.height };
		zoneAtt.left += m_aDroite ? m_hitbox.width : -HBAtt.width * 3 / 5;
		attTrigger(m_attaques[m_numAtt], zoneAtt.intersects(m_ptrGroup->perso->getHitbox()) && m_auSol);
	}
	bool const slashing{ m_attTmp > 0 };
	if (!slashing)
	{
		if (m_hitFrames <= 0)
		{
			if (m_auSol)
			{
				run();
			}
			else
			{
				m_vecteurX = -m_vecteurX;//soluce de la flemme
			}
		}
		else
		{
			m_ptrGroup->ptrAtt->erase(m_attaques[m_numAtt]);
			m_hitFrames -= 1;
		}
	}
	attack();
	physique();
	//bug restant : tombe si se retourne et attaque en meme tmp + se retourne si attaque après être attaqué
	if ((!m_auSol || (m_avPos == getPosition() && !slashing)) && m_hitFrames <= 0 && m_tmpSaut <= 1)// tombe la 2 eme fois au bord
	{
		setPosition(m_avPos);
		m_aDroite = !m_aDroite;
	}
}

const type_info& Ennemie::getType() { return typeid(this); }

//------------------------------------------------------------PJ------------------------------------------------------------

PJ::PJ(EntityLists* drawable)
{
	setPosition(sf::Vector2f(250, 250));
	m_hp = m_stat.maxHP;
	m_ptrGroup = drawable;
	m_dmgRect.setFillColor(sf::Color::Transparent);
	m_dmgRect.setOutlineColor(sf::Color::Red);
	m_dmgRect.setOutlineThickness(15);
	m_dmgRect.setPosition(15, 15);
}

void PJ::run()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_vecteurX = m_stat.vit;
		m_aDroite = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		m_vecteurX = -m_stat.vit;
		m_aDroite = false;
	}
}

void PJ::saut()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		{
			if (m_auSol && !m_sbMaintenue)
			{
				m_vecteurY = 7;
				m_auSol = false;
				m_sbMaintenue = true;
			}
			else if (m_doubleSaut && !m_sbMaintenue)
			{
				m_tmpSaut = 0;
				m_vecteurY = 4;
				m_doubleSaut = false;
				m_sbMaintenue = true;
			}
		}
	}
	else
	{
		m_sbMaintenue = false;
	}
}

JointVar PJ::resetVar()
{
	m_numAtt = 0;
	m_attTmp = -1;
	//m_speAtt = 0;
	m_attChain = 0;
	m_attaques[m_numAtt]->reset();
	return { getPosition(),m_vecteurX,m_vecteurY,m_tmpSaut,m_aDroite };
}

void PJ::recoverVar(JointVar vars)
{
	setPosition(vars.pos);
	m_avPos = getPosition();
	m_vecteurX = vars.vecX;
	m_vecteurY = vars.vecY;
	m_tmpSaut = vars.tmpSaut;
	m_aDroite = vars.aDroite;
	m_doubleSaut = false;
	m_auSol = false;
}

void PJ::update()//--------------------------------------REGLER ANIMATION------------------------------------------------
{
	setTextureRect(sf::IntRect(m_imgCoord.x * 40, int(!m_aDroite) * 120, 40, 120));
	if (m_attTmp == 0 && m_attaques[m_numAtt]==m_attChosen)
	{
		nextAtt();
	}
	if (!m_auSol && !m_sbMaintenue && -0.0045 * (m_tmpSaut * m_tmpSaut) + m_vecteurY > 0 && m_doubleSaut)
	{
		m_vecteurY -= 1;
	}
	if (m_auSol)
	{
		m_doubleSaut = true;
	}
	if (m_attTmp >= 0 && !m_attHold && m_numAtt != m_attaques.size() - 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::L))
	{
		m_attChain = true;
	}
	bool const casting{ m_speTmp > 0 || m_attTmp >= 0 };
	if (m_hitFrames <= 0)//casting avant attack pour pas changer direction a la fin du cast
	{
		//m_speType(this, m_speTmp);
		bool conditionAtt{ (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && !m_attHold) || m_attChain };
		if (m_attTmp < 0)
		{
			if (conditionAtt)//reset attaque chain
			{
				m_attChain = false;
				m_rAttDelay = 0;
			}
			if (m_rAttDelay == 0.3 * 60)//reset attaque combo
			{
				if (m_numAtt != 0)
				{
					m_numAtt = 0;
				}
			}
			else
			{
				m_rAttDelay += 1;
			}
		}
		attTrigger(m_attaques[m_numAtt], conditionAtt);
	}
	if (!casting)
	{
		saut();
		if (m_hitFrames <= 0)
		{
			run();
			m_ptrGroup->hud->erase(&m_dmgRect);
		}
		else
		{
			m_ptrGroup->hud->insert(&m_dmgRect);
			m_ptrGroup->ptrAtt->erase(m_attaques[m_numAtt]);
			m_hitFrames -= 1;
		}
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::L))
	{
		m_attHold = false;
	}
	else
	{
		m_attHold = true;
	}
	attack();
	special();
	physique();
}

void PJ::nextAtt()
{
	m_numAtt = m_attaques.size() - 1 > m_numAtt ? m_numAtt + 1 : 0;
}


P1::P1(EntityLists* drawable) : PJ(drawable)
{
	setImg("Sprites/billy.png");
	m_stat = { 150, 20, 5 };
	m_attaques = { new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.2, 1, 4), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.2, 1, 4), new CacAtt(this, "Sprites/attaque3.png", {0.3,0.55}, 0.3, 2, 10) };
}

void P1::special()
{
	if (m_speTmp > -0.35 * 60)
	{
		if (m_speTmp > 0)
		{
			setVecteurX(getADroite() ? 18 : -18);
		}
		else if (m_speTmp == 0)
		{
			setVecteurX(0);
		}
		m_speTmp -= 1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		m_speTmp = static_cast<int>(0.18 * 60);
	}
}


P2::P2(EntityLists* drawable) : PJ(drawable)
{
	setImg("Sprites/tanky.png");
	m_stat = { 200, 30, 4 };
	//m_speType = &shoryuken;
	m_speAtt = new CacAtt(this, "Sprites/uppercut.png", { 0.3,0.3 }, 0.25, 1, 3);
	m_speAtt->setKB({ 0, 10 });
	m_attaques = { new CacAtt(this, "Sprites/attaque2.png", {0.15,0.25}, 0.25, 1.5, 7), new DistAtt(this, "Sprites/carreau.png", {0.25,0.5}, 2, 1, 5) };
}

void P2::special()
{
	attTrigger(m_speAtt, sf::Keyboard::isKeyPressed(sf::Keyboard::M) && m_speTmp < -0.5 * 60);
	if (m_attTmp == m_timeAtt.before + m_timeAtt.after && sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		setVecteurX(m_aDroite ? 10 : -10);
		m_speTmp = 0;
	}
	if (m_attTmp < 0 && m_speTmp >= -0.5 * 60)
	{
		m_speTmp -= 1;
	}
}



P3::P3(EntityLists* drawable) : PJ(drawable)
{
	setImg("Sprites/slimy.png");
	m_stat = { 50, 5, 5 };
	//m_speType = &bomb;
	m_attaques = { new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5) };
}

void P3::special()
{

}


const type_info& PJ::getType() { return typeid(this); }

int PJ::getHP() { return m_hp; }

void PJ::setVecteurX(int x) { m_vecteurX = x; }



Attaque::Attaque(Unit* joueur, std::string filepath, frameAtt time, float duration, float multiplier, float knockback) : m_time(time), m_duration(duration * 60), m_multiplier(multiplier), m_ptrPerso(joueur)
{
	setImg(filepath);
	setHitbox();
	m_kb[0] = knockback;
	m_kb[1] = 0;
}

void Attaque::spawn()
{
	//setImg(m_imgName);//je ne sais pas pq c'estait nécéssaire
	setTextureRect(sf::IntRect(0, int(!m_ptrPerso->getADroite() * m_img.getSize().y / 2), m_img.getSize().x, int(m_img.getSize().y / 2)));
	m_ptrPerso->getPtrGroup()->ptrAtt->insert(this);
}

void Attaque::reset()
{
	m_ptrPerso->getPtrGroup()->ptrAtt->erase(this);
}

//                                 __________|___|_______
void Attaque::update()// |       | after <-- |   | during <-- | | before
{
	if (m_delay > 0)
	{
		update();
	}
	else if (m_delay == 0)
	{
		reset();
	}
	m_delay -= 1;
}

float Attaque::getDelay() { return m_delay; }

frameAtt Attaque::getTime() { return m_time; }

//void Attaque::startDelay() { m_delay = m_delayStatic; }

void Attaque::setKB(std::array<float, 2> kb) { m_kb = kb; }

const type_info& Attaque::getType() { return typeid(this); }

void CacAtt::spawn()
{
	Attaque::spawn();
	m_delay = m_duration;
}

void CacAtt::reset()
{
	Attaque::reset();
	m_lstHit.clear();
}

void CacAtt::update()
{
	if (getTextureRect().top == 0)
	{
		setPosition(m_ptrPerso->getPosition().x, m_ptrPerso->getPosition().y - 25);
	}
	else
	{
		setPosition(m_ptrPerso->getPosition().x - m_hitbox.width + m_ptrPerso->getHitbox().width, m_ptrPerso->getPosition().y - 25);
	}
	setHitbox();
	if (m_ptrPerso->getType() == typeid(Ennemie*))
	{
		Unit* p{ m_ptrPerso->getPtrGroup()->perso };
		if (m_hitbox.intersects(p->getHitbox()) && !std::any_of(m_lstHit.begin(), m_lstHit.end(), [p](Unit* unit) {return unit == p; }))
		{
			p->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), getTextureRect().top == 0 ? m_kb[0] : -m_kb[0], m_kb[1]);
			m_lstHit.insert(p);
		}
	}
	else if (m_ptrPerso->getType() == typeid(PJ*))
	{
		for (Unit* ennemie : *m_ptrPerso->getPtrGroup()->ptrEnn)
		{
			if (m_hitbox.intersects(ennemie->getHitbox()))
			{
				if (!std::any_of(m_lstHit.begin(), m_lstHit.end(), [ennemie](Unit* unit) {return unit == ennemie; }))
				{
					ennemie->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), getTextureRect().top == 0 ? m_kb[0] : -m_kb[0], m_kb[1]);
					m_lstHit.insert(ennemie);
				}
			}
		}
	}
}

const type_info& CacAtt::getType() { return typeid(this); }

void DistAtt::spawn()
{
	if (m_ptrPerso->getADroite())
	{
		setPosition(m_ptrPerso->getPosition().x, m_ptrPerso->getPosition().y + 27);
	}
	else
	{
		setPosition(m_ptrPerso->getPosition().x - m_hitbox.width + m_ptrPerso->getHitbox().width, m_ptrPerso->getPosition().y + 27);
	}
	Attaque::spawn();
	m_delay = 2 * 60;
}

void DistAtt::update()
{
	if (getTextureRect().top == 0)
	{
		move(10, 0);
	}
	else
	{
		move(-10, 0);
	}
	setHitbox();
	if (m_ptrPerso->getType() == typeid(Ennemie*))
	{
		Unit* p{ m_ptrPerso->getPtrGroup()->perso };
		p->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), getTextureRect().top == 0 ? m_kb[0] : -m_kb[0]);
		reset();
	}
	else if (m_ptrPerso->getType() == typeid(PJ*))
	{
		for (Unit* ennemie : *m_ptrPerso->getPtrGroup()->ptrEnn)
		{
			if (m_hitbox.intersects(ennemie->getHitbox()))
			{
				ennemie->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), getTextureRect().top == 0 ? m_kb[0] : -m_kb[0]);
				reset();
				break;
			}
		}
	}
}

const type_info& DistAtt::getType() { return typeid(this); }


frameAtt::frameAtt(float b, float a)
{
	before = static_cast<int>(b * 60);
	after = static_cast<int>(a * 60);
}

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

void Unit::hit(int const dmg, double push, double kbY = 0)
{
	m_hp -= dmg;
	m_vecteurX = push;
	if (kbY == 0) { m_vecteurY = kbY; }
	m_hitFrames = static_cast<int>(abs(push) > abs(kbY) ? abs(push) : abs(kbY) * 2);
	m_ptrGroup->ptrAtt->erase(m_attaque[m_numAtt]);
	//m_attaque[m_numAtt]->setDelay(0);
	m_attTmp = 0;
}

void Unit::attTrigger(Attaque* att, int& tmp, bool condition)
{
	if (m_imgCoord.x != 0) { m_imgCoord.x = 0; }
	if (tmp < 0)//semble fonctionner plus de test pour etre sur
	{
		if (condition)
		{
			//std::cout<<m_attaque[m_numAtt]->getDelay()<<std::endl;
			m_timeAtt = att->getTime();
			tmp = m_timeAtt.before + m_timeAtt.after;
			//m_attChain = false;
			//m_attHold = true;
		}
	}
	else
	{
		attack(att, tmp);
	}
}

void Unit::attack(Attaque* att, int& tmp)
{
	if (tmp == m_timeAtt.after)
	{
		att->spawn();
	}
	else if (tmp > m_timeAtt.after)
	{
		if (m_imgCoord.x != 1) { m_imgCoord.x = 1; }
	}
	tmp -= 1;
}

EntityLists* Unit::getPtrGroup() { return m_ptrGroup; }

frameAtt Unit::getTime() { return m_timeAtt; }

bool Unit::getADroite() { return m_aDroite; }

void Unit::setADroite(bool d) { m_aDroite = d; }

Stats Unit::getStat() { return m_stat; }

int Unit::getNumAtt() { return m_numAtt; }

const type_info& Unit::getType() { return typeid(this); }

void Unit::nextAtt()
{
	m_numAtt = m_stat.AS.size() - 1 > m_numAtt ? m_numAtt + 1 : 0;
}

void SolUnit::physique()
{
	m_avPos = getPosition();

	double posY{ -0.0045 * (m_tmpSaut * m_tmpSaut) + m_vecteurY };
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
			while (hitboxPF.top < getPosition().y + m_hitbox.height - 5)
			{
				move(0, -abs(diffPos.y / 5));
			}
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
	m_stat = { 150, 20, 5, std::vector<double>{ 2 } };
	m_attaque = { new CacAtt(this, "Sprites/Attaque1.png", {0.1,0.3}, 0.3, 1.0, 10) };
	for (auto& e : m_stat.AS) {
		e *= 60;
	}
}

/*
void Ennemie::attTrigger()
{
	if (m_attaque[m_numAtt]->getDelay() <= 0)
	{
		//m_attaque[m_numAtt]->setHitbox();
		sf::FloatRect HB {m_attaque[m_numAtt]->getHitbox()};
		sf::FloatRect zoneAtt = m_aDroite ? sf::FloatRect{getPosition().x, getPosition().y + m_hitbox.height - HB.height, HB.width, HB.height} : sf::FloatRect{ getPosition().x - HB.width + m_hitbox.width, getPosition().y + m_hitbox.height - HB.height,HB.width, HB.height };
		if (m_rAttDelay >= m_stat.AS[m_numAtt])
		{
			if (zoneAtt.intersects(m_ptrGroup->perso->getHitbox()))
			{
				m_timeAtt = m_attaque[m_numAtt]->getTime();
				m_attTmp = m_timeAtt.before + m_timeAtt.after;
				//m_attTmp = m_attaque[m_numAtt]->getTime().before;
				//m_attaque[m_numAtt]->setDelay(m_attaque[m_numAtt]->getDelayStatic());
			}
		}
	}
	if (m_attTmp == m_timeAtt.after)
	{
		m_attaque[m_numAtt]->spawn();
	}
	else if (m_attTmp == 0)
	{
		nextAtt();
	}
	else if (m_attTmp < -0.3 * 60)
	{
		if (m_numAtt != 0) { m_numAtt = 0; }
	}
	m_attTmp -= 1;
}
*/

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
		m_ptrGroup->ptrAtt->erase(m_attaque[m_numAtt]);
		m_ptrGroup->ptrEnn->erase(this);
	}
	//m_attaque[m_numAtt]->setHitbox();
	/*if (m_hitFrames <= 0)
	{
		sf::FloatRect HBAtt {m_attaque[m_numAtt]->getHitbox()};
		sf::FloatRect zoneAtt = m_aDroite ? sf::FloatRect{getPosition().x, getPosition().y + m_hitbox.height - HBAtt.height, HBAtt.width, HBAtt.height} : sf::FloatRect{ getPosition().x - HBAtt.width + m_hitbox.width, getPosition().y + m_hitbox.height - HBAtt.height,HBAtt.width, HBAtt.height };
		bool conditionAtt{ zoneAtt.intersects(m_ptrGroup->perso->getHitbox()) };
		attTrigger(m_attaque[m_numAtt], m_attTmp, conditionAtt);
	}*/
	bool const slashing{ m_attTmp > 0 };
	if (!slashing)
	{
		if (m_hitFrames <= 0)
		{
			run();
		}
		else
		{
			m_ptrGroup->ptrAtt->erase(m_attaque[m_numAtt]);
			m_hitFrames -= 1;
		}
	}
	physique();
	//bug restant : tombe si se retourne et attaque en meme tmp + se retourne si attaque après être attaqué
	if ((!m_auSol || (m_avPos.x == getPosition().x && !slashing)) && m_hitFrames <= 0 && m_tmpSaut <= 1)
	{
		setPosition(m_avPos);
		m_aDroite = !m_aDroite;
	}
}

const type_info& Ennemie::getType() { return typeid(this); }

//------------------------------------------------------------PJ------------------------------------------------------------

PJ::PJ(EntityLists* drawable, int p)
{
	switch (p)
	{
	case (1):
		setImg("Sprites/billy.png");
		m_stat = { 150, 20, 5, {0.5,0.3,0.3 } };
		m_speType = &dash;
		m_attaque = { new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.2, 1, 3), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.2, 1.2, 3), new CacAtt(this, "Sprites/attaque3.png", {0.3,0.55}, 0.3, 2, 10) };
		break;
	case (2):
		setImg("Sprites/tanky.png");
		m_stat = { 200, 30, 4, {0.5,0.2} };
		m_speType = &shoryuken;
		m_speAtt = new CacAtt(this, "Sprites/uppercut.png", { 0.1,0.3 }, 0.3, 1, 3);
		m_speAtt->setKB({ 2,12 });
		m_attaque = { new CacAtt(this, "Sprites/attaque2.png", {0.15,0.25}, 0.25, 1.5, 7), new DistAtt(this, "Sprites/carreau.png", {0.25,0.5}, 2, 1, 5) };
		break;
	case (3):
		setImg("Sprites/slimy.png");
		m_stat = { 50, 5, 5, {0.3,0.3,0.3} };
		m_speType = &bomb;
		m_attaque = { new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5) };
		break;
	default:
		setImg("Sprites/billy.png");
		m_stat = { 5000, 5, 5, {0.2,0.7} };
		m_speType = &bomb;
		m_attaque = { new CacAtt(this,  "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5), new CacAtt(this, "Sprites/attaque1.png", {0.1,0.3}, 0.3, 1.2, 5) };
	}
	for (auto& e : m_stat.AS) {
		e *= 60;
	}
	setPosition(sf::Vector2f(250, 250));
	m_hp = m_stat.maxHP;
	m_ptrGroup = drawable;
	m_dmgRect.setFillColor(sf::Color::Transparent);
	m_dmgRect.setOutlineColor(sf::Color::Red);
	m_dmgRect.setOutlineThickness(15);
	m_dmgRect.setPosition(15, 15);
}

/*
void PJ::attTrigger()
{
	
	if (m_imgCoord.x != 0) { m_imgCoord.x = 0; }
	if (m_attTmp < 0)//semble fonctionner plus de test pour etre sur
	{
		if (m_attTmp == -0.3 * 60)
		{
			m_numAtt = 0;
		}
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::L) && !m_attHold) || m_attChain)
		{
			//std::cout<<m_attaque[m_numAtt]->getDelay()<<std::endl;
			m_timeAtt = m_attaque[m_numAtt]->getTime();
			m_attTmp = m_timeAtt.before + m_timeAtt.after;
			m_attChain = false;
			m_attHold = true;
		}
	}
	else
	{
		attack(m_attaque[m_numAtt], m_attTmp);
	}
	if (m_attTmp == 0 && !std::any_of(m_ptrGroup->ptrAtt->begin(), m_ptrGroup->ptrAtt->end(), [this](Attaque* att) -> bool {return att == m_attaque[m_numAtt]; }))
	{
		m_vecteurY = (-0.0045 * (m_tmpSaut * m_tmpSaut) + m_vecteurY > 0) ? 1 : -1;
		m_tmpSaut = 0;
		m_attaque[m_numAtt]->spawn();
		m_attTmp -= 1;
	}

	else if (m_attTmp > 0)
	{
		m_attTmp -= 1;
	}
}
*/

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
	m_attTmp = 0;
	m_attChain = 0;
	m_attaque[m_numAtt]->reset();
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

void PJ::update()
{
	setTextureRect(sf::IntRect(m_imgCoord.x * 40, int(!m_aDroite) * 120, 40, 120));
	if (!m_auSol && !m_sbMaintenue && -0.0045 * (m_tmpSaut * m_tmpSaut) + m_vecteurY > 0 && m_doubleSaut)
	{
		m_vecteurY -= 1;
	}
	if (m_auSol)
	{
		m_doubleSaut = true;
	}
	if (m_attTmp > 0 && !m_attHold && m_numAtt != m_attaque.size() - 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::L))
	{
		m_attChain = true;
	}
	bool const casting{ m_speTmp > 0 || m_attTmp > 0 };
	if (m_hitFrames <= 0)//casting avant attack pour pas changer direction a la fin du cast
	{
		m_speType(this, m_speTmp);
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
		attTrigger(m_attaque[m_numAtt], m_attTmp, conditionAtt);
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
			m_ptrGroup->ptrAtt->erase(m_attaque[m_numAtt]);
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
	physique();
}

const type_info& PJ::getType() { return typeid(this); }

int PJ::getHP() { return m_hp; }

void PJ::setVecteurX(int x) { m_vecteurX = x; }

CacAtt* PJ::getSpeAtt() { return m_speAtt; }

void dash(PJ* player, int& tmp)
{
	if (tmp > -0.35 * 60)
	{
		if (tmp > 0)
		{
			player->setVecteurX(player->getADroite() ? 18 : -18);
		}
		else if (tmp == 0)
		{
			player->setVecteurX(0);
		}
		tmp -= 1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		tmp = 0.18 * 60;
	}
}

void shoryuken(PJ* player, int& tmp)
{
	if (tmp < 0)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
		{
		}
	}
	else
	{
		std::cout << tmp << std::endl;
		player->setVecteurX(player->getADroite() ? 4 : -4);
	}
	player->attTrigger(player->getSpeAtt(), tmp, sf::Keyboard::isKeyPressed(sf::Keyboard::M));
}

void bomb(PJ* player, int& tmp)
{

}


Attaque::Attaque(Unit* joueur, std::string filepath, frameAtt time, double duration, double multiplier, double knockback) : m_time(time), m_duration(duration * 60), m_multiplier(multiplier), m_ptrPerso(joueur)
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

double Attaque::getDelay() { return m_delay; }

frameAtt Attaque::getTime() { return m_time; }

//void Attaque::startDelay() { m_delay = m_delayStatic; }

void Attaque::setKB(std::array<double, 2> kb) { m_kb = kb; }

const type_info& Attaque::getType() { return typeid(this); }

void CacAtt::spawn()
{
	Attaque::spawn();
	m_delay = m_duration;
}

void CacAtt::reset()
{
	Attaque::reset();
	m_ptrPerso->nextAtt();
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
	m_ptrPerso->nextAtt();
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
	before = b * 60;
	after = a * 60;
}

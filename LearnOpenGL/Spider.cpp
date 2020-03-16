#include "Spider.h"

#define M_PI 3.1415926535897932384626433832795

Spider::Spider()
{
}

Spider::~Spider()
{
}

inline glm::vec3 easyLerp(glm::vec3 start, glm::vec3 end, float factor)
{	//przejscie z wektora do wektora zaleznie od wspolczynnika
	return glm::normalize(start + ((end - start) * factor));
}

inline float angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 origin)
{	//kat w radianach miedzy punktami o okreslonym poczatku
	glm::vec3 da = glm::normalize(a - origin);
	glm::vec3 db = glm::normalize(b - origin);
	return glm::acos(abs(glm::dot(da, db)) - (int)abs(glm::dot(da, db)));
}

inline glm::vec3 rotator(glm::vec3 vect1, glm::vec3 vect2, float factor, float deltaTime)
{	//obracam postac o rowny kat pomiedzy dwoma wektorami

	if (abs(vect1.x + vect2.x + vect1.z + vect2.z + vect1.y + vect2.y) < 0.01f)
	{	//przypadek brzegowy, jak obrocic postac gdy kierunek docelowy jest idealnie na przeciw kierunku aktualnego
		vect2 = glm::vec3(0.01f + vect2.x, vect2.y, vect2.z - 0.01f);
	}

	glm::vec3 vector1 = glm::normalize(glm::vec3(vect1.x, vect1.y, vect1.z));
	glm::vec3 vector2 = glm::normalize(glm::vec3(vect2.x, vect2.y, vect2.z));
	float datangle = angleBetween(vector1, vector2, glm::vec3(0.0f, 0.0f, 0.0f)) / M_PI;
	glm::vec3 result = easyLerp(vector2, vector1, factor * (1 - datangle) * deltaTime);
	if (datangle < 0.005f)
		result = vector1;

	return result;
}

glm::mat4 Spider::getModel(glm::vec3 pos, glm::vec3 targVec, glm::vec3 scale, float deltaTime)
{
	if (pos.y < -0.1f)	//jezeli pajak jest pod woda to nie zyje
	{
		isDead = true;
	}

	if (!isRunning && actVel > 0.0f)
	{	//spowalniam pajka i ustawiam mu animacje niechodzenia
		mainModel->setCurrentAnimation(0);
		actVel -= accel * deltaTime;
		if (actVel < 0.0f)
			actVel = 0.0f;
	}
	//przypisanie danych do obiektu
	forward = glm::normalize(glm::vec3(targVec.x, 0.0f, targVec.z));
	mainScale = scale;
	//

	glm::mat4 model(1.0f);

	glm::vec3 normal;
	//obliczam wysokosc na terenie oraz odchylenie na krzywej nawierzchni (normal jako wskaŸnik)
	float h = mainMap->getHeight(glm::vec2(pos.x, pos.z), normal);

	if (normal.z == 0.0f)
	{	//naprawianie problemow glupimi sposobami, ktore jednak dzialaja. Jezeli normal.z == 0 to pajak robil sie plaski. 
		//Wiec wystarczy doprowadzic do stanu kiedy normal.z nie bedzie == 0 w zadnym przypadku
		normal.z = 0.01f;
		normal = glm::normalize(normal);
	}

	//miedzy kolejnymi zmianami obrotu wzgledem podloza dodaje stany posrednie by wygladalo to dosyc gladko
	actNormal = rotator(normal, actNormal, 15.0f, deltaTime);
	normal = actNormal;

	//przypisanie danych do obiektu
	position = glm::vec3(pos.x, h, pos.z);
	//

	//przypisanie danych do obiektu, gora postaci jest skierowana tam gdzie wektor normalny plaszczyzny, a prawy wektor obliczam
	upward = normal;
	right = glm::normalize(glm::cross(upward, forward));

	//wektor wskazujacy dokladnie w gore jest nachylony w bok by udawac, ze gora jest nieznacznie przechylona wzgledem powierzchni
	//chodzi o to by pajak zawsze byl skierowaqny wzdluz osi X
	glm::vec3 up(normal.x, normal.y, 0.0f);
	up = glm::normalize(up);

	//przesuwam postac do pozycji
	model = glm::translate(model, glm::vec3(pos.x, h, pos.z));

	//obliczam wektor prawy, zawsze ma byc skierowany w jednym kierunku niezaleznie od nachylenia wiêc wyciagam absolutn¹ wartoœæ z Z
	glm::vec3 objRight = -glm::normalize(glm::cross(up, glm::normalize(glm::vec3(normal.x, normal.y, abs(normal.z)))));
	//jezeli akurat wektor up oraz normal nachodza na siebie to nie mozna obliczyc wektora prawego wiec nadaje mu wtedy standardowa wartosc
	if (up == normal || normal.z == 0.0f)
		objRight = glm::vec3(-1.0f, 0.0f, 0.0f);

	//obliczam wektor w gore
	glm::vec3 objUp = glm::normalize(glm::cross(normal, objRight));

	//macierz obrotu
	glm::mat4 objLookAt(
		objRight.x, objRight.y, objRight.z, 0.0f,
		normal.x, normal.y, normal.z, 0.0f,
		objUp.x, objUp.y, objUp.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	//druga miacierz obrotu wokol wlasnej osi (zeruje y, poniewaz pajak nie moze patrzec w gore)
	glm::mat4 rotTemp = spiderLookAt(forward, glm::vec3(0.0f, 1.0f, 0.0f));

	model = model * objLookAt * rotTemp;

	//skalowanie
	if (!isDead)
		model = glm::scale(model, scale);
	else
		model = glm::scale(model, glm::vec3(scale.x, scale.y * 0.1f, scale.z));	//jezeli pajak jest martwy to jest plaski

	return model;
}

glm::mat4 Spider::spiderLookAt(glm::vec3 targetVec, glm::vec3 WorldUpVec)
{	//prywatny lookAt
	targetVec = glm::normalize(targetVec);
	glm::vec3 rightVec = glm::normalize(glm::cross(glm::normalize(WorldUpVec), targetVec));	//wektor prawy kamery
	glm::vec3 upVec = glm::cross(targetVec, rightVec);										//wektor gory kamery
	glm::mat4 lookAt = glm::mat4(1.0f);														//macierz poszczegolnych obrotow
	lookAt[0][0] = rightVec.x;	//pierwsza kolumna, pierwszy rzad
	lookAt[1][0] = rightVec.y;	//druga kolumna, pierwszy rzad
	lookAt[2][0] = rightVec.z;	//trzecia kolumna, pierwszy rzad
	lookAt[0][1] = upVec.x;		//pierwsza kolumna, drugi rzad
	lookAt[1][1] = upVec.y;
	lookAt[2][1] = upVec.z;
	lookAt[0][2] = targetVec.x;	//pierwsza kolumna trzeci rzad
	lookAt[1][2] = targetVec.y;
	lookAt[2][2] = targetVec.z;

	return lookAt;
}

void Spider::move(int dir, float deltaTime, glm::vec3 camForw, glm::vec3 camRight)
{
	//0 - przod, 1 - prawo, 2 - tyl, 3 - lewo, 10 - prawy przod, 30 - lewy przod, 12 - prawy tyl, 32 - lewy tyl 
	mainModel->setCurrentAnimation(1);
	isRunning = true;

	float localActVel = actVel;

	localActVel += accel * deltaTime;
	if (localActVel > maxVelocity)
		localActVel = maxVelocity;

	actVel = localActVel;

	glm::vec3 result(0.0f);
	glm::vec3 direction(0.0f);

	if (dir == 0)
		direction = camForw;

	if (dir == 2)
		direction = -camForw;

	if (dir == 1)
		direction = camRight;

	if (dir == 3)
		direction = -camRight;

	if (dir == 10)
		direction = glm::normalize(camForw + camRight);

	if (dir == 30)
		direction = glm::normalize(camForw - camRight);

	if (dir == 12)
		direction = glm::normalize(-camForw + camRight);

	if (dir == 32)
		direction = glm::normalize(-camForw - camRight);

	forward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
	direction = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));

	result = rotator(direction, forward, 5.0f, deltaTime);

	getModel(position + result * actVel * deltaTime, result, mainScale, deltaTime);
}
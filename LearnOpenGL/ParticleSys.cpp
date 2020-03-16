#include "ParticleSys.h"

ParticleSys::ParticleSys(float delay, glm::vec3 pos, float mSize, float mxSize, glm::vec3 vel, float err, glm::vec3 grav, float rot, float maxLife, Shader* sh, int tTil, int nrTil)
{
	spawnDelay = delay;
	position = pos;
	minSize = mSize;
	maxSize = mxSize;
	velocity = vel;
	error = err;
	gravity = grav;
	rotation = rot;
	maxLifeTime = maxLife;
	shader = sh;
	texTiles = tTil;
	nrTiles = nrTil;
}

ParticleSys::~ParticleSys()
{
	glDeleteVertexArrays(1, &VAO);
}

void ParticleSys::singleUse()
{
	glm::vec3 tmpVel = glm::vec3(((float)rand() / ((float)RAND_MAX / 2) - 1.0f) * error + velocity.x, ((float)rand() / ((float)RAND_MAX / 2) - 1.0f) * error + velocity.y, ((float)rand() / ((float)RAND_MAX / 2) - 1.0f) * error + velocity.z);
	particles.push_back(new Particle(position, minSize + (maxSize - minSize) * ((float)rand() / (float)RAND_MAX), tmpVel, gravity, rotation + ((float)rand() / ((float)RAND_MAX / 2) - 1), maxLifeTime));
}

void ParticleSys::Update(float deltaTime)
{
	actTime += deltaTime;
	if (actTime > spawnDelay)
	{	//spawnuj co pewien okres czasu
		actTime = 0.0f;
		if (isActive)
		{	//jezeli jest aktywny to uzywaj
			singleUse();
		}
	}

	int tmpSize = particles.size();

	for (int i = 0; i < tmpSize; i++)
	{	//jezeli particle zbyt dlugo zyja
		if (particles[i]->lifeTime > particles[i]->maxLifeTime)
		{	//sprawdzam lifeTime kazdego particle - jezeli jest zbyt dlugi to go usuwam i juz nie aktualizuje
			Particle* part = particles[i];
			particles.erase(particles.begin() + i);
			tmpSize--;
			i--;
			delete part;
			continue;
		}

		particles[i]->Update(deltaTime);	//update particlow
	}
}

void ParticleSys::init(const char* image_path)
{
	unsigned int VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	for (int i = 0; i < 4; i++)
	{
		vertices[5 * i + 3] = vertices[5 * i + 3] / (float)texTiles;
		vertices[5 * i + 4] = vertices[5 * i + 4] / (float)texTiles;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int tmpTex;

	if (image_path != nullptr)
	{
		glGenTextures(1, &tmpTex);
		glBindTexture(GL_TEXTURE_2D, tmpTex);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.	SPRAWDZ ZAWSZE CZY OBRACAC CZY NIEEEEEE
		// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		unsigned char* data = stbi_load(image_path, &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format;	//system sam okresla sobie format danych
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
			stbi_image_free(data);
			return;
		}

		texture = tmpTex;
	}

	shader->setInt("texture1", 0);
}

inline void insertionSort(std::vector<Particle*>& data2, glm::vec3 camPos)
{
	std::vector<float> data;
	int i, j;
	float tmp;
	Particle* tmp2;

	for (i = 0; i < data2.size(); i++)
	{
		data.push_back(glm::length(data2[i]->position - camPos));
	}

	for (i = 1; i < data.size(); i++)
	{
		j = i;
		tmp = data[i];
		tmp2 = data2[i];
		while (j > 0 && tmp > data[j - 1])
		{
			data[j] = data[j - 1];
			data2[j] = data2[j - 1];
			j--;
		}
		data[j] = tmp;
		data2[j] = tmp2;
	}
}

inline glm::mat4 transposeViewToModel(glm::mat4 view, glm::mat4 model)
{
	model[0][0] = view[0][0];
	model[0][1] = view[1][0];
	model[0][2] = view[2][0];
	model[1][0] = view[0][1];
	model[1][1] = view[1][1];
	model[1][2] = view[2][1];
	model[2][0] = view[0][2];
	model[2][1] = view[1][2];
	model[2][2] = view[2][2];
	return model;
}

void ParticleSys::Draw(glm::mat4 V, glm::mat4 P, glm::vec3 camPos)
{
	glEnable(GL_BLEND);	//wlaczam blendowanie sie poszczegolnych particlow
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//zale¿ne od alphy
	glDepthMask(false);									//wylaczam maske obliczania g³êbi

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	insertionSort(particles, camPos);	//sortuje particle zaleznie od odleglosci od kamery dzieki czemu pierw renderuje dalsze, a potem te blizsze

	shader->use();	//uzywam tego samego shadera do wszystkich particli

	for (int i = 0; i < particles.size(); i++)
	{
		glm::mat4 model(1.0f);
		glm::vec2 texTile0(0.0f);	//kafelka 0
		glm::vec2 texTile1(0.0f);	//kafelka, do której przejdzie texTile0
		float factor = 0.0f;		//% przejscia miedzy kafelkami

		//obliczanie kafelek - wraz z uplywem czasu zycia particla po kolei zmieniaja sie klatki

		float lifeFactor = particles[i]->lifeTime / particles[i]->maxLifeTime;

		if (lifeFactor >= 1.0f)
			lifeFactor = 0.99f;

		lifeFactor = lifeFactor * nrTiles;

		texTile0.x = (float)((int)lifeFactor % texTiles) / texTiles;
		texTile0.y = 1.0f - (float)((int)(lifeFactor / texTiles)) / texTiles - (float)(1.0f / texTiles);
		factor = lifeFactor - (int)lifeFactor;
		texTile1.x = (texTile0.x + (1.0f / texTiles));
		if (texTile1.x >= 1.0f)
		{
			texTile1.x -= 1.0f;
			texTile1.y = texTile0.y - (1.0f / texTiles);
		}
		else
		{
			texTile1.y = texTile0.y;
		}

		//koniec

		model = glm::translate(model, particles[i]->position);	//przejscie do pozycji
		model = transposeViewToModel(V, model);					//stworzenie macierzy transponowanej w czêœci 3x3 czyli tam gdzie jest rotacja
																//transponuje czesc z View i wklejam do modelu
																//dzieki temu particle jest zawsze skierowany w strone widza
		model = glm::rotate(model, particles[i]->actRotation, glm::vec3(0.0f, 0.0f, 1.0f));	//obracam particle wzgledem wlasnej osi Z

		model = glm::scale(model, particles[i]->size * glm::vec3(1.0f));

		shader->setMat4("projection", P);
		shader->setMat4("view", V);
		shader->setMat4("model", model);

		shader->setVec2("texTile0", texTile0);
		shader->setVec2("texTile1", texTile1);
		shader->setFloat("factor", factor);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	//rysuje
		glBindVertexArray(0);
	}

	//wylaczam poprzednio wlaczone opcje
	glDisable(GL_BLEND);
	glDepthMask(true);
}
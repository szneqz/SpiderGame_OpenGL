#include "water.h"

water::water(Shader* shad, const char* image_path, const char* image_path2, float texSiz, float center, float velocity)
{
	shader = shad;
	texSize = texSiz;
	centerLoc = center;
	waterVelocity = velocity;

	init(image_path, image_path2);
}

water::~water()
{
	glDeleteVertexArrays(1, &VAO);
}

void water::init(const char* image_path, const char* image_path2)
{	//wczytywanie tekstur
	unsigned int VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	for (int i = 0; i < 4; i++)
	{	//ustalam wielkosci powatrzajacych sie kafelkow wody
		vertices[8 * i + 6] = vertices[8 * i + 6] * texSize;
		vertices[8 * i + 7] = vertices[8 * i + 7] * texSize;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

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

	shader->use();
	shader->setInt("texture_diffuse1", 0);

	unsigned int tmpTex2;

	if (image_path2 != nullptr)
	{
		glGenTextures(1, &tmpTex2);
		glBindTexture(GL_TEXTURE_2D, tmpTex2);
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
		unsigned char* data = stbi_load(image_path2, &width, &height, &nrChannels, 0);
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

		texture2 = tmpTex2;
	}

	shader->setInt("texture_specular1", 1);
}

void water::Draw(glm::mat4 V, glm::mat4 P, float deltaTime)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	shader->use();

	glm::mat4 model(1.0f);
	actTime += deltaTime * waterVelocity;
	float factor = (1000.0f / texSize) / 1.41f;	//zmienna pomocnicza do okreslania predosci wody

	//woda ma jezdzic po okregu
	model = glm::translate(model, glm::vec3(centerLoc + sin(actTime) * factor, -0.1f, centerLoc + cos(actTime) * factor));
	//woda ma byc plaska do terenu
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//woda ma byc wielka
	model = glm::scale(model, glm::vec3(1000.0f));

	shader->setMat4("projection", P);
	shader->setMat4("view", V);
	shader->setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	//rysuje
	glBindVertexArray(0);
}
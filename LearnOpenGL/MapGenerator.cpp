#include "MapGenerator.h"

	MapGenerator::MapGenerator()
	{

	}
	
	MapGenerator::MapGenerator(int tsize, float factor, float maxHeight, float minStep, float maxStep, int nrHills)
	{
		size = tsize;

		for (int i = 0; i < size * size; i++)
		{
			heights.push_back(0.0f);
		}

		for (int i = 0; i < nrHills; i++)
		{
			int point = rand() % (size * size);
			int step = minStep + ((float)rand() / (float)RAND_MAX) * (maxStep - minStep);
			heights[point] = ((((float)rand() / (float)RAND_MAX) * maxHeight) + heights[point]) / 2;	//szczyt
			AroundHill(point, step, heights[point]);
		}

		Border();
		calcStraightNormals();
		calcVertexData(factor);
		init();
	}

	MapGenerator::~MapGenerator()
	{

	}

	void MapGenerator::calcStraightNormals()
	{	//obliczam pomocnicze tablice z wektorami normalnymi stalymi, a nastepnie sumuje je ze soba w wierzcholkach by shader mogl je interpolowac
		//wektory te sa okreslane dla danej sciany wiec jest ich (size-1)(size-1)*2
		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < size; j++)
			{	//zeby troche lepiej sie liczylo to licze do size, a w wartosci (size - 1) przeskakuje o krok
				if ((j + 1) % size == 0)
					continue;
				//wektory dla dolnego trojkata
				glm::vec3 a = glm::vec3(j, heights[(i + 1) * size + j] , i + 1);
				glm::vec3 b = glm::vec3(j + 1, heights[(i + 1) * size + (j + 1)], i + 1);
				glm::vec3 c = glm::vec3(j + 1, heights[i * size + (j + 1)], i);
				glm::vec3 normal = calcNormals(a, b, c);
				normals.push_back(normal);
				
				//wektory dla gornego trojkata
				a = glm::vec3(j + 1, heights[i * size + (j + 1)], i);
				b = glm::vec3(j, heights[i * size + j], i);
				c = glm::vec3(j, heights[(i + 1) * size + j], i + 1);
				normal = calcNormals(a, b, c);
				normals.push_back(normal);
			}
		}

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				glm::vec3 normal(0.0f);
				glm::vec3 a(0.0f);
				glm::vec3 b(0.0f);
				glm::vec3 c(0.0f);
				glm::vec3 d(0.0f);

				//tutaj sumuje wektory, tkore naleza do danego punktu
				//zeby nie wychodzic za zakres wektora sprawdzam wartosci
				if (i > 0 && j > 0)
				a = normals[(i - 1) * (size - 1) * 2 + (j - 1) * 2];
				if (i < size - 1 && j < size - 1)
				b = normals[(i * (size - 1) * 2 + j * 2) + 1];
				if (i < size - 1 && j > 0)
				c = glm::normalize(normals[(i * (size - 1) * 2 + (j - 1) * 2) + 1] + normals[i * (size - 1) * 2 + (j - 1) * 2]);
				if (i > 0 && j < size - 1)
				d = glm::normalize(normals[((i - 1) * (size - 1) * 2 + j * 2) + 1] + normals[(i - 1) * (size - 1) * 2 + j * 2]);

				normal = a + b + c + d;

				interNormals.push_back(normal);
			}
		}
	}

	void MapGenerator::AroundHill(int point, float step, float actHeight)
	{
		int pointx = point % size;
		int pointy = point / size;

		for (int i = 0; i < size * size; i++)
		{
			int x = i % size;
			int y = i / size;

			float distance = sqrt((x - pointx) * (x - pointx) + (y - pointy) * (y - pointy)); //a^2 + b^2 = c^2

			float tempHeight = 0;
			if(distance > 0)													//logarytm do wartosci <= 0 to niezbyt dobry pomysl
				tempHeight = actHeight - log10(distance) * distance * step;		//logarytm to ladnie lagodzi


			if (tempHeight < 0)
				continue;

			heights[i] = (heights[i] + tempHeight) / 2;	//srednia z aktualnej wysokosci i obliczonej
		}
	}

	void MapGenerator::Border()	//zewnetrzna granica, moze dodam jakies lagodne zjazdy
	{
		for (int i = 0; i < size * size; i++)
		{
			if (i % size == 0 || (i + 1) % size == 0 || i < size || i >= (size * size - size))
				heights[i] = -1;
		}
	}

	void MapGenerator::calcVertexData(float factor)
	{
		for (int i = 0; i < (size) * (size - 1); i++)
		{
			if ((i + 1) % size == 0)	//przy prawej krawedzi kontunyuuj, chodzi o to by dobrze sie liczylo
				continue;
			//lewy-dol
			vertexData.push_back(i % size);
			vertexData.push_back(heights[size + i]);
			vertexData.push_back((i + size) / size);
			vertexData.push_back(interNormals[size + i].x);
			vertexData.push_back(interNormals[size + i].y);
			vertexData.push_back(interNormals[size + i].z);
			vertexData.push_back(factor * vertexData[vertexData.size() - 6] / size);
			vertexData.push_back(factor * vertexData[vertexData.size() - 5] / size);

			//prawy-dol
			vertexData.push_back(vertexData[vertexData.size() - 8] + 1);
			vertexData.push_back(heights[size + i + 1]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(interNormals[size + i + 1].x);
			vertexData.push_back(interNormals[size + i + 1].y);
			vertexData.push_back(interNormals[size + i + 1].z);
			vertexData.push_back(factor * vertexData[vertexData.size() - 6] / size);
			vertexData.push_back(factor * vertexData[vertexData.size() - 5] / size);

			//prawa-gora
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(heights[i + 1]);
			vertexData.push_back(vertexData[vertexData.size() - 8] - 1);
			vertexData.push_back(interNormals[i + 1].x);
			vertexData.push_back(interNormals[i + 1].y);
			vertexData.push_back(interNormals[i + 1].z);
			vertexData.push_back(factor * vertexData[vertexData.size() - 6] / size);
			vertexData.push_back(factor * vertexData[vertexData.size() - 5] / size);

			//prawa-gora
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			//lewa-gora
			vertexData.push_back(i % size);
			vertexData.push_back(heights[i]);
			vertexData.push_back(vertexData[vertexData.size() - 8]);
			vertexData.push_back(interNormals[i].x);
			vertexData.push_back(interNormals[i].y);
			vertexData.push_back(interNormals[i].z);
			vertexData.push_back(factor * vertexData[vertexData.size() - 6] / size);
			vertexData.push_back(factor * vertexData[vertexData.size() - 5] / size);

			//lewy-dol
			vertexData.push_back(vertexData[vertexData.size() - 40]);
			vertexData.push_back(vertexData[vertexData.size() - 40]);
			vertexData.push_back(vertexData[vertexData.size() - 40]);
			vertexData.push_back(vertexData[vertexData.size() - 40]);
			vertexData.push_back(vertexData[vertexData.size() - 40]);
			vertexData.push_back(vertexData[vertexData.size() - 40]);
			vertexData.push_back(vertexData[vertexData.size() - 40]);
			vertexData.push_back(vertexData[vertexData.size() - 40]);
		}
	}

	glm::vec3 MapGenerator::calcNormals(glm::vec3 vect1, glm::vec3 vect2, glm::vec3 vect3)
	{	//algorytm obliczania wektorow normalnych
		glm::vec3 normal;
		glm::vec3 U = vect2 - vect1;
		glm::vec3 V = vect3 - vect1;

		normal.x = (U.y * V.z) - (U.z * V.y);
		normal.y = (U.z * V.x) - (U.x * V.z);
		normal.z = (U.x * V.y) - (U.y * V.x);

		normal = glm::normalize(normal);

		if (normal.y < 0)
			normal = -normal;	//zawsze skierowany w gore

		return normal;
	}

	void MapGenerator::init()
	{
		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	void MapGenerator::loadImageToTexture(const char* image_path_diff, const char* image_path_spec)
	{
		unsigned int texture1, texture2;

		if (image_path_diff != nullptr)
		{
			glGenTextures(1, &texture1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load image, create texture and generate mipmaps
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.	SPRAWDZ ZAWSZE CZY OBRACAC CZY NIEEEEEE
			// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
			unsigned char* data = stbi_load(image_path_diff, &width, &height, &nrChannels, 0);
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

			diffTexture = texture1;
		}

		if (image_path_spec != nullptr)
		{
			glGenTextures(1, &texture2);
			glBindTexture(GL_TEXTURE_2D, texture2);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load image, create texture and generate mipmaps
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.	SPRAWDZ ZAWSZE CZY OBRACAC CZY NIEEEEEE
			// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
			unsigned char* data = stbi_load(image_path_spec, &width, &height, &nrChannels, 0);
			if (data)
			{
				GLenum format; //system sam okresla sobie format danych
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

			specTexture = texture2;
		}
	}

	void MapGenerator::Draw()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specTexture);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 8);
	}

	void MapGenerator::initShader(Shader shader)
	{
		shader.use();
		shader.setInt("texture_diffuse1", 0);
		shader.setInt("texture_specular1", 1);
		shader.setInt("material.diffuse", 0);
		shader.setInt("material.specular", 1);
	}

	inline bool Intersect(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 rayOrigin, glm::vec3 ray, glm::vec3& hit)
	{
		//wektory bokow trojkata
		glm::vec3 e1 = p2 - p1;
		glm::vec3 e2 = p3 - p1;

		//Calculate determinant
		glm::vec3 p = glm::cross(ray, e2);

		//Calculate determinant
		float det = glm::dot(e1, p);

		//if determinant is near zero, ray lies in plane of triangle otherwise not
		if (det > -0.0001f && det < 0.0001f) { return false; }
		float invDet = 1.0f / det;

		//calculate distance from p1 to ray origin
		glm::vec3 t = rayOrigin - p1;

		//Calculate u parameter
		float u = glm::dot(t, p) * invDet;

		//Check for ray hit
		if (u < 0 || u > 1) { return false; }

		//Prepare to test v parameter
		glm::vec3 q = glm::cross(t, e1);

		//Calculate v parameter
		float v = glm::dot(ray, q) * invDet;

		//Check for ray hit
		if (v < 0 || u + v > 1) { return false; }

		//punkt zderzenia
		hit = p1 + u * e1 + v * e2;

		if (glm::dot(e2, q) * invDet > 0.0001f) {
			//ray does intersect
			return true;
		}

		// No hit at all
		return false;
	}

	float MapGenerator::isHit(glm::vec3 origin, glm::vec3 vector, float skip)	
	{	//najmniejsza odleglosc od punktu przecieca z trojkatem
		glm::vec3 hit(0.0f);	//zmienna punktu trafienia
		float distance = skip;	//odleglosc od punktu trafienia
		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < size; j++)
			{	//iterujemy po calej tablicy verticlow
				if (j == size - 1)	//dla ulatwienia liczenia tutaj zostawiamy dodatkowa liczbe, do ktorej nigdy nie dojdziemy
					continue;

				//trojkat dolny
				glm::vec3 a = glm::vec3(j, heights[(i + 1) * size + j], i + 1);
				glm::vec3 b = glm::vec3(j + 1, heights[(i + 1) * size + j + 1], i + 1);
				glm::vec3 c = glm::vec3(j + 1, heights[i * size + j + 1], i);

				//sprawdzenie czy oplaca nam sie liczyc przeciecia z trojkatem (proces ten jest kosztowny wiec warto nadmaiarowe punkty odrzucic)
				if (glm::length((a + b + c + glm::vec3(j, heights[i * size + j], i)) * 0.25f - origin) > skip)
					continue;

				//obliczam odleglosci od przeciec i zostawiam najkrotsze
				if (Intersect(a, b, c, origin, vector, hit))
				{
					float tmpDist = glm::length(hit - origin);
					if (tmpDist < distance)
						distance = tmpDist;
				}

				//powtorzenie dla gornego trojkata
				b = glm::vec3(j, heights[i * size + j], i);

				if (Intersect(a, b, c, origin, vector, hit))
				{
					float tmpDist = glm::length(hit - origin);
					if (tmpDist < distance)
						distance = tmpDist;
				}
			}
		}

		//zwracam odleglosc zmniejszona o 0.1f ze wzgledu na to, ze stosuje to dla kamery, a niechcialbym by kamera wchodzila w teren
		return distance - 0.1f;
	}

	inline float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
	{	//algorytm wyszukujacy wysokosci na danym trojkacie gdy mamy podane punkty trojkata oraz wspolrzedne x,z na trojkacie
		float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
		float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
		float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.y + l2 * p2.y + l3 * p3.y;
	}

	float MapGenerator::getHeight(glm::vec2 pos, glm::vec3 & normal)	//obliczam wysokosc w danym punkcie
	{
		if (pos.x < 0.0f || pos.y < 0.0f || pos.x > size - 1 || pos.y > size - 1)	//jezeli poza terenem
		{
			normal = glm::vec3(0.0f, 1.0f, 0.0f);	//poza terenem zawsze normal jest na gorze
			return -1.0f;
		}

		bool isTop;	//czy jestem w gornym trojkacie

		int x1, y1;	//odcinanie na sile czesci dziesietnej z liczb
		x1 = pos.x;
		y1 = pos.y;

		glm::vec2 a, b, c, quadVec;

		quadVec = glm::vec2(x1, y1);	//staly wektor sluzacy potem do okreslenia wektorow normalnych przy wyciaganiu ich z tablicy

		if (pos.x - x1 < 1 - (pos.y - y1))	//jezeli jestem ponad przecieciem kwadratu
		{
			a = glm::vec2(x1 + 1, y1);
			b = glm::vec2(x1, y1);
			c = glm::vec2(x1, y1 + 1);	
			isTop = true;	//gorny trojkat
		}
		else								//jezeli jestem pod przecieciem kwadratu
		{
			a = glm::vec2(x1, y1 + 1);
			b = glm::vec2(x1 + 1, y1 + 1);
			c = glm::vec2(x1 + 1, y1);
			isTop = false;	//dolny trojkat
		}

		float h1, h2, h3;	//wyciagniecie z tablicy wysokosci zadanych wysokosci
		h1 = heights[(int)a.x % size + (int)a.y * size];
		h2 = heights[(int)b.x % size + (int)b.y * size];
		h3 = heights[(int)c.x % size + (int)c.y * size];

		if (isTop)	//wyciagam z tablicy verticlow informacje na temat wektorow normalnych
		{
			normal = normals[((int)quadVec.x % (size - 1))*2 + ((int)quadVec.y * (size - 1)) * 2 + 1];
		}
		else
		{
			normal = normals[((int)quadVec.x % (size - 1)) * 2 + ((int)quadVec.y * (size - 1)) * 2];
		}

		//wyciagniecie za pomoca algorytmu wspó³rzednych barycentrycznych wysokosci punktu na trojkacie
		return barryCentric(glm::vec3(a.x, h1, a.y), glm::vec3(b.x, h2, b.y), glm::vec3(c.x, h3, c.y), pos);
	}

	void MapGenerator::makeHole(glm::vec3 pos, float radius)
	{
		//obliczam wysokosci terenu - celem jest tworzenie okraglych dziur o okreslonych promieniach
		for (int i = 0; i < size * size; i++)
		{
			//jezeli odleglosc jest wieksza niz nasz zasieg to nic nie robimy
			if (glm::length(glm::vec3(i % size, 0.0f, i / size) - glm::vec3(pos.x, 0.0f, pos.z)) > radius)
				continue;

			float dist = glm::length(glm::vec3(i % size, 0.0f, i / size) - glm::vec3(pos.x, 0.0f, pos.z));
			dist = 1 - ((dist / radius) * (dist / radius));

			if(pos.y - radius * dist < heights[i])
				heights[i] = pos.y - radius * dist;

			if (heights[i] < -1)
				heights[i] = -1;
		}

		//obliczam pomocnicze tablice z wektorami normalnymi stalymi, a nastepnie sumuje je ze soba w wierzcholkach by shader mogl je interpolowac
		//wektory te sa okreslane dla danej sciany wiec jest ich (size-1)(size-1)*2
		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < size; j++)
			{	//zeby troche lepiej sie liczylo to licze do size, a w wartosci (size - 1) przeskakuje o krok
				if ((j + 1) % size == 0)
					continue;
				//wektory dla dolnego trojkata
				glm::vec3 a = glm::vec3(j, heights[(i + 1) * size + j], i + 1);
				glm::vec3 b = glm::vec3(j + 1, heights[(i + 1) * size + (j + 1)], i + 1);
				glm::vec3 c = glm::vec3(j + 1, heights[i * size + (j + 1)], i);
				glm::vec3 normal = calcNormals(a, b, c);
				normals[i * (size - 1) * 2 + j * 2] = normal;

				//wektory dla gornego trojkata
				a = glm::vec3(j + 1, heights[i * size + (j + 1)], i);
				b = glm::vec3(j, heights[i * size + j], i);
				c = glm::vec3(j, heights[(i + 1) * size + j], i + 1);
				normal = calcNormals(a, b, c);
				normals[i * (size - 1) * 2 + j * 2 + 1] = normal;
			}
		}

		//obliczanie wektorów normalnych œwiat³a
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				glm::vec3 normal(0.0f);
				glm::vec3 a(0.0f);
				glm::vec3 b(0.0f);
				glm::vec3 c(0.0f);
				glm::vec3 d(0.0f);

				//tutaj sumuje wektory, tkore naleza do danego punktu
				//zeby nie wychodzic za zakres wektora sprawdzam wartosci
				if (i > 0 && j > 0)
					a = normals[(i - 1) * (size - 1) * 2 + (j - 1) * 2];
				if (i < size - 1 && j < size - 1)
					b = normals[(i * (size - 1) * 2 + j * 2) + 1];
				if (i < size - 1 && j > 0)
					c = glm::normalize(normals[(i * (size - 1) * 2 + (j - 1) * 2) + 1] + normals[i * (size - 1) * 2 + (j - 1) * 2]);
				if (i > 0 && j < size - 1)
					d = glm::normalize(normals[((i - 1) * (size - 1) * 2 + j * 2) + 1] + normals[(i - 1) * (size - 1) * 2 + j * 2]);

				normal = a + b + c + d;

				interNormals[i * size + j] = normal;
			}
		}

		//wpisanie danych do tablic wyœwietlania
		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (j == size - 1)	//przy prawej krawedzi kontunyuuj, chodzi o to by dobrze sie liczylo
					continue;

				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 1] = heights[(i + 1) * size + j];
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 3] = interNormals[(i + 1) * size + j].x;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 4] = interNormals[(i + 1) * size + j].y;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 5] = interNormals[(i + 1) * size + j].z;

				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 8 + 1] = heights[(i + 1) * size + j + 1];
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 8 + 3] = interNormals[(i + 1) * size + j + 1].x;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 8 + 4] = interNormals[(i + 1) * size + j + 1].y;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 8 + 5] = interNormals[(i + 1) * size + j + 1].z;

				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 2 * 8 + 1] = heights[i * size + j + 1];
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 2 * 8 + 3] = interNormals[i * size + j + 1].x;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 2 * 8 + 4] = interNormals[i * size + j + 1].y;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 2 * 8 + 5] = interNormals[i * size + j + 1].z;

				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 3 * 8 + 1] = heights[i * size + j + 1];
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 3 * 8 + 3] = interNormals[i * size + j + 1].x;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 3 * 8 + 4] = interNormals[i * size + j + 1].y;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 3 * 8 + 5] = interNormals[i * size + j + 1].z;

				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 4 * 8 + 1] = heights[i * size + j];
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 4 * 8 + 3] = interNormals[i * size + j].x;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 4 * 8 + 4] = interNormals[i * size + j].y;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 4 * 8 + 5] = interNormals[i * size + j].z;

				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 5 * 8 + 1] = heights[(i + 1) * size + j];
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 5 * 8 + 3] = interNormals[(i + 1) * size + j].x;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 5 * 8 + 4] = interNormals[(i + 1) * size + j].y;
				vertexData[(j % (size - 1) + i * (size - 1)) * 48 + 5 * 8 + 5] = interNormals[(i + 1) * size + j].z;
			}
		}

		init();	//aktualizacja wizualna
	}
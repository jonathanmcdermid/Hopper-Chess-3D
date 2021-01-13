#pragma once

class Primitive
{
private:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

public:
	Primitive() {}
	virtual ~Primitive() {}

	void set(const Vertex* vertices,
		const unsigned cVertices,
		const GLuint* indices,
		const unsigned cIndices)
	{
		for (size_t i = 0; i < cVertices; ++i)
			this->vertices.push_back(vertices[i]);

		for (size_t i = 0; i < cIndices; ++i)
			this->indices.push_back(indices[i]);
	}

	inline Vertex* getVertices() { return this->vertices.data(); }
	inline GLuint* getIndices() { return this->indices.data(); }
	inline const unsigned getNrOfVertices() { return (int) this->vertices.size(); }
	inline const unsigned getNrOfIndices() { return (int) this->indices.size(); }
};

class Quad : public Primitive
{
public:
	Quad()
		: Primitive()
	{
		Vertex vertices[] =
		{
			//Position								Color							Texcoords					Normals
			glm::vec3(0.5f, 0.f, 0.5f),				glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f),		glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.5f, 0.f, -0.5f),			glm::vec3(1.f, 0.f, 1.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(-0.5f, 0.f, -0.5f),			glm::vec3(0.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(-0.5f, 0.f, 0.5f),			glm::vec3(0.f, 0.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 1.f, 0.f)
		};
		unsigned cVertices = sizeof(vertices) / sizeof(Vertex);

		GLuint indices[] =
		{
			0, 1, 2,	//Triangle 1
			0, 2, 3		//Triangle 2
		};
		unsigned cIndices = sizeof(indices) / sizeof(GLuint);

		this->set(vertices, cVertices, indices, cIndices);
	}
};
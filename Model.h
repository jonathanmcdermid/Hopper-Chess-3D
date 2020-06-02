#pragma once

#include "OBJLoader.h"

class Model
{
private:
	Material* material;
	Texture* overrideTextureDiffuse;
	Texture* overrideTextureSpecular;
	std::vector<Mesh*> meshes;
	glm::vec3 position;

public:
	Model(
		glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		std::vector<Mesh*>& meshes
	)
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;

		for (auto* i : meshes)
			this->meshes.push_back(new Mesh(*i));

		for (auto& i : this->meshes)
		{
			i->Move(this->position);
			i->setOrigin(this->position);
		}
	}

	Model(
		glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		const char* objFile
	)
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;

		std::vector<Vertex> mesh = loadOBJ(objFile);
		this->meshes.push_back(new Mesh(mesh.data(), (int) mesh.size(), NULL, 0,
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(1.f)));

		for (auto& i : this->meshes)
		{
			i->Move(this->position);
			i->setOrigin(this->position);
		}
	}

	~Model()
	{
		for (auto*& i : this->meshes)
			delete i;
	}

	glm::vec3 getPosition()
	{
		return position;
	}

	void setPosition(glm::vec3 destination)
	{
		position = destination;
	}

	void rotate(const glm::vec3 rotation)
	{
		for (auto& i : this->meshes)
			i->rotate(rotation);
	}

	void move(const glm::vec3 translation)
	{
		for (auto& i : this->meshes)
			i->Move(translation);
		position += translation;
	}

	void update(const char* objFile)
	{
		for (auto*& i : this->meshes)
			delete i;
		std::vector<Vertex> mesh = loadOBJ(objFile);
		this->meshes.push_back(new Mesh(mesh.data(), (int) mesh.size(), NULL, 0,
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(1.f)));
		for (auto& i : this->meshes)
		{
			i->Move(this->position);
			i->setOrigin(this->position);
		}
	}

	void render(Shader* shader)
	{
		this->material->sendToShader(*shader);

		shader->use();

		for (auto& i : this->meshes)
		{
			this->overrideTextureDiffuse->bind(0);
			this->overrideTextureSpecular->bind(1);
			i->render(shader);
		}
	}
};
#pragma once
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include <string>

class Light {
private:
	unsigned int lightID;
	glm::vec4 ambientProperty;
	glm::vec4 diffuseProperty;
public:

	Light() {
		lightID = 0x4000;
		ambientProperty = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		diffuseProperty = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	}
	Light(unsigned int lightID) {
		this->lightID  = lightID;
		ambientProperty = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		diffuseProperty = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	}

	virtual glm::vec4 getLightPosition() = 0;
	virtual void setLightPosition(glm::vec4 lightPosition) = 0;

	glm::vec4 getAmbientProperty() {
		return this->ambientProperty;
	}
	glm::vec4 getDiffusedProperty() {
		return this->diffuseProperty;
	}
	unsigned int getLightID() {
		return lightID;
	}

	void setAmbientProperty(glm::vec4 ambientPosition) {
		this->ambientProperty = ambientProperty;
	}
	void setDiffuseProperty(glm::vec4 diffuseProperty) {
		this->diffuseProperty = diffuseProperty;
	}


};

class globalLight : public Light {
private:


public:

};

class PointLight : public Light {

private:

	//Components of Light
	glm::vec4 specularProperty;
	glm::vec4 lightPosition;

public:
	// Class Constructor
	PointLight() {
		specularProperty = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lightPosition = glm::vec4(0.0f, 10.0f, 0.0f, 1.0f);
	}

	PointLight(unsigned int lightID) :
		Light(lightID) {
		specularProperty = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		setLightPosition(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	~PointLight() {};

	glm::vec4 getLightPosition() {
		return this->lightPosition;
	}

	glm::vec4 getSpecularProperty() {
		return this->specularProperty;
	}

	void setLightPosition(glm::vec4 lightPosition) {
		this->lightPosition = lightPosition;
	}
	void setSpecularProperty(glm::vec4 diffuseProperty) {
		this->specularProperty = diffuseProperty;
	}



};

class DirectionalLight : public Light {

private:
	glm::vec4 lightPosition;
	glm::vec3 lightDirection;
	glm::vec4 specularProperty;


public:

	DirectionalLight(unsigned int lightID)
		: Light(lightID) {
		lightDirection = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
		specularProperty = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		setLightPosition(glm::vec4(0.0f, 10.0f, 0.0f, 0.0f));
	}

	~DirectionalLight() {

	}

	glm::vec4 getLightPosition() {
		return this->lightPosition;
	}
	glm::vec3 getLightDirection() {
		return this->lightDirection;
	}
	glm::vec4 getSpecularProperty() {
		return this->specularProperty;
	}
	void setLightPosition(glm::vec4 lightPosition) {
		this->lightPosition = lightPosition;
	}

	void setLightDirection(glm::vec4 direction) {
		lightDirection = direction;
	}
	void setSpecularProperty(glm::vec4 diffuseProperty) {
		this->specularProperty = diffuseProperty;
	}

};

class PositionalLight : public Light {

private:
	glm::vec3 lightDirection;
	float cutoff;
	unsigned int exponent;
	glm::vec3 spotDirection;
	glm::vec4 specularProperty;
	glm::vec4 lightPosition;
	float attenuationCoeff;
	unsigned int attenuationType;

public:

	PositionalLight(unsigned int lightID)
		: Light(lightID) {
		cutoff = 45.0f;
		exponent = 0.0f;
		spotDirection = glm::vec3(0.0f, 0.0f, -10.0f);
		specularProperty = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		attenuationCoeff = 1;
		attenuationType = 0x1207;
		lightDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		setLightPosition(glm::vec4(0.0f, 0.0f, -10.0f, 1.0f));
	}
	~PositionalLight() {

	}
	//GL_CONSTANT_ATTENUATION 0x1207
	//GL_LINEAR_ATTENUATION 0x1208
	//GL_QUADRATIC_ATTENUATION 0x1209


	   //getters
	unsigned int getAttenutationType() {
		return attenuationType;
	}
	float getCutoff() {
		return cutoff;
	}
	unsigned int getExponent() {
		return exponent;
	}
	float getattenuationCoeff() {
		return attenuationCoeff;
	}

	glm::vec3 getLightDirection() {
		return this->lightDirection;
	}
	glm::vec4 getSpecularProperty() {
		return this->specularProperty;
	}
	glm::vec4 getLightPosition() {
		return this->lightPosition;
	}
	// setters
	void setAttenutationType(unsigned int attenuationType) {
		this->attenuationType = attenuationType;
	}
	void setattenuationCoeff(float attenuationCoeff) {
		this->attenuationCoeff = attenuationCoeff;
	}
	void setExponent(unsigned int exponent) {
		this->exponent = exponent;
	}
	void setCutoff(float cutoff) {
		this->cutoff = cutoff;
	}
	void setLightDirection(glm::vec4 direction) {
		lightDirection = direction;
	}
	void setSpecularProperty(glm::vec4 diffuseProperty) {
		this->specularProperty = diffuseProperty;
	}
	void setLightPosition(glm::vec4 lightPosition) {
		this->lightPosition = lightPosition;
	}


};
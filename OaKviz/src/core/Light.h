#pragma once
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include <string>

class Light {

public:
	unsigned int lightID;
	glm::vec4 ambientProperty;
	glm::vec4 diffuseProperty;
	glm::vec4 lightPosition;
	bool visible;
	glm::vec4 specularProperty;
	std::string typeName;

	Light() {
		lightID = 0x4000;
		ambientProperty = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		diffuseProperty = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		lightPosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		specularProperty = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		visible = true;
		typeName = "Light";
	}
	Light(unsigned int lightID) {
		this->lightID  = lightID;
		ambientProperty = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		diffuseProperty = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		lightPosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		specularProperty = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		visible = true;
		typeName = "Light";
	}

	virtual glm::vec4 getLightPosition() = 0;
	virtual void setLightPosition(glm::vec4 lightPosition) = 0;
	virtual std::string getLightName() = 0;

	glm::vec4 getAmbientProperty() {
		return this->ambientProperty;
	}
	glm::vec4 getDiffusedProperty() {
		return this->diffuseProperty;
	}
	glm::vec4 getSpecularProperty() {
		return this->specularProperty;
	}

	void setLightID(unsigned int light_id) {
		this->lightID = light_id;
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
	void setSpecularProperty(glm::vec4 specularProperty) {
		this->specularProperty = specularProperty;
	}


};

class PointLight : public Light {

public:
	
	// Class Constructor
	PointLight() {
		lightPosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		typeName = "Point";
	}

	PointLight(unsigned int lightID) :
		Light(lightID) {
		setLightPosition(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		typeName = "Point";
	}

	~PointLight() {};

	glm::vec4 getLightPosition() {
		return this->lightPosition;
	}

	std::string getLightName() {
		return typeName;
	}

	void setLightPosition(glm::vec4 lightPosition) {
		this->lightPosition = lightPosition;
	}

};

class DirectionalLight : public Light {
public:
	
	glm::vec3 lightDirection;

	DirectionalLight(unsigned int lightID)
		: Light(lightID) {
		lightDirection = glm::vec3(0.0f, -10.0f, 0.0f);
		specularProperty = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		setLightPosition(glm::vec4(0.0f, 10.0f, 0.0f, 0.0f));
		typeName = "Directional";
	}

	~DirectionalLight() {

	}

	glm::vec4 getLightPosition() {
		return this->lightPosition;
	}
	glm::vec3 getLightDirection() {
		return this->lightDirection;
	}
	std::string getLightName() {
		return typeName;
	}
	void setLightPosition(glm::vec4 lightPosition) {
		this->lightPosition = lightPosition;
	}

	void setLightDirection(glm::vec3 direction) {
		lightDirection = direction;
	}

};

class PositionalLight : public Light {

public:

	glm::vec3 lightDirection;
	
	float cutoff;
	unsigned int exponent;
	float attenuationCoeff;
	unsigned int attenuationType;
	std::string attenTypeName;

	PositionalLight(unsigned int lightID)
		: Light(lightID) {

		cutoff = 45.0f;
		exponent = 0.0f;
		//spotDirection = glm::vec3(0.0f, 0.0f, -10.0f);
		attenuationCoeff = 1;
		attenuationType = 0x1207;
		lightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		setLightPosition(glm::vec4(0.0f, 0.0f, -10.0f, 1.0f));
		typeName = "Positional";

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
	glm::vec4 getLightPosition() {
		return this->lightPosition;
	}
	std::string getAttenName() {
		if (attenuationType == 0) {
			return std::string("CONSTANT");
		}else if (attenuationType == 1) {
			return std::string("LINEAR");
		}else if (attenuationType == 2) {
			return std::string("QUADRATIC");
		}
	}

	std::string getLightName() {
		return typeName;
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
	void setLightDirection(glm::vec3 direction) {
		this->lightDirection = direction;
	}
	void setLightPosition(glm::vec4 lightPosition) {
		this->lightPosition = lightPosition;
	}


};
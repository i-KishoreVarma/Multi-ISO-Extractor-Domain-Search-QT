#include "MyLibrary.h"
#include <string>
#include <fstream>
#include <iostream>
#include <string.h>

using namespace std;

Shader::Shader(){
	ID = 0;
}

void Shader::compile()
{
    GLuint ShaderProgram = glCreateProgram();
	ID = ShaderProgram;
	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	for(auto &shader:arr)
	{
		AddShader(shader.code.c_str(), shader.type);
	}

	GLint Success = 0;
	GLchar ErrorLog[1024] = {0};

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof (ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof (ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	bind();
}

bool Shader::ReadFile(const char *pFileName, string &outFile)
{
	ifstream f(pFileName);

	bool ret = false;

	if (f.is_open())
	{
		string line;
		while (getline(f, line))
		{
			outFile.append(line);
			outFile.append("\n");
		}

		f.close();

		ret = true;
	}
	else
	{
		fprintf(stderr, "Error in loading file: '%s'\n", pFileName);
	}
	return ret;
}

void Shader::add(const char * fileName,GLenum shaderType)
{
	string str;
	if (!ReadFile(fileName,str)) {
		exit(1);
	}
	ShaderProps obj;
	obj.code.swap(str);
	obj.type = shaderType;
	arr.push_back(obj);
}

void Shader::load(string& data,GLenum shaderType)
{
	ShaderProps obj;
	obj.code.swap(data);
	obj.type = shaderType;
	arr.push_back(obj);
}

void Shader::bind()
{
	glUseProgram(ID);
	ErrorHandler();
}

void Shader::unbind()
{
	glUseProgram(0);
	ErrorHandler();
}

void Shader::AddShader(const char* pShaderText, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar * p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ID, ShaderObj);
}

int Shader::getUniformLocation(const string &name)
{
	auto it = location.find(name);
	int locationID = 0;
	if(it==location.end()) locationID = location[name] = glGetUniformLocation(ID, name.c_str());
	else locationID = it->second;
	return locationID;
}

void Shader::setUniformMat4f(const string &name,glm::mat4 &data)
{
	int locationID = getUniformLocation(name);
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &data[0][0]);
    ErrorHandler();
}

void Shader::setUniformVec3(const string &name,glm::vec3 &data)
{
	int locationID = getUniformLocation(name);
	glUniform3fv(locationID, 1, glm::value_ptr(data));
    ErrorHandler();
}

void Shader::setUniform1i(const string &name,GLuint inp)
{
	int locationID = getUniformLocation(name);
	glUniform1i(locationID,inp);
}

Shader::~Shader()
{
}

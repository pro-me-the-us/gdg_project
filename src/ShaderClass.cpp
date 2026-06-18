#include "ShaderClass.h"

//some bullshit source code to get contents of a file
std::string get_file_content(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	//getting the content of the file as string and storing them in string variable
	std::string vertexCode = get_file_content(vertexFile);
	std::string fragmentCode = get_file_content(fragmentFile);


	//converting the strings data into a char arrau
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//creating an empty container to store future bullshit that we do
	//vertexshader stores the location of the container
	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	//storing 1 string of text in the container provided by GPU
	// the string is stored in vertexshadersource
	glShaderSource(VertexShader, 1, &vertexSource, NULL);
	//The string is text so we compile it so GPU can understand it
	glCompileShader(VertexShader);

	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(FragmentShader);





	//wrapping both the shaders into a single object
	ID = glCreateProgram();
	glAttachShader(ID, VertexShader);
	glAttachShader(ID, FragmentShader);

	glLinkProgram(ID);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
}

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}
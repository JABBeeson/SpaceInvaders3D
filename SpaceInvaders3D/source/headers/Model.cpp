#include "Model.h"

Model::Model() { }
Model::Model(GLchar* path, GLchar* texture) {
	this->loadModel(path, texture);
}

void Model::SetPos(GLfloat x, GLfloat y, GLfloat z) {
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].position.x = x;
		this->meshes[i].position.y = y;
		this->meshes[i].position.z = z;
	}
}

void Model::Move(GLfloat x, GLfloat y, GLfloat z) {
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].position.x += x;
		this->meshes[i].position.y += y;
		this->meshes[i].position.z += z;
	}
}

void Model::Rotate(GLfloat x, GLfloat y, GLfloat z) {	
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].rotation.x += x;
		this->meshes[i].rotation.y += y;
		this->meshes[i].rotation.z += z;
	}
}

void Model::Scale(GLfloat v) {
	for (int i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].scale.x = v;
		this->meshes[i].scale.y = v;
		this->meshes[i].scale.z = v;
	}
}

void Model::Render(Shader shader, mat4 &projectionMatrix, Vector3 viewMatrix, Vector3 viewRotation) {
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].Render(shader, projectionMatrix, viewMatrix, viewRotation);
	}
}

void Model::loadModel(string modelPath, const char* texturePath) {
	//use Assimp to import model file.
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);
	
	//error checking
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ASSIMP Error: " << import.GetErrorString() << std::endl;
	}
	else {	//store in directory
		this->directory = modelPath.substr(0, modelPath.find_last_of('/'));
		this->processNode(scene->mRootNode, scene, texturePath);
	}
}

GLuint Model::loadTexture(const char* texturePath) {
	SDL_Surface* image = IMG_Load(texturePath);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	cout << image->w << endl;
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(image);
	
	return textureID;
}

void Model::processNode(aiNode* node, const aiScene* scene, const char* texturePath) {
	//process the node's meshes
	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		//concatinate the processed meshes into 'meshes'
		this->meshes.push_back(this->processMesh(mesh, scene, texturePath));
	}

	for (GLuint i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene, texturePath);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const char* texturePath) {
	vertices;
	indices;
	texture;
	
	// Process vertex positions, normals and texture coordinates
	for (GLuint i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vec3 vector;

		//Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		//Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		//Texture Coords
		vec2 vec = vec2(0.0f, 0.0f);
		if (mesh->mTextureCoords[0]) {
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else {
			vertex.texCoords = vec;
		}
		//pushback finialised vertices data
		vertices.push_back(vertex);
	}

	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	texture = loadTexture(texturePath);

	return Mesh(vertices, indices, texture);
}

void Model::Remove(bool doRemove) {
	if (doRemove) {
		Move(100.0f, 0.0f, 0.0f);
	}
	else Move(-100.0f, 0.0f, 0.0f);
	
}

Model::~Model()
{
}

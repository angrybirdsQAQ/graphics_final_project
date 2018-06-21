//
//  load_utils.cpp
//  AS2_Framework
//
#include "load_utils.hpp"

///////////////////////// Texture  //////////////////////////////////
void Texture::set_tex(TextureData *content)
{
	// use only 1 texture!
	glGenTextures(1, &tex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F,
				   content->width,
				   content->height);

	glTexSubImage2D(GL_TEXTURE_2D,
		0,
		0, 0,
		content->width,
		content->height,
		GL_RGBA, GL_UNSIGNED_BYTE,
		content->data);


	// generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
}

///////////////////////// Mesh //////////////////////////////////////
// modified code based on https://learnopengl.com/Model-Loading/Mesh
Mesh::Mesh(vector<Vertex> vecs, vector<int> inds, vector<int> texs)
{
    this->vertices = vecs;
    this->indices = inds;
    this->textures = texs;
    this->setup();
}

void Mesh::setup()
{
    // generate buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    // bind buffers
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    // move data to buffer
    glBufferData(GL_ARRAY_BUFFER,
                 this->vertices.size() * sizeof(Vertex),
                 &(this->vertices[0]),
                 GL_STATIC_DRAW);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 this->indices.size() * sizeof(int),
                 &(this->indices[0]),
                 GL_STATIC_DRAW);
    
    /* enable vertex attribute arrays & set vertex attribute pointers */
    
    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
    
    // tex coord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texcoord));
    
    // vertex normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    
    
    glBindVertexArray(0);   // unbind current binding
    
    
    //----- prepare texture
	if (this->textures.size() == 0) {
		printf("texture not found\n");
		return;
	}
        

	this->tex = this->textures[0];
}

void Mesh::render(vector<Texture> &loaded_tex){
    glBindVertexArray(this->vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, loaded_tex[this->tex].tex);
    
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    
    // unbind
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::print() {
    cout << "Mesh info"  << endl;
    cout << "vertices: " << this-> vertices.size() << endl;
    cout << "indices: "  << this->  indices.size() << endl;
    cout << "textures: " << this-> textures.size() << endl;
    
	/*
    for(auto t:this->textures){
        cout << "texture id: " << t.id << endl;
        cout << "texture type: " << t.type << endl;
        cout << "texture path: " << t.load_path << endl;
    }
	*/
}

//////////////////////// Model ////////////////////////
Model::Model(char *mdlDir, char *mdlFile){
    // set model directory
    char path[50];
    strcpy(path, mdlDir);
    strcat(path, mdlFile);
    this->dir = mdlDir;
    
    // import scene
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
									aiProcessPreset_TargetRealtime_MaxQuality);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }
    
	// load all textures

	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		aiMaterial *material = scene->mMaterials[i];
		Texture mmaterial;
		aiString texturePath;

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS) {
			string load_path = string(this->dir) + string(texturePath.C_Str());
			TextureData tex = loadPNG(load_path.c_str());
			glGenTextures(1, &mmaterial.tex);
			glBindTexture(GL_TEXTURE_2D, mmaterial.tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		loaded_tex.push_back(mmaterial);
	}


    processNode(scene->mRootNode, scene);

	printf("model has %d meshes\n", this->meshes.size());
	printf("importer gets %d meshes\n", scene->mNumMeshes);

}

void Model::add_texture(int meshIdx, string texture_path) {
	Texture mmaterial;

	string load_path = string(this->dir) + texture_path;
	TextureData tex = loadPNG(load_path.c_str());
	glGenTextures(1, &mmaterial.tex);
	glBindTexture(GL_TEXTURE_2D, mmaterial.tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	loaded_tex.push_back(mmaterial);

	int matIdx = loaded_tex.size() - 1;
	meshes[meshIdx].textures[0] = matIdx;
	meshes[meshIdx].setup();
	//meshes[meshIdx].print();
}

/*
void Model::transform(glm::mat4 transMat) {
	
}
*/

void Model::render(){
    
    for(auto mesh:this->meshes){
        mesh.render(loaded_tex);
    }
}

void Model::print(){
    cout << "Model info: " << endl;
    cout << "Model has " << meshes.size() << "meshes" << endl;
    for(auto mesh:meshes){
        mesh.print();
    }
}

void Model::processNode(aiNode *node, const aiScene *scene){
    // process all the node's meshes (if any)
    for(unsigned int i=0; i<node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i=0; i<node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}



Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    vector<Vertex> verts;
    vector<int> inds;
    vector<int> tex;
    
    // set vertex
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        Vertex v;
        
        // set vector position
        if(mesh->HasPositions()) {
            v.position.x = mesh->mVertices[i].x;
            v.position.y = mesh->mVertices[i].y;
            v.position.z = mesh->mVertices[i].z;
            //cout << "set vertex positon" << endl;
        }
        
        // set vector normal
        if(mesh->HasNormals()) {
            v.normal.x = mesh->mNormals[i].x;
            v.normal.y = mesh->mNormals[i].y;
            v.normal.z = mesh->mNormals[i].z;
            //cout << "set vertex normal" << endl;
        }
        // set tex coord
        if(mesh->mTextureCoords[0]){ // check if mesh contains tex coord
            v.texcoord.x = mesh->mTextureCoords[0][i].x;
            v.texcoord.y = mesh->mTextureCoords[0][i].y;
        } else {
            v.texcoord = glm::vec2(0.0f, 0.0f);
        }
        
        verts.push_back(v);
    }
    
    // set index
    for(unsigned int i=0; i<mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j=0; j<face.mNumIndices; j++)
            inds.push_back(face.mIndices[j]);
    }
    
    // set material
	int matIdx = mesh->mMaterialIndex;
	tex.push_back(matIdx);
	

    return Mesh(verts, inds, tex);
}

// load a png image and return a TextureData structure with raw data
// not limited to png format. works with any image format that is RGBA-32bit
TextureData Model::loadPNG(const char* const pngFilepath)
{
    TextureData texture;
    int components;
    
    // load the texture with stb image, force RGBA (4 components required)
    stbi_uc *data = stbi_load(pngFilepath, &texture.width, &texture.height, &components, 4);
    
    // is the image successfully loaded?
    if (data != NULL)
    {
        printf("loading PNG from %s\n", pngFilepath);
        
        // copy the raw data
        size_t dataSize = texture.width * texture.height * 4 * sizeof(unsigned char);
        texture.data = new unsigned char[dataSize];
        memcpy(texture.data, data, dataSize);
        
        // mirror the image vertically to comply with OpenGL convention
        for (size_t i = 0; i < (size_t)texture.width; ++i)
        {
            for (size_t j = 0; j < (size_t)texture.height / 2; ++j)
            {
                for (size_t k = 0; k < 4; ++k)
                {
                    size_t coord1 = (j * texture.width + i) * 4 + k;
                    size_t coord2 = ((texture.height - j - 1) * texture.width + i) * 4 + k;
                    std::swap(texture.data[coord1], texture.data[coord2]);
                }
            }
        }
        
        // release the loaded image
        stbi_image_free(data);
    }
    
    return texture;
}


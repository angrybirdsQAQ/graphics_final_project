//
//  load_utils.hpp
//  AS2_Framework
//
// // modified code based on https://learnopengl.com/Model-Loading/Model


#ifndef load_utils_hpp
#define load_utils_hpp

#include "../Externals/Include/Include.h"
#include <algorithm>
#include <ctime>
using namespace std;

///////////////// structs /////////////////////////////
// data structure for storing texture image raw data 
typedef struct _TextureData
{
    _TextureData(void) :
    width(0),
    height(0),
    data(0)
    {
    }
    
    int width;
    int height;
    unsigned char* data;
} TextureData;


// encapsulate texture image raw data
typedef struct _texture{
    int id;
    string type;
    string load_path;
    TextureData *content;
	GLuint tex;

	void set_tex(TextureData *content);
} Texture;



typedef struct _vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
} Vertex;


class Mesh{
public:
    vector<Vertex>      vertices;
    vector<int>         indices;
    vector<int>			textures;	// indices of textures 
									// which will be set to Mesh.tex
    
	Mesh(vector<Vertex> vecs, vector<int> inds, vector<int> texs);
	void setup();
    void render(vector<Texture> &loaded_tex);
    void print();
    
private:
    GLuint vao, vbo, ebo;
    GLuint tex;			// index of texture in Model.loaded_tex
};

class Model {
public:
    Model(char *mdlDir, char *mdlFile);
	void add_texture(int meshIdx, string texture_path);
	//void transform(glm::mat4 transMat);
    void render();
    void print();
    
private:
    string dir; // directory
    vector<Mesh> meshes;
    vector<Texture> loaded_tex;
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    
    TextureData loadPNG(const char* const pngFilepath);
};




#endif /* load_utils_hpp */

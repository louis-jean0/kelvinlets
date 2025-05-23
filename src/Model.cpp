#include <Model.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <assimp/postprocess.h>
#if _WIN32
#include <Windows.h>
#endif

// Static variable
std::vector<std::shared_ptr<Texture>> Model::textures_loaded;

// Constructors
Model::Model()  {}

Model::Model(const std::string& path) {
    load_model(path);
}

// Public methods
void Model::draw() {
    for(const auto &entry : entries) {
        entry.mesh->draw();
    }
}

void Model::bind_shader_to_meshes(std::shared_ptr<Shader> shader) {
    for(const auto &entry : entries) {
        entry.mesh->bind_shader(shader);
    }
}

void Model::bind_shader_to_meshes(const GLchar* vertex_path, const GLchar* fragment_path) {
    for(const auto &entry : entries) {
        entry.mesh->bind_shader(vertex_path,fragment_path);
    }
}

void Model::bind_texture_to_meshes(std::shared_ptr<Texture> texture) {
    for(const auto& entry : entries) {
        entry.mesh->add_texture(texture);
    }
}

// Private methods
void Model::load_model(const std::string& path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);
    if(!scene || (scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        std::cout<<"ERROR::ASSIMP"<<importer.GetErrorString()<<std::endl;
    }
    directory = path.substr(0, path.find_last_of('/'));
    process_node(scene->mRootNode, scene, glm::mat4(1.0f)); // -1 for root node
}

void Model::process_node(aiNode *node, const aiScene *scene, glm::mat4 parent_transform)  {
    glm::mat4 node_transform = parent_transform * aiMatrixToGlm(node->mTransformation);
    //std::cout << "Processing node: " << node->mName.C_Str() << std::endl;
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        auto mesh_data = process_mesh(mesh, scene);
        entries.push_back(MeshEntry{mesh_data, node_transform});
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene, node_transform);
    }
}

std::shared_ptr<Material> Model::load_material_textures(aiMaterial *material) {
    auto mat = std::make_shared<Material>();
    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
        mat->name = name.C_Str();
    }
    aiColor3D color(0.f, 0.f, 0.f);
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
        mat->ambient = glm::vec3(color.r, color.g, color.b);
    }
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        mat->diffuse = glm::vec3(color.r, color.g, color.b);
    }
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        mat->specular = glm::vec3(color.r, color.g, color.b);
    }
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
        mat->emissive = glm::vec3(color.r, color.g, color.b);
    }
    float shininess;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
        mat->shininess = shininess;
    }
    load_textures_from_material(material, aiTextureType_DIFFUSE, "texture_diffuse", mat);
    load_textures_from_material(material, aiTextureType_SPECULAR, "texture_specular", mat);
    load_textures_from_material(material, aiTextureType_NORMALS, "normal_map", mat);
    return mat;
}

void Model::load_textures_from_material(aiMaterial *material, aiTextureType tex_type, std::string tex_type_name, std::shared_ptr<Material> mat) {
    for (unsigned int i = 0; i < material->GetTextureCount(tex_type); i++) {
        aiString str;
        material->GetTexture(tex_type, i, &str);

        // Full texture path
        std::string texture_path = directory + "/" + std::string(str.C_Str());
        std::replace(texture_path.begin(), texture_path.end(), '\\', '/');

        bool skip = false;
        for (auto& loaded_tex : textures_loaded) {
            if (loaded_tex->path == texture_path) {
                mat->add_texture(loaded_tex);
                skip = true;
                break;
            }
        }
        if (!skip) {
            auto texture = std::make_shared<Texture>(texture_path, tex_type_name);
            textures_loaded.push_back(texture);
            mat->add_texture(texture);
        }
    }
}

std::shared_ptr<Mesh> Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;
    // Vertices data
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 temp;
        // Position
        temp.x = mesh->mVertices[i].x;
        temp.y = mesh->mVertices[i].y;
        temp.z = mesh->mVertices[i].z;
        vertex.position = temp;
        // Normal
        temp.x = mesh->mNormals[i].x;
        temp.y = mesh->mNormals[i].y;
        temp.z = mesh->mNormals[i].z;
        vertex.normal = temp;
        if(mesh->HasTangentsAndBitangents()) {
            // Tangent
            temp.x = mesh->mTangents[i].x;
            temp.y = mesh->mTangents[i].y;
            temp.z = mesh->mTangents[i].z;
            vertex.tangent = temp;
            // Bitangent
            temp.x = mesh->mBitangents[i].x;
            temp.y = mesh->mBitangents[i].y;
            temp.z = mesh->mBitangents[i].z;
            vertex.bitangent = temp;
        }
        else {
            vertex.tangent = glm::vec3(0.0f);
            vertex.bitangent = glm::vec3(0.0f);
        }
        if(mesh->mTextureCoords[0]) {
            glm::vec2 temp_uv;
            // UVs
            temp_uv.x = mesh->mTextureCoords[0][i].x;
            temp_uv.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = temp_uv;
        }
        else {
            vertex.uv = glm::vec2(0.0f,0.0f);
        }
        vertices.push_back(vertex);
    }
    // Indices data
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    // Textures data
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        material = load_material_textures(mat);
    }
    auto newMesh = std::make_shared<Mesh>(vertices, indices, material);
    return newMesh;
}

glm::mat4 Model::aiMatrixToGlm(aiMatrix4x4 from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}
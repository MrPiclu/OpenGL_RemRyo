#include "Model.h"

std::unique_ptr<Model> Model::Load(const std::string& filename, std::vector <Texture> tex) {
    auto model = std::unique_ptr<Model>(new Model());
    if (!model->LoadByAssimp(filename, tex))
        return nullptr;
    return std::move(model);
}

bool Model::LoadByAssimp(const std::string& filename, std::vector<Texture> tex) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }

    ProcessNode(scene->mRootNode, scene, tex);
    return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Texture> tex) {
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessMesh(mesh, scene, tex);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, tex);
    }
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<Texture> tex) {

    vertices.resize(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        auto& v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        v.texUV = glm::vec2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y); // Blender to OpenGL UV coord
    }

    std::vector<uint32_t> indices;
    indices.resize(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        indices[3 * i] = mesh->mFaces[i].mIndices[0];
        indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }

    auto glMesh = Mesh(vertices, indices, tex);
    m_meshes.push_back(std::move(glMesh));
}

void Model::Draw(Shader & shader, Camera & camera) const {
    for (Mesh mesh : m_meshes) {
        mesh.Draw(shader, camera);
    }
}

std::vector<glm::vec3> Model::getVertexPositions() const {
    std::vector<glm::vec3> positions;
    positions.reserve(vertices.size());
    for (const auto& vertex : vertices) {
        positions.push_back(vertex.position);
    }
    return positions;
}
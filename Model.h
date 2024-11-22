#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include "Mesh.h"
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

 class Model {
    public:
        static std::unique_ptr<Model> Load(const std::string& filename, std::vector<Texture> tex);

        int GetMeshCount() const { return (int)m_meshes.size(); }
        Mesh GetMesh(int index) const { return m_meshes[index]; }
        void Draw(Shader& shader, Camera& camera) const;

    private:
        Model() {}
        bool LoadByAssimp(const std::string& filename, std::vector<Texture> tex);
        void ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<Texture> tex);
        void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Texture> tex);

        std::vector<Mesh> m_meshes;
};

#endif 
#include "Model.h"

#include "Mesh.h"
#include "ShaderProgram.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <CCGL.h>

namespace sus3d
{
    bool Model::init(const aiScene* scene) {

        for (int i = 0; i < scene->mNumMeshes; ++i) {
            Mesh* mesh = Mesh::create(scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
            meshes.push_back(mesh);
        }

        return true;
    }

    glm::mat4 Model::prepareModelMatrix() {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        if (rotation.z)
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        if (rotation.y)
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        if (rotation.x)
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

        model = glm::scale(model, scale);

        return model;
    }

    Model* Model::create(const aiScene* scene, ShaderProgram* shaderProgram) {
        Model* ret = new Model();
        ret->shaderProgram = shaderProgram;
        if (!ret || !ret->init(scene)) {
            return nullptr;
        }
        return ret;
    }
    void Model::render(glm::mat4 view, glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 cameraPos, glm::mat4 projection) {

        if (visible) {
            glm::mat4 model = prepareModelMatrix();

            shaderProgram->use();
            shaderProgram->setBasicUniforms();

            shaderProgram->setMat4("model", model);
            shaderProgram->setMat4("view", view);
            shaderProgram->setMat4("projection", projection);

            shaderProgram->setVec3("lightColor", lightColor);
            shaderProgram->setVec3("lightPos", lightPos);
            shaderProgram->setVec3("viewPos", cameraPos);

            for (auto mesh : meshes)
                mesh->render(shaderProgram);
        }
    }

    Model* loadModel(const std::filesystem::path& path, ShaderProgram* shaderProgram) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string(),
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

        auto model = Model::create(scene, shaderProgram);
        importer.FreeScene();
        return model;
    }

    Model::~Model() {
        for (Mesh* mesh : meshes) {
            delete mesh;
        }
        meshes.clear();
    }
}
/*
    object3d.cpp
    author: Telo PHILIPPE

    Implementation of the Object3D class.
*/

#include "object3d.hpp"
#include "shader.hpp"


void Object3D::render(GLuint program) const {
    // Set uniforms and bind texture
    setUniform(program, "u_modelMat", modelMatrix);
    
    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    setUniform(program, "u_texture", 0);

    mesh->render();

    glActiveTexture(GL_TEXTURE0);
    texture->unbind();
    setUniform(program, "u_modelMat", glm::mat4(1.0f));
}
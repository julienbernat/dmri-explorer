#include <scene.h>
#include <sh_field.h>
#include <glm/gtx/transform.hpp>
#include <utils.hpp>
#include <application_state.h>
#include <camera.h>

namespace Slicer
{
Scene::Scene(const std::shared_ptr<ApplicationState>& state, const std::shared_ptr<Camera>& camera)
:mState(state)
,mCoordinateSystem(new CoordinateSystem())
,mCamera(camera)
{
    registerStateCallbacks();
    blockRotation = false;
}

Scene::~Scene()
{
}

void Scene::AddSHField()
{
    // create a SH Field model
    mModels.push_back(std::shared_ptr<SHField>(new SHField(mState, mCoordinateSystem)));
}

void Scene::registerStateCallbacks()
{
    mState->ViewMode.Mode.RegisterCallback(
        [this](State::ModeEnum p, State::ModeEnum n)
        {
            this->setMode(p, n);
        }
    );

}

void Scene::setMode(State::ModeEnum previous, State::ModeEnum mode)
{
    if(previous != mode)
    {
        if(mode == State::ModeEnum::off){
            mCamera->ApplyPerspectiveProjection();
            blockRotation=false;
            return;
        }
        blockRotation=true;
        mCamera->ApplyOrthogonalProjection();
        const glm::mat4 transform(1.0f);
        mCoordinateSystem->ApplyMatrix(transform);
        if(mode == State::ModeEnum::x){
            glm::mat4 transform = glm::rotate(1.5708f, glm::vec3(0.0, -1.0, 0.0)); 
            glm::mat4 transform2 = glm::rotate(1.5708f, glm::vec3(0.0, 0.0, -1.0)); 
            mCoordinateSystem->ApplyTransform(transform);
            mCoordinateSystem->ApplyTransform(transform2);

        }
        if(mode == State::ModeEnum::y){
            glm::mat4 transform = glm::rotate(1.5708f, glm::vec3(1.0, 0.0, 0.0));
            glm::mat4 transform2 = glm::rotate(3.14159f, glm::vec3(0.0, 0.0, 1.0));
            mCoordinateSystem->ApplyTransform(transform); 
            mCoordinateSystem->ApplyTransform(transform2);
        }
    }
}

void Scene::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto model : mModels)
    {
        model->Draw();
    }
}

void Scene::RotateCS(const glm::vec2& vec)
{
    if(!blockRotation){
        const float& rotationSpeed = mState->Window.RotationSpeed.Get();
        const float dx = -vec.x * rotationSpeed;
        const float dy = -vec.y * rotationSpeed;
        glm::mat4 transform = glm::rotate(dx, glm::vec3(0.0, 1.0, 0.0));
        transform = glm::rotate(dy, glm::vec3(1.0, 0.0, 0.0)) * transform;
        mCoordinateSystem->ApplyTransform(transform);   
    }
}

void Scene::TranslateCS(const glm::vec2& vec)
{
    const float& translationSpeed = mState->Window.TranslationSpeed.Get();
    const float dx = vec.x * translationSpeed;
    const float dy = vec.y * translationSpeed;
    const glm::mat4 transform = glm::translate(- dx * glm::vec3(1.0, 0.0, 0.0)
                                               + dy * glm::vec3(0.0, 1.0, 0.0));
    mCoordinateSystem->ApplyTransform(transform);
}
} // namespace Slicer

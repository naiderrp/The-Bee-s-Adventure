#pragma once
#include "pch.h"
#include "SceneObject/Object3D.h"

namespace GraphicsEngine {
    class GRAPHICS_API Camera
    {
    public:
        Camera()
            : mPosition(DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f }), mTarget(DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f }),
            mUp(DirectX::XMVECTOR{0.0f, 1.0f, 0.0f }), mYaw(0.0), mPitch(DirectX::XMConvertToRadians(30)), mDistance(3.5f),
            mLastMouseX(0), mLastMouseY(0), mIsRightMouseButtonDown(false)
        {
            updateViewMatrix();
            updateProjectionMatrix();
        }

        const auto& viewMatrix() const { return mViewMatrix; }
        const auto& projectionMatrix() const { return mPerspectiveMatrix; }

        void reset() {
            mPosition = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f };
            mTarget = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f };
            mUp = DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f }; 
            mYaw = 0.0; 
            mPitch = DirectX::XMConvertToRadians(30);
            mDistance = 3.5f;
            mLastMouseX = 0; 
            mLastMouseY = 0;
            mIsRightMouseButtonDown = false;
        }

        void rotate(float deltaX, float deltaY) {
            const float rotationSpeed = 0.005f;
            mYaw += deltaX * rotationSpeed;
            mPitch += deltaY * rotationSpeed;

            // Ограничим pitch для предотвращения переворота камеры
            const float limit = DirectX::XM_PI / 2.0f - 0.1f;
            mPitch = std::max<float>(-limit, std::min<float>(mPitch, limit));

            updateViewMatrix();
        }

        void zoom(float deltaZoom) {
            const float zoomSpeed = 1.0f;
            mDistance -= deltaZoom * zoomSpeed;
            mDistance = std::max<float>(4.0f, std::min<float>(mDistance, 8.0f)); // Ограничим диапазон зума

            updateViewMatrix();
        }

        void update(float deltaTime)
        {
            // Получаем текущее состояние мыши (эти функции нужно реализовать в системе ввода)
            int mouseX = getMouseX();
            int mouseY = getMouseY();
            bool rightMouseButtonDown = isRightMouseButtonDown(); // Функция для проверки состояния ЛКМ
            float mouseWheelDelta = getMouseWheelDelta(); // Получение изменений колесика мыши

            // Если ПКМ нажата, вращаем камеру
            if (rightMouseButtonDown)
            {
                if (mIsRightMouseButtonDown)
                {
                    float deltaX = static_cast<float>(mouseX - mLastMouseX);
                    float deltaY = static_cast<float>(mouseY - mLastMouseY);

                    rotate(deltaX, deltaY);
                }
                mIsRightMouseButtonDown = true;
            }
            else
            {
                mIsRightMouseButtonDown = false;
            }

            // Обрабатываем зум камеры
            if (mouseWheelDelta != 0)
            {
                zoom(mouseWheelDelta);
            }

            // Сохраняем текущее положение мыши для расчета разницы при следующем кадре
            mLastMouseX = mouseX;
            mLastMouseY = mouseY;
        }
    
        void followObject(Object3D& object)
        {
            auto& objectTransform = object.transform;

            // Позиция объекта
            mTarget = { objectTransform.translation.x, objectTransform.translation.y, objectTransform.translation.z };

            // Определяем позицию камеры позади объекта (например, с небольшим смещением по оси Z)
            DirectX::XMVECTOR offset = { 0.0f, 5.0f, -mDistance };  // Смещение: на 5 единиц выше и 10 единиц позади

            // Применяем вращение объекта к позиции камеры
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
                objectTransform.rotation.x,
                objectTransform.rotation.y,
                objectTransform.rotation.z);

            DirectX::XMVECTOR cameraPosition = DirectX::XMVector3Transform(offset, rotationMatrix);
            cameraPosition = DirectX::XMVectorAdd(cameraPosition, mTarget);

            // Обновляем матрицу вида, чтобы камера смотрела на объект
            mPosition = cameraPosition;
            updateViewMatrix();
        }

        DirectX::XMFLOAT3 getForwardDirection() const {
            return { sin(mYaw), 0.f, cos(mYaw) };
        }

        auto getPosition() const {
            DirectX::XMFLOAT3 cameraPosition;
            DirectX::XMStoreFloat3(&cameraPosition, mPosition);
            return cameraPosition;
        }

        float& mouseWheelDelta() { return mMouseWheelDelta; }

    public:
        DirectX::XMMATRIX mViewMatrix;
        DirectX::XMMATRIX mPerspectiveMatrix;

        DirectX::XMVECTOR mPosition;
        DirectX::XMVECTOR mTarget;
        DirectX::XMVECTOR mUp;

        float mYaw; // Угол поворота по оси Y
        float mPitch; // Угол поворота по оси X
        float mDistance; // Расстояние от камеры до цели

        int mLastMouseX;
        int mLastMouseY;
        bool mIsRightMouseButtonDown;

        float mMouseWheelDelta = 0.0f;

    private:
        void updateViewMatrix() {
            // Рассчитываем новое положение камеры на основе углов и расстояния
            DirectX::XMVECTOR positionOffset = {
                mDistance * cosf(mPitch) * sinf(mYaw - DirectX::XM_PI),
                mDistance * sinf(mPitch),
                mDistance * cosf(mPitch) * cosf(mYaw - DirectX::XM_PI)
            };

            mPosition = DirectX::XMVectorAdd(mTarget, positionOffset);
            mViewMatrix = DirectX::XMMatrixLookAtLH(mPosition, mTarget, mUp);
        }

        void updateProjectionMatrix() {
            mPerspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(
                1.2217304764f, 16.0f / 9.0f, 1.0f, 50.0f);
        }

        bool isRightMouseButtonDown() { 
            return GetAsyncKeyState(VK_RBUTTON);
        }

        int getMouseX() { 
            POINT currentMousePos;
            GetCursorPos(&currentMousePos);
            return currentMousePos.x;
        }
        
        int getMouseY() { 
            POINT currentMousePos;
            GetCursorPos(&currentMousePos);
            return currentMousePos.y;
        }
        float getMouseWheelDelta() {
            float delta = mMouseWheelDelta;
            mMouseWheelDelta = 0.0f;
            return delta;
        }
    };
}


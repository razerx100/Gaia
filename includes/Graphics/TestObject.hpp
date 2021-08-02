#ifndef __TEST_OBJECT_HPP__
#define __TEST_OBJECT_HPP__
#include <DirectXMath.h>
#include <memory>
#include <Graphics.hpp>
#include <ImGuiImpl.hpp>
#include <Model.hpp>

struct Position {
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	inline void Update() noexcept {
		m_transform =
			DirectX::XMMatrixRotationRollPitchYaw(
				roll, pitch, yaw
			)
			*
			DirectX::XMMatrixTranslation(
				x, y, z
			);
	}

	inline DirectX::XMMATRIX GetTransform() const noexcept {
		return m_transform;
	}

private:
	DirectX::XMMATRIX m_transform;
};

template<typename T>
class TestObject {
public:
	void Init(
		Graphics& gfx,
		const std::string& objectName,
		float posX, float posY, float posZ,
		const std::string& texturePath = ""
	) {
		m_positionData.x = posX;
		m_positionData.y = posY;
		m_positionData.z = posZ;

		m_model = std::make_unique<T>(gfx, objectName, texturePath);
	}

	void Draw(Graphics& gfx) {
		m_positionData.Update();
		m_model->Draw(gfx);
		m_model->SetTransform(m_positionData.GetTransform());
		ImGuiImpl::ImGuiModelControl(m_model->GetName(), m_positionData);
	}

private:
	Position m_positionData;
	std::unique_ptr<T> m_model;
};

template<>
class TestObject<Model> {
public:
	void Init(
		Graphics& gfx,
		const std::string& objectName,
		float posX, float posY, float posZ,
		const std::string& texturePath = ""
	) {
		m_positionData.x = posX;
		m_positionData.y = posY;
		m_positionData.z = posZ;

		m_model = std::make_unique<Model>(gfx, objectName, texturePath);
	}

	void Draw(Graphics& gfx) {
		m_positionData.Update();
		m_model->Draw(gfx, m_positionData.GetTransform());
		ImGuiImpl::ImGuiModelControl(m_model->GetName(), m_positionData);
	}

private:
	Position m_positionData;
	std::unique_ptr<Model> m_model;
};
#endif

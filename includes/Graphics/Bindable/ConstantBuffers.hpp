#ifndef __CONSTANT_BUFFERS_HPP__
#define __CONSTANT_BUFFERS_HPP__
#include <Bindable.hpp>
#include <Drawable.hpp>
#include <GraphicsThrowMacros.hpp>

template <typename T>
class ConstantBuffer : public Bindable {
public:
	ConstantBuffer(Drawable& parent)
		: m_ParentRef(parent) {}

	static void SetBuffer(Graphics& gfx) {
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(T);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0u;
		cbDesc.StructureByteStride = 0u;

		HRESULT s_hr;

		GFX_THROW_FAILED(s_hr, GetDevice(gfx)->CreateBuffer(
			&cbDesc, nullptr, &s_pConstantBuffer)
		);
	}

	static void SetBuffer(Graphics& gfx, const T& data) {
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(data);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0u;
		cbDesc.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA cData = {};
		cData.pSysMem = &data;

		HRESULT s_hr;

		GFX_THROW_FAILED(s_hr, GetDevice(gfx)->CreateBuffer(
			&cbDesc, &cData, &s_pConstantBuffer)
		);
	}

	void Update(Graphics& gfx, const T& vcBuffer) {

		D3D11_MAPPED_SUBRESOURCE mRes;
		GFX_THROW_FAILED(hr, GetDeviceContext(gfx)->Map(
			s_pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&mRes
		));

		memcpy(mRes.pData, &vcBuffer, sizeof(vcBuffer));

		GetDeviceContext(gfx)->Unmap(s_pConstantBuffer.Get(), 0u);
	}

protected:
	static ComPtr<ID3D11Buffer> s_pConstantBuffer;
	Drawable& m_ParentRef;
};

template<typename T>
ComPtr<ID3D11Buffer> ConstantBuffer<T>::s_pConstantBuffer;

template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T> {
public:
	using ConstantBuffer<T>::ConstantBuffer;

	void Bind(Graphics& gfx) noexcept override {
		DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(
			ConstantBuffer<T>::m_ParentRef.GetTranformationMatrix()
		);

		ConstantBuffer<T>::Update(gfx, transform);

		Bindable::GetDeviceContext(gfx)->VSSetConstantBuffers(
			0u, 1u, ConstantBuffer<T>::s_pConstantBuffer.GetAddressOf()
		);
	}
};

template<typename T>
class PixelConstantBuffer : public ConstantBuffer<T> {
public:
	using ConstantBuffer<T>::ConstantBuffer;

	void Bind(Graphics& gfx) noexcept override {
		Bindable::GetDeviceContext(gfx)->PSSetConstantBuffers(
			0u, 1u, ConstantBuffer<T>::s_pConstantBuffer.GetAddressOf()
		);
	}
};
#endif

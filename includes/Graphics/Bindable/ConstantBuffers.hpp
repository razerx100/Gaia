#ifndef __CONSTANT_BUFFERS_HPP__
#define __CONSTANT_BUFFERS_HPP__
#include <Bindable.hpp>
#include <GraphicsThrowMacros.hpp>

template <typename T>
class ConstantBuffer : public Bindable {
public:
	ConstantBuffer(Graphics& gfx) {

		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(T);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0u;
		cbDesc.StructureByteStride = 0u;

		GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateBuffer(
			&cbDesc, nullptr, &m_pConstantBuffer)
		);

		GetDeviceContext(gfx)->VSSetConstantBuffers(
			0u, 1u, m_pConstantBuffer.GetAddressOf()
		);
	}

	ConstantBuffer(Graphics& gfx, const T& cBuffer) {

		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(cBuffer);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0u;
		cbDesc.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA cbData = {};
		cbData.pSysMem = &cBuffer;

		GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateBuffer(
			&cbDesc, &cbData, &m_pConstantBuffer)
		);
	}

	void Update(Graphics& gfx, const T& cBuffer) {

		D3D11_MAPPED_SUBRESOURCE mRes;
		GFX_THROW_FAILED(hr, GetDeviceContext(gfx)->Map(
			m_pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&mRes
		));

		memcpy(mRes.pData, &cBuffer, sizeof(cBuffer));

		GetDeviceContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
	}

protected:
	ComPtr<ID3D11Buffer> m_pConstantBuffer;
};

template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T> {
public:
	using ConstantBuffer<T>::ConstantBuffer;

	void Bind(Graphics& gfx) noexcept override {
		Bindable::GetDeviceContext(gfx)->VSSetConstantBuffers(
			0u, 1u, ConstantBuffer<T>::m_pConstantBuffer.GetAddressOf()
		);
	}
};

template<typename T>
class PixelConstantBuffer : public ConstantBuffer<T> {
public:
	using ConstantBuffer<T>::ConstantBuffer;

	void Bind(Graphics& gfx) noexcept override {
		Bindable::GetDeviceContext(gfx)->PSSetConstantBuffers(
			0u, 1u, ConstantBuffer<T>::m_pConstantBuffer.GetAddressOf()
		);
	}
};
#endif

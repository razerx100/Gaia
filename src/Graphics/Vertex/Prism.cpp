#include <Prism.hpp>

IndexedTriangleList Prism::MakeTesselated(int longDiv) {
	assert(longDiv > 2);

	const DirectX::XMVECTOR base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR offset = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
	const float longitudeAngle = DirectX::XM_2PI / longDiv;

	// Near center
	std::vector<DirectX::XMFLOAT3> vertices;
	vertices.emplace_back();
	vertices.back() = { 0.0f, 0.0f, -1.0f };
	const std::uint16_t iCenterNear = static_cast<std::uint16_t>(vertices.size() - 1);

	// Far center
	vertices.emplace_back();
	vertices.back() = { 0.0f, 0.0f, 1.0f };
	const std::uint16_t iCenterFar = static_cast<std::uint16_t>(vertices.size() - 1);

	// Base vertices
	for (int iLong = 0; iLong < longDiv; iLong++) {
		// Near Base
		vertices.emplace_back();
		DirectX::XMVECTOR v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);
		DirectX::XMStoreFloat3(&vertices.back(), v);

		// Far Base
		vertices.emplace_back();
		v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);
		v = DirectX::XMVectorAdd(v, offset);
		DirectX::XMStoreFloat3(&vertices.back(), v);
	}

	// Side indices
	std::vector<std::uint16_t> indices;
	for (std::uint16_t iLong = 0; iLong < longDiv; iLong++) {
		const std::uint16_t i = static_cast<std::uint16_t>(iLong * 2u);
		const std::uint16_t mod = static_cast<std::uint16_t>(longDiv * 2u);

		indices.push_back(i + 2u);
		indices.push_back((i + 2u) % mod + 2u);
		indices.push_back(i + 1u + 2u);
		indices.push_back((i + 2u) % mod + 2u);
		indices.push_back((i + 3u) % mod + 2u);
		indices.push_back(i + 1u + 2u);
	}

	// Base indices
	for (std::uint16_t iLong = 0; iLong < longDiv; iLong++) {
		const std::uint16_t i = static_cast<std::uint16_t>(iLong * 2u);
		const std::uint16_t mod = static_cast<std::uint16_t>(longDiv * 2u);

		indices.push_back(i + 2u);
		indices.push_back(iCenterNear);
		indices.push_back((i + 2u) % mod + 2u);
		indices.push_back(iCenterFar);
		indices.push_back(i + 1u + 2u);
		indices.push_back((i + 3u) % mod + 2u);
	}

	return IndexedTriangleList(std::move(vertices), std::move(indices));
}

IndexedTriangleList Prism::MakeTesselatedIndependentCapNormals(int longDiv) {
	assert(longDiv >= 3);

	const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
	const auto offset = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
	const float longitudeAngle = 2.0f * DirectX::XM_PI / longDiv;

	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT3> normals;

	// near center
	const auto iCenterNear = (unsigned short)vertices.size();
	vertices.emplace_back();
	vertices.back() = { 0.0f,0.0f,-1.0f };
	normals.emplace_back();
	normals.back() = { 0.0f,0.0f,-1.0f };
	// near base vertices
	const auto iBaseNear = (unsigned short)vertices.size();
	for (int iLong = 0; iLong < longDiv; iLong++) {
		vertices.emplace_back();
		auto v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);
		DirectX::XMStoreFloat3(&vertices.back(), v);
		normals.emplace_back();
		normals.back() = { 0.0f,0.0f,-1.0f };
	}
	// far center
	const auto iCenterFar = (unsigned short)vertices.size();
	vertices.emplace_back();
	vertices.back() = { 0.0f,0.0f,1.0f };
	normals.emplace_back();
	normals.back() = { 0.0f,0.0f,1.0f };
	// far base vertices
	const auto iBaseFar = (unsigned short)vertices.size();
	for (int iLong = 0; iLong < longDiv; iLong++) {
		vertices.emplace_back();
		auto v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);
		v = DirectX::XMVectorAdd(v, offset);
		DirectX::XMStoreFloat3(&vertices.back(), v);
		normals.emplace_back();
		normals.back() = { 0.0f,0.0f,1.0f };
	}
	// fusilage vertices
	const auto iFusilage = (unsigned short)vertices.size();
	for (int iLong = 0; iLong < longDiv; iLong++) {
		// near base
		{
			vertices.emplace_back();
			auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			DirectX::XMStoreFloat3(&vertices.back(), v);
			normals.emplace_back();
			normals.back() = { vertices.back().x,vertices.back().y,0.0f };
		}
		// far base
		{
			vertices.emplace_back();
			auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			v = DirectX::XMVectorAdd(v, offset);
			DirectX::XMStoreFloat3(&vertices.back(), v);
			normals.emplace_back();
			normals.back() = { vertices.back().x,vertices.back().y,0.0f };
		}
	}

	std::vector<std::uint16_t> indices;

	// near base indices
	for (unsigned short iLong = 0; iLong < longDiv; iLong++)
	{
		const auto i = iLong;
		const auto mod = longDiv;
		// near
		indices.push_back(i + iBaseNear);
		indices.push_back(iCenterNear);
		indices.push_back((i + 1) % mod + iBaseNear);
	}
	// far base indices
	for (unsigned short iLong = 0; iLong < longDiv; iLong++)
	{
		const auto i = iLong;
		const auto mod = longDiv;
		// far
		indices.push_back(iCenterFar);
		indices.push_back(i + iBaseFar);
		indices.push_back((i + 1) % mod + iBaseFar);
	}
	// fusilage indices
	for (unsigned short iLong = 0; iLong < longDiv; iLong++)
	{
		const auto i = iLong * 2;
		const auto mod = longDiv * 2;
		indices.push_back(static_cast<std::uint16_t>(i + iFusilage));
		indices.push_back(static_cast<std::uint16_t>((i + 2) % mod + iFusilage));
		indices.push_back(static_cast<std::uint16_t>(i + 1 + iFusilage));
		indices.push_back(static_cast<std::uint16_t>((i + 2) % mod + iFusilage));
		indices.push_back(static_cast<std::uint16_t>((i + 3) % mod + iFusilage));
		indices.push_back(static_cast<std::uint16_t>(i + 1 + iFusilage));
	}

	return { std::move(vertices), std::move(normals), std::move(indices) };
}

IndexedTriangleList Prism::Make() {
	return MakeTesselated(6u);
}

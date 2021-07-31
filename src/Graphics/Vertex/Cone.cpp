#include <Cone.hpp>

IndexedTriangleList Cone::MakeTesselated(std::uint16_t longDiv) {
	assert(longDiv > 2);

	const DirectX::XMVECTOR base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
	const float longitudeAngle = DirectX::XM_2PI / longDiv;

	// Vertices
	std::vector<DirectX::XMFLOAT3> vertices;
	for (std::uint16_t iLong = 0; iLong < longDiv; iLong++) {
		vertices.emplace_back();
		DirectX::XMVECTOR v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);
		DirectX::XMStoreFloat3(&vertices.back(), v);
	}

	// The center
	vertices.emplace_back();
	vertices.back() = { 0.0f, 0.0f, -1.0f };
	const std::uint16_t iCenter = static_cast<std::uint16_t>(vertices.size() - 1);

	// The tip
	vertices.emplace_back();
	vertices.back() = { 0.0f, 0.0f, 1.0f };
	const std::uint16_t iTip = static_cast<std::uint16_t>(vertices.size() - 1);

	// Base Indices
	std::vector<std::uint16_t> indices;
	for (std::uint16_t iLong = 0; iLong < longDiv; iLong++) {
		indices.push_back(iCenter);
		indices.push_back((iLong + 1) % longDiv);
		indices.push_back(iLong);
	}

	// Cone Indices
	for (std::uint16_t iLong = 0; iLong < longDiv; iLong++) {
		indices.push_back(iLong);
		indices.push_back((iLong + 1) % longDiv);
		indices.push_back(iTip);
	}

	IndexedTriangleList data;
	data.SetVertices(std::move(vertices));
	data.SetIndices(std::move(indices));

	return data;
}

IndexedTriangleList Cone::MakeTesselatedIndependentFaces(int longDiv) {
	assert(longDiv >= 3);

	const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
	const float longitudeAngle = 2.0f * DirectX::XM_PI / longDiv;

	std::vector<DirectX::XMFLOAT3> vertices;

	// cone vertices
	const auto iCone = (unsigned short)vertices.size();
	for (int iLong = 0; iLong < longDiv; iLong++) {
		const float thetas[] = {
			longitudeAngle * iLong,
			longitudeAngle * (((iLong + 1) == longDiv) ? 0 : (iLong + 1))
		};
		vertices.emplace_back();
		vertices.back() = { 0.0f,0.0f,1.0f };
		for (auto theta : thetas) {
			vertices.emplace_back();
			const auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(theta)
			);
			DirectX::XMStoreFloat3(&vertices.back(), v);
		}
	}
	// base vertices
	const auto iBaseCenter = (unsigned short)vertices.size();
	vertices.emplace_back();
	vertices.back() = { 0.0f,0.0f,-1.0f };
	const auto iBaseEdge = (unsigned short)vertices.size();
	for (int iLong = 0; iLong < longDiv; iLong++) {
		vertices.emplace_back();
		auto v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);
		DirectX::XMStoreFloat3(&vertices.back(), v);
	}

	std::vector<std::uint16_t> indices;

	// cone indices
	for (std::uint16_t i = 0; i < longDiv * 3; i++) {
		indices.push_back(i + iCone);
	}
	// base indices
	for (std::uint16_t iLong = 0; iLong < longDiv; iLong++) {
		indices.push_back(iBaseCenter);
		indices.push_back((iLong + 1) % longDiv + iBaseEdge);
		indices.push_back(iLong + iBaseEdge);
	}

	IndexedTriangleList data;
	data.SetVertices(std::move(vertices));
	data.SetIndices(std::move(indices));

	return data;
}

IndexedTriangleList Cone::Make() {
	return MakeTesselated(24u);
}

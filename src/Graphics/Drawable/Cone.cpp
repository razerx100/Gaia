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

	return IndexedTriangleList(std::move(vertices), std::move(indices));
}

IndexedTriangleList Cone::Make() {
	return MakeTesselated(24u);
}

#include <Sphere.hpp>
#include <functional>

IndexedTriangleList Sphere::MakeTesselated(std::uint16_t latDiv, std::uint16_t longDiv) {

	assert(latDiv > 2);
	assert(longDiv > 2);

	constexpr float radius = 1.0f;
	const DirectX::XMVECTOR base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	const float lattitudeAngle = DirectX::XM_PI / latDiv;
	const float longitudeAngle = DirectX::XM_2PI / longDiv;

	// Vertices
	std::vector<DirectX::XMFLOAT3> vertices;
	for (std::uint16_t iLat = 1; iLat < latDiv; iLat++) {
		DirectX::XMVECTOR latBase = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationX(lattitudeAngle * iLat)
		);

		for (std::uint16_t iLong = 0; iLong < longDiv; iLong++) {
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(
				latBase,
				DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
			);

			vertices.emplace_back();
			DirectX::XMStoreFloat3(&vertices.back(), v);
		}
	}

	// Cap vertices
	const std::uint16_t iNorthPole = static_cast<std::uint16_t>(vertices.size());
	vertices.emplace_back();
	DirectX::XMStoreFloat3(&vertices.back(), base);

	const std::uint16_t iSouthPole = static_cast<std::uint16_t>(vertices.size());
	vertices.emplace_back();
	DirectX::XMStoreFloat3(&vertices.back(), DirectX::XMVectorNegate(base));

	// Indices
	const auto calcIndex = [longDiv]
	(std::uint16_t iLat, std::uint16_t iLong)->std::uint16_t {
		return iLat * longDiv + iLong;
	};

	std::vector<std::uint16_t> indices;
	for (std::uint16_t iLat = 0; iLat < latDiv - 2; iLat++) {
		for (std::uint16_t iLong = 0; iLong < longDiv - 1; iLong++) {
			indices.push_back(calcIndex(iLat, iLong));
			indices.push_back(calcIndex(iLat + 1, iLong));
			indices.push_back(calcIndex(iLat, iLong + 1));
			indices.push_back(calcIndex(iLat, iLong + 1));
			indices.push_back(calcIndex(iLat + 1, iLong));
			indices.push_back(calcIndex(iLat + 1, iLong + 1));
		}
		// Wrap band
		indices.push_back(calcIndex(iLat, longDiv - 1));
		indices.push_back(calcIndex(iLat + 1u, longDiv - 1));
		indices.push_back(calcIndex(iLat, 0u));
		indices.push_back(calcIndex(iLat, 0u));
		indices.push_back(calcIndex(iLat + 1u, longDiv - 1));
		indices.push_back(calcIndex(iLat + 1u, 0u));
	}

	// Cap fans
	for (std::uint16_t iLong = 0; iLong < longDiv - 1; iLong++) {
		// North
		indices.push_back(iNorthPole);
		indices.push_back(calcIndex(0u, iLong));
		indices.push_back(calcIndex(0u, iLong + 1u));
		// South
		indices.push_back(calcIndex(latDiv - 2u, iLong + 1u));
		indices.push_back(calcIndex(latDiv - 2u, iLong));
		indices.push_back(iSouthPole);
	}
	// Wrap Triangles
	// North
	indices.push_back(iNorthPole);
	indices.push_back(calcIndex(0, longDiv - 1));
	indices.push_back(calcIndex(0, 0));
	// South
	indices.push_back(calcIndex(latDiv - 2, 0));
	indices.push_back(calcIndex(latDiv - 2, longDiv - 1));
	indices.push_back(iSouthPole);

	return IndexedTriangleList(std::move(vertices), std::move(indices));
}

IndexedTriangleList Sphere::Make() {
	return MakeTesselated(12u, 24u);
}

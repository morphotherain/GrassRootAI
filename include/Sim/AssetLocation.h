#pragma once

#include <cstdint>

// 与 dynGameObjects.OwnerKind 列一致
enum class AssetOwnerKind : std::uint8_t {
	None = 0,
	Character = 1,
	Corporation = 2,  // 预留：军团资产
	Alliance = 3,     // 预留
};

struct AssetOwner {
	AssetOwnerKind kind = AssetOwnerKind::None;
	std::uint32_t id = 0;

	static AssetOwner None() { return {}; }
	static AssetOwner Character(std::uint32_t characterId)
	{
		AssetOwner owner;
		owner.kind = AssetOwnerKind::Character;
		owner.id = characterId;
		return owner;
	}

	bool IsNone() const { return kind == AssetOwnerKind::None || id == 0; }

	static AssetOwner FromDyn(std::uint32_t ownerKind, std::uint32_t ownerId)
	{
		if (ownerId == 0 && ownerKind == 0)
			return None();
		AssetOwner owner;
		owner.kind = static_cast<AssetOwnerKind>(ownerKind);
		owner.id = ownerId;
		return owner;
	}

	std::uint32_t DynOwnerKind() const { return static_cast<std::uint32_t>(kind); }
	std::uint32_t DynOwnerId() const { return id; }
};

// 与 dynGameObjects.LocationKind 列一致
enum class AssetLocationKind : std::uint8_t {
	SolarSystem = 0,        // 在星系空间中自由存在
	ContainerBag = 1,       // LocationRef = dynContainers.containerID
	DockedAtStructure = 2,  // LocationRef = 空间站/建筑 objectID
	StructureHangar = 3,    // 预留：站内机库 bag
	Instance = 4,           // 预留：副本/深渊实例 ID
};

struct AssetLocation {
	AssetLocationKind kind = AssetLocationKind::SolarSystem;
	std::uint32_t solarSystemId = 0;
	std::uint32_t locationRef = 0;

	static AssetLocation InSolarSystem(std::uint32_t solarSystemId)
	{
		AssetLocation loc;
		loc.kind = AssetLocationKind::SolarSystem;
		loc.solarSystemId = solarSystemId;
		return loc;
	}

	static AssetLocation InBag(std::uint32_t bagId, std::uint32_t contextSolarSystemId)
	{
		AssetLocation loc;
		loc.kind = AssetLocationKind::ContainerBag;
		loc.locationRef = bagId;
		loc.solarSystemId = contextSolarSystemId;
		return loc;
	}

	static AssetLocation DockedAt(std::uint32_t structureObjectId, std::uint32_t solarSystemId)
	{
		AssetLocation loc;
		loc.kind = AssetLocationKind::DockedAtStructure;
		loc.locationRef = structureObjectId;
		loc.solarSystemId = solarSystemId;
		return loc;
	}

	static AssetLocation FromDyn(std::uint32_t locationKind, std::uint32_t locationRef, std::uint32_t solarSystemId)
	{
		AssetLocation loc;
		loc.kind = static_cast<AssetLocationKind>(locationKind);
		loc.locationRef = locationRef;
		loc.solarSystemId = solarSystemId;
		return loc;
	}

	std::uint32_t DynLocationKind() const { return static_cast<std::uint32_t>(kind); }
	std::uint32_t DynLocationRef() const { return locationRef; }

	bool IsInSolarSystem() const { return kind == AssetLocationKind::SolarSystem; }
	bool IsDocked() const { return kind == AssetLocationKind::DockedAtStructure; }
	bool IsInBag() const { return kind == AssetLocationKind::ContainerBag; }
};

struct ContainerBagRef {
	std::uint32_t bagId = 0;
	std::uint32_t hostObjectId = 0;
	std::uint32_t containerType = 0;
};

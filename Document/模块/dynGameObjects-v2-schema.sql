-- dynGameObjects v2 迁移脚本
-- 适用：save/initial + 你要保留的那一个测试存档
-- 执行前请备份。旧测试存档可直接删，不必跑此脚本。
--
-- 语义对照（旧 ContainerID 推断 → 新显式列）：
--   ContainerID = 0                          → LocationKind=0 (SolarSystem), LocationRef=0
--   ContainerID ∈ dynContainers.containerID  → LocationKind=1 (ContainerBag), LocationRef=bagId
--   其它正整数                               → LocationKind=2 (DockedAtStructure), LocationRef=stationObjectId
--   OwnerID != 0                             → OwnerKind=1 (Character)
--   mapDenormalize 静态天体旧 hack（ContainerID=SolarSystemID）→ LocationKind=0, LocationRef=0

BEGIN TRANSACTION;

-- 1) 加列（若列已存在会失败，说明已迁移过）
ALTER TABLE dynGameObjects ADD COLUMN OwnerKind INTEGER NOT NULL DEFAULT 0;
ALTER TABLE dynGameObjects ADD COLUMN LocationKind INTEGER NOT NULL DEFAULT 0;
ALTER TABLE dynGameObjects ADD COLUMN LocationRef INTEGER NOT NULL DEFAULT 0;

-- 2) OwnerKind
UPDATE dynGameObjects
SET OwnerKind = CASE WHEN OwnerID != 0 THEN 1 ELSE 0 END;

-- 3) 在星系空间
UPDATE dynGameObjects
SET LocationKind = 0, LocationRef = 0
WHERE ContainerID = 0;

-- 4) 在容器 bag 内
UPDATE dynGameObjects
SET LocationKind = 1, LocationRef = ContainerID
WHERE ContainerID IN (SELECT containerID FROM dynContainers);

-- 5) 停靠建筑（旧 dock hack）
UPDATE dynGameObjects
SET LocationKind = 2, LocationRef = ContainerID
WHERE ContainerID != 0
  AND ContainerID NOT IN (SELECT containerID FROM dynContainers);

-- 6) 修正静态天体：旧代码把 ContainerID 设成 SolarSystemID
UPDATE dynGameObjects
SET LocationKind = 0, LocationRef = 0
WHERE ContainerID = SolarSystemID
  AND ContainerID NOT IN (SELECT containerID FROM dynContainers);

-- 7) 删除废弃列（SQLite 3.35+）
ALTER TABLE dynGameObjects DROP COLUMN ContainerID;

COMMIT;

-- 校验示例：
-- SELECT LocationKind, COUNT(*) FROM dynGameObjects GROUP BY LocationKind;
-- SELECT * FROM dynGameObjects WHERE LocationKind=1 AND LocationRef NOT IN (SELECT containerID FROM dynContainers);

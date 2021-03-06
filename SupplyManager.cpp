#include "SupplyManager.h"

#include "BuildingQueue.h"
#include "Neohuman.h"
#include "Util.h"

Neolib::SupplyManager supplyManager;

namespace Neolib {

SupplyCount::SupplyCount() : protoss(0), terran(0), zerg(0) {}

SupplyCount::SupplyCount(int protoss, int terran, int zerg)
    : protoss(protoss), terran(terran), zerg(zerg) {}

SupplyCount SupplyCount::operator+(const SupplyCount &other) const {
  return SupplyCount(protoss + other.protoss, terran + other.terran,
                     zerg + other.zerg);
}

SupplyCount &SupplyCount::operator+=(const SupplyCount &other) {
  protoss += other.protoss;
  terran += other.terran;
  zerg += other.zerg;
  return *this;
}

SupplyCount SupplyCount::operator-(const SupplyCount &other) const {
  return SupplyCount(protoss - other.protoss, terran - other.terran,
                     zerg - other.zerg);
}

SupplyCount &SupplyCount::operator-=(const SupplyCount &other) {
  protoss -= other.protoss;
  terran -= other.terran;
  zerg -= other.zerg;
  return *this;
}

SupplyCount SupplyCount::operator*(const int factor) const {
  return SupplyCount(protoss * factor, terran * factor, zerg * factor);
}

SupplyCount &SupplyCount::operator*=(const int factor) {
  protoss *= factor;
  terran *= factor;
  zerg *= factor;
  return *this;
}

SupplyCount SupplyCount::operator/(const int divisor) const {
  return SupplyCount(protoss / divisor, terran / divisor, zerg / divisor);
}

SupplyCount &SupplyCount::operator/=(const int divisor) {
  protoss /= divisor;
  terran /= divisor;
  zerg /= divisor;
  return *this;
}

int SupplyCount::operator()(BWAPI::Race race) const {
  switch (race) {
  case BWAPI::Races::Protoss:
    return protoss;
  case BWAPI::Races::Terran:
    return terran;
  case BWAPI::Races::Zerg:
    return zerg;
  default:
    return protoss + terran + zerg;
  }
}

int mainSupply(SupplyCount sc) {
  if (neoInstance->playingRace == BWAPI::Races::Protoss)
    return sc.protoss;
  if (neoInstance->playingRace == BWAPI::Races::Terran)
    return sc.terran;
  if (neoInstance->playingRace == BWAPI::Races::Zerg)
    return sc.zerg;

  return 0;
}

SupplyCount SupplyManager::usedSupply() {
  return SupplyCount(BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Protoss),
                     BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Terran),
                     BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Zerg));
}

SupplyCount SupplyManager::availableSupply() {
  return SupplyCount(
      BWAPI::Broodwar->self()->supplyTotal(BWAPI::Races::Protoss),
      BWAPI::Broodwar->self()->supplyTotal(BWAPI::Races::Terran),
      BWAPI::Broodwar->self()->supplyTotal(BWAPI::Races::Zerg));
}

SupplyCount SupplyManager::wantedSupplyOverhead() {
  SupplyCount wantedOverhead;

  if (BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Protoss) ||
      BWAPI::Broodwar->self()->supplyTotal(BWAPI::Races::Protoss))
    wantedOverhead.protoss =
        3 +
        MAX((BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Protoss) - 12) /
                6,
            0);

  if (BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Terran) ||
      BWAPI::Broodwar->self()->supplyTotal(BWAPI::Races::Terran))
    wantedOverhead.terran =
        4 +
        MAX((BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Terran) - 12) /
                6,
            0);

  if (BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Zerg) ||
      BWAPI::Broodwar->self()->supplyTotal(BWAPI::Races::Zerg))
    wantedOverhead.zerg =
        3 +
        MAX((BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Zerg) - 12) / 6,
            0);

  return wantedOverhead;
}

SupplyCount SupplyManager::wantedAdditionalSupply() {
  auto targetSupply = usedSupply() + wantedSupplyOverhead();
  if (targetSupply.protoss >= 400)
    targetSupply.protoss = 400;
  if (targetSupply.terran >= 400)
    targetSupply.terran = 400;
  if (targetSupply.zerg >= 400)
    targetSupply.zerg = 400;
  auto additionalSupplyRequired =
      targetSupply - availableSupply() - buildingQueue.getQueuedSupply(false);

  return additionalSupplyRequired;
}

} // namespace Neolib

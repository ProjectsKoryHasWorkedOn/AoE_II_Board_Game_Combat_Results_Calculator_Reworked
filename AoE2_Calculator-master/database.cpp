#include <stdexcept>

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "database.hpp"
#include "mainwindow.hpp"

Database::Database() : m_database{QSqlDatabase::addDatabase("QSQLITE")}
{
  m_database.setDatabaseName(":memory:");

  if (!m_database.open()) {
    throw std::runtime_error{"Could not open database"};
  }
}

// This was written by a numbat
// See:
// https://www.qtcentre.org/threads/27167-How-can-i-execute-a-database-script-within-Qt
int Database::runSqlScript(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return 0;

  QTextStream in(&file);
  QString     sql = in.readAll();
  if (sql.length() == 0)
    return 0;

  QList<int> splitPoints;
  enum { IN_STR, IN_ESC, NORMAL } state = NORMAL;
  int successCount                      = 0;

  for (int i = 0; i < sql.length(); i++) {
    const int character = sql.at(i).unicode();
    switch (state) {
    case IN_STR:
      switch (character) {
      case '\'':
        state = NORMAL;
        break;
      case '\\':
        state = IN_ESC;
        break;
      }
      break;

    case IN_ESC:
      state = IN_STR;
      break;

    case NORMAL:
      switch (character) {
      case ';':
        splitPoints.push_back(i);
        break;

      case '\'':
        state = IN_STR;
        break;
      }
    }
  }

  splitPoints.push_back(sql.length() - 1);

  for (int i = 0, j = 0; i < splitPoints.length(); i++) {
    QString statement = sql.mid(j, splitPoints.at(i) - j + 1);
    j                 = splitPoints.at(i) + 1;

    if (statement.trimmed().length() > 0) {
      QSqlQuery query(m_database);
      if (query.exec(statement)) {
        successCount++;
      }
      else {
        qDebug() << "Failed:" << statement << "\nReason:" << query.lastError();
        throw std::runtime_error{
          "Error while running SQL script (" + fileName.toStdString()
          + ") statement: " + statement.toStdString()
          + "Reason: " + query.lastError().text().toStdString()};
      }
    }
  }

  return successCount;
}

QStringList Database::getBuildingNames(int ageID)
{
  QStringList buildingNames{};

  // This is considered unsafe by SQL nerds on the internet, :(
  QSqlQuery query{QString{"SELECT * FROM Buildings WHERE ageID = %1"}.arg(
    QString::number(ageID))};
  const int buildingNameIndex{query.record().indexOf("buildingName")};

  while (query.next()) {
    const QString buildingName{query.value(buildingNameIndex).toString()};
    buildingNames.push_back(buildingName);
  }

  return buildingNames;
}

QStringList Database::getTechnologyNames()
{
  QStringList technologyNames{};
  QSqlQuery   query{QString{"SELECT * FROM Technologies"}};
  const int   technologyNameIndex{query.record().indexOf("technologyName")};

  while (query.next()) {
    const QString technologyName{query.value(technologyNameIndex).toString()};
    technologyNames.push_back(technologyName);
  }

  return technologyNames;
}

QStringList Database::getEventNames()
{
  QStringList eventNames{};
  QSqlQuery   query{QString{"SELECT eventName FROM Events"}};
  const int   eventNameIndex{query.record().indexOf("eventName")};

  while (query.next()) {
    const QString eventName{query.value(eventNameIndex).toString()};
    eventNames.push_back(eventName);
  }

  return eventNames;
}

QStringList Database::getUnitNames(int ageID)
{
  QStringList unitNames{};

  // This is considered unsafe by SQL nerds on the internet, :(
  QSqlQuery query{QString{"SELECT * FROM Units WHERE ageID = %1"}.arg(
    QString::number(ageID))};
  const int unitNameIndex{query.record().indexOf("unitName")};

  while (query.next()) {
    const QString unitName{query.value(unitNameIndex).toString()};
    unitNames.push_back(unitName);
  }

  return unitNames;
}

static void adjustUnitName(QString& unitName)
{
  unitName = unitName.toUpper();
  unitName = MainWindow::convertSpacesToUnderscores(unitName);
}

std::unordered_map<std::string, Entity> Database::getUnitEntities()
{
  std::unordered_map<std::string, Entity> map{};

  /*
   * SELECT u.unitName, u.unitHealth, u.unitStandardDamage, u.unitRangedDamage,
u.unitPointValue, a.armorName FROM Units u INNER JOIN unitArmorClasses ua on
u.unitID = ua.unitID INNER JOIN ArmorClasses a on ua.armorID = a.armorID;
   *
   */

  // TODO: First character might have to be lowercase
  QSqlQuery query{QString{R"(
SELECT
    Units.unitID,
    Units.ageID,
    Units.unitName,
    Units.unitHealth,
    Units.unitStandardDamage,
    Units.unitRangedDamage,
    Units.unitPointValue,
    ArmorClasses.armorID
FROM
    Units
INNER JOIN
    UnitArmorClasses ON Units.unitID = UnitArmorClasses.unitID
INNER JOIN
    ArmorClasses ON UnitArmorClasses.armorID = ArmorClasses.armorID;
)"}};
  const int ageIdIndex{query.record().indexOf("ageID")};
  const int unitNameIndex{query.record().indexOf("unitName")};
  const int unitHealthIndex{query.record().indexOf("unitHealth")};
  const int unitStandardDamageIndex{
    query.record().indexOf("unitStandardDamage")};
  const int unitRangedDamageIndex{query.record().indexOf("unitRangedDamage")};
  const int unitPointValueIndex{query.record().indexOf("unitPointValue")};
  const int armorIdIndex{query.record().indexOf("armorID")};

  while (query.next()) {
    bool      ok{};
    const int ageId{query.value(ageIdIndex).toInt(&ok)};
    if (!ok) {
      qFatal() << "Could not convert age Id to integer.";
    }
    QString unitNameQStr{query.value(unitNameIndex).toString()};
    adjustUnitName(unitNameQStr);
    const std::string unitName{unitNameQStr.toStdString()};
    const QString     unitHealth{query.value(unitHealthIndex).toString()};
    const QString     unitStandardDamage{
      query.value(unitStandardDamageIndex).toString()};
    const QString unitRangedDamage{
      query.value(unitRangedDamageIndex).toString()};
    const QString unitPointValue{query.value(unitPointValueIndex).toString()};
    const int     armorId{query.value(armorIdIndex).toInt(&ok)};
    if (!ok) {
      qFatal() << "Could not convert armor Id to integer.";
    }

    // TODO: Load special trebuchet and wonder the old way and load buildings
    // separately.

    if (unitName == "TREBUCHET") {
      continue;
    }

    const std::unordered_map<std::string, Entity>::iterator it{
      map.find(unitName)};

    if (it == map.end()) {
      // It wasn't found.
      Entity entity{};
      entity.entityAge           = ageId;
      entity.entityName          = unitName;
      entity.entityHealth        = unitHealth.toInt(&ok);
      entity.standardDamage      = unitStandardDamage.toInt(&ok);
      entity.rangedDamage        = unitRangedDamage.toInt(&ok);
      entity.pointValue          = unitPointValue.toInt(&ok);
      entity.armorClass[armorId] = true;
      map.emplace(unitName, entity);
    }
    else {
      // Found it.
      Entity& entity             = it->second;
      entity.armorClass[armorId] = true;
    }
  }

  return map;
}

#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace appFiles {

constexpr auto appFolderName = "qt-chat-app";

enum class Location
{
    Config,
    Data,
    Cache
};

QString appName();

QString baseDirectory(Location location);
QString directory(Location location);
QString path(Location location, const QString& relativePath);

QString settingsPath();
QString keystorePath();

bool ensureDirectory(Location location, QString* error = nullptr);
bool ensureParentDirectory(Location location, const QString& relativePath, QString* error = nullptr);

bool exists(Location location, const QString& relativePath);
std::optional<QByteArray> readFile(Location location, const QString& relativePath, QString* error = nullptr);
bool writeFile(Location location, const QString& relativePath, const QByteArray& bytes, QString* error = nullptr);
bool removeFile(Location location, const QString& relativePath, QString* error = nullptr);

} // namespace appFiles

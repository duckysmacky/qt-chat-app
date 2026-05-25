#include "app_files.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>

namespace appFiles {

namespace {

QString cleanRelativePath(const QString& relativePath)
{
    QString cleanPath = QDir::cleanPath(relativePath);
    while (cleanPath.startsWith('/'))
        cleanPath.remove(0, 1);

    while (cleanPath.startsWith("../"))
        cleanPath.remove(0, 3);

    if (cleanPath == ".")
        return {};

    return cleanPath;
}

QString environmentPath(const char* variableName)
{
    const QByteArray value = qgetenv(variableName);
    return value.isEmpty() ? QString() : QString::fromLocal8Bit(value);
}

QString fallbackHomePath(const QString& relativePath)
{
    return QDir::home().filePath(relativePath);
}

QString writableStandardPath(QStandardPaths::StandardLocation location)
{
    const QString path = QStandardPaths::writableLocation(location);
    return path.isEmpty() ? QDir::homePath() : path;
}

void setError(QString* error, const QString& message)
{
    if (error != nullptr)
        *error = message;
}

} // namespace

QString appName()
{
    return appFolderName;
}

QString baseDirectory(const Location location)
{
#ifdef Q_OS_WIN
    switch (location)
    {
    case Location::Config:
        {
            const QString appData = environmentPath("APPDATA");
            return appData.isEmpty()
                ? writableStandardPath(QStandardPaths::AppConfigLocation)
                : appData;
        }
    case Location::Data:
    case Location::Cache:
        {
            const QString localAppData = environmentPath("LOCALAPPDATA");
            return localAppData.isEmpty()
                ? writableStandardPath(QStandardPaths::GenericDataLocation)
                : localAppData;
        }
    }
#else
    switch (location)
    {
    case Location::Config:
        return fallbackHomePath(".config");
    case Location::Data:
        return fallbackHomePath(".local/share");
    case Location::Cache:
        return fallbackHomePath(".cache");
    }
#endif

    return QDir::homePath();
}

QString directory(const Location location)
{
    const QString appDirectory = QDir(baseDirectory(location)).filePath(appFolderName);

#ifdef Q_OS_WIN
    if (location == Location::Cache)
        return QDir(appDirectory).filePath("cache");
#endif

    return appDirectory;
}

QString path(const Location location, const QString& relativePath)
{
    const QString cleanPath = cleanRelativePath(relativePath);
    if (cleanPath.isEmpty())
        return directory(location);

    return QDir(directory(location)).filePath(cleanPath);
}

bool ensureDirectory(const Location location, QString* error)
{
    QDir dir;
    const QString target = directory(location);
    if (dir.mkpath(target))
        return true;

    setError(error, QString("Unable to create directory: %1").arg(target));
    return false;
}

bool ensureParentDirectory(const Location location, const QString& relativePath, QString* error)
{
    const QString target = path(location, relativePath);
    const QString parent = QFileInfo(target).absolutePath();
    QDir dir;

    if (dir.mkpath(parent))
        return true;

    setError(error, QString("Unable to create directory: %1").arg(parent));
    return false;
}

bool exists(const Location location, const QString& relativePath)
{
    return QFile::exists(path(location, relativePath));
}

std::optional<QByteArray> readFile(const Location location, const QString& relativePath, QString* error)
{
    QFile file(path(location, relativePath));
    if (!file.open(QIODevice::ReadOnly))
    {
        setError(error, file.errorString());
        return std::nullopt;
    }

    return file.readAll();
}

bool writeFile(const Location location, const QString& relativePath, const QByteArray& bytes, QString* error)
{
    if (!ensureParentDirectory(location, relativePath, error))
        return false;

    QSaveFile file(path(location, relativePath));
    if (!file.open(QIODevice::WriteOnly))
    {
        setError(error, file.errorString());
        return false;
    }

    if (file.write(bytes) != bytes.size())
    {
        setError(error, file.errorString());
        return false;
    }

    if (file.commit())
        return true;

    setError(error, file.errorString());
    return false;
}

bool removeFile(const Location location, const QString& relativePath, QString* error)
{
    QFile file(path(location, relativePath));
    if (!file.exists())
        return true;

    if (file.remove())
        return true;

    setError(error, file.errorString());
    return false;
}

} // namespace appFiles

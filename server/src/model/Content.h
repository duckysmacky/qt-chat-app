#pragma once

#include <QString>
#include <QUuid>

namespace model {

/**
 * @class Content
 * @brief Represents content associated with a message (text or file).
 */
class Content
{
public:
    /**
     * @brief Default constructor. Creates a Content object with default values.
     */
    Content();

    /**
     * @brief Constructs a Content object with specified parameters.
     * @param content The text content.
     * @param file The file path or identifier.
     * @param fileSize The size of the file in bytes.
     */
    Content(QString content,
            QString file,
            double fileSize);

    /// @brief Returns the unique identifier of the content.
    /// @return Constant reference to the content UUID.
    const QUuid& id() const;

    /// @brief Returns the text content.
    /// @return Constant reference to the content string.
    const QString& content() const;

    /// @brief Returns the file path or identifier.
    /// @return Constant reference to the file string.
    const QString& file() const;

    /// @brief Returns the file size.
    /// @return File size in bytes.
    double fileSize() const;

    /// @brief Sets the unique identifier of the content.
    /// @param id The new content UUID.
    void setId(const QUuid& id);

    /// @brief Sets the text content.
    /// @param content The new content string.
    void setContent(const QString& content);

    /// @brief Sets the file path or identifier.
    /// @param file The new file string.
    void setFile(const QString& file);

    /// @brief Sets the file size.
    /// @param fileSize The new file size in bytes.
    void setFileSize(double fileSize);

private:
    QUuid m_id;           ///< Unique identifier of the content
    QString m_content;    ///< Text content
    QString m_file;       ///< File path or identifier
    double m_fileSize = 0.0; ///< File size in bytes
};

} // namespace model
#pragma once

#include <QByteArray>
#include <QString>

namespace shared {

/**
 * @enum OperationResultType
 * @brief Represents the type of an operation result (success or error).
 */
enum class OperationResultType
{
    SUCCESS, ///< Indicates a successful operation
    ERROR    ///< Indicates an error condition
};

/**
 * @class OperationResult
 * @brief Represents the outcome of an operation with a type and descriptive text.
 */
class OperationResult
{
private:
    OperationResultType m_type; ///< Type of the result (success or error)
    QString m_text;             ///< Descriptive text associated with the result

public:
    /**
     * @brief Constructs an OperationResult with specified type and text.
     * @param type The type of the result.
     * @param text The descriptive text.
     */
    OperationResult(OperationResultType type, QString text);

    /// @brief Default copy constructor.
    OperationResult(const OperationResult& other) = default;

    /// @brief Default copy assignment operator.
    OperationResult& operator =(const OperationResult& other) = default;

    /// @brief Move constructor.
    /// @param other The OperationResult object to move from.
    OperationResult(OperationResult&& other) noexcept;

    /// @brief Move assignment operator.
    /// @param other The OperationResult object to move from.
    /// @return Reference to this OperationResult.
    OperationResult& operator =(OperationResult&& other) noexcept;

    /// @brief Default destructor.
    ~OperationResult() = default;

    /**
     * @brief Deserializes a byte array into an OperationResult object.
     * @param bytes The byte array to deserialize.
     * @return The deserialized OperationResult object.
     */
    static OperationResult deserialize(QByteArray bytes);

    /**
     * @brief Serializes the OperationResult object into a byte array.
     * @return QByteArray containing the serialized data.
     */
    QByteArray serialize() const;

    /// @brief Returns the type of the result.
    /// @return The OperationResultType value.
    OperationResultType type() const { return m_type; }

    /// @brief Returns the descriptive text of the result.
    /// @return Constant reference to the text string.
    const QString& text() const { return m_text; }
};

} // namespace shared

#pragma once

#include <QByteArray>
#include <QString>

namespace shared {

/**
 * @enum ResultType
 * @brief Represents the type of a result (success or error).
 */
enum class ResultType
{
    SUCCESS, ///< Indicates a successful operation
    ERROR    ///< Indicates an error condition
};

/**
 * @class Result
 * @brief Represents the outcome of an operation with a type and descriptive text.
 */
class Result
{
private:
    ResultType m_type; ///< Type of the result (success or error)
    QString m_text;    ///< Descriptive text associated with the result

public:
    /**
     * @brief Constructs a Result with specified type and text.
     * @param type The type of the result.
     * @param text The descriptive text.
     */
    Result(ResultType type, QString text);

    /// @brief Default copy constructor.
    Result(const Result& other) = default;

    /// @brief Default copy assignment operator.
    Result& operator =(const Result& other) = default;

    /// @brief Move constructor.
    /// @param other The Result object to move from.
    Result(Result&& other) noexcept;

    /// @brief Move assignment operator.
    /// @param other The Result object to move from.
    /// @return Reference to this Result.
    Result& operator =(Result&& other) noexcept;

    /// @brief Default destructor.
    ~Result() = default;

    /**
     * @brief Deserializes a byte array into a Result object.
     * @param bytes The byte array to deserialize.
     * @return The deserialized Result object.
     */
    static Result deserialize(QByteArray bytes);

    /**
     * @brief Serializes the Result object into a byte array.
     * @return QByteArray containing the serialized data.
     */
    QByteArray serialize() const;

    /// @brief Returns the type of the result.
    /// @return The ResultType value.
    ResultType type() const { return m_type; }

    /// @brief Returns the descriptive text of the result.
    /// @return Constant reference to the text string.
    const QString& text() const { return m_text; }
};

} // namespace shared
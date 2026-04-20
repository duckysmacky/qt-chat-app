#include "Content.h"

#include <utility>

namespace model {

Content::Content() = default;

Content::Content(QString content,
                 QString file,
                 double fileSize)
    : m_id(QUuid::createUuid()),
      m_content(std::move(content)),
      m_file(std::move(file)),
      m_fileSize(fileSize)
{
}

const QUuid& Content::id() const
{
    return m_id;
}

const QString& Content::content() const
{
    return m_content;
}

const QString& Content::file() const
{
    return m_file;
}

double Content::fileSize() const
{
    return m_fileSize;
}

void Content::setId(const QUuid& id)
{
    m_id = id;
}

void Content::setContent(const QString& content)
{
    m_content = content;
}

void Content::setFile(const QString& file)
{
    m_file = file;
}

void Content::setFileSize(double fileSize)
{
    m_fileSize = fileSize;
}

}

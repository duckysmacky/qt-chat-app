#pragma once

#include <QString>
#include <QUuid>

namespace model {

class Content
{
public:
    Content();
    Content(QString content,
            QString file,
            double fileSize);

    const QUuid& id() const;
    const QString& content() const;
    const QString& file() const;
    double fileSize() const;

    void setId(const QUuid& id);
    void setContent(const QString& content);
    void setFile(const QString& file);
    void setFileSize(double fileSize);

private:
    QUuid m_id;
    QString m_content;
    QString m_file;
    double m_fileSize = 0.0;
};

}

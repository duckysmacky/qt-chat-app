#pragma once

#include <QByteArray>
#include <QList>

#include "Message.h"

namespace shared::util {

QList<Message> parse(const QByteArray& bytes);

}

#pragma once

#include <QByteArray>
#include <QList>
#include "Message.h"

namespace shared {

QList<shared::Message> parse(const QByteArray& bytes);

}

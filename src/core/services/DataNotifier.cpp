#include "DataNotifier.h"

DataNotifier &DataNotifier::instance()
{
    static DataNotifier instance;
    return instance;
}

void DataNotifier::notifier()
{
    emit donneesModifiees();
}
